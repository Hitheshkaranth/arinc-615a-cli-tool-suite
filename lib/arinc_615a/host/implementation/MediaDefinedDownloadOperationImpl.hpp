// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Host::MediaDefinedDownloadOperationImpl.
 **/

#ifndef ARINC_615A_HOST_MEDIADEFINEDDOWNLOADOPERATIONIMPL_HPP
#define ARINC_615A_HOST_MEDIADEFINEDDOWNLOADOPERATIONIMPL_HPP

#include <arinc_615a/host/MediaDefinedDownloadOperation.hpp>
#include <arinc_615a/host/implementation/DownloadOperationImpl.hpp>

namespace Arinc615a::Host {

/**
 * @brief ARINC 615A Host Media Defined Download Operation.
 **/
class MediaDefinedDownloadOperationImpl final : public MediaDefinedDownloadOperation, public DownloadOperationImpl
{
  public:
    /**
     * @brief Initialises the ARINC 615A host media defined download operation.
     *
     * @param[in] ioContext
     *   I/O context used for asynchronous operations.
     * @param[in] operationConfiguration
     *   Operation Configuration.
     **/
    MediaDefinedDownloadOperationImpl(
      boost::asio::io_context &ioContext,
      MediaDefinedDownloadOperationConfiguration operationConfiguration );

    //! Destructor
    ~MediaDefinedDownloadOperationImpl() override;

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

    //! @copydoc MediaDefinedDownloadOperation::request
    void request( Information::DownloadFiles files, Helper::RawData userDefinedData = {} ) override;

  private:
    /**
     * @copydoc OperationImpl::tftpRequest()
     *
     * TFTP Server requests are analysed and decomposed:
     *  1. a write request with a filename, which is decoded as status file is redirected to the operation
     *     @ref statusFileRequest().
     *  2. a write request is directed to the handler interface, which must provide the requested file.
     *  3. All other requests are rejected.
     **/
    void tftpRequest(
      boost::asio::ip::udp::endpoint remote,
      Tftp::RequestType requestType,
      std::string filename,
      ::Tftp::Packets::TftpOptions clientTftpOptions,
      Tftp::Arinc615aOptions clientArinc615aOptions ) override;

    /**
     * @brief Option Negotiation Handler for the download request file transmission.
     *
     * Negotiates the additional options received from the Target HW TFTP Server.
     *
     * @param[in] serverOptions
     *   Received Additional TFTP Options.
     *
     * @return If Option Negotiation was successful.
     **/
    bool requestOptionsNegotiation( const Tftp::Arinc615aOptions &serverOptions );

    /**
     * @brief Completion handler for download request file transmission operation.
     *
     * @param[in] status
     *   TFTP transfer status.
     *
     * @sa @ref request()
     **/
    void requestCompleted( Tftp::TransferStatus status );

    //! Handler.
    MediaDefinedDownloadOperationHandler &handlerV;
    //! Download Request Sent indicator.
    bool requestSentV{ false };
    //! Download Request Operation.
    Tftp::Clients::WriteOperationPtr requestOperationV;
};

}

#endif
