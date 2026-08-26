// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Unit Tests for Class
 *   Arinc615a::Find::TargetInformationTest.
 **/

#include <arinc_615a/find/TargetInformation.hpp>

#include <boost/test/unit_test.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <iostream>
#include <sstream>

namespace Arinc615a::Find {

BOOST_AUTO_TEST_SUITE( Arinc615aTest )
BOOST_AUTO_TEST_SUITE( FindTest )
BOOST_AUTO_TEST_SUITE( TargetInformationTest )

//! constructor test
BOOST_AUTO_TEST_CASE( constructor1 )
{
  TargetInformation targetInformation;

  BOOST_CHECK( targetInformation.thwId.empty());
  BOOST_CHECK( targetInformation.thwTypeName.empty());
  BOOST_CHECK( targetInformation.thwPosition.empty());
  BOOST_CHECK( targetInformation.literalName.empty());
  BOOST_CHECK( targetInformation.manufacturerCode.empty());
}

//! json test data
char const findJson[] =
{
  "{"
    "\"find_information\":"
  "  { "
        "\"thwId\"  :         \"IDX1\","
        "\"thwTypeName\":     \"TYPE1\","
        "\"thwPosition\":     \"POS1\","
        "\"literalName\":     \"LITERAL1\","
        "\"manufacturerCode\": \"MANUFACTURER1\""
  "  },"
  "\"find_information\":"
  "  { "
        "\"thwId\"  :         \"IDX2\","
        "\"thwTypeName\":     \"TYPE2\","
        "\"thwPosition\":     \"POS2\","
        "\"literalName\":     \"LITERAL2\","
        "\"manufacturerCode\": \"MANUFACTURER2\""
  "  }"
  "}"
};

//! constructor test
BOOST_AUTO_TEST_CASE( constructor2 )
{
  boost::property_tree::ptree properties{};
  std::istringstream jsonStream( findJson );
  boost::property_tree::json_parser::read_json( jsonStream, properties );

  auto &subTree( properties.get_child( "find_information" ) );

  TargetInformation targetInformation( subTree);

  BOOST_CHECK( targetInformation.thwId == "IDX1" );
  BOOST_CHECK( targetInformation.thwTypeName == "TYPE1" );
  BOOST_CHECK( targetInformation.thwPosition == "POS1" );
  BOOST_CHECK( targetInformation.literalName == "LITERAL1" );
  BOOST_CHECK( targetInformation.manufacturerCode == "MANUFACTURER1" );
  BOOST_CHECK( targetInformation.targetId() == "IDX1_POS1" );
}

//! constructor test
BOOST_AUTO_TEST_CASE( constructor3 )
{
  boost::property_tree::ptree properties;
  std::istringstream jsonStream( findJson);
  boost::property_tree::json_parser::read_json( jsonStream, properties );

  auto targetsInformation{ TargetInformation::targetsInformation( properties ) };

  BOOST_REQUIRE( targetsInformation.size() == 2U );

  auto targetInformation( targetsInformation.cbegin() );

  BOOST_CHECK( targetInformation->thwId == "IDX1" );
  BOOST_CHECK( targetInformation->thwTypeName == "TYPE1" );
  BOOST_CHECK( targetInformation->thwPosition == "POS1" );
  BOOST_CHECK( targetInformation->literalName == "LITERAL1" );
  BOOST_CHECK( targetInformation->manufacturerCode == "MANUFACTURER1" );
  BOOST_CHECK( targetInformation->targetId() == "IDX1_POS1" );

  ++targetInformation;

  BOOST_CHECK( targetInformation->thwId == "IDX2" );
  BOOST_CHECK( targetInformation->thwTypeName == "TYPE2" );
  BOOST_CHECK( targetInformation->thwPosition == "POS2" );
  BOOST_CHECK( targetInformation->literalName == "LITERAL2" );
  BOOST_CHECK( targetInformation->manufacturerCode == "MANUFACTURER2" );
  BOOST_CHECK( targetInformation->targetId() == "IDX2_POS2" );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
