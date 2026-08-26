// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Host::UploadOperationImpl.
 **/

#ifndef ARINC_615A_HOST_UPLOADOPERATIONIMPL_HPP
#define ARINC_615A_HOST_UPLOADOPERATIONIMPL_HPP

#include <arinc_615a/host/Host.hpp>
#include <arinc_615a/host/UploadOperation.hpp>
#include <arinc_615a/host/implementation/OperationImpl.hpp>

#include <arinc_615a/files/Files.hpp>

#include <forward_list>

namespace Arinc615a::Host {

/**
 * @brief ARINC 615A Host Upload Operation.
 **/
class UploadOperationImpl final : public UploadOperation, public OperationImpl
{
  public:
    /**
     * @brief Initialises the ARINC 615A host upload operation.
     *
     * @param[in] ioContext
     *   I/O context used for asynchronous operations.
     * @param[in] operationConfiguration
     *   Operation Configuration.
     **/
    UploadOperationImpl(
      boost::asio::io_context &ioContext,
      UploadOperationConfiguration operationConfiguration );

    //! Destructor
    ~UploadOperationImpl() override;

    //! @copydoc Operation::start
    void start() override;

    //! @copydoc Operation::abort
    void abort( AbortReason reason ) override;

    //! @copydoc Operation::terminate
    void terminate( AbortReason reason ) override;

    //! @copydoc UploadOperation::loadList()
    void loadList( Information::UploadLoads loads ) override;

    //! @copydoc UploadOperation::fileTransfer()
    Tftp::Servers::ReadOperationPtr fileTransfer(
      Tftp::TransmitDataHandlerPtr dataHandler,
      boost::asio::ip::udp::endpoint remote,
      ::Tftp::Packets::TftpOptions clientTftpOptions,
      std::string partNumber,
      Arinc649::CheckValue checkValue ) override;

    //! @copydoc UploadOperation::waitOperation()
    void waitOperation(
      const boost::asio::ip::udp::endpoint &remote,
      std::chrono::seconds waitTime ) override;

    //! @copydoc UploadOperation::errorOperation()
    void errorOperation(
      const boost::asio::ip::udp::endpoint &remote,
      ::Tftp::Packets::ErrorCode errorCode,
      std::string errorMessage ) override;

  private:
    /**
     * @copydoc OperationImpl::tftpRequest()
     *
     * TFTP Server requests are analysed and decomposed:
     *  1. a read request is directed to the handler interface, which must provide the requested files (Load Headers and
     *     Load Files).
     *  2. a write request with a filename, which is decoded as an upload operation status file is redirected to the
     *     operation @ref statusFileRequest().
     *  3. All other requests are rejected.
     **/
    void tftpRequest(
      boost::asio::ip::udp::endpoint remote,
      Tftp::RequestType requestType,
      std::string filename,
      ::Tftp::Packets::TftpOptions clientTftpOptions,
      Tftp::Arinc615aOptions clientArinc615aOptions ) override;

    /**
     * @name Load List Handling
     *
     * @{
     **/

    /**
     * @brief Performs the Client Side Option Negotiation for the Load List.
     *
     * @param[in] serverOptions
     *   Received Additional TFTP Options.
     *
     * @return If Option Negotiation was successful.
     **/
    bool loadListOptionNegotiation( const Tftp::Arinc615aOptions &serverOptions );

    /**
     * @brief Completion handler for load list file transmission operation.
     *
     * Within this method, the transfer status of the load list file transmission is checked.
     *
     * The operation continues to wait for the reception of status files or media set (load upload header, data, or
     * support) files.
     *
     * @param[in] status
     *   TFTP transfer status.
     *
     * @sa loadList
     **/
    void loadListCompleted( Tftp::TransferStatus status );

    /** @} **/

    /**
     * @name Status Reception
     *
     * @{
     **/

    /**
     * @brief Handles the TFTP write request of the status file.
     *
     * Creates a TFTP server operation to receive the status file.
     *
     * @param[in] remote
     *   Source of request.
     * @param[in] clientTftpOptions
     *   Received TFTP options.
     * @param[in] clientArinc615aOptions
     *   Received ARINC 615A Options.
     **/
    void statusFileRequest(
      boost::asio::ip::udp::endpoint remote,
      ::Tftp::Packets::TftpOptions clientTftpOptions,
      Tftp::Arinc615aOptions clientArinc615aOptions );

    /**
     * @brief Handles the received status file.
     *
     * Decodes the raw file.
     * Calls the @ref UploadOperationHandler::status handler for evaluating the status.
     *
     * @param[in] rawStatusFile
     *   Raw Status File.
     * @param[in,out] operation
     *   TFTP Server Operation
     * @param[in] status
     *   TFTP Transfer Status.
     **/
    void statusFileCompleted(
      ::Tftp::Files::MemoryFilePtr rawStatusFile,
      const Tftp::Servers::WriteOperationPtr &operation,
      ::Tftp::TransferStatus status );

    /** @}**/

    //! Operation Handler
    UploadOperationHandler &handlerV;
    //! Load List Sent indicator.
    bool loadListSentV{ false };
    //! Load List Operation
    Tftp::Clients::WriteOperationPtr loadListOperationV;
    //! Status File Operation.
    std::forward_list< Tftp::Servers::WriteOperationPtr > statusFileOperationsV;
};

}

#endif
