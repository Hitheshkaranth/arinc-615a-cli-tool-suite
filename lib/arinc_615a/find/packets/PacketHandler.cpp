// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615a::Find::Packets::PacketHandler.
 **/

#include "PacketHandler.hpp"

#include <arinc_615a/find/packets/Packet.hpp>
#include <arinc_615a/find/packets/PacketException.hpp>
#include <arinc_615a/find/packets/PacketStatistic.hpp>

#include <spdlog/spdlog.h>

namespace Arinc615a::Find::Packets {

void PacketHandler::packet( const boost::asio::ip::udp::endpoint &remote, Helper::ConstRawDataSpan rawPacket )
{
  switch ( Packet::packetType( rawPacket ) )
  {
    using enum Opcode;

    case InformationRequest:
      try
      {
        informationRequestPacket( remote, Packet{ rawPacket } );

        // Update statistic
        PacketStatistic::globalReceive().packet( InformationRequest, rawPacket.size() );
      }
      catch ( const InvalidFindPacket &e )
      {
        SPDLOG_ERROR( "Error decoding/ handling IRQ packet: {}", e.what() );

        // Update statistic
        PacketStatistic::globalReceive().packet( Invalid, rawPacket.size() );

        invalidPacket( remote, rawPacket );
      }
      break;

    case InformationAnswer:
      try
      {
        informationAnswerPacket( remote, Packet{ rawPacket } );

        // Update statistic
        PacketStatistic::globalReceive().packet( InformationAnswer, rawPacket.size() );
      }
      catch ( const InvalidFindPacket &e )
      {
        SPDLOG_ERROR( "Error decoding/ handling IAN packet: {}", e.what() );

        // Update statistic
        PacketStatistic::globalReceive().packet( Invalid, rawPacket.size() );

        invalidPacket( remote, rawPacket );
      }
      break;

    default:
      // Update statistic
      PacketStatistic::globalReceive().packet( Invalid, rawPacket.size() );

      invalidPacket( remote, rawPacket );
      break;
  }
}

}
