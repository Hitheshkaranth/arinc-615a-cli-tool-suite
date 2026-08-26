// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Find::Clients::QueryImpl.
 **/

#ifndef ARINC_615A_FIND_CLIENTS_QUERYIMPL_HPP
#define ARINC_615A_FIND_CLIENTS_QUERYIMPL_HPP

#include <arinc_615a/find/clients/Clients.hpp>
#include <arinc_615a/find/clients/Query.hpp>

#include <arinc_615a/find/packets/PacketHandler.hpp>
#include <arinc_615a/find/packets/Packets.hpp>

#include <boost/asio/ip/udp.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/system_timer.hpp>

#include <cstdint>
#include <chrono>

namespace Arinc615a::Find::Clients {

/**
 * @brief Implementation of a FIND Query.
 *
 * The ARINC 615A FIND client sends a FIND information request package to the supplied destination and waits a given
 * time for any responses.
 *
 * For each response the call-back FindResponseHandler::receivedFindResponse() operation is called.
 * After completion of the FIND operation, the call-back FindResponseHandler::finishedQuery() is called.
 **/
class QueryImpl final : public Query, private Packets::PacketHandler
{
  public:
    /**
     * @brief Generates a FIND Query.
     *
     * @param[in] ioContext
     *   I/O Context used for communication.
     *
     * @throw Arinc615aException
     *   When the socket cannot be opened or bind fails to the local address.
     **/
    explicit QueryImpl( boost::asio::io_context &ioContext );

    /**
     * @brief Destructor of FIND Query
     *
     * finishes the communication.
     **/
    ~QueryImpl() noexcept override;

    //! @copydoc Query::responseHandler()
    Query& responseHandler( ResponseHandler responseHandler ) override;

    //! @copydoc Query::completionHandler()
    Query& completionHandler( CompletionHandler completionHandler ) override;

    //! @copydoc Query::localAddress()
    Query& localAddress( boost::asio::ip::address localAddress ) override;

    //! @copydoc Query::remoteAddress()
    Query& remoteAddress( boost::asio::ip::address remoteAddress ) override;

    //! @copydoc Query::port()
    Query& port( uint16_t port ) override;

    //! @copydoc Query::dynamicLocalPort()
    Query& dynamicLocalPort( bool dynamicLocalPort ) override;

    //! @copydoc Query::timeout()
    Query& timeout( std::chrono::seconds timeout ) override;

    /**
     * @copydoc Query::start()
     * @throw FindClientException
     *   only internal errors are handled by this exception
     **/
    void start() override;

    //! @copydoc Query::abort()
    void abort() override;

  private:
    /**
     * @brief Sends the given packet.
     *
     * @param[in] remote
     *   Remote address.
     * @param[in] packet
     *   FIND packet.
     **/
    void send( const boost::asio::ip::udp::endpoint &remote, const Packets::Packet &packet );

    /**
     * @brief Starts the reception operation.
     **/
    void receive();

    /**
     * @brief Handler for internal timer.
     *
     * @param[in] error
     *   Error code, if any.
     **/
    void timerHandler( const boost::system::error_code& error );

    /**
     * @brief Handler for received datagrams.
     *
     * @param[in] error
     *   Error code, if any.
     * @param[in] bytesTransferred
     *   Number of received bytes.
     **/
    void receiveHandler( const boost::system::error_code &error, std::size_t bytesTransferred );

    /**
     * @brief Handler for FIND information request packets.
     *
     * Request Packets are ignored.
     *
     * @param[in] remote
     *   Source of the packet.
     * @param[in] request
     *   FIND information request packet.
     **/
    void informationRequestPacket(
      const boost::asio::ip::udp::endpoint &remote,
      const Packets::Packet &request ) override;

    /**
     * @brief Handler for FIND information answer packets.
     *
     * Answer packets are decoded, and the FindResponseHandler::receivedFindResponse call-back is called.
     *
     * @param[in] remote
     *   Source of the packet.
     * @param[in] answer
     *   FIND information answer packet.
     **/
    void informationAnswerPacket(
      const boost::asio::ip::udp::endpoint &remote,
      const Packets::Packet &answer ) override;

    /**
     * @brief Handler for invalid FIND packets.
     *
     * Invalid Packets are ignored.
     *
     * @param[in] remote
     *   Source of received Packet.
     * @param[in] rawPacket
     *   Raw Packet
     **/
    void invalidPacket( const boost::asio::ip::udp::endpoint &remote,Helper::ConstRawDataSpan rawPacket ) override;

    //! Timer, which handles the reception timeout
    boost::asio::system_timer timerV;
    //! Query socket
    boost::asio::ip::udp::socket socketV;
    //! Storage of received data
    Helper::RawData rawReceivePacketV;
    //! Address of sender of received data.
    boost::asio::ip::udp::endpoint receiveRemoteEndpointV;

    //! Handler, which is called during operation.
    ResponseHandler responseHandlerV;
    //! Handler, which is called on completion of the FIND operation
    CompletionHandler completionHandlerV;
    //! Local endpoint to connect.
    boost::asio::ip::address localAddressV{ boost::asio::ip::address_v4::any() };
    //! remote endpoint to connect.
    boost::asio::ip::address remoteAddressV{ boost::asio::ip::address_v4::broadcast() };
    //! FIND Port
    uint16_t portV{ DefaultPort };
    //!  Use dynamic local port.
    bool dynamicLocalPortV{ false };
    //! FIND Operation timeout.
    std::chrono::seconds timeoutV{ DefaultReceiveTimeout };
};

}

#endif
