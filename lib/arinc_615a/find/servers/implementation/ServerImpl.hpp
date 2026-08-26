// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Find::Servers::ServerImpl.
 **/

#ifndef ARINC_615A_FIND_SERVERS_SERVERIMPL_HPP
#define ARINC_615A_FIND_SERVERS_SERVERIMPL_HPP

#include <arinc_615a/find/servers/Server.hpp>

#include <arinc_615a/find/packets/PacketHandler.hpp>

#include <arinc_615a/Arinc615aException.hpp>

#include <boost/asio/ip/udp.hpp>
#include <boost/asio/io_context.hpp>

namespace Arinc615a::Find::Servers {

/**
 * @brief ARINC 615A FIND Server Implementation.
 **/
class ServerImpl final : public Server, private Packets::PacketHandler
{
  public:
    /**
     * @brief Initialises the FIND Server Instance.
     *
     * @param[in] ioContext
     *   I/O context used for Communication.
     **/
    explicit ServerImpl( boost::asio::io_context &ioContext );

    /**
     * @brief Destructor
     *
     * Performs cleanup of the FIND server.
     **/
    ~ServerImpl() noexcept override;

    //! @copydoc Server::requestHandler()
    Server& requestHandler( FindRequestHandler requestHandler ) override;

    //! @copydoc Server::localEndpoint()
    Server& localEndpoint(
      boost::asio::ip::udp::endpoint localEndpoint ) override;

    //! @copydoc Server::start()
    void start() override;

    //! @copydoc Server::stop()
    void stop() override;

    //! @copydoc Server::response()
    void response(
      const boost::asio::ip::udp::endpoint &remote,
      const TargetInformation &findInformation ) override;

  private:
    /**
     * @brief Starts the reception operation.
     *
     * On reception of data, the receiveHandler() is called.
     */
    void receive();

    /**
     * @brief Handler of received data.
     *
     * The received data is checked and transferred to handlePacket().
     *
     * @param[in] error
     *   Error information.
     * @param[in] bytesTransferred
     *   Number of received bytes.
     **/
    void receiveHandler( const boost::system::error_code &error, std::size_t bytesTransferred );

    /**
     * @brief Handler of ARINC 615A FIND Information Request packets.
     *
     * @param[in] remote
     *   The source of the packet.
     * @param[in] request
     *   The ARINC 615a FIND Information Request packet.
     **/
    void
    informationRequestPacket( const boost::asio::ip::udp::endpoint &remote, const Packets::Packet &request ) override;

    /**
     * @brief Handler of ARINC 615A FIND Information Answer packets.
     *
     * @param[in] remote
     *   Source of the packet.
     * @param[in] answer
     *   ARINC 615a FIND Information Answer Packet.
     **/
    void informationAnswerPacket(
      const boost::asio::ip::udp::endpoint &remote,
      const Packets::Packet &answer ) override;

    /**
     * @brief Handler of received invalid packets.
     *
     * @param[in] remote
     *   Source of the packet.
     * @param[in] rawPacket
     *   Received data.
     **/
    void invalidPacket( const boost::asio::ip::udp::endpoint &remote, Helper::ConstRawDataSpan rawPacket ) override;

    //! Server Socket
    boost::asio::ip::udp::socket socketV;

    //! Received packet
    Helper::RawData rawReceivePacketV;
    //! Source address of the received data.
    boost::asio::ip::udp::endpoint receiveRemoteEndpointV;
    //! FIND Request Handler
    FindRequestHandler requestHandlerV;
    //! Local endpoint.
    boost::asio::ip::udp::endpoint localEndpointV{ boost::asio::ip::address_v4::any(), DefaultPort };
};

}

#endif
