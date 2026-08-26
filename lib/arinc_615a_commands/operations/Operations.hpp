// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Namespace Arinc615aCommands::Operations.
 **/

/**
 * @dir
 * @brief Declaration/ Definition of Namespace Arinc615aCommands::Operations.
 **/

#ifndef ARINC_615A_COMMANDS_OPERATIONS_OPERATIONS_HPP
#define ARINC_615A_COMMANDS_OPERATIONS_OPERATIONS_HPP

#include <arinc_615a_commands/Arinc615aCommands.hpp>

/**
 * @brief Namespace for ARINC 615A Commands.
 *
 * Contains the commands which implements the ARINC 615A %Commands:
 * - @ref InformationOperationCommand
 * - @ref UploadOperationCommand
 * - @ref BatchUploadOperationCommand
 * - @ref MediaDefinedDownloadOperationCommand
 * - @ref OperatorDefinedDownloadOperationCommand
 **/
namespace Arinc615aCommands::Operations {

/**
 * @brief Registers the ARINC 615A %Commands to the Registry.
 *
 * @param[in,out] registry
 *   Command registry.
 * @param[in] ioContext
 *   I/O Context used of operations.
 * @param[in] abortSignal
 *   Signal raised when application is aborted.
 * @param[in] terminateSignal
 *   Signal raised when application is terminated.
 *   Registry, where the commands shall be registered to.
 **/
ARINC_615A_COMMANDS_EXPORT void registerCommands(
  const Commands::CommandRegistryPtr &registry,
  boost::asio::io_context &ioContext,
  AbortTerminateSignal &abortSignal,
  AbortTerminateSignal &terminateSignal );

}

#endif
