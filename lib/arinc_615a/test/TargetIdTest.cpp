// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of unit tests for class Arinc615a::TargetId.
 **/

#include <arinc_615a/TargetId.hpp>

#include <boost/test/unit_test.hpp>

#include <iostream>

namespace Arinc615a {

BOOST_AUTO_TEST_SUITE( Arinc615aTest )
BOOST_AUTO_TEST_SUITE( TargetIdTest )

//! Test THW ID
static constexpr auto validThwId{ "THWID1" };
//! Test THW Position
static constexpr auto validPosition{ "POS1" };
//! Test Target ID
static constexpr auto validTargetId{ "THWID1_POS1" };

//! TargetId Constructor test
BOOST_AUTO_TEST_CASE( splitTargetId )
{
  auto response{ TargetId::splitTargetId( validTargetId)};

  BOOST_CHECK( std::get< 0>(response) == validThwId);
  BOOST_CHECK( std::get< 1>(response) == validPosition);

  BOOST_CHECK( TargetId::splitTargetId( "" ) == ( std::make_tuple< std::string, std::string >( {}, {} ) ) );
  BOOST_CHECK( TargetId::splitTargetId( "_" ) == ( std::make_tuple< std::string, std::string >( {}, {} ) ) );
}

//! Target ID is THW ID Test
BOOST_AUTO_TEST_CASE( isThwId )
{
  BOOST_CHECK( !TargetId::isThwId( "" ) );
  BOOST_CHECK( !TargetId::isThwId( "1" ) );
  BOOST_CHECK( !TargetId::isThwId( "12" ) );
  BOOST_CHECK( !TargetId::isThwId( "123" ) );
  BOOST_CHECK( TargetId::isThwId( "1234" ) );
  BOOST_CHECK( TargetId::isThwId( "123456789012345" ) );
  BOOST_CHECK( !TargetId::isThwId( "1234567890123456" ) );
}

//! Target ID is Position Test
BOOST_AUTO_TEST_CASE( isPosition )
{
  BOOST_CHECK( TargetId::isPosition( "" ) );
  BOOST_CHECK( TargetId::isPosition( "1" ) );
  BOOST_CHECK( TargetId::isPosition( "12" ) );
  BOOST_CHECK( TargetId::isPosition( "1234567" ) );
  BOOST_CHECK( TargetId::isPosition( "12345678" ) );
  BOOST_CHECK( !TargetId::isPosition( "123456789" ) );
}

//! TargetId Constructor test
BOOST_AUTO_TEST_CASE( constructor )
{
  BOOST_CHECK( validTargetId == TargetId( validTargetId ).toString() );
  BOOST_CHECK( validTargetId == TargetId( validThwId, validPosition ).toString() );

  BOOST_CHECK( !TargetId( std::format( "{}{}", validThwId, validPosition ) ) );

  BOOST_CHECK( !TargetId( std::string( "000" ) + "_" + validPosition ) );
  BOOST_CHECK( !TargetId( std::string( "00" ) + "_" + validPosition ) );
  BOOST_CHECK( !TargetId( std::string( "0" ) + "_" + validPosition ) );
  BOOST_CHECK( !TargetId( std::string( "" ) + "_" + validPosition ) );

  BOOST_CHECK( !TargetId( std::format( "{}_{}", validThwId, "000000000" ) ) );

  BOOST_CHECK( !TargetId( "*000_001"));

  BOOST_CHECK( !TargetId( "000", validPosition));
  BOOST_CHECK( !TargetId( validThwId, "000000000"));
}

//! TargetId Constructor test
BOOST_AUTO_TEST_CASE( constructor2)
{
  using namespace std::literals;

  TargetId tId1{ "ABCD"s, "123456"s};
  BOOST_CHECK( tId1);
  BOOST_CHECK( tId1.thwId() == "ABCD");
  BOOST_CHECK( tId1.position() == "123456");
}

//! TargetID test
BOOST_AUTO_TEST_CASE( targetId)
{
  TargetId targetId( validTargetId);

  BOOST_CHECK( validThwId    == targetId.thwId() );
  BOOST_CHECK( validPosition == targetId.position() );
  BOOST_CHECK( validTargetId == targetId.toString() );

  using namespace std::literals::string_view_literals;
  targetId.thwId( "XXXX" );

  BOOST_CHECK( "XXXX"        == targetId.thwId() );
  BOOST_CHECK( validPosition == targetId.position() );
  BOOST_CHECK( validTargetId != targetId.toString() );

  targetId.thwId( validThwId);

  BOOST_CHECK( validThwId    == targetId.thwId() );
  BOOST_CHECK( validPosition == targetId.position() );
  BOOST_CHECK( validTargetId == targetId.toString() );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
