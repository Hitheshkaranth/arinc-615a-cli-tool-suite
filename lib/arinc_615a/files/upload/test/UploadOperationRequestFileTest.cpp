// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of unit tests for class Arinc615a::Files::UploadOperationRequestFile.
 **/

#include <arinc_615a/files/upload/UploadOperationRequestFile.hpp>

#include <boost/test/unit_test.hpp>

namespace Arinc615a::Files {

BOOST_AUTO_TEST_SUITE( Arinc615aTest )
BOOST_AUTO_TEST_SUITE( FilesTest )
BOOST_AUTO_TEST_SUITE( UploadOperationRequestFileTest )

//! Raw Upload Operation Request Test File
static const uint8_t rawUploadOperationRequestFile[]{
  0,0,0,32,

  0x41, 0x34,

  0,2,

  /* Header File 1 */
  5,
  'H', 'F', '_', '1', 0,

  5,
  'P', 'N', '_', '1', 0,


  /* Header File 2 */
  5,
  'H', 'F', '_', '2', 0,

  5,
  'P', 'N', '_', '2', 0,
};

//! UploadOperationRequestFile Constructor Test
BOOST_AUTO_TEST_CASE( constructor )
{
  UploadOperationRequestFile file( std::as_bytes( std::span{ rawUploadOperationRequestFile } ) );

  BOOST_CHECK( file.protocolVersion() == Arinc615aVersion::Arinc615a34 );

  BOOST_REQUIRE( file.loads().size() == 2U );

  auto uploadInfo = file.loads().begin();

  BOOST_CHECK( uploadInfo->headerFilename == "HF_1" );
  BOOST_CHECK( uploadInfo->partNumber == "PN_1" );

  ++uploadInfo;

  BOOST_CHECK( uploadInfo->headerFilename == "HF_2" );
  BOOST_CHECK( uploadInfo->partNumber == "PN_2" );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
