// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration/ definition of class
 *   Arinc615a::Find::Packets::PacketException and
 *   Arinc615a::Find::Packets::InvalidFindPacket.
 **/

#ifndef ARINC_615A_FIND_PACKETS_PACKETEXCEPTION_HPP
#define ARINC_615A_FIND_PACKETS_PACKETEXCEPTION_HPP

#include <arinc_615a/Arinc615aException.hpp>

namespace Arinc615a::Find::Packets {

/**
 * @brief ARINC 615A FIND %Packet Exception.
 *
 * This exception is thrown when a FIND packet cannot be decoded.
 **/
class PacketException : public Arinc615aException
{
  public:
    /**
     * @brief Returns an exception description.
     *
     * @return An exception description.
     **/
    const char * what() const noexcept override
    {
      return "ARINC 615A FIND Packet Exception";
    }
};

//! Invalid ARINC 615A FIND %Packet Exception.
class InvalidFindPacket: public PacketException
{
  public:
    /**
     * @brief Returns an exception description.
     *
     * @return An exception description.
     **/
    const char * what() const noexcept override
    {
      return "Invalid ARINC 615A FIND Packet Exception";
    }
};

}

#endif
