// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Namespace Arinc615a::Find::Packets.
 **/

/**
 * @dir
 * @brief Declaration/ Definition of Namespace Arinc615a::Find::Packets.
 **/

#ifndef ARINC_615A_FIND_PACKETS_PACKETS_HPP
#define ARINC_615A_FIND_PACKETS_PACKETS_HPP

#include <arinc_615a/find/Find.hpp>

#include <cstdint>
#include <map>

/**
 * @brief ARINC 615A FIND %Packets.
 *
 * This namespace provides the implementation of the data packets, which are transferred during
 * *ARINC 615A FIND Operations*.
 **/
namespace Arinc615a::Find::Packets {

//! Opcodes of known FIND Packets.
enum class Opcode : uint16_t
{
  //! Information Request (IRQ)
  InformationRequest = 1U,
  //! Information Answer (IAN)
  InformationAnswer  = 2U,

  //! Invalid value
  Invalid            = 0xFFFFU
};

class InvalidFindPacket;

// Forward declaration
class Packet;

class OpcodeDescription;

class PacketHandler;

class PacketStatistic;

//! Parameter list of the FIND response.
enum class ParameterList : size_t
{
  ThwId,            //!< Target Hardware ID
  ThwTypeName,      //!< Target Hardware Type Name
  ThwPosition,      //!< Target Hardware Position
  LiteralName,      //!< Literal Name
  ManufacturerCode, //!< Manufacturer Code

  Last              //!< Last entry element
};

//! Parameter Max length map type
using ParameterMaxLengthMap = std::map< ParameterList, size_t >;

/**
 * @brief Retrieves a map defining the maximum length for each parameter in the FIND response.
 *
 * The map associates entries from the ParameterList enumeration with their corresponding
 * maximum allowable length.
 *
 * @return A constant reference to a map (ParameterMaxLengthMap) with entries specifying
 *   parameter identifiers (ParameterList) and their respective maximum lengths (size_t).
 **/
[[nodiscard]] ARINC_615A_EXPORT const ParameterMaxLengthMap& parameterMaxLengths();

}

#endif
