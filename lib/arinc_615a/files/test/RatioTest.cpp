// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Unit Tests of Module Arinc615a::Files Utils.
 **/

#include <arinc_615a/files/Ratio.hpp>

#include <arinc_615a/information/Ratio.hpp>

#include <arinc_615a/Arinc615aException.hpp>

#include <boost/test/unit_test.hpp>

#include <array>

namespace Arinc615a::Files {

BOOST_AUTO_TEST_SUITE( Arinc615aTest )
BOOST_AUTO_TEST_SUITE( FilesTest )
BOOST_AUTO_TEST_SUITE( RatioTest )

//! Ratio decode test
BOOST_AUTO_TEST_CASE( RatioDecode )
{
  BOOST_CHECK_THROW( boost::ignore_unused( Ratio_decode( {} ) ), Arinc615aException );
  BOOST_CHECK_THROW(
    boost::ignore_unused( Ratio_decode( std::as_bytes( std::span{ "1", 1 } ) ) ),
    Arinc615aException );
  BOOST_CHECK_THROW(
    boost::ignore_unused( Ratio_decode( std::as_bytes( std::span{ "11", 2 } ) ) ),
    Arinc615aException );

  auto [remaining1, ratio1 ]{ Ratio_decode( std::as_bytes( std::span{ "  0", 3 } ) ) };
  BOOST_CHECK( ratio1.value() == 0 );
  BOOST_CHECK( remaining1.empty() );

  auto [remaining2, ratio2 ]{ Ratio_decode( std::as_bytes( std::span{ " 25", 3 } ) ) };
  BOOST_CHECK( ratio2.value() == 25 );
  BOOST_CHECK( remaining2.empty() );

  auto [remaining3, ratio3 ]{ Ratio_decode( std::as_bytes( std::span{ "100", 3 } ) ) };
  BOOST_CHECK( ratio3.value() == 100 );
  BOOST_CHECK( remaining3.empty() );

  BOOST_CHECK_THROW(
    boost::ignore_unused( Ratio_decode( std::as_bytes( std::span{ "101", 3 } ) ) ),
    Arinc615aException );
}

//! Ratio encode test
BOOST_AUTO_TEST_CASE( RatioEncode )
{
  using Helper::operator ""_b;

  const auto rawRatio1{ Ratio_encode( Information::Ratio{} ) };
  BOOST_CHECK( rawRatio1.size() == 3U );
  BOOST_CHECK( std::ranges::equal( rawRatio1, std::to_array< std::byte >( { ' '_b, ' '_b, '0'_b } ) ) );

  const auto rawRatio2{ Ratio_encode( Information::Ratio{ 25 } ) };
  BOOST_CHECK( rawRatio2.size() == 3U );
  BOOST_CHECK( std::ranges::equal( rawRatio2, std::to_array< std::byte >( { ' '_b, '2'_b, '5'_b } ) ) );

  const auto rawRatio3{ Ratio_encode( Information::Ratio{ 100 } ) };
  BOOST_CHECK( rawRatio3.size() == 3U );
  BOOST_CHECK( std::ranges::equal( rawRatio3, std::to_array< std::byte >( { '1'_b, '0'_b, '0'_b } ) ) );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
