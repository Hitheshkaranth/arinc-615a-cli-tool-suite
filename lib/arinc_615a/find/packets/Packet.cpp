// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615a::Find::Packets::Packet.
 **/

#include "Packet.hpp"

#include <arinc_615a/find/packets/PacketException.hpp>

#include <helper/Dump.hpp>
#include <helper/Exception.hpp>

#include <spdlog/spdlog.h>

#include <boost/exception/all.hpp>

#include <format>

namespace Arinc615a::Find::Packets {

Opcode Packet::packetType( const Helper::ConstRawDataSpan rawPacket )
{
  // check size
  if ( rawPacket.size() < sizeof( uint16_t ) )
  {
    SPDLOG_ERROR( "Packet too small" );
    return Opcode::Invalid;
  }

  const auto [ _, opcode ]{ Helper::RawData_getInt< uint16_t >( rawPacket ) };

  // determine opcode
  // NOLINTNEXTLINE( clang-analyzer-optin.core.EnumCastOutOfRange ): Check for validity
  switch ( Opcode{ opcode } )
  {
    case Opcode::InformationRequest:
    case Opcode::InformationAnswer:
      break;

    default:
      SPDLOG_ERROR( "Invalid opcode 0x{:04X}", opcode );
      return Opcode::Invalid;
  }

  return Opcode{ opcode };
}

Packet::Packet( const Opcode opcode, const size_t numberOfParameters ) :
  opcodeV{ opcode },
  parametersV( numberOfParameters )
{
}

Packet::Packet( const Opcode opcode, Parameters parameters ) :
  opcodeV{ opcode },
  parametersV{ std::move( parameters ) }
{
}

Packet::Packet( Helper::ConstRawDataSpan rawPacket )
{
  // check minimum packet size
  if ( rawPacket.size() <= sizeof( uint16_t ) )
  {
    BOOST_THROW_EXCEPTION( InvalidFindPacket{}
      << Helper::AdditionalInfo{ std::format( "Packet to small: {}", rawPacket.size() ) } );
  }

  auto remainingData{ rawPacket };

  // check packet terminating character
  if ( remainingData.back() != PacketTerminator )
  {
    BOOST_THROW_EXCEPTION( InvalidFindPacket()
      << Helper::AdditionalInfo{ "Packet is not terminated by packet terminator" } );
  }

  remainingData = remainingData.first( remainingData.size() - 1U );

  // decode opcode
  uint16_t intOpcode{};
  std::tie( remainingData, intOpcode ) = Helper::RawData_getInt< uint16_t >( remainingData );
  // NOLINTNEXTLINE( clang-analyzer-optin.core.EnumCastOutOfRange ): Check for validity
  switch ( Opcode{ intOpcode } )
  {
    case Opcode::InformationRequest:
    case Opcode::InformationAnswer:
      break;

    default:
      BOOST_THROW_EXCEPTION( InvalidFindPacket() << Helper::AdditionalInfo{ "Invalid opcode" } );
      /* no break - because BOOST_THROW_EXCEPTION throws*/
  }

  opcodeV = Opcode{ intOpcode };

  // decode parameters
  for (
    auto [ _, parametersString ] { Helper::RawData_getString( remainingData, remainingData.size() ) };
    !parametersString.empty(); )
  {
    const auto parameterEnd{ parametersString.find( '\0' ) };

    if ( parameterEnd == std::string_view::npos )
    {
      BOOST_THROW_EXCEPTION( InvalidFindPacket()
        << Helper::AdditionalInfo{ "String Terminator missing" } );
    }

    parametersV.emplace_back( parametersString.substr( 0, parameterEnd ) );

    parametersString = parametersString.substr( parameterEnd + 1U );
  }

  if ( parametersV.empty() )
  {
    BOOST_THROW_EXCEPTION( InvalidFindPacket()
      << Helper::AdditionalInfo{ "Packet must contain at least one (empty) parameter" } );
  }
}

Opcode Packet::opcode() const
{
  return opcodeV;
}

void Packet::opcode( const Opcode opcode )
{
  opcodeV = opcode;
}

size_t Packet::numberOfParameters() const
{
  return parametersV.size();
}

const Packet::Parameters& Packet::parameters() const
{
  return parametersV;
}

Packet::Parameters& Packet::parameters()
{
  return parametersV;
}

void Packet::parameter( const size_t position, std::string parameter )
{
  parametersV.at( position ) = std::move( parameter );
}

std::string Packet::parameter( const size_t position ) const
{
  return parametersV.at( position );
}

Helper::RawData Packet::encode() const
{
  // reserve space of raw packet ( Opcode + String-Sizes (incl. 0) + Packet Terminator)
  Helper::RawData rawPacket( 2U );

  // add opcode opcode
  Helper::RawData_setInt( rawPacket, std::to_underlying( opcodeV ) );

  // add parameters
  for ( const auto &parameter : parametersV )
  {
    rawPacket.reserve( rawPacket.size() + parameter.size() + 1U );

    // option name
    auto rawParameter{ Helper::RawData_asRaw( parameter ) };
    rawPacket.insert( rawPacket.end(), rawParameter.begin(), rawParameter.end() );

    // name value divider
    rawPacket.push_back( StringTerminator );
  }

  // Packet terminator
  rawPacket.push_back( PacketTerminator );

  return rawPacket;
}

}
