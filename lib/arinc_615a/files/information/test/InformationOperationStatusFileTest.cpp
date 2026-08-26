// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of unit tests for class Arinc615a::Files::FileInitializationFile.
 **/

#include <arinc_615a/files/information/InformationOperationStatusFile.hpp>

#include <boost/test/unit_test.hpp>

namespace Arinc615a::Files {

BOOST_AUTO_TEST_SUITE( Arinc615aTest )
BOOST_AUTO_TEST_SUITE( FilesTest )
BOOST_AUTO_TEST_SUITE( InformationOperationStatusFileTest )

//! Raw Information Operation Status Test File
static const uint8_t rawInformationOperationStatusFile[]{
  0,0,0,19,

  0x41, 0x34,

  0x00, 0x02,

  0x00, 0x01,

  0x00, 0x00,

  0xFF, 0xFF,

  4,
  'X', 'X', 'X', 0
};

//! Constructor Test
BOOST_AUTO_TEST_CASE( constructor )
{
  InformationOperationStatusFile file( std::as_bytes( std::span{ rawInformationOperationStatusFile } ) );

  BOOST_CHECK( file.protocolVersion() == Arinc615aVersion::Arinc615a34 );
  BOOST_CHECK( file.status().counter() == 2);
  BOOST_CHECK( file.status().exceptionTimer() == 0);
  BOOST_CHECK( file.status().estimatedTime() == -1);
  BOOST_CHECK( file.status().code() == StatusCode::OperationAccepted);
  BOOST_CHECK( file.status().description() == "XXX");
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
