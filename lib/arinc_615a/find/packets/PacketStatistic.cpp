// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615a::Find::Packets::PacketStatistic.
 **/

#include "PacketStatistic.hpp"

#include <arinc_615a/find/packets/OpcodeDescription.hpp>

#include <algorithm>
#include <format>
#include <mutex>
#include <ostream>

namespace Arinc615a::Find::Packets {

PacketStatistic& PacketStatistic::globalReceive()
{
  static PacketStatistic globalReceivePacketStatistics{};
  return globalReceivePacketStatistics;
}

PacketStatistic& PacketStatistic::globalTransmit()
{
  static PacketStatistic globalTransmitPacketStatistics{};
  return globalTransmitPacketStatistics;
}

PacketStatistic::Value PacketStatistic::total( const Statistic &statistic )
{
  Value total{ 0, 0};

  // accumulate all values
  std::ranges::for_each( statistic, [ &total ]( auto value ){
    std::get< 0 >( total ) += std::get< 0 >( value.second );
    std::get< 1 >( total ) += std::get< 1 >( value.second );
  } );

  return total;
}

void PacketStatistic::packet( const Opcode type, const size_t size )
{
  std::unique_lock lock{ mutex };
  auto &[ count, totalSize ] = statisticV[ type ];
  ++count;
  totalSize += size;
}

PacketStatistic::Statistic PacketStatistic::statistic() const
{
  std::shared_lock lock{ mutex };
  return statisticV;
}

void PacketStatistic::reset()
{
  std::unique_lock lock{ mutex };
  statisticV.clear();
}

std::string PacketStatistic::toString() const
{
  std::string str{};

  size_t count{ 0 };
  size_t size{ 0 };

  for ( const auto &[ packetType, statistic ] : statistic() )
  {
    str += std::format(
      "{:22}: Count: {} Total Size: {}\n",
      packetType,
      std::get< 0 >( statistic ),
      std::get< 1 >( statistic ) );

    count += std::get< 0 >( statistic );
    size += std::get< 1 >( statistic );
  }

  str += std::format( "{:22}: Count: {} Total Size: {}\n", "Total", count, size );

  return str;
}

std::ostream &operator<<( std::ostream &stream, const PacketStatistic &statistic )
{
  return ( stream << statistic.toString() );
}

}
