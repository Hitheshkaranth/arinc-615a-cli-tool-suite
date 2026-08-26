// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Host::OperatorDefinedDownloadOperationImpl.
 **/

#ifndef ARINC_615A_HOST_OPERATORDEFINEDDOWNLOADOPERATIONIMPL_HPP
#define ARINC_615A_HOST_OPERATORDEFINEDDOWNLOADOPERATIONIMPL_HPP

#include <arinc_615a/host/Host.hpp>
#include <arinc_615a/host/OperatorDefinedDownloadOperation.hpp>
#include <arinc_615a/host/implementation/DownloadOperationImpl.hpp>

#include <arinc_615a/tftp/servers/WriteOperation.hpp>

namespace Arinc615a::Host {

/**
 * @brief ARINC 615A Host Operator Defined Download Operation.
 **/
class OperatorDefinedDownloadOperationImpl final :
  public OperatorDefinedDownloadOperation,
  public DownloadOperationImpl
{
  public:
    /**
     * @brief Initialises the ARINC 615A host operator defined download operation.
     *
     * @param[in] ioContext
     *   I/O context used for asynchronous operations.
     * @param[in] operationConfiguration
     *   Operation Configuration.
     **/
    OperatorDefinedDownloadOperationImpl(
      boost::asio::io_context &ioContext,
      OperatorDefinedDownloadOperationConfiguration operationConfiguration );

    //! Destructor
    ~OperatorDefinedDownloadOperationImpl() override;

    //! @copydoc Operation::start
    void start() override;

    //! @copydoc Operation::abort
    void abort( AbortReason reason ) override;

    //! @copydoc Operation::terminate
    void terminate( AbortReason reason ) override;

    //! @copydoc DownloadOperation::fileTransfer()
    Tftp::Servers::WriteOperationPtr fileTransfer(
      Tftp::ReceiveDataHandlerPtr dataHandler,
      boost::asio::ip::udp::endpoint remote,
      ::Tftp::Packets::TftpOptions clientTftpOptions,
      std::string partNumber,
      Arinc649::CheckValue checkValue ) override;

    //! @copydoc DownloadOperation::waitOperation()
    void waitOperation(
      const boost::asio::ip::udp::endpoint &remote,
      std::chrono::seconds waitTime ) override;

    //! @copydoc DownloadOperation::errorOperation()
    void errorOperation(
      const boost::asio::ip::udp::endpoint &remote,
      ::Tftp::Packets::ErrorCode errorCode,
      std::string errorMessage ) override;

    //! @copydoc OperatorDefinedDownloadOperation::answer
    void answer( Information::DownloadFiles files ) override;

  private:
    /**
     * @copydoc OperationImpl::tftpRequest()
     *
     * TFTP Server requests are analysed and decomposed:
     *  1. a write request with a filename, which is decoded as status file is redirected to the operation
     *     @ref statusFileRequest().
     *  2. a write request with a filename, which is decoded as download list file is redirected to the operation
     *     @ref listFileRequest()
     *  3. a write request is directed to the handler interface, which must provide the requested file.
     *  4. All other requests are rejected.
     **/
    void tftpRequest(
      boost::asio::ip::udp::endpoint remote,
      Tftp::RequestType requestType,
      std::string filename,
      ::Tftp::Packets::TftpOptions clientTftpOptions,
      Tftp::Arinc615aOptions clientArinc615aOptions ) override;

    /**
     * @brief Handles the write request of the list file.
     *
     * @param[in] remote
     *   Target Address of the Request.
     * @param[in] clientTftpOptions
     *   Received TFTP options.
     * @param[in] clientArinc615aOptions
     *   Received ARINC 615A options.
     **/
    void listFileRequest(
      boost::asio::ip::udp::endpoint remote,
      ::Tftp::Packets::TftpOptions clientTftpOptions,
      Tftp::Arinc615aOptions clientArinc615aOptions );

    /**
     * @brief Handler called when the list file transfer has been completed.
     *
     * @param[in] rawListFile
     *   Received List File as Raw Data.
     * @param[in] status
     *   Receive Status.
     **/
    void listFileCompleted( ::Tftp::Files::MemoryFilePtr rawListFile, ::Tftp::TransferStatus status );

    /**
     * @brief Option Negotiation Handler for the download answer file transmission.
     *
     * Negotiates the additional options received from the Target HW TFTP Server.
     *
     * @param[in] serverOptions
     *   Received Additional TFTP Options.
     *
     * @return If Option Negotiation was successful.
     **/
    bool answerOptionsNegotiation( const Tftp::Arinc615aOptions &serverOptions );

    /**
     * @brief Completion handler for download answer file transmission operation.
     *
     * @param[in] status
     *   TFTP Transfer status.
     *
     * @sa @ref answer()
     **/
    void answerCompleted( Tftp::TransferStatus status );

    //! Operation Handler.
    OperatorDefinedDownloadOperationHandler &handlerV;
    //! Download Answer Sent indicator.
    bool answerSentV{ false };
    //! Download Answer Operation.
    Tftp::Clients::WriteOperationPtr answerOperationV;
    //! Download List File Operation.
    Tftp::Servers::WriteOperationPtr listFileOperationV;
};

}

#endif
