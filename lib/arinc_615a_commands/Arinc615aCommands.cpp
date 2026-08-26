// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Namespace Arinc615aCommands.
 **/

#include "Arinc615aCommands.hpp"

#include <arinc_615a_commands/operations/Operations.hpp>

#include <arinc_615a_commands/targets/Targets.hpp>

#include <helper/BoostAsioProgramOptions.hpp>

namespace Arinc615aCommands {

void registerCommands(
  const Commands::CommandRegistryPtr &registry,
  boost::asio::io_context &ioContext,
  AbortTerminateSignal &abortSignal,
  AbortTerminateSignal &terminateSignal )
{
  Targets::registerCommands( registry, ioContext, abortSignal, terminateSignal );

  Operations::registerCommands( registry, ioContext, abortSignal, terminateSignal );
}

boost::shared_ptr< boost::program_options::option_description > targetAddress(
  boost::asio::ip::address * const targetAddress,
  bool const defaultIsBroadcast )
{
  auto * const valueSemantic{ boost::program_options::value( targetAddress ) };
  valueSemantic
    ->value_name( "ip" )
    ->required();
  if ( defaultIsBroadcast )
  {
    valueSemantic->default_value( boost::asio::ip::address_v4::broadcast() );
  }

  return boost::make_shared< boost::program_options::option_description >(
    "target-address,a",
    valueSemantic,
    "IP address of target hardware." );
}

void setLogLevel( const spdlog::level::level_enum level )
{
  spdlog::set_level( level );
}

}
