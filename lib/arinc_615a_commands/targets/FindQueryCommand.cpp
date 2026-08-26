// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615aCommands::Targets::FindQueryCommand.
 **/

#include "FindQueryCommand.hpp"

#include <arinc_615a/find/clients/Client.hpp>
#include <arinc_615a/find/clients/Query.hpp>

#include <helper/BoostAsioProgramOptions.hpp>
#include <helper/SeverityLevelDescription.hpp>

#include <spdlog/spdlog.h>

#include <boost/program_options.hpp>

#include <boost/exception/all.hpp>

#include <boost/property_tree/json_parser.hpp>

#include <format>
#include <iostream>

namespace Arinc615aCommands::Targets {

FindQueryCommand::FindQueryCommand(
  boost::asio::io_context &ioContext,
  AbortTerminateSignal &abortSignal,
  AbortTerminateSignal &terminateSignal ) :
  ioContextV{ ioContext },
  abortSignalV{ abortSignal },
  terminateSignalV{ terminateSignal },
  optionsDescriptionV{ "ARINC 615A FIND Query Options" }
{
  optionsDescriptionV.add_options()
  (
    "log-level,l",
    boost::program_options::value< spdlog::level::level_enum >()
      ->default_value( spdlog::level::level_enum::warn, "warn" )
      ->value_name( "log-level" )
      ->notifier( []( const auto &logLevel ) {
        spdlog::set_level( logLevel );
        Arinc615aCommands::setLogLevel( logLevel );
        Arinc615a::setLogLevel( logLevel );
        Helper::setLogLevel( logLevel );
      } ),
    Helper::SeverityLevelDescription::instance().allLevels().c_str()
  );
  optionsDescriptionV.add( configurationV.options() );
  optionsDescriptionV.add( targetAddress( &targetAddressV, true ) );
  optionsDescriptionV.add_options()
  (
    "dynamic-port,p",
    boost::program_options::bool_switch( &dynamicPortV ),
    "Use local dynamic port"
  )
  (
    "timeout,t",
    boost::program_options::value< std::chrono::seconds::rep >()
      ->default_value( Arinc615a::Find::DefaultReceiveTimeout.count() )
      ->value_name( "timeout" )
      ->notifier(
        [ &timeout= timeoutV ]( const auto timeoutInt )
        {
          timeout = std::chrono::seconds{ timeoutInt };
        } ),
    "FIND timeout in seconds"
  )
  (
    "targets-list,l",
    boost::program_options::value( &targetsListPathV )->value_name( "file" ),
    "If set, saves the found targets into target list JSON file."
  );
}

void FindQueryCommand::execute( const Commands::Parameters &parameters )
{
  try
  {
    std::cout << "ARINC 615A FIND Query\n";

    boost::program_options::variables_map variablesMap;
    boost::program_options::store(
      boost::program_options::command_line_parser( parameters ).options( optionsDescriptionV ).run(),
      variablesMap );
    boost::program_options::notify( variablesMap );

    auto client{ Arinc615a::Find::Clients::Client::instance( ioContextV ) };
    assert( client );

    auto query{ client->query() };
    assert( query );

    boost::signals2::scoped_connection abortConnection{
      abortSignalV.connect( AbortTerminateSignal::slot_type{ &Arinc615a::Find::Clients::Query::abort, query } ) };
    boost::signals2::scoped_connection terminateConnection{
      terminateSignalV.connect( AbortTerminateSignal::slot_type{ &Arinc615a::Find::Clients::Query::abort, query } ) };

    // configure
    query
      ->responseHandler( std::bind_front( &FindQueryCommand::response, this ) )
      .completionHandler( std::bind_front( &FindQueryCommand::finishedFind, this ) )
      .localAddress( configurationV.localInterfaceAddress )
      .remoteAddress( targetAddressV )
      .port( configurationV.findPort )
      .dynamicLocalPort( dynamicPortV )
      .timeout( timeoutV );

    // execute operation
    query->start();

    // Wait for completion
    done.wait();

    // write the targets list
    if ( !targetsListPathV.empty() && !targetsV.empty() )
    {
      boost::property_tree::write_json(
        targetsListPathV.string(),
        Arinc615a::Find::TargetInformation::targetsAddressInformation( targetsV ) );
    }
  }
  catch ( const boost::exception &e )
  {
    std::cerr << std::format( "FIND query failed: {}\n", boost::diagnostic_information( e ) );
  }
  catch ( const std::exception &e )
  {
    std::cerr << std::format( "FIND query failed: {}\n", e.what() );
  }
  catch ( ... )
  {
    std::cerr << "FIND query failed: UNKNOWN EXCEPTION\n";
  }
}

void FindQueryCommand::help() const
{
  std::cout << "ARINC 615A FIND Query\n" << optionsDescriptionV;
}

void FindQueryCommand::response(
  const boost::asio::ip::address &target,
  const Arinc615a::Find::TargetInformation &information )
{
  std::cout
    << "Response from " << target << ":\n"
    << "\tTHW ID:            '" << information.thwId << "'\n"
    << "\tTHW Type Name:     '" << information.thwTypeName << "'\n"
    << "\tTHW Position:      '" << information.thwPosition << "'\n"
    << "\tLiteral Name:      '" << information.literalName << "'\n"
    << "\tManufacturer Code: '" << information.manufacturerCode << "'\n"
    << "\t** Target ID **    '" << information.targetId() << "'\n";

  targetsV.emplace_back( std::make_pair( target, information ) );
}

void FindQueryCommand::finishedFind()
{
  std::cout << "ARINC 615A FIND Query finished\n";

  done.count_down();
}

}
