// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Host::DownloadOperationImpl.
 **/

#ifndef ARINC_615A_HOST_DOWNLOADOPERATIONIMPL_HPP
#define ARINC_615A_HOST_DOWNLOADOPERATIONIMPL_HPP

#include <arinc_615a/host/Host.hpp>
#include <arinc_615a/host/DownloadOperation.hpp>
#include <arinc_615a/host/DownloadOperationHandler.hpp>
#include <arinc_615a/host/implementation/OperationImpl.hpp>

#include <forward_list>

namespace Arinc615a::Host {

/**
 * @brief ARINC 615A Host Download Operation.
 *
 * This class is specialised for Media and Operator defined download operations.
 *
 * Common implementations are:
 * - the status file handling and
 * - the file request handling.
 * - denial of read requests
 **/
class DownloadOperationImpl : public OperationImpl
{
  protected:
    /**
     * @brief Initialises the ARINC 615A host download operation.
     *
     * @param[in] ioContext
     *   I/O context used for asynchronous operations.
     * @param[in] configuration
     *   Data Loader Configuration.
     * @param[in] handler
     *   Operation handler.
     * @param[in] targetAddress
     *   Target Address.
     * @param[in] targetId
     *   Target ID.
     * @param[in] dlpTimeout
     *   Timeout for DL-Transfers (e.g. Status Information)
     * @param[in] portOption
     *   Use dynamic port and port option.
     **/
    DownloadOperationImpl(
      boost::asio::io_context &ioContext,
      Arinc615aConfiguration configuration,
      DownloadOperationHandler &handler,
      boost::asio::ip::address targetAddress,
      TargetId targetId,
      std::chrono::seconds dlpTimeout,
      bool portOption );

    //! @copydoc DownloadOperation::fileTransfer()
    Tftp::Servers::WriteOperationPtr doFileTransfer(
      Tftp::ReceiveDataHandlerPtr dataHandler,
      boost::asio::ip::udp::endpoint remote,
      ::Tftp::Packets::TftpOptions clientTftpOptions,
      std::string partNumber,
      Arinc649::CheckValue checkValue );

    //! @copydoc DownloadOperation::waitOperation()
    void doWaitOperation( const boost::asio::ip::udp::endpoint &remote, std::chrono::seconds waitTime );

    //! @copydoc DownloadOperation::errorOperation()
    void doErrorOperation(
      const boost::asio::ip::udp::endpoint &remote,
      ::Tftp::Packets::ErrorCode errorCode,
      std::string errorMessage );

    /**
     * @brief Handles the write request of the status file.
     *
     * Creates a TFTP server operation to receive the file.
     * On completion calls handleStatusFile().
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

  private:
    /**
     * @brief Handles the received status file.
     *
     * Decodes the raw file.
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

    //! Operation Handler
    DownloadOperationHandler &handler;
    //! Status File Operation
    std::forward_list< Tftp::Servers::WriteOperationPtr > statusFileOperationsV{};
};

}

#endif
