// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615aCommands::Operations::InformationOperationCommand.
 **/

#include "InformationOperationCommand.hpp"

#include <arinc_615a/find/TargetInformation.hpp>

#include <arinc_615a/host/InformationOperation.hpp>
#include <arinc_615a/host/OperationConfiguration.hpp>
#include <arinc_615a/host/Protocol.hpp>

#include <arinc_615a/information/InitializationResponse.hpp>
#include <arinc_615a/information/PartNumber.hpp>
#include <arinc_615a/information/Status.hpp>
#include <arinc_615a/information/TargetHardware.hpp>

#include <arinc_615a/StatusCodeDescription.hpp>

#include <tftp/packets/PacketStatistic.hpp>

#include <helper/BoostAsioProgramOptions.hpp>
#include <helper/SeverityLevelDescription.hpp>

#include <spdlog/spdlog.h>

#include <boost/exception/all.hpp>

#include <boost/program_options.hpp>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include <format>
#include <iostream>

namespace Arinc615aCommands::Operations {

InformationOperationCommand::InformationOperationCommand(
  boost::asio::io_context &ioContext,
  AbortTerminateSignal &abortSignal,
  AbortTerminateSignal &terminateSignal ) :
  ioContextV{ ioContext },
  abortSignalV{ abortSignal },
  terminateSignalV{ terminateSignal },
  optionsDescriptionV{ "ARINC 615A Information Operation Options" }
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
        Tftp::setLogLevel( logLevel );
        Helper::setLogLevel( logLevel );
      } ),
    Helper::SeverityLevelDescription::instance().allLevels().c_str()
  );
  optionsDescriptionV.add( configurationV.options() );
  optionsDescriptionV.add_options()
  (
    "target-address,a",
    boost::program_options::value( &targetAddressV )
      ->value_name( "ip" ),
    "IP address of target hardware."
  )
  (
    "targets-list,l",
    boost::program_options::value( &targetsListPathV )
      ->value_name( "file" ),
    "List of targets JSON File to map Target IDs to IP addresses."
  )
  (
    "target-id,i",
    boost::program_options::value( &targetIdV )
      ->required()
      ->value_name( "target-id" ),
    "ARINC 615A Target ID.\n"
    "Required."
  )
  (
    "dlp-timeout",
    boost::program_options::value< std::chrono::seconds::rep >()
      ->default_value( Arinc615a::DefaultArinc615aDlpTimeout.count() )
      ->value_name( "timeout" )
      ->notifier(
        [ &dlpTimeout = dlpTimeoutV ]( const auto dlpTimeoutInt )
        {
          dlpTimeout = std::chrono::seconds{ dlpTimeoutInt };
        } ),
    "ARINC 615A DLP timeout in seconds."
  )
  (
    "port-option",
    boost::program_options::bool_switch( &portOptionV ),
    "Enables the ARINC 615A-3 Port Option."
  );
}

void InformationOperationCommand::execute( const Commands::Parameters &parameters )
{
  try
  {
    std::cout << "ARINC 615A Information Operation\n";

    boost::program_options::variables_map variablesMap;
    boost::program_options::store(
      boost::program_options::command_line_parser( parameters ).options( optionsDescriptionV ).run(),
      variablesMap );
    boost::program_options::notify( variablesMap );

    if ( !targetsListPathV.empty() )
    {
      boost::property_tree::ptree targetsListPTree{};
      boost::property_tree::read_json( targetsListPathV.string(), targetsListPTree );

      const auto targetsInformation{ Arinc615a::Find::TargetsAddressInformationMap_fromTargetsAddressInformation(
        Arinc615a::Find::TargetInformation::targetsAddressInformation( targetsListPTree ) )};

      if ( const auto targetInformation{ targetsInformation.find( targetIdV ) }; targetInformation != targetsInformation.end() )
      {
        SPDLOG_INFO(
          "Use target address '{}' for '{}'",
          std::get< 0 >( targetInformation->second ).to_string(),
          targetIdV );
        targetAddressV = std::get< 0 >( targetInformation->second );
      }
    }

    if ( targetAddressV.is_unspecified() )
    {
      BOOST_THROW_EXCEPTION(
        boost::program_options::error{ "Target IP address invalid, not provided, or not in target list." } );
    }

    const auto protocol{ Arinc615a::Host::Protocol::instance( ioContextV ) };
    assert( protocol );

    operationV = protocol->informationOperation(
      Arinc615a::Host::InformationOperationConfiguration{
        .dataLoaderConfiguration = configurationV,
        .handler = *this,
        .targetAddress = targetAddressV,
        .targetId = Arinc615a::TargetId{ targetIdV },
        .dlpTimeout = dlpTimeoutV,
        .portOption = portOptionV
      } );
    assert( operationV );

    boost::signals2::scoped_connection abortConnection{ abortSignalV.connect(
      std::bind_front(
        &Arinc615a::Host::InformationOperation::abort,
        operationV,
        Arinc615a::Host::InformationOperation::AbortReason::Operator ) ) };
    boost::signals2::scoped_connection terminateConnection{ terminateSignalV.connect(
      std::bind_front(
        &Arinc615a::Host::InformationOperation::terminate,
        operationV,
        Arinc615a::Host::InformationOperation::AbortReason::Operator ) ) };

    // execute operation
    operationV->start();

    // Wait for completion
    done.wait();

    // Print TFTP Statistic
    std::cout
      << "TFTP RX:\n" << Tftp::Packets::PacketStatistic::globalReceive() << "\n"
      << "TFTP TX:\n" << Tftp::Packets::PacketStatistic::globalTransmit() << "\n";
  }
  catch ( const boost::program_options::error & )
  {
    // Command line parsing errors are handled within command handler
    throw;
  }
  catch ( const boost::exception &e )
  {
    std::cerr << std::format( "Information Operation failed: {}\n", boost::diagnostic_information( e ) );
  }
  catch ( const std::exception &e )
  {
    std::cerr << std::format( "Information Operation failed: {}\n", e.what() );
  }
  catch ( ... )
  {
    std::cerr << "Information Operation failed: UNKNOWN EXCEPTION\n";
  }
}

void InformationOperationCommand::help() const
{
  std::cout << "Information Operation\n";
  std::cout << optionsDescriptionV;
}

void InformationOperationCommand::initialisationDeferred( const std::chrono::seconds waitTime )
{
  std::cout << std::format( "Operation deferred for: {}\n", waitTime );
}

void InformationOperationCommand::initialisationResponse(
  const Arinc615a::Information::InitializationResponse &response )
{
  std::cout << std::format(
    "Initialisation Response:\n"
    "\tInitialisation Code: {}\n"
    "\tStatus Description:  '{}'\n",
    response.code(),
    response.description() );
}

void InformationOperationCommand::finished( const Arinc615a::StatusCode code, std::string_view description )
{
  std::cout << std::format(
    "Operation finished:\n"
    "\tFinal Status Code:        {}\n"
    "\tFinal Status Description: '{}'\n",
    code,
    description );

  done.count_down();
}

void InformationOperationCommand::status( const Arinc615a::Information::InformationStatus &status )
{
  assert( operationV );

  std::cout << std::format(
    "Information Status:\n"
    "\tCounter:            {}\n"
    "\tStatus Code:        {}\n"
    "\tStatus Description: '{}'\n"
    "\tException Timer:    {}\n"
    "\tEstimated Time:     {}\n",
    status.counter(),
    status.code(),
    status.description(),
    status.exceptionTimer(),
    status.estimatedTime() );
}

void InformationOperationCommand::targetInformation(
  const Arinc615a::Information::TargetsHardware &targetsHardware,
  const bool integrity )
{
  std::cout
    << "Information Integrity: " << ( integrity ? "Valid" : "Invalid" ) << "\n";

  for ( const auto &targetHardware : targetsHardware )
  {
    std::cout << targetHardware.toString();
  }
}

}
