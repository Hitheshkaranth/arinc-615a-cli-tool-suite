// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Target::OperatorDefinedDownloadOperationImpl..
 **/

#ifndef ARINC_615A_TARGET_TARGETOPERATORDEFINEDDOWNLOADOPERATIONIMPL_HPP
#define ARINC_615A_TARGET_TARGETOPERATORDEFINEDDOWNLOADOPERATIONIMPL_HPP

#include <arinc_615a/target/Target.hpp>
#include <arinc_615a/target/OperatorDefinedDownloadOperation.hpp>
#include <arinc_615a/target/OperatorDefinedDownloadOperationHandler.hpp>

#include <arinc_615a/target/implementation/OperationImpl.hpp>

#include <arinc_615a/information/DownloadStatus.hpp>

#include <tftp/files/Files.hpp>

#include <mutex>

namespace Arinc615a::Target {

/**
 * @brief ARINC 615A %Target Operator Defined Download %Operation.
 *
 * Implementation of class OperatorDefinedDownloadOperation.
 **/
class OperatorDefinedDownloadOperationImpl final : public OperatorDefinedDownloadOperation, public OperationImpl
{
  public:
    /**
     * @brief Initialises the ARINC 615A target operator defined download operation instance.
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
    OperatorDefinedDownloadOperationImpl(
      boost::asio::io_context &ioContext,
      Arinc615aConfiguration dlConfiguration,
      Arinc615aVersion protocolVersion,
      Tftp::Servers::ServerPtr tftpServer,
      OperatorDefinedDownloadOperationConfiguration operationConfiguration );

    //! Destructor
    ~OperatorDefinedDownloadOperationImpl() override = default;

    //! @copydoc OperatorDefinedDownloadOperation::start()
    void start(
      boost::asio::ip::udp::endpoint hostAddress,
      ::Tftp::Packets::TftpOptions clientTftpOptions,
      std::optional< uint16_t > port ) override;

    //! @copydoc OperatorDefinedDownloadOperation::accepted
    void accepted() override;

    //! @copydoc OperatorDefinedDownloadOperation::inProgress(bool,int16_t,std::string)
    void inProgress(
      bool immediateTransmission = false,
      int16_t estimatedTime = -1,
      std::string description = {} ) override;

    //! @copydoc OperatorDefinedDownloadOperation::inProgress
    void inProgress(
      bool immediateTransmission,
      int16_t estimatedTime,
      std::string description,
      Information::Ratio listRatio ) override;

    //! @copydoc OperatorDefinedDownloadOperation::fileAccepted
    void fileAccepted( std::string filename ) override;

    //! @copydoc OperatorDefinedDownloadOperation::fileInProgress
    void fileInProgress( std::string_view filename, std::string description = {} ) override;

    //! @copydoc OperatorDefinedDownloadOperation::fileFinished
    void fileFinished(
      std::string_view filename,
      FinalStatus finalStatus,
      std::string description = {} ) override;

    //! @copydoc OperatorDefinedDownloadOperation::filesFinished
    void filesFinished( FinalStatus finalStatus, std::string_view description = {} ) override;

    /**
     * @copydoc OperatorDefinedDownloadOperation::transferFile()
     *
     * Creates a TFTP Client WRQ and transmits the file.
     **/
    Tftp::Clients::OperationPtr transferFile(
      Arinc615aOptionNegotiationHandler optionNegotiationHandler,
      Tftp::Clients::OperationCompletedHandler completionHandler,
      Tftp::TransmitDataHandlerPtr dataHandler,
      std::string filename,
      std::string partNumber = {},
      Arinc649::CheckValue checkValue = Arinc649::CheckValue::NoCheckValue ) override;

    //! @copydoc OperatorDefinedDownloadOperation::downloadingList
    void downloadingList( Information::DownloadFilesInformation files ) override;

    //! @copydoc OperatorDefinedDownloadOperation::finished()
    void finished( FinalStatus finalStatus, std::string description = {} ) override;

  private:
    /**
     * @copydoc OperationImpl::tftpRequest()
     *
     * A write request of the _Download Answer_ file is accepted.
     * All other requests are denied.
     *
     * @sa @ref answerFileRequest
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
    void statusFileCompleted( Information::DownloadStatus sentStatus, Tftp::TransferStatus transferStatus );

    /**
     * @brief Handles the reception of the download answer file.
     *
     * @param[in] remote
     *   Remote endpoint.
     * @param[in] clientTftpOptions
     *   Received TFTP Options
     * @param[in] clientArinc615aOptions
     *   Received ARINC 615A Options
     *
     * @sa @ref answerFileCompleted
     **/
    void answerFileRequest(
      boost::asio::ip::udp::endpoint remote,
      ::Tftp::Packets::TftpOptions clientTftpOptions,
      Tftp::Arinc615aOptions clientArinc615aOptions );

    /**
     * @brief Handles completion of download answer file reception.
     *
     * @param[in] rawFile
     *   Raw file data.
     * @param[in] transferStatus
     *   TFTP transfer status.
     **/
    void answerFileCompleted( ::Tftp::Files::MemoryFilePtr rawFile, ::Tftp::TransferStatus transferStatus );

    /**
     * @brief Handles completion of downloading list file transmission.
     *
     * @param[in] transferStatus
     *   Transfer status.
     **/
    void downloadingListCompleted( Tftp::TransferStatus transferStatus );

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
    OperatorDefinedDownloadOperationHandler &handler;

    //! Status Mutex
    std::mutex statusMutex;
    //! Status information, which is periodically transmitted.
    Information::DownloadStatus statusV;

    //! Status Operation Mutex
    std::mutex statusOperationMutex;
    //! Status TFTP Client Operation.
    Tftp::Clients::WriteOperationPtr statusOperation;

    //! Downloading List TFTP Client Operation.
    Tftp::Clients::WriteOperationPtr downloadingListOperation;
    //! Answer File TFTP Server Operation.
    Tftp::Servers::WriteOperationPtr answerFileOperation;
};

}

#endif
