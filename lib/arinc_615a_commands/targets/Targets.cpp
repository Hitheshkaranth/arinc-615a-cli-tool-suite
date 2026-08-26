// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Namespace Arinc615aCommands::Targets.
 **/

#include "Targets.hpp"

#include <arinc_615a_commands/targets/FindQueryCommand.hpp>
#include <arinc_615a_commands/targets/ListTargetsCommand.hpp>

#include <commands/CommandRegistry.hpp>

namespace Arinc615aCommands::Targets {

void registerCommands(
  const Commands::CommandRegistryPtr &registry,
  boost::asio::io_context &ioContext,
  AbortTerminateSignal &abortSignal,
  AbortTerminateSignal &terminateSignal )
{
  auto findQueryCommand{ std::make_shared< FindQueryCommand >( ioContext, abortSignal, terminateSignal ) };

  registry->command(
    "Find",
    "ARINC 615A FIND Query",
    std::bind_front( &FindQueryCommand::execute, findQueryCommand ),
    std::bind_front( &FindQueryCommand::help, findQueryCommand ) );

  auto listTargetsCommand{ std::make_shared< ListTargetsCommand >() };

  registry->command(
    "Targets",
    "List ARINC 615A Targets",
    std::bind_front( &ListTargetsCommand::execute, listTargetsCommand ),
    std::bind_front( &ListTargetsCommand::help, listTargetsCommand ) );
}

}
