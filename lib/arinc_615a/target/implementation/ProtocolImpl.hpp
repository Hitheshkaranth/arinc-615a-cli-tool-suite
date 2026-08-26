// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Target::ProtocolImpl.
 **/

#ifndef ARINC_615A_TARGET_PROTOCOLIMPL_HPP
#define ARINC_615A_TARGET_PROTOCOLIMPL_HPP

#include <arinc_615a/target/Protocol.hpp>
#include <arinc_615a/target/ProtocolConfiguration.hpp>

#include <arinc_615a/tftp/servers/Servers.hpp>

#include <arinc_615a/files/Files.hpp>

#include <tftp/Tftp.hpp>

#include <boost/asio/ip/udp.hpp>
#include <boost/asio/io_context.hpp>

#include <string>
#include <map>

namespace Arinc615a::Target {

class OperationImpl;

/**
 * @brief ARINC 615A %Target Data Loader %Protocol.
 *
 * Implementation of Class Protocol.
 **/
class ProtocolImpl final : public Protocol
{
  public:
    /**
     * @brief Creates the target protocol.
     *
     * @param[in] ioContext
     *   I/O Context used for Communication.
     * @param[in] configuration
     *   Target Protocol Configuration.
     **/
    ProtocolImpl( boost::asio::io_context &ioContext, ProtocolConfiguration configuration );

    //! Destructor
    ~ProtocolImpl() noexcept override;

    /**
     * @copydoc Protocol::start()
     *
     * Starts the TFTP server thread
     **/
    void start() override;

    /**
     * @copydoc Protocol::stop()
     *
     * Signals the TFTP server to stop and waits for the TFTP server thread
     * to terminate.
     **/
    void stop() override;

    //! @copydoc Protocol::errorOperation
    ErrorOperationPtr errorOperation( ErrorOperationConfiguration configuration ) override;

    //! @copydoc Protocol::informationOperation
    InformationOperationPtr informationOperation( InformationOperationConfiguration configuration ) override;

    //! @copydoc Protocol::uploadOperation
    UploadOperationPtr uploadOperation( UploadOperationConfiguration configuration  ) override;

    //! @copydoc Protocol::mediaDefinedDownloadOperation
    MediaDefinedDownloadOperationPtr mediaDefinedDownloadOperation(
      MediaDefinedDownloadOperationConfiguration configuration ) override;

    //! @copydoc Protocol::operatorDefinedDownloadOperation
    OperatorDefinedDownloadOperationPtr operatorDefinedDownloadOperation(
      OperatorDefinedDownloadOperationConfiguration configuration  ) override;

  private:
    //! Internal Operation Pointer
    using OperationImplPtr = std::shared_ptr< OperationImpl >;

    /**
     * Handles the TFTP read request, if the filename is an initialisation protocol file, handle the new request.
     * If the file request does not handle an _Operation Initialisation_ request, the request is forward to an active
     * operation, if any.
     *
     * @param[in] remote
     *   Source of TFTP request.
     * @param[in] requestType
     *   TFTP Request Type.
     * @param[in] filename
     *   Requested filename.
     * @param[in] clientTftpOptions
     *   Received TFTP Options.
     * @param[in] clientArinc615aOptions
     *   Received ARINC 615A Options.
     **/
    void receivedTftpRequest(
      const boost::asio::ip::udp::endpoint &remote,
      Tftp::RequestType requestType,
      std::string_view filename,
      const ::Tftp::Packets::TftpOptions &clientTftpOptions,
      const Tftp::Arinc615aOptions &clientArinc615aOptions );

    /**
     * @brief Handles the reception of the initialisation file RRQ.
     *
     * @param[in] remote
     *   Source of the request.
     * @param[in] filename
     *   Requested filename.
     * @param[in] clientTftpOptions
     *   Received TFTP Options.
     * @param[in] clientArinc615aOptions
     *   Received ARINC 615A Options.
     **/
    void handleInitialisationFile(
      const boost::asio::ip::udp::endpoint &remote,
      const Files::ProtocolFilename &filename,
      const ::Tftp::Packets::TftpOptions &clientTftpOptions,
      const Tftp::Arinc615aOptions &clientArinc615aOptions );

    //! I/O Context
    boost::asio::io_context &ioContext;
    //! Protocol Configuration
    ProtocolConfiguration configurationV;

    //! TFTP Server for Data Loading
    Tftp::Servers::ServerPtr tftpServerV;

    //! Active Operation.
    OperationImplPtr::weak_type operationV;
};

}

#endif
