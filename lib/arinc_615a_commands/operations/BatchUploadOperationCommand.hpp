// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615aCommands::Operations::BatchUploadOperationCommand.
 **/

#ifndef ARINC_615A_BATCHUPLOADOPERATIONCOMMAND_HPP
#define ARINC_615A_BATCHUPLOADOPERATIONCOMMAND_HPP

#include <arinc_615a_commands/operations/Operations.hpp>

#include <arinc_615a/find/TargetInformation.hpp>

#include <arinc_615a/information/Information.hpp>

#include <arinc_615a/host/BatchUploadOperationHandler.hpp>
#include <arinc_615a/host/BatchUploadOperationProxy.hpp>

#include <arinc_615a/tftp/servers/Servers.hpp>

#include <arinc_615a/Arinc615aConfiguration.hpp>

#include <arinc_665/media/Media.hpp>

#include <arinc_665/utils/MediaSetManager.hpp>
#include <arinc_665/utils/MediaSetManagerConfiguration.hpp>

#include <commands/Commands.hpp>

#include <forward_list>
#include <latch>
#include <string>

namespace Arinc615aCommands::Operations {

/**
 * @brief ARINC 615A Batch Upload Operation Command.
 *
 * Takes a Batch and loads all items specified within.
 *
 * @sa @ref UploadOperationCommand
 * @sa @ref Arinc615a::Host::UploadOperation
 **/
class ARINC_615A_COMMANDS_EXPORT BatchUploadOperationCommand final :
  private Arinc615a::Host::BatchUploadOperationHandler
{
  public:
    /**
     * @brief Constructs the ARINC 615A Batch Upload Operation.
     *
     * @param[in] ioContext
     *   I/O Context used of operations.
     * @param[in] abortSignal
     *   Signal raised when application is aborted.
     * @param[in] terminateSignal
     *   Signal raised when application is terminated.
     **/
    BatchUploadOperationCommand(
      boost::asio::io_context &ioContext,
      AbortTerminateSignal &abortSignal,
      AbortTerminateSignal &terminateSignal );

    /**
     * @brief Executes the Batch Upload Operation.
     *
     * @param[in] parameters
     *   Command parameters.
     **/
    void execute( const Commands::Parameters &parameters );

    //! Prints command help.
    void help() const;

  private:
    /**
     * @brief Load progress indicator.
     *
     * @param[in] mediaSet
     *   Media Set information
     * @param[in] partNumber
     *   Media Set Part Number
     * @param[in] medium
     *   Medium information
     **/
    void loadProgress(
      std::pair< std::size_t, std::size_t > mediaSet,
      std::string_view partNumber,
      std::pair< Arinc665::MediumNumber, Arinc665::MediumNumber > medium );

    /**
     * @brief Represents the target for the next batch operation.
     *
     * Indicates the recipient or destination for the upcoming batch processing task.
     **/
    void nextBatchTarget();

    //! @copydoc Arinc615a::Host::BatchUploadOperationHandler::initialisationDeferred
    void initialisationDeferred( const Arinc615a::TargetId &targetId, std::chrono::seconds waitTime ) override;

    //! @copydoc Arinc615a::Host::BatchUploadOperationHandler::initialisationResponse
    void initialisationResponse(
      const Arinc615a::TargetId &targetId,
      const Arinc615a::Information::InitializationResponse &response ) override;

    //! @copydoc Arinc615a::Host::BatchUploadOperationHandler::finished
    void finished(
      const Arinc615a::TargetId &targetId,
      Arinc615a::StatusCode code, std::string_view description ) override;

    //! @copydoc Arinc615a::Host::BatchUploadOperationHandler::status
    void status( const Arinc615a::TargetId &targetId, const Arinc615a::Information::UploadStatus &status ) override;

    //! @copydoc Arinc615a::Host::BatchUploadOperationHandler::fileRequest
    void fileRequest(
      const Arinc615a::TargetId &targetId,
      const boost::asio::ip::udp::endpoint &remote,
      std::string_view filename,
      const Tftp::Packets::TftpOptions &clientTftpOptions,
      std::string_view loadPartNumber,
      const Arinc649::CheckValue &checkValue ) override;

    /**
     * @brief File transfer completed handler.
     *
     * @param[in,out] operation
     *   TFTP Server Operation
     * @param[in] status
     *   Transfer status.
     **/
    void fileCompleted( const Arinc615a::Tftp::Servers::ReadOperationPtr &operation, Tftp::TransferStatus status );

    //! ASIO I/O context
    boost::asio::io_context &ioContextV;

    //! Abort Signal
    AbortTerminateSignal &abortSignalV;
    //! Terminate Signal
    AbortTerminateSignal &terminateSignalV;
    //! Operation Done Indicator for Synchronisation.
    std::latch done{ 1 };

    //! Program Options Description.
    boost::program_options::options_description optionsDescriptionV;

    //! Targets List JSON Path
    std::filesystem::path targetsListPathV;
    //! DLP Timeout
    std::chrono::seconds dlpTimeoutV;
    //! Use Port Option
    bool portOptionV{ false };
    //! Media Set Manager Directory.
    std::filesystem::path mediaSetManagerDirectoryV;
    //! Check Media Set Manager Integrity during Initialisation.
    bool checkMediaSetManagerIntegrityV{ true };
    //! Media Set Part Number
    std::string mediaSetPartNumberV;
    //! Batch Filename
    std::string batchFileV;

    //! Targets Information
    Arinc615a::Find::TargetsAddressInformationMap targetsV;
    //! Media Set Manager
    Arinc665::Utils::MediaSetManagerPtr mediaSetManagerV;
    //! ARINC 615A Data Loader Configuration
    Arinc615a::Arinc615aConfiguration configurationV;
    //! Media Set
    Arinc665::Media::ConstMediaSetPtr mediaSetV;
    //! ARINC 649 Check Values for file lookup
    Arinc665::Media::CheckValues checkValuesV;
    //! Protocol
    Arinc615a::Host::ProtocolPtr protocolV;
    //! ARINC 665 Batch Information
    Arinc665::Media::ConstBatchPtr batchV;
    //! Batch Targets
    Arinc665::Media::ConstBatchInformation batchTargetsV;
    //! ARINC 665 Loads
    Arinc665::Media::ConstLoads loadsV;

    //! Batch Upload Operation Proxy
    std::unique_ptr< Arinc615a::Host::BatchUploadOperationProxy > batchUploadOperationProxyV;
    //! Operation
    Arinc615a::Host::UploadOperationPtr operationV;
    //! File Transfer Operations
    std::forward_list< Arinc615a::Tftp::Servers::ReadOperationPtr > fileOperationsV;

    //! Load List Transmitted
    bool loadListTransmittedV{ false };
};

}

#endif
