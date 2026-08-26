// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615aCommands::Operations::MediaDefinedDownloadOperationCommand.
 **/

#ifndef ARINC_615A_COMMANDS_OPERATIONS_MEDIADEFINEDDOWNLOADOPERATIONCOMMAND_HPP
#define ARINC_615A_COMMANDS_OPERATIONS_MEDIADEFINEDDOWNLOADOPERATIONCOMMAND_HPP

#include <arinc_615a_commands/operations/Operations.hpp>

#include <arinc_615a/information/Information.hpp>

#include <arinc_615a/host/DownloadInformation.hpp>
#include <arinc_615a/host/MediaDefinedDownloadOperationHandler.hpp>

#include <arinc_615a/tftp/servers/Servers.hpp>

#include <arinc_615a/Arinc615aConfiguration.hpp>

#include <commands/Commands.hpp>

#include <filesystem>
#include <forward_list>
#include <latch>
#include <string>
#include <vector>

namespace Arinc615aCommands::Operations {

/**
 * @brief ARINC 615A Media Defined Download Operation Command.
 *
 * The operation requests a specified list of files from the target hardware.
 *
 * @sa @ref Arinc615a::Host::MediaDefinedDownloadOperation
 **/
class ARINC_615A_COMMANDS_EXPORT MediaDefinedDownloadOperationCommand final :
  private Arinc615a::Host::MediaDefinedDownloadOperationHandler
{
  public:
    /**
     * @brief Constructs the ARINC 615A Media Defined Download Operation Command.
     *
     * @param[in] ioContext
     *   I/O Context used of operations.
     * @param[in] abortSignal
     *   Signal raised when application is aborted.
     * @param[in] terminateSignal
     *   Signal raised when application is terminated.
     **/
    MediaDefinedDownloadOperationCommand(
      boost::asio::io_context &ioContext,
      AbortTerminateSignal &abortSignal,
      AbortTerminateSignal &terminateSignal );

    /**
     * @brief Executes the Media Defined Download Operation.
     *
     * @param[in] parameters
     *   Command parameters.
     **/
    void execute( const Commands::Parameters &parameters );

    //! Prints command help
    void help() const;

  private:
    //! @copydoc Arinc615a::Host::MediaDefinedDownloadOperationHandler::initialisationDeferred
    void initialisationDeferred( std::chrono::seconds waitTime ) override;

    //! @copydoc Arinc615a::Host::MediaDefinedDownloadOperationHandler::initialisationResponse
    void initialisationResponse( const Arinc615a::Information::InitializationResponse &response ) override;

    //! @copydoc Arinc615a::Host::MediaDefinedDownloadOperationHandler::finished
    void finished( Arinc615a::StatusCode code, std::string_view description ) override;

    //! @copydoc Arinc615a::Host::MediaDefinedDownloadOperationHandler::status
    void status( const Arinc615a::Information::DownloadStatus &status ) override;

    //! @copydoc Arinc615a::Host::MediaDefinedDownloadOperationHandler::fileRequest
    void fileRequest(
      const boost::asio::ip::udp::endpoint &remote,
      std::string_view filename,
      const Tftp::Packets::TftpOptions &clientTftpOptions,
      std::string_view partNumber,
      const Arinc649::CheckValue &checkValue ) override;

    /**
     * @brief File transfer completed handler.
     *
     * @param[in] filename
     *   Filename
     * @param[in] filePath
     *   Path where data is stored.
     * @param[in] expectedFileSize
     *   Expected File Size
     * @param[in,out] operation
     *   TFTP Server Operation
     * @param[in] status
     *   Transfer status.
     **/
    void fileCompleted(
      std::string_view filename,
      const std::filesystem::path &filePath,
      const std::optional< const std::size_t > &expectedFileSize,
      const Arinc615a::Tftp::Servers::WriteOperationPtr &operation,
      Tftp::TransferStatus status );

    //! ASIO I/O context
    boost::asio::io_context &ioContextV;

    //! Abort Signal
    AbortTerminateSignal &abortSignalV;
    //! Terminate Signal
    AbortTerminateSignal &terminateSignalV;
    //! Operation Done Indicator for Synchronisation.
    std::latch done{ 1 };

    //! Program Options Description
    boost::program_options::options_description optionsDescriptionV;

    //! %Target address
    boost::asio::ip::address targetAddressV;
    //! Targets List JSON Path
    std::filesystem::path targetsListPathV;
    //! Target ID
    std::string targetIdV;
    //! DLP Timeout
    std::chrono::seconds dlpTimeoutV;
    //! Use Port Option
    bool portOptionV{ false };
    //! File list, which will be requested.
    std::vector< std::string > requestedFilesV;
    //! User Defined Data.
    std::string userDefinedDataV;
    //! Download Base Directory
    std::filesystem::path downloadBaseDirectoryV;
    //! Check Validity of Downloaded Files Disabled
    bool noVerifyDownloadFilesV{ false };
    //! Don't create download specific directory
    bool noDownloadDirectoryV{ false };

    //! ARINC 615A Data Loader Configuration
    Arinc615a::Arinc615aConfiguration configurationV;

    //! Actual Download Data Directory
    std::filesystem::path downloadDataPathV;
    //! Operation
    Arinc615a::Host::MediaDefinedDownloadOperationPtr operationV;
    //! Download Information
    Arinc615a::Host::DownloadInformation downloadInformationV;
    //! File Transfer Operations
    std::forward_list< Arinc615a::Tftp::Servers::OperationPtr > fileOperationsV;

    //! List Transmitted
    bool listTransmittedV{ false };
};

}

#endif
