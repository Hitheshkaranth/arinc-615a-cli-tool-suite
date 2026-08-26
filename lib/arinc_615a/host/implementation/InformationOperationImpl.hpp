// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Host::InformationOperationImpl.
 **/

#ifndef ARINC_615A_HOST_INFORMATIONOPERATIONIMPL_HPP
#define ARINC_615A_HOST_INFORMATIONOPERATIONIMPL_HPP

#include <arinc_615a/host/Host.hpp>
#include <arinc_615a/host/InformationOperation.hpp>
#include <arinc_615a/host/implementation/OperationImpl.hpp>

#include <arinc_649/Arinc649.hpp>

#include <tftp/servers/Servers.hpp>

#include <forward_list>

namespace Arinc615a::Host {

/**
 * @brief ARINC 615A Host Information Operation.
 *
 * - TFTP Read Request to Target with filename "<THW ID>_<Position>.LCI"
 * - start TFTP server to receive "<THW ID>_<Position>.LCS" and
 *   "<THW ID>_<Position>.LCL" files.
 **/
class InformationOperationImpl final : public InformationOperation, public OperationImpl
{
  public:
    /**
     * @brief Create an Instance of this Class.
     *
     * @param[in] ioContext
     *   I/O context used for asynchronous operations.
     * @param[in] operationConfiguration
     *   Operation Configuration.
     **/
    InformationOperationImpl(
      boost::asio::io_context &ioContext,
      InformationOperationConfiguration operationConfiguration );

    //! Destructor
    ~InformationOperationImpl() override;

    //! @copydoc Operation::start
    void start() override;

    //! @copydoc Operation::abort
    void abort( AbortReason reason ) override;

    //! @copydoc Operation::terminate
    void terminate( AbortReason reason ) override;

  private:
    /**
     * @copydoc OperationImpl::tftpRequest()
     *
     * TFTP Server requests are analysed and decomposed:
     * - a write request with a filename, which is decoded as status file is redirected to the operation
     *   @ref statusFileRequest()
     * - a write request with a filename, which is decoded as list file is redirected to the operation
     *   @ref listFileRequest()
     * - All other requests are rejected.
     **/
    void tftpRequest(
      boost::asio::ip::udp::endpoint remote,
      Tftp::RequestType requestType,
      std::string filename,
      ::Tftp::Packets::TftpOptions clientTftpOptions,
      Tftp::Arinc615aOptions clientArinc615aOptions ) override;

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

    /**
     * @brief Handles the TFTP Server write request of the list file.
     *
     * Creates a TFTP server operation to receive the file.
     * On completion calls handleListFile().
     *
     * @param[in] remote
     *   Source of request.
     * @param[in] clientTftpOptions
     *   Received TFTP options.
     * @param[in] clientArinc615aOptions
     *   Received ARINC 615A Options.
     **/
    void listFileRequest(
      boost::asio::ip::udp::endpoint remote,
      ::Tftp::Packets::TftpOptions clientTftpOptions,
      Tftp::Arinc615aOptions clientArinc615aOptions );

    /**
     * @brief Handles the received list file.
     *
     * Decodes the raw file and calls the handler.
     * Finishes the operation.
     *
     * @sa @ref InformationOperationHandler::targetInformation
     *
     * @param[in] rawListFile
     *   Raw list file.
     * @param[in] checkValue
     *   Check Value, when advertised on Request.
     * @param[in] status
     *   TFTP transfer status.
     **/
    void listFileCompleted(
      ::Tftp::Files::MemoryFilePtr rawListFile,
      Arinc649::CheckValue checkValue,
      ::Tftp::TransferStatus status );

    //! Operation Handler
    InformationOperationHandler &handlerV;
    //! This flag is set, when a status message is received from a target
    bool waitForFinalStatusV{ false };
    //! Status File Operation.
    std::forward_list< Tftp::Servers::WriteOperationPtr > statusFileOperationsV;
    //! List File Operation.
    Tftp::Servers::WriteOperationPtr listFileOperationV;
};

}

#endif
