// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615aCommands::Targets::ListTargetsCommand.
 **/

#include "ListTargetsCommand.hpp"

#include <arinc_615a/find/TargetInformation.hpp>

#include <helper/SeverityLevelDescription.hpp>

#include <spdlog/spdlog.h>

#include <boost/program_options.hpp>

#include <boost/exception/all.hpp>

#include <boost/property_tree/json_parser.hpp>

#include <format>
#include <iostream>

namespace Arinc615aCommands::Targets {

ListTargetsCommand::ListTargetsCommand() :
  optionsDescription{ "List Targets List Options" }
{
  optionsDescription.add_options()
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
  )
  (
    "targets-list,l",
    boost::program_options::value( &targetsListPathV )
      ->value_name( "file" )
      ->required(),
    "targets into target list JSON file."
  );
}

void ListTargetsCommand::execute( const Commands::Parameters &parameters )
{
  try
  {
    std::cout << "List ARINC 615A Targets\n";

    boost::program_options::variables_map variablesMap;
    boost::program_options::store(
      boost::program_options::command_line_parser( parameters ).options( optionsDescription ).run(),
      variablesMap );
    boost::program_options::notify( variablesMap );

    boost::property_tree::ptree targetsListPTree{};
    boost::property_tree::read_json( targetsListPathV.string(), targetsListPTree );

    for (
      const auto targetsInformation{ Arinc615a::Find::TargetInformation::targetsAddressInformation( targetsListPTree ) };
      const auto &[ address, targetInformation ] : targetsInformation )
    {
      std::cout
        << "Target\n"
        << "\tAddress:           '" << address << "'\n"
        << "\tTHW ID:            '" << targetInformation.thwId << "'\n"
        << "\tTHW Type Name:     '" << targetInformation.thwTypeName << "'\n"
        << "\tTHW Position:      '" << targetInformation.thwPosition << "'\n"
        << "\tLiteral Name:      '" << targetInformation.literalName << "'\n"
        << "\tManufacturer Code: '" << targetInformation.manufacturerCode << "'\n"
        << "\t** Target ID **    '" << targetInformation.targetId() << "'\n";
    }
  }
  catch ( const boost::exception &e )
  {
    std::cerr << std::format( "Operation failed: {}\n", boost::diagnostic_information( e ) );
  }
  catch ( const std::exception &e )
  {
    std::cerr << std::format( "Operation failed: {}\n", e.what() );
  }
  catch ( ... )
  {
    std::cerr << "Operation failed: UNKNOWN EXCEPTION\n";
  }
}

void ListTargetsCommand::help() const
{
  std::cout << "List ARINC 615A Targets\n" << optionsDescription;
}

}
