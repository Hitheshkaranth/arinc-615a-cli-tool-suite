// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Tftp::Servers::Server.
 **/

#ifndef ARINC_615A_TFTP_SERVERS_SERVER_HPP
#define ARINC_615A_TFTP_SERVERS_SERVER_HPP

#include <arinc_615a/tftp/servers/Servers.hpp>

#include <tftp/servers/Servers.hpp>

#include <tftp/TftpConfiguration.hpp>

#include <chrono>
#include <string>

namespace Arinc615a::Tftp::Servers {

/**
 * @brief ARINC 615A TFTP %Server.
 *
 * Waits on the specified port for a valid TFTP request and calls the appropriate call-back, which has to handle the
 * request.
 *
 * If a not expected packet or invalid packet is received, an error is sent back to the sender.
 *
 * Valid requests are TFTP Read Request (RRQ) and TFTP Write Request (WRQ)
 **/
class ARINC_615A_EXPORT Server
{
  public:
    /**
     * @brief Creates a TFTP Server Instance.
     *
     * @param[in] ioContext
     *   I/O context used for communication.
     *
     * @return TFTP Server Instance.
     **/
    [[nodiscard]] static ServerPtr instance( boost::asio::io_context &ioContext );

    //! Destructor
    virtual ~Server() = default;

    /**
     * @name TFTP Server Configuration
     * @{
     **/

    /**
     * @brief Set TFTP Request Received Handler
     *
     * @param[in] handler
     *   TFTP Request Handler
     *
     * @return *this for chaining.
     **/
    virtual Server& requestHandler( ReceivedTftpRequestHandler handler ) = 0;

    /**
     * @brief Set the Address where the TFTP server should listen on.
     *
     * @param[in] serverAddress
     *   Address where the TFTP server should listen on.
     *
     * @return *this for chaining.
     **/
    virtual Server& serverAddress( boost::asio::ip::udp::endpoint serverAddress ) = 0;

    /** @} **/

    /**
     * @brief Returns the effective local endpoint.
     *
     * Is used to determine the local endpoint when an automatic local endpoint is selected.
     *
     * @note
     * The return value is valid after calling @ref start(), when the port is bound, actually.
     *
     * @return Local endpoint.
     **/
    [[nodiscard]] virtual boost::asio::ip::udp::endpoint localEndpoint() const = 0;

    /**
     * @brief Starts the TFTP Server.
     *
     * This routine starts the server receive operation.
     * This routine returns immediately.
     **/
    virtual void start() = 0;

    /**
     * @brief Stops the TFTP Server.
     *
     * This operation does not stop TFTP Server transfers.
     * The caller must handle this.
     **/
    virtual void stop() = 0;

    /**
     * @brief Creates a TFTP %Server %Operation (TFTP RRQ), which reads data from disk and sends them to a TFTP Client.
     *
     * Data is obtained from @p dataHandler and transmitted to TFTP Client.
     *
     * @return TFTP server read operation.
     **/
    [[nodiscard]] virtual ReadOperationPtr readOperation() = 0;

    /**
     * @brief Creates a TFTP Server Operation (TFTP WRQ), which receives data from a TFTP Client and weites them to
     *   disk.
     *
     * Data is received from the client and written to @p dataHandler.
     *
     * @return TFTP server write operation.
     **/
    [[nodiscard]] virtual WriteOperationPtr writeOperation() = 0;

    /**
     * @brief Executes TFTP Error Operation.
     *
     * The error operation is executed (Error Packet sent) immediately.
     *
     * @param[in] remote
     *   Where the error packet shall be transmitted to.
     * @param[in] errorCode
     *   Error code of the error packet.
     * @param[in] errorMessage
     *   Error message of the packet.
     **/
    virtual void errorOperation(
      const boost::asio::ip::udp::endpoint &remote,
      ::Tftp::Packets::ErrorCode errorCode,
      std::string errorMessage = {} ) = 0;

    /**
     * @brief Executes a ARINC 615A WAIT Error Operation.
     *
     * Sends a TFTP Error Packet with WAIT Code.
     *
     * @param[in] remote
     *   Client address.
     * @param[in] waitTime
     *   Wait time.
     **/
    virtual void waitOperation( const boost::asio::ip::udp::endpoint &remote, std::chrono::seconds waitTime ) = 0;

    /**
     * @brief Executes a ARINC 615A ABORT Error Operation.
     *
     * Sends a TFTP Error Packet with ABORT Code.
     *
     * @param[in] remote
     *   Client address.
     * @param[in] status
     *   Status code.
     **/
    virtual void abortOperation( const boost::asio::ip::udp::endpoint &remote, StatusCode status ) = 0;

  protected:
    //! Constructor.
    Server() = default;
};

}

#endif
