// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Files::ProtocolFileStatistic.
 **/

#ifndef ARINC_615A_FILES_PROTOCOLFILESTATISTIC_HPP
#define ARINC_615A_FILES_PROTOCOLFILESTATISTIC_HPP

#include <arinc_615a/files/Files.hpp>

#include <iosfwd>
#include <map>
#include <shared_mutex>
#include <string>

namespace Arinc615a::Files {

/**
 * @brief Protocol File Statistic
 *
 * This class provides operations to count protocol files (count and total size).
 * The statistic is captured separately for each file type.
 *
 * The class provides global instances for transmitted and received protocol files.
 * @sa @ref ProtocolFileStatistic::globalReceive()
 * @sa @ref ProtocolFileStatistic::globalTransmit()
 **/
class ARINC_615A_EXPORT ProtocolFileStatistic final
{
  public:
    //! Statistic Type (Protocol File type -> Count)
    using Statistic = std::map< ProtocolFileType, std::size_t >;

    /**
     * @brief Global Receive Protocol File Statistic counter.
     *
     * @return Global Receive Protocol File Statistic counter.
     **/
    [[nodiscard]] static ProtocolFileStatistic& globalReceive();

    /**
     * @brief Global Transmit Protocol File Statistic counter.
     *
     * @return Global Transmit Protocol File Statistic counter.
     **/
    [[nodiscard]] static ProtocolFileStatistic& globalTransmit();

    /**
     * @brief Accumulates all Statistic values to one overall result.
     *
     * @param[in] statistic
     *   Packet Statistic information
     *
     * @return Total information.
     **/
    [[nodiscard]] static size_t total( const Statistic &statistic );

    //! Default Constructor
    ProtocolFileStatistic() = default;

    /**
     * @brief Log a protocol file.
     *
     * @param[in] type
     *   Protocol File Type
     **/
    void protocolFile( ProtocolFileType type );

    /**
     * @brief Returns the Protocol File statistic.
     *
     * @return Protocol File statistic.
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
    //! Protocol File Statistic
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
ARINC_615A_EXPORT std::ostream& operator<<( std::ostream &stream, const ProtocolFileStatistic &statistic );

}

#endif
