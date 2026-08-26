// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of unit tests for class Arinc615a::Information::Ratio.
 **/

#include <arinc_615a/information/Ratio.hpp>

#include <arinc_615a/Arinc615aException.hpp>

#include <boost/test/unit_test.hpp>

namespace Arinc615a::Information {

BOOST_AUTO_TEST_SUITE( Arinc615aTest )
BOOST_AUTO_TEST_SUITE( InformationTest )
BOOST_AUTO_TEST_SUITE( RatioTest )

//! Constructor test
BOOST_AUTO_TEST_CASE( constructor1 )
{
  Ratio ratio1{};
  BOOST_CHECK( ratio1.value() == 0 );

  Ratio ratio2{ 25 };
  BOOST_CHECK( ratio2.value() == 25 );

  Ratio ratio3{ 100 };
  BOOST_CHECK( ratio3.value() == 100 );

  BOOST_CHECK_THROW( Ratio{ 101 }, Arinc615aException );
}

//! Ratio Test
BOOST_AUTO_TEST_CASE( combined )
{
  for ( unsigned int i = 0; i <= 100; ++i )
  {
    Ratio ratio1{ i };
    BOOST_CHECK( ratio1.value() == i );

    Ratio ratio2{};
    BOOST_CHECK( ratio2.value() == 0 );
    ratio2 = i;
    BOOST_CHECK( ratio2.value() == i );
  }
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
