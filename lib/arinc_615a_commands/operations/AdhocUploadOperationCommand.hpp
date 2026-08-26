// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615aCommands::Operations::AdhocUploadOperationCommand.
 **/

#ifndef ARINC_615A_COMMANDS_OPERATIONS_ADHOCUPLOADOPERATIONCOMMAND_HPP
#define ARINC_615A_COMMANDS_OPERATIONS_ADHOCUPLOADOPERATIONCOMMAND_HPP

#include <arinc_615a_commands/operations/Operations.hpp>

#include <arinc_615a/information/Information.hpp>

#include <arinc_615a/host/UploadOperationHandler.hpp>

#include <arinc_615a/tftp/servers/Servers.hpp>

#include <arinc_615a/Arinc615aConfiguration.hpp>

#include <arinc_665/media/Media.hpp>

#include <arinc_665/utils/Utils.hpp>

#include <commands/Commands.hpp>

#include <forward_list>
#include <latch>
#include <string>
#include <vector>

namespace Arinc615aCommands::Operations {

/**
 * @brief ARINC 615A Adhoc Upload Operation Command.
 *
 * This command is used to perform an ARINC 615A Upload Operation without accessing a Media Set Manager.
 *
 * @sa @ref Arinc615a::Host::UploadOperation
 **/
class ARINC_615A_COMMANDS_EXPORT AdhocUploadOperationCommand final : private Arinc615a::Host::UploadOperationHandler
{
  public:
    /**
     * @brief Constructs the ARINC 615A Upload Operation.
     *
     * @param[in] ioContext
     *   I/O Context used of operations.
     * @param[in] abortSignal
     *   Signal raised when application is aborted.
     * @param[in] terminateSignal
     *   Signal raised when application is terminated.
     **/
    AdhocUploadOperationCommand(
      boost::asio::io_context &ioContext,
      AbortTerminateSignal &abortSignal,
      AbortTerminateSignal &terminateSignal );

    /**
     * @brief Executes the Upload Operation.
     *
     * @param[in] parameters
     *   Command parameters.
     **/
    void execute( const Commands::Parameters &parameters );

    //! Prints command help.
    void help() const;

  private:
    /**
     * @brief Return the Path of the given File.
     *
     * @param[in] file
     *   File to get the path for.
     *
     * @return Path of the given file.
     * @retval {}
     *   When the file is null or invalid.
     **/
    std::filesystem::path filePath( const Arinc665::Media::ConstFilePtr &file );

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

    //! @copydoc Arinc615a::Host::UploadOperationHandler::initialisationDeferred
    void initialisationDeferred( std::chrono::seconds waitTime ) override;

    //! @copydoc Arinc615a::Host::UploadOperationHandler::initialisationResponse
    void initialisationResponse( const Arinc615a::Information::InitializationResponse &response ) override;

    //! @copydoc Arinc615a::Host::UploadOperationHandler::finished
    void finished( Arinc615a::StatusCode code, std::string_view description ) override;

    //! @copydoc Arinc615a::Host::UploadOperationHandler::status
    void status( const Arinc615a::Information::UploadStatus &status ) override;

    //! @copydoc Arinc615a::Host::UploadOperationHandler::fileRequest
    void fileRequest(
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
    //! Requested Load Headers
    std::vector< std::string > loadHeadersV;
    //! Media Source Directories.
    std::vector< std::filesystem::path > mediaSourceDirectoriesV;
    //! Check File Integrity.
    bool checkFileIntegrityV;

    //! ARINC 615A Data Loader Configuration
    Arinc615a::Arinc615aConfiguration configurationV;
    //! ARINC 665 Media Paths (used for file location determination)
    Arinc665::Utils::MediaPaths mediaPathsV;
    //! ARINC 665 Media Set
    Arinc665::Media::ConstMediaSetPtr mediaSetV;
    //! ARINC 649 Check Values for file lookup
    Arinc665::Media::CheckValues checkValuesV;
    //! ARINC 665 Loads
    Arinc665::Media::ConstLoads loadsV;

    //! Operation
    Arinc615a::Host::UploadOperationPtr operationV;
    //! File Transfer Operations
    std::forward_list< Arinc615a::Tftp::Servers::ReadOperationPtr > fileOperationsV;

    //! Load List Transmitted
    bool loadListTransmittedV{ false };
};

}

#endif
