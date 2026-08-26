// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Unit Tests for Arinc615a::Tftp ErrorMessage.
 **/

#include <arinc_615a/Arinc615aException.hpp>
#include <arinc_615a/tftp/ErrorMessage.hpp>

#include <tftp/packets/ErrorPacket.hpp>

#include <tftp/packets/PacketException.hpp>

#include <boost/test/unit_test.hpp>

namespace Arinc615a::Tftp {

BOOST_AUTO_TEST_SUITE( Arinc615aTest )
BOOST_AUTO_TEST_SUITE( TftpTest )
BOOST_AUTO_TEST_SUITE( ErrorMessageTest )

//! Packet Type Test
BOOST_AUTO_TEST_CASE( packetType )
{
  BOOST_CHECK(
    ErrorMessage_type( ::Tftp::Packets::ErrorInformation{} ) == ErrorMessageType::Invalid );

  BOOST_CHECK(
    ErrorMessage_type(
      ::Tftp::Packets::ErrorInformation{
        std::make_tuple( ::Tftp::Packets::ErrorCode::AccessViolation, std::string( "WAIT:" ) ) } )
    == ErrorMessageType::Invalid );

  BOOST_CHECK(
    ErrorMessage_type(
      ::Tftp::Packets::ErrorInformation{
        std::make_tuple( ::Tftp::Packets::ErrorCode::NotDefined, std::string( "ABORT:" ) ) } )
    == ErrorMessageType::Invalid );
  BOOST_CHECK(
    ErrorMessage_type(
      ::Tftp::Packets::ErrorInformation{
        std::make_tuple( ::Tftp::Packets::ErrorCode::NotDefined, std::string( "ABORT:X" ) ) } )
    == ErrorMessageType::Abort );

  BOOST_CHECK(
    ErrorMessage_type(
      ::Tftp::Packets::ErrorInformation{
        std::make_tuple( ::Tftp::Packets::ErrorCode::NotDefined, std::string( "WAIT:" ) ) } )
    == ErrorMessageType::Invalid );
  BOOST_CHECK(
    ErrorMessage_type(
      ::Tftp::Packets::ErrorInformation{
        std::make_tuple( ::Tftp::Packets::ErrorCode::NotDefined, std::string( "WAIT:X" ) ) } )
    == ErrorMessageType::Wait );
}

//! Abort Packet test
BOOST_AUTO_TEST_CASE( packetType_Abort )
{
  BOOST_CHECK( ErrorMessage_abort( "ABORT:" ) == StatusCode::Invalid );
  BOOST_CHECK( ErrorMessage_abort( "ABORT:XXXX" ) == StatusCode::Invalid );

  BOOST_CHECK( ErrorMessage_abort( "ABORT:1004" ) == StatusCode::OperationAbortedByDlp );
}

//! Wait Packet test
BOOST_AUTO_TEST_CASE( packetType_Wait )
{
  BOOST_CHECK( !ErrorMessage_wait( "" ) );
  BOOST_CHECK( !ErrorMessage_wait( "WAIT:" ) );
  BOOST_CHECK( !ErrorMessage_wait( "WAIT:XXXX" ) );

  BOOST_CHECK( ErrorMessage_wait( "WAIT:5" ) == std::chrono::seconds{ 5 } );
  BOOST_CHECK( ErrorMessage_wait( "WAIT:0" ) == std::chrono::seconds{ 0 } );
  BOOST_CHECK( ErrorMessage_wait( "WAIT:65535" ) == std::chrono::seconds{ 65535 } );

  BOOST_CHECK( !ErrorMessage_wait(  "WAIT:65536" ) );
  BOOST_CHECK( !ErrorMessage_wait(  "WAIT:-1" ) );
}

//! Abort Message Test
BOOST_AUTO_TEST_CASE( abortErrorMessage )
{
  BOOST_CHECK( ErrorMessage_abort( StatusCode::OperationAbortedByDlp ) == "ABORT:1004" );
}

//! Wait Message Test
BOOST_AUTO_TEST_CASE( waitMessage )
{
  BOOST_CHECK( ErrorMessage_wait( std::chrono::seconds{ 10 } ) == "WAIT:10" );
  BOOST_CHECK( ErrorMessage_wait( std::chrono::seconds{ 100 } ) == "WAIT:100" );
  BOOST_CHECK( ErrorMessage_wait( std::chrono::seconds{ 0 } ) == "WAIT:0" );
  BOOST_CHECK( ErrorMessage_wait( std::chrono::seconds{ 65535 } ) == "WAIT:65535" );
  BOOST_CHECK_THROW( (void)ErrorMessage_wait( std::chrono::seconds{ 65536 } ), Arinc615aException );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
