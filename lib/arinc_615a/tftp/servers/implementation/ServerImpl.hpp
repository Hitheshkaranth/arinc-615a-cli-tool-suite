// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Tftp::Servers::ServerImpl.
 **/

#ifndef ARINC_615A_TFTP_SERVERS_SERVERIMPL_HPP
#define ARINC_615A_TFTP_SERVERS_SERVERIMPL_HPP

#include <arinc_615a/tftp/servers/Servers.hpp>
#include <arinc_615a/tftp/servers/Server.hpp>

#include <tftp/servers/Servers.hpp>

namespace Arinc615a::Tftp::Servers {

/**
 * @brief ARINC 615A TFTP %Server.
 **/
class ServerImpl final : public Server
{
  public:
    /**
     * @brief Creates an Instance of the TFTP %Server.
     *
     * @param[in] ioContext
     *   I/O context used for communication.
     **/
    ServerImpl( boost::asio::io_context &ioContext );

    //! Destructor
    ~ServerImpl() override;

    //! @copydoc Server::requestHandler()
    Server& requestHandler( ReceivedTftpRequestHandler handler ) override;

    //! @copydoc Server::serverAddress()
    Server &serverAddress( boost::asio::ip::udp::endpoint serverAddress ) override;

    //! @copydoc Server::localEndpoint()
    [[nodiscard]] boost::asio::ip::udp::endpoint localEndpoint() const override;

    //! @copydoc Server::start()
    void start() override;

    //! @copydoc Server::stop()
    void stop() override;

    //! @copydoc Server::readOperation()
    [[nodiscard]] ReadOperationPtr readOperation() override;

    //! @copydoc Server::writeOperation()
    [[nodiscard]] WriteOperationPtr writeOperation() override;

    //! @copydoc Server::errorOperation()
    void errorOperation(
      const boost::asio::ip::udp::endpoint &remote,
      ::Tftp::Packets::ErrorCode errorCode,
      std::string errorMessage ) override;

    //! @copydoc Server::waitOperation()
    void waitOperation( const boost::asio::ip::udp::endpoint &remote, std::chrono::seconds waitTime ) override;

    //! @copydoc Server::abortOperation()
    void abortOperation( const boost::asio::ip::udp::endpoint &remote, StatusCode status ) override;

  private:
    /**
     * @brief TFTP Request Handler.
     *
     * Will check the Transfer Type, decode the ARINC 615A Options and calls the
     * further request handler.
     *
     * @param[in] remote
     *   Source of TFTP Request
     * @param[in] requestType
     *   TFTP Request Type (RRQ/WRQ)
     * @param[in] filename
     *   Requested filename.
     * @param[in] mode
     *   TFTP Transfer Mode
     * @param[in] clientOptions
     *   TFTP Options
     * @param[in] additionalClientOptions
     *   Additional Options
     */
    void tftpRequestHandler(
      const boost::asio::ip::udp::endpoint &remote,
      RequestType requestType,
      std::string_view filename,
      ::Tftp::Packets::TransferMode mode,
      const ::Tftp::Packets::TftpOptions &clientOptions,
      const ::Tftp::Packets::Options &additionalClientOptions );

    //! Underlying TFTP %Server
    ::Tftp::Servers::ServerPtr baseTftpServerV;
    //! TFTP Request Handler
    ReceivedTftpRequestHandler tftpRequestHandlerV;
};

}

#endif
