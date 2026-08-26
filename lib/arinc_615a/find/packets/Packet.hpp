// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Find::Packets::Packet.
 **/

#ifndef ARINC_615A_FIND_PACKETS_PACKET_HPP
#define ARINC_615A_FIND_PACKETS_PACKET_HPP

#include <arinc_615a/find/packets/Packets.hpp>

#include <helper/RawData.hpp>

#include <string>
#include <vector>

namespace Arinc615a::Find::Packets {

/**
 * @brief ARINC 615A FIND %Packet.
 *
 * The FIND packet is a:
 * - FIND Information Request %Packet
 * - FIND Information Answer %Packet
 *
 * @par Packet Format
 *
 * | Field                   | Size       | Description    |
 * |:------------------------|:-----------|:---------------|
 * | **Opcode**              | 2 Bytes    | %Packet Opcode |
 * | **String 1**            | 0..N Bytes | String         |
 * | **String 1 Terminator** | 1 Byte     | Always `0x00`  |
 * | **String N**            | 0..N Bytes | String         |
 * | **String N Terminator** | 1 Byte     | Always `0x00`  |
 * | **%Packet Terminator**  | 1 Byte     | Always `0x10`  |
 *
 * @note
 * The number of strings including corresponding terminator is variable.
 * There must be at least one (empty) string available.
 *
 * @par FIND Information Request %Packet
 *
 * Opcode is set to `0x0001`.
 * %Packet contains one empty string (only ASCII string terminator).
 *
 * Fixed %Packet:
 * `0001 00 10`
 *
 * @par FIND Information Answer %Packet
 *
 * Opcode is set to `0x0002`.
 * %Packet contains 5 Strings (including ASCII string terminator):
 * - %Target Hardware Identifier
 * - %Target Type Name
 * - %Target Position
 * - Literal Name
 * - Manufacturer Code
 **/
class ARINC_615A_EXPORT Packet final
{
  public:
    //! FIND Parameters Type.
    using Parameters = std::vector< std::string >;

    //! Maximum Packet Size of FIND Packet.
    static constexpr size_t MaxPacketSize{ 1024U };

    //! A FIND Packet is terminated by this byte.
    static constexpr std::byte PacketTerminator{ 0x10U };

    //! A FIND Packet String is terminated by this byte.
    static constexpr std::byte StringTerminator{ 0x00U };

    /**
     * @brief Returns the packet type of the given raw FIND packet.
     *
     * @param[in] rawPacket
     *   FIND packet as raw data
     *
     * @return The packet type of the raw FIND Packet.
     * @retval Opcode::Invalid
     *   If the packet type cannot be determined.
     **/
    static Opcode packetType( Helper::ConstRawDataSpan rawPacket );

    /**
     * @brief Generate a FIND packet with the given opcode and the given number of empty parameters.
     *
     * @param[in] opcode
     *   Opcode of the new FIND packet.
     * @param[in] numberOfParameters
     *   Number of empty parameters.
     **/
    Packet( Opcode opcode, size_t numberOfParameters );

    /**
     * @brief Generate a FIND packet with the given opcode and the given parameters.
     *
     * @param[in] opcode
     *   Opcode of the new FIND packet.
     * @param[in] parameters
     *   Parameters
     **/
    Packet( Opcode opcode, Parameters parameters );

    /**
     * @brief Load a FIND Packet from a Received Data Buffer.
     *
     * @param[in] rawPacket
     *   The raw data buffer
     *
     * @throw InvalidFindPacket
     *   When the package size is too small
     * @throw InvalidFindPacket
     *   When the internal coding of the message is wrong.
     * @throw InvalidFindPacket
     *   When invalid opcode is given.
     * @throw InvalidFindPacket
     *   When FIND packet does not contain at least one (empty) parameter.
     **/
    explicit Packet( Helper::ConstRawDataSpan rawPacket );

    //! Destructor
    ~Packet() noexcept = default;

    /**
     * @name Opcode
     * @{
     **/

    /**
     * @brief Gets the opcode of the FIND Packet.
     *
     * @return Opcode of the FIND Packet.
     **/
    [[nodiscard]] Opcode opcode() const;

    /**
     * @brief Sets the opcode of the FIND Packet.
     *
     * @param[in] opcode
     *   Opcode.
     **/
    void opcode( Opcode opcode );

    /** @} **/

    /**
     * @name Parameters
     * @{
     **/

    /**
     * @brief Returns the Number of Parameters.
     *
     * @return Number of Parameters
     **/
    [[nodiscard]] size_t numberOfParameters() const;

    /**
     * @brief Returns the Parameter Vector.
     *
     * @return Parameter Vector.
     **/
    [[nodiscard]] const Parameters& parameters() const;

    /**
     * @brief Direct access to the raw data.
     *
     * @return Reference to the parameter vector.
     **/
    [[nodiscard]] Parameters& parameters();

    /**
     * @brief Set the Parameter at the given Position.
     *
     * @param[in] position
     *   Position of the Parameter.
     * @param[in] parameter
     *   Parameter
     **/
    void parameter( size_t position, std::string parameter );

    /**
     * @brief Returns the parameter at the given Position.
     *
     * @param[in] position
     *   Parameter position.
     *
     * @return Parameter at the given Position.
     **/
    [[nodiscard]] std::string parameter( size_t position ) const;

    /** @} **/

    /**
     * @brief Returns the packet as raw data.
     *
     * @return Packet as raw data.
     **/
    [[nodiscard]] Helper::RawData encode() const;

  private:
    //! Packet Opcode
    Opcode opcodeV;
    //! Parameter List
    Parameters parametersV;
};

}

#endif
