// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Find::Packets::PacketHandler.
 **/

#ifndef ARINC_615A_FIND_PACKETS_PACKETHANDLER_HPP
#define ARINC_615A_FIND_PACKETS_PACKETHANDLER_HPP

#include <arinc_615a/find/packets/Packets.hpp>

#include <helper/RawData.hpp>

#include <boost/asio/ip/udp.hpp>

namespace Arinc615a::Find::Packets {

/**
 * @brief ARINC 615A FIND Packet Handler.
 *
 * The protected virtual functions must be implemented by a FIND packet handler.
 * They are called when the appropriate packets are received.
 *
 * The user of the FIND implementation doesn't need to implement this interface.
 * This is done by the FindQuery class and the FindServer class.
 **/
class ARINC_615A_EXPORT PacketHandler
{
  public:
    //! Destructor
    virtual ~PacketHandler() noexcept = default;

    /**
     * @brief Try to decode the received packet as ARINC 615A FIND packet.
     *
     * Tries to determine the type of the TFTP package.
     *
     * If the packet is a FIND information request packet, the handler handleFindInformationRequestPacket() is called.
     *
     * If the packet is a FIND information answer packet, the handler handleFindInformationAnswerPacket() is called.
     *
     * If during handling (including packet conversion) an @ref InvalidFindPacket exception is thrown,
     * @ref invalidPacket() is called automatically.
     * This exception is not re-thrown.
     *
     * @param[in] remote
     *   Source of the packet.
     * @param[in] rawPacket
     *   Received data.
     **/
    void packet( const boost::asio::ip::udp::endpoint &remote, Helper::ConstRawDataSpan rawPacket );

  protected:
    /**
     * @brief This call-back is called by the @ref packet() method when a new FIND information request packet has been
     *   received.
     *
     * @param[in] remote
     *   Source of the FIND packet.
     * @param[in] request
     *   Received FIND information request packet
     **/
    virtual void informationRequestPacket( const boost::asio::ip::udp::endpoint &remote, const Packet &request ) = 0;

    /**
     * @brief This call-back is called by the handlePacket() method when a new FIND information answer packet has been
     *   received.
     *
     * @param[in] remote
     *   Source of the FIND packet.
     * @param[in] answer
     *   Received FIND information answer packet
     **/
    virtual void informationAnswerPacket( const boost::asio::ip::udp::endpoint &remote, const Packet &answer ) = 0;

    /**
     * @brief This call-back is called by the handlePacket() method when an invalid packet has been received.
     *
     * @param[in] remote
     *   Source of the FIND packet.
     * @param[in] rawPacket
     *   Raw packet data.
     **/
    virtual void invalidPacket( const boost::asio::ip::udp::endpoint &remote, Helper::ConstRawDataSpan rawPacket ) = 0;
};

}

#endif
