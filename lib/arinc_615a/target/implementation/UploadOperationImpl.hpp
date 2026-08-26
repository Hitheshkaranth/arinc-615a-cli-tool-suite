// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Target::UploadOperationImpl.
 **/

#ifndef ARINC_615A_TARGET_UPLOADOPERATIONIMPL_HPP
#define ARINC_615A_TARGET_UPLOADOPERATIONIMPL_HPP

#include <arinc_615a/target/Target.hpp>
#include <arinc_615a/target/UploadOperation.hpp>
#include <arinc_615a/target/UploadOperationHandler.hpp>

#include <arinc_615a/target/implementation/OperationImpl.hpp>

#include <arinc_615a/information/UploadStatus.hpp>

#include <tftp/files/Files.hpp>

#include <mutex>

namespace Arinc615a::Target {

/**
 * @brief ARINC 615A %Target %Upload %Operation.
 *
 * Server side implementation of ARINC 615A upload operation.
 **/
class UploadOperationImpl final : public UploadOperation, public OperationImpl
{
  public:
    /**
     * @brief Initialises the ARINC 615A target upload operation instance.
     *
     * @param[in] ioContext
     *   I/O context used for asynchronous operation
     * @param[in] dlConfiguration
     *   Data Loader Configuration.
     * @param[in] protocolVersion
     *   ARINC 615A Version used for protocol files.
     * @param[in] tftpServer
     *   TFTP Server
     * @param[in] operationConfiguration
     *   Operation Configuration.
     **/
    UploadOperationImpl(
      boost::asio::io_context &ioContext,
      Arinc615aConfiguration dlConfiguration,
      Arinc615aVersion protocolVersion,
      Tftp::Servers::ServerPtr tftpServer,
      UploadOperationConfiguration operationConfiguration );

    //! Destructor
    ~UploadOperationImpl() override = default;

    //! @copydoc UploadOperation::start()
    void start(
      boost::asio::ip::udp::endpoint hostAddress,
      ::Tftp::Packets::TftpOptions clientTftpOptions,
      std::optional< uint16_t > port ) override;

    //! @copydoc UploadOperation::accepted
    void accepted() override;

    //! @copydoc UploadOperation::inProgress(bool,int16_t,std::string)
    void inProgress(
      bool immediateTransmission = false,
      int16_t estimatedTime = -1,
      std::string description = {} ) override;

    //! @copydoc UploadOperation::inProgress
    void inProgress(
      bool immediateTransmission,
      int16_t estimatedTime,
      std::string description,
      Information::Ratio listRatio ) override;

    //! @copydoc UploadOperation::loadAccepted
    void loadAccepted( std::string headerFilename, std::string partNumber ) override;

    //! @copydoc UploadOperation::loadInProgress
    void loadInProgress(
      std::string_view headerFilename,
      Information::Ratio ratio,
      std::string description = {} ) override;

    //! @copydoc UploadOperation::loadFinished
    void loadFinished(
      std::string_view headerFilename,
      FinalStatus finalStatus,
      std::string description = {} ) override;

    //! @copydoc UploadOperation::loadsFinished
    void loadsFinished( FinalStatus finalStatus, std::string_view description = {} ) override;

    /**
     * @copydoc UploadOperation::transferFile()
     *
     * Creates a TFTP Client RRQ and transmits the file.
     **/
    Tftp::Clients::OperationPtr transferFile(
      Arinc615aOptionNegotiationHandler optionNegotiationHandler,
      Tftp::Clients::OperationCompletedHandler completionHandler,
      Tftp::ReceiveDataHandlerPtr dataHandler,
      std::string filename,
      std::string partNumber = {},
      Arinc649::CheckValue checkValue = Arinc649::CheckValue::NoCheckValue ) override;

    //! @copydoc UploadOperation::finished()
    void finished( FinalStatus finalStatus, std::string description = {} ) override;

  private:
    /**
     * @copydoc OperationImpl::tftpRequest()
     *
     * If a WRQ of the upload request file is received handle it.
     * All other requests are denied.
     *
     * @sa @ref requestFileRequest
     **/
    void tftpRequest(
      boost::asio::ip::udp::endpoint remote,
      Tftp::RequestType requestType,
      std::string filename,
      ::Tftp::Packets::TftpOptions clientTftpOptions,
      Tftp::Arinc615aOptions clientArinc615aOptions ) override;

    //! @copydoc OperationImpl::status(StatusCode,std::string,int16_t)
    void status( StatusCode code, std::string description = {}, int16_t estimatedTime = 0 ) override;

    //! @copydoc OperationImpl::statusFile
    void statusFile() override;

    /**
     * @brief Completion handler of status transmission operation.
     *
     * @param[in] sentStatus
     *   Sent Status.
     * @param[in] transferStatus
     *   Transfer status.
     **/
    void statusFileCompleted( Information::UploadStatus sentStatus, Tftp::TransferStatus transferStatus );

    /**
     * @brief Handles the Upload request file request.
     *
     * This method is registered to the target protocol TFTP server and is
     * called when a write request of the Load Upload Request File is
     * receipt.
     *
     * @param[in] remote
     *   Remote endpoint.
     * @param[in] clientTftpOptions
     *   Received TFTP Options
     * @param[in] clientArinc615aOptions
     *   Received ARINC 615A Options
     *
     * @sa @ref  requestFileCompleted
     **/
    void requestFileRequest(
      boost::asio::ip::udp::endpoint remote,
      ::Tftp::Packets::TftpOptions clientTftpOptions,
      Tftp::Arinc615aOptions clientArinc615aOptions );

    /**
     * @brief Handler when Upload Request File has been received completely.
     *
     * @param[in] rawFile
     *   Raw file data.
     * @param[in] transferStatus
     *   TFTP transfer status.
     **/
    void requestFileCompleted( ::Tftp::Files::MemoryFilePtr rawFile, ::Tftp::TransferStatus transferStatus );

    /**
     * @brief File Transfer Option Negotiation Handler.
     *
     * Encapsulates the ARINC 615A TFTP Client Option negotiation.
     * Rejects port option.
     * Calls handler for checksum option.
     *
     * @param[in] optionNegotiationHandler
     *   ARINC 615A Option Negotiation Handler
     * @param[in] options
     *   Received ARINC 615A Options
     *
     * @return If checksum option is valid.
     **/
    bool fileOptionNegotiation(
      Arinc615aOptionNegotiationHandler optionNegotiationHandler,
      const Tftp::Arinc615aOptions &options );

    //! Operation Handler.
    UploadOperationHandler &handler;

    //! Status Mutex
    std::mutex statusMutex;
    //! Status information, which is periodically transmitted.
    Information::UploadStatus statusV;

    //! Status Operation Mutex
    std::mutex statusOperationMutex;
    //! Status TFTP Client Operation.
    Tftp::Clients::WriteOperationPtr statusOperation;

    //! Request File TFTP Server Operation.
    Tftp::Servers::WriteOperationPtr requestFileOperation;
};

}

#endif
