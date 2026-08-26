// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615a::Files::ProtocolFileStatistic.
 **/

#include "ProtocolFileStatistic.hpp"

#include <arinc_615a/files/ProtocolFileTypeDescription.hpp>

#include <algorithm>
#include <format>
#include <mutex>
#include <ostream>

namespace Arinc615a::Files {

ProtocolFileStatistic& ProtocolFileStatistic::globalReceive()
{
  static ProtocolFileStatistic globalReceivePacketStatistics{};
  return globalReceivePacketStatistics;
}

ProtocolFileStatistic& ProtocolFileStatistic::globalTransmit()
{
  static ProtocolFileStatistic globalTransmitPacketStatistics{};
  return globalTransmitPacketStatistics;
}

size_t ProtocolFileStatistic::total( const Statistic &statistic )
{
  size_t total{ 0 };

  // accumulate all values
  std::ranges::for_each(
    statistic,
    [ &total ]( auto value ){
      total += value.second;
    } );

  return total;
}

void ProtocolFileStatistic::protocolFile( const Arinc615a::Files::ProtocolFileType type )
{
  std::unique_lock lock{ mutex };
  ++statisticV[ type ];
}

ProtocolFileStatistic::Statistic ProtocolFileStatistic::statistic() const
{
  std::shared_lock lock{ mutex };
  return statisticV;
}

void ProtocolFileStatistic::reset()
{
  std::unique_lock lock{ mutex };
  statisticV.clear();
}

std::string ProtocolFileStatistic::toString() const
{
  std::string str;

  size_t count{ 0 };

  for ( const auto &[ packetType, statistic ] : statistic() )
  {
    str += std::format(
      "{:22}: Count: {}\n",
      ProtocolFileTypeDescription::instance().name( packetType ),
      statistic );

    count += statistic;
  }

  str += std::format( "{:22}: Count: {}\n", "Total", count );

  return str;
}

std::ostream &operator<<( std::ostream &stream, const ProtocolFileStatistic &statistic )
{
  return ( stream << statistic.toString() );
}

}
