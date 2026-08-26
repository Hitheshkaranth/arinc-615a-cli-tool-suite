// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of unit tests for class Arinc615a::Files::LoadConfigurationListFile.
 **/

#include <arinc_615a/files/information/LoadConfigurationListFile.hpp>

#include <arinc_615a/information/TargetHardware.hpp>
#include <arinc_615a/information/PartNumber.hpp>

#include <boost/test/unit_test.hpp>

namespace Arinc615a::Files {

BOOST_AUTO_TEST_SUITE( Arinc615aTest )
BOOST_AUTO_TEST_SUITE( FilesTest )
BOOST_AUTO_TEST_SUITE( LoadConfigurationListFileTest )

//! Raw Load Configuration List Test File
static const uint8_t rawLoadConfigurationListFile[]{
  0,0,0,134,

  0x41, 0x34,

  0,2,

  /* THW 1 */
  6,
  'L', 'I', 'T', '_', '1', 0,

  5,
  'S', 'N', '_', '1', 0,

  0,2,

  7,
  'P', 'N', '_', '1', '_', '1', 0,

  7,
  'A', 'M', '_', '1', '_', '1', 0,

  7,
  'P', 'D', '_', '1', '_', '1', 0,


  7,
  'P', 'N', '_', '1', '_', '2', 0,

  7,
  'A', 'M', '_', '1', '_', '2', 0,

  7,
  'P', 'D', '_', '1', '_', '2', 0,


  /* THW 2 */
  6,
  'L', 'I', 'T', '_', '2', 0,

  5,
  'S', 'N', '_', '2', 0,

  0,2,

  7,
  'P', 'N', '_', '2', '_', '1', 0,

  7,
  'A', 'M', '_', '2', '_', '1', 0,

  7,
  'P', 'D', '_', '2', '_', '1', 0,


  7,
  'P', 'N', '_', '2', '_', '2', 0,

  7,
  'A', 'M', '_', '2', '_', '2', 0,

  7,
  'P', 'D', '_', '2', '_', '2', 0
};

//! Default Construction Test
BOOST_AUTO_TEST_CASE( constructorDefault )
{
  LoadConfigurationListFile file{};
  BOOST_CHECK( file.protocolVersion() == Arinc615aVersion::Arinc615a34 );
  BOOST_REQUIRE( file.targetsHardware().empty() );
}

//! LoadConfigurationListFile Constructor Test
BOOST_AUTO_TEST_CASE( constructor )
{
  LoadConfigurationListFile file( std::as_bytes( std::span{ rawLoadConfigurationListFile } ) );

  BOOST_CHECK( file.protocolVersion() == Arinc615aVersion::Arinc615a34 );
  BOOST_REQUIRE( file.targetsHardware().size() == 2);

  auto thw = file.targetsHardware().begin();

  BOOST_CHECK( thw->literalName == "LIT_1" );
  BOOST_CHECK( thw->serialNumber == "SN_1" );
  BOOST_REQUIRE( thw->partNumbers.size() == 2U );

  auto partNumber = thw->partNumbers.begin();

  BOOST_CHECK( partNumber->partNumber == "PN_1_1" );
  BOOST_CHECK( partNumber->amendment == "AM_1_1" );
  BOOST_CHECK( partNumber->partDesignation == "PD_1_1" );

  ++partNumber;

  BOOST_CHECK( partNumber->partNumber == "PN_1_2" );
  BOOST_CHECK( partNumber->amendment == "AM_1_2" );
  BOOST_CHECK( partNumber->partDesignation == "PD_1_2" );

  ++thw;

  BOOST_CHECK( thw->literalName == "LIT_2" );
  BOOST_CHECK( thw->serialNumber == "SN_2" );
  BOOST_REQUIRE( thw->partNumbers.size() == 2U );

  partNumber = thw->partNumbers.begin();

  BOOST_CHECK( partNumber->partNumber == "PN_2_1" );
  BOOST_CHECK( partNumber->amendment == "AM_2_1" );
  BOOST_CHECK( partNumber->partDesignation == "PD_2_1" );

  ++partNumber;

  BOOST_CHECK( partNumber->partNumber == "PN_2_2" );
  BOOST_CHECK( partNumber->amendment == "AM_2_2" );
  BOOST_CHECK( partNumber->partDesignation == "PD_2_2" );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
