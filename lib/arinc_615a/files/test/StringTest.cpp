// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Unit Tests of Module Arinc615a::Files String.
 **/

#include <arinc_615a/files/String.hpp>

#include <arinc_615a/Arinc615aException.hpp>

#include <boost/test/unit_test.hpp>

#include <array>

namespace Arinc615a::Files {

BOOST_AUTO_TEST_SUITE( Arinc615aTest )
BOOST_AUTO_TEST_SUITE( FilesTest )
BOOST_AUTO_TEST_SUITE( StringTest )

//! Variable String wich is only as long as necessary
static const auto rawStrDynamicSize{ std::to_array< uint8_t >(
  { 9, 'T', 'E', 'S', 'T', '_', '1', '2', '3', 0 } ) };
//! Fixed sized string filled with `0`
static const auto rawStrFixedSize{ std::to_array< uint8_t >(
  { 10, 'T', 'E', 'S', 'T', '_', '1', '2', '3', 0, 0 } ) };
//! Expected decoded String
static const auto str{ "TEST_123" };

//! Empty String only length
static const auto rawStrEmpty{ std::to_array< uint8_t >( { 0 } ) };
//! Empty String with NULL
static const auto rawStrEmptyDynamic{ std::to_array< uint8_t >( { 1, 0 } ) };
//! Empty String with NULL
static const auto rawStrEmptyFixed{ std::to_array< uint8_t >( { 5 , 0, 0, 0, 0, 0 } ) };

//! Invalid String (Not NULL terminated)
static const auto rawStrInvalid1{ std::to_array< uint8_t >(
  { 8, 'T', 'E', 'S', 'T', '_', '1', '2', '3', 0 } ) };

//! Invalid String (too short)
static const auto rawStrInvalid2{
  std::to_array< uint8_t >( { 10, 'T', 'E', 'S', 'T', '_', '1', '2', '3', 0 } ) };

//! Decode string test
BOOST_AUTO_TEST_CASE( decodeString1 )
{
  auto [ remaining1, decodedStr1 ]{ String_decode( std::as_bytes( std::span{ rawStrFixedSize } ) ) };
  BOOST_CHECK( decodedStr1 == str );
  BOOST_CHECK( remaining1.empty() );

  auto [ remaining2, decodedStr2 ]{ String_decode( std::as_bytes( std::span{ rawStrDynamicSize } ) ) };
  BOOST_CHECK( decodedStr2 == str );
  BOOST_CHECK( remaining2.empty() );
}

//! Decode Empty string test
BOOST_AUTO_TEST_CASE( decodeStringEmpty )
{
  auto [ remaining1, decodedStr1 ]{ String_decode( std::as_bytes( std::span{ rawStrEmpty } ) ) };
  BOOST_CHECK( remaining1.empty() );
  BOOST_CHECK( decodedStr1.empty() );

  auto [ remaining2, decodedStr2 ]{ String_decode( std::as_bytes( std::span{ rawStrEmptyDynamic } ) ) };
  BOOST_CHECK( remaining2.empty() );
  BOOST_CHECK( decodedStr2.empty() );

  auto [ remaining3, decodedStr3 ]{ String_decode( std::as_bytes( std::span{ rawStrEmptyFixed } ) ) };
  BOOST_CHECK( remaining3.empty() );
  BOOST_CHECK( decodedStr3.empty() );
}

//! Decode Invalid strings
BOOST_AUTO_TEST_CASE( decodeStringInvalid )
{
  BOOST_CHECK_THROW( boost::ignore_unused( String_decode( {} ) ), Arinc615aException );

  BOOST_REQUIRE_THROW( boost::ignore_unused( String_decode( std::as_bytes( std::span{ rawStrInvalid1 } ) ) ), Arinc615aException );

  BOOST_REQUIRE_THROW( boost::ignore_unused( String_decode( std::as_bytes( std::span{ rawStrInvalid2 } ) ) ), Arinc615aException );

}

//! Encode string test variable Length
BOOST_AUTO_TEST_CASE( encodeStringVariableLength )
{
  const auto rawString1{ String_encode( str ) };
  BOOST_CHECK( std::ranges::equal( rawString1, std::as_bytes( std::span{ rawStrDynamicSize } ) ) );

  const auto rawString2{ String_encode( std::string{} ) };
  BOOST_CHECK( std::ranges::equal( rawString2, std::as_bytes( std::span{ rawStrEmpty } ) ) );

  BOOST_CHECK_THROW( boost::ignore_unused( String_encode( std::string( 256, '1' ) ) ), Arinc615aException );
}

//! Encode string test fixed Length
BOOST_AUTO_TEST_CASE( encodeStringFixedLength )
{
  const auto rawString1{ String_encode( str, 10 ) };
  BOOST_CHECK( std::ranges::equal( rawString1, std::as_bytes( std::span{ rawStrFixedSize  } ) ) );

  const auto rawString2{ String_encode( std::string{}, 5 ) };
  BOOST_CHECK( std::ranges::equal( rawString2, std::as_bytes( std::span{ rawStrEmptyFixed } ) ) );

  BOOST_CHECK_THROW( boost::ignore_unused( String_encode( std::string( 10, '1' ), 10 ) ), Arinc615aException );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
