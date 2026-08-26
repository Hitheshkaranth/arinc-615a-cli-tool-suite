// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Namespace Arinc615aCommands::Operations.
 **/

#include "Operations.hpp"

#include <arinc_615a_commands/operations/AdhocUploadOperationCommand.hpp>
#include <arinc_615a_commands/operations/BatchUploadOperationCommand.hpp>
#include <arinc_615a_commands/operations/InformationOperationCommand.hpp>
#include <arinc_615a_commands/operations/MediaDefinedDownloadOperationCommand.hpp>
#include <arinc_615a_commands/operations/OperatorDefinedDownloadOperationCommand.hpp>
#include <arinc_615a_commands/operations/UploadOperationCommand.hpp>
#include <arinc_615a_commands/operations/UploadLoadsOperationCommand.hpp>

#include <commands/CommandRegistry.hpp>

namespace Arinc615aCommands::Operations {

void registerCommands(
  const Commands::CommandRegistryPtr &registry,
  boost::asio::io_context &ioContext,
  AbortTerminateSignal &abortSignal,
  AbortTerminateSignal &terminateSignal )
{
  auto informationOperation{
    std::make_shared< InformationOperationCommand >( ioContext, abortSignal, terminateSignal ) };

  registry->command(
    "Information",
    "ARINC 615A Information Operation",
    std::bind_front( &InformationOperationCommand::execute, informationOperation ),
    std::bind_front( &InformationOperationCommand::help, informationOperation ) );

  auto uploadOperationCommand{ std::make_shared< UploadOperationCommand >( ioContext, abortSignal, terminateSignal ) };

  registry->command(
    "Upload",
    "ARINC 615A Upload Operation",
    std::bind_front( &UploadOperationCommand::execute, uploadOperationCommand ),
    std::bind_front( &UploadOperationCommand::help, uploadOperationCommand ) );

  auto adhocUploadOperationCommand{ std::make_shared< AdhocUploadOperationCommand >( ioContext, abortSignal, terminateSignal ) };

  registry->command(
    "AdhocUpload",
    "ARINC 615A Adhoc Upload Operation",
    std::bind_front( &AdhocUploadOperationCommand::execute, adhocUploadOperationCommand ),
    std::bind_front( &AdhocUploadOperationCommand::help, adhocUploadOperationCommand ) );

  auto uploadLoadsOperationCommand{ std::make_shared< UploadLoadsOperationCommand >( ioContext, abortSignal, terminateSignal ) };

  registry->command(
    "UploadLoads",
    "ARINC 615A Upload Loads Operation",
    std::bind_front( &UploadLoadsOperationCommand::execute, uploadLoadsOperationCommand ),
    std::bind_front( &UploadLoadsOperationCommand::help, uploadLoadsOperationCommand ) );

  auto batchUploadOperationCommand{
    std::make_shared< BatchUploadOperationCommand >( ioContext, abortSignal, terminateSignal ) };

  registry->command(
    "BatchUpload",
    "ARINC 615A Batch Upload Operation",
    std::bind_front( &BatchUploadOperationCommand::execute, batchUploadOperationCommand ),
    std::bind_front( &BatchUploadOperationCommand::help, batchUploadOperationCommand ) );

  auto mediaDefinedDownloadOperationCommand{
    std::make_shared< MediaDefinedDownloadOperationCommand >( ioContext, abortSignal, terminateSignal ) };

  registry->command(
    "MedDownload",
    "ARINC 615A Media Defined Download Operation",
    std::bind_front( &MediaDefinedDownloadOperationCommand::execute, mediaDefinedDownloadOperationCommand ),
    std::bind_front( &MediaDefinedDownloadOperationCommand::help, mediaDefinedDownloadOperationCommand ) );

  auto operatorDefinedDownloadOperation{
    std::make_shared< OperatorDefinedDownloadOperationCommand >( ioContext, abortSignal, terminateSignal ) };

  registry->command(
    "OpDownload",
    "ARINC 615A Operator Defined Download Operation",
    std::bind_front( &OperatorDefinedDownloadOperationCommand::execute, operatorDefinedDownloadOperation ),
    std::bind_front( &OperatorDefinedDownloadOperationCommand::help, operatorDefinedDownloadOperation ) );
}

}
