// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Find::Packets::OpcodeDescription.
 **/

#ifndef ARINC_615A_FIND_PACKETS_OPCODEDESCRIPTION_HPP
#define ARINC_615A_FIND_PACKETS_OPCODEDESCRIPTION_HPP

#include <arinc_615a/find/packets/Packets.hpp>

#include <helper/Description.hpp>

#include <format>
#include <iosfwd>

namespace Arinc615a::Find::Packets {

/**
 * @name ARINC FIND Packet Opcode
 * @{
 **/

/**
 * @brief ARINC FIND Packet Opcode Description.
 *
 * @sa @ref Opcode
 **/
class ARINC_615A_EXPORT OpcodeDescription final : public Helper::Description< OpcodeDescription, Opcode >
{
  public:
    //! Initialises Instance
    OpcodeDescription();
};

/**
 * @brief Streaming Operator to Output ARINC 615A Version as String
 *
 * @param[in,out] stream
 *   Output stream
 * @param[in] opcode
 *   ARINC 615A FIND Opcode
 *
 * @return @p stream for chaining.
 *
 * @sa @ref OpcodeDescription
 * @sa @ref Opcode
 **/
ARINC_615A_EXPORT std::ostream &operator<<( std::ostream &stream, Opcode opcode );

/** @} **/

}


namespace std {
/**
 * @brief Specialisation of @p std::formatter for @ref Arinc615a::Find::Packets::Opcode.
 **/
template<>
struct formatter< Arinc615a::Find::Packets::Opcode > : std::formatter< std::string_view >
{
  /**
   * @brief Arinc615a::Find::Packets::Opcode format routine.
   *
   * @tparam FmtContext
   *   Formatting Context
   * @param[in] opcode
   *   Opcode
   * @param[in,out] ctx
   *   Formatting Context
   *
   * @return Iterator to the end of output.
   **/
  template< class FmtContext >
  FmtContext::iterator format( const Arinc615a::Find::Packets::Opcode &opcode, FmtContext &ctx ) const
  {
    return std::formatter< string_view >::format(
      Arinc615a::Find::Packets::OpcodeDescription::instance().name( opcode ),
      ctx );
  }
};

}

#endif
