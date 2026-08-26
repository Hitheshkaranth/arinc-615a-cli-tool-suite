// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Find::Packets::PacketStatistic.
 **/

#ifndef ARINC_615A_FIND_PACKETS_PACKETSTATISTIC_HPP
#define ARINC_615A_FIND_PACKETS_PACKETSTATISTIC_HPP

#include <arinc_615a/find/packets/Packets.hpp>

#include <iosfwd>
#include <map>
#include <shared_mutex>
#include <string>
#include <tuple>

namespace Arinc615a::Find::Packets {

/**
 * @brief ARINC 615A FIND Packet Statistic.
 *
 * This class provides operations to count packets (count and total size).
 * The statistic differentiates per FIND packet type.
 *
 **/
class ARINC_615A_EXPORT PacketStatistic
{
  public:
    //! Statistic Value Type (Count, Size)
    using Value = std::tuple< std::size_t, std::size_t >;
    //! Statistic Type (Maps the Opcode to statistic value)
    using Statistic = std::map< Opcode, Value >;

    /**
     * @brief Global Receive Packet Statistic counter.
     *
     * @return Global Receive Packet Statistic counter.
     **/
    [[nodiscard]] static PacketStatistic& globalReceive();

    /**
     * @brief Global Transmit Packet Statistic counter.
     *
     * @return Global Transmit Packet Statistic counter.
     **/
    [[nodiscard]] static PacketStatistic& globalTransmit();

    /**
     * @brief Accumulates all Statistic values to one overall result.
     *
     * @param[in] statistic
     *   Packet Statistic information
     *
     * @return Total information.
     **/
    [[nodiscard]] static Value total( const Statistic &statistic );

    //! Default Constructor
    PacketStatistic() = default;

    /**
     * @brief Log a transmitted packet.
     *
     * @param[in] type
     *   Opcode of FIND packet.
     * @param[in] size
     *   Packet Size.
     **/
    void packet( Opcode type, size_t size );

    /**
     * @brief Returns the statistic.
     *
     * @return Packet statistic.
     **/
    [[nodiscard]] Statistic statistic() const;

    /**
     * @brief Reset the statistic counters.
     **/
    void reset();

    /**
     * @brief Gives the statistic as a printable string.
     *
     * @return Static as string representation
     **/
    [[nodiscard]] std::string toString() const;

  private:
    //! Sent statistic
    Statistic statisticV;
    //! Mutex protecting the access to @p statisticV
    mutable std::shared_mutex mutex;
};

/**
 * @brief Stream output operator of @p PacketStatistic.
 *
 * @param[in,out] stream
 *   Output Stream
 * @param[in] statistic
 *   Packet Statistic
 *
 * @return @p stream for chaining.
 **/
ARINC_615A_EXPORT std::ostream &operator<<( std::ostream &stream, const PacketStatistic &statistic );

}

#endif
