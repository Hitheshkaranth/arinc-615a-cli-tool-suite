// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Unit Tests for Class Arinc615a::Find::Packets::Packet.
 **/

#include <arinc_615a/find/packets/Packet.hpp>
#include <arinc_615a/find/packets/PacketException.hpp>

#include <helper/RawData.hpp>

#include <boost/test/unit_test.hpp>

namespace Arinc615a::Find::Packets {

BOOST_AUTO_TEST_SUITE( Arinc615aTest )
BOOST_AUTO_TEST_SUITE( FindTest )
BOOST_AUTO_TEST_SUITE( PacketsTest )
BOOST_AUTO_TEST_SUITE( PacketTest )

//! FIND Request Packet
static uint8_t findRequestPacket[]{
  0, 1,
  0x00,
  0x10
};

//! FIND Answer Packet
static uint8_t findAnswerPacket[]{
  0, 2, // Opcode
  'T', 'H', 'W', '_', 'I', 'D', 0x00,
  'T', 'H', 'W', '_', 'T', 'Y', 'P', 'E', 0x00,
  'T', 'H', 'W', '_', 'P', 'O', 'S', 0x00,
  'L', 'I', 'T', '_', 'N', 'A', 'M', 'E', 0x00,
  'M', 'A', 'N', '_', 'C', 'O', 'D', 'E', 0x00,
  0x10 };

//! get packet type test
BOOST_AUTO_TEST_CASE( packetType )
{
  using Helper::operator ""_b;

  BOOST_CHECK( Packet::packetType( Helper::RawData{} ) == Opcode::Invalid );
  BOOST_CHECK( Packet::packetType( Helper::RawData{ 0_b } ) == Opcode::Invalid );

  BOOST_CHECK( Packet::packetType( Helper::RawData{ 0_b, 1_b } ) == Opcode::InformationRequest );
  BOOST_CHECK( Packet::packetType( Helper::RawData{ 0_b, 2_b } ) == Opcode::InformationAnswer );
  BOOST_CHECK( Packet::packetType( Helper::RawData{ 0_b, 4_b } ) == Opcode::Invalid );
}

//! Information Request packet Test
BOOST_AUTO_TEST_CASE( informationRequestPacket1 )
{
  Packet packet{ std::as_bytes( std::span{ findRequestPacket } ) };

  BOOST_CHECK( packet.opcode() == Opcode::InformationRequest );
  BOOST_CHECK( packet.numberOfParameters() == 1 );
  BOOST_CHECK( packet.parameter( 0U ).empty() );
}

//! Information Request packet test
BOOST_AUTO_TEST_CASE( informationRequestPacket2 )
{
  Packet packet{ Opcode::InformationRequest, 1U };

  BOOST_CHECK( packet.opcode() == Opcode::InformationRequest );
  BOOST_CHECK( packet.numberOfParameters() == 1 );
  BOOST_CHECK( packet.parameter( 0U ).empty() );

  auto rawPacket{ packet.encode() };
  BOOST_CHECK( std::ranges::equal( rawPacket, std::as_bytes( std::span{ findRequestPacket } ) ) ) ;
}

//! Information Answer packet test
BOOST_AUTO_TEST_CASE( informationAnswerPacket1 )
{
  Packet packet{ std::as_bytes( std::span{ findAnswerPacket } ) };

  BOOST_CHECK( packet.opcode() == Opcode::InformationAnswer );
  BOOST_CHECK( packet.numberOfParameters() == 5U );
  BOOST_CHECK( packet.parameter( 0 ) == "THW_ID" );
  BOOST_CHECK( packet.parameter( 1 ) == "THW_TYPE" );
  BOOST_CHECK( packet.parameter( 2 ) == "THW_POS" );
  BOOST_CHECK( packet.parameter( 3 ) == "LIT_NAME" );
  BOOST_CHECK( packet.parameter( 4 ) == "MAN_CODE" );
}

//! Information Answer packet test
BOOST_AUTO_TEST_CASE( informationAnswerPacket2 )
{
  Packet packet{
    Opcode::InformationAnswer,
    {
      "THW_ID",
      "THW_TYPE",
      "THW_POS",
      "LIT_NAME",
      "MAN_CODE"
    } };

  BOOST_CHECK( packet.opcode() == Opcode::InformationAnswer );

  BOOST_CHECK( packet.numberOfParameters() == 5U );
  BOOST_CHECK( packet.parameter( 0 ) == "THW_ID" );
  BOOST_CHECK( packet.parameter( 1 ) == "THW_TYPE" );
  BOOST_CHECK( packet.parameter( 2 ) == "THW_POS" );
  BOOST_CHECK( packet.parameter( 3 ) == "LIT_NAME" );
  BOOST_CHECK( packet.parameter( 4 ) == "MAN_CODE" );

  auto rawPacket{ packet.encode() };
  BOOST_CHECK( std::ranges::equal( rawPacket, std::as_bytes( std::span{ findAnswerPacket } ) ) );
}

//! Invalid packet test
BOOST_AUTO_TEST_CASE( invalidPacket )
{
  using Helper::operator ""_b;

  BOOST_CHECK_THROW( Packet{ Helper::RawData{} }, InvalidFindPacket );
  BOOST_CHECK_THROW( Packet{ Helper::RawData{ 0_b } }, InvalidFindPacket );

  BOOST_CHECK_THROW( ( Packet{ Helper::RawData{ 0_b, 1_b, 0x00_b } } ), InvalidFindPacket );
  BOOST_CHECK_THROW( ( Packet{ Helper::RawData{ 0_b, 1_b, 0x10_b } } ), InvalidFindPacket );

  BOOST_CHECK_THROW( ( Packet{ Helper::RawData{ 0_b, 0_b, 0x00_b, 0x10_b } } ), InvalidFindPacket );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
