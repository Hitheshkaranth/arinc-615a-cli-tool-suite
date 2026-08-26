// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of unit tests for class Arinc615a::Files::UploadOperationStatusFile.
 **/

#include <arinc_615a/files/upload/UploadOperationStatusFile.hpp>

#include <boost/test/unit_test.hpp>

namespace Arinc615a::Files {

BOOST_AUTO_TEST_SUITE( Arinc615aTest )
BOOST_AUTO_TEST_SUITE( FilesTest )
BOOST_AUTO_TEST_SUITE( UploadOperationStatusFileTest )

//! Raw Upload Operation Status Test File
static const uint8_t rawUploadOperationStatusFile[]{
  0,0,0,68,

  0x41, 0x34,

  0x00, 0x02,

  4,
  'X', 'X', 'X', 0,

  0x00, 0x01,

  0x00, 0x00,

  0xFF, 0xFF,

  ' ', '1', '0',

  0, 2,

  /* Header File 1 */
  5,
  'H', 'F', '_', '1', 0,

  5,
  'P', 'N', '_', '1', 0,

  ' ', ' ', '0',

  0x00, 0x02,

  4,
  'Y', 'Y', 'Y', 0,


  /* Header File 2 */
  5,
  'H', 'F', '_', '2', 0,

  5,
  'P', 'N', '_', '2', 0,

  '1', '0', '0',

  0x00, 0x03,

  4,
  'Z', 'Z', 'Z', 0
};

//! UploadOperationStatusFile Constructor Test
BOOST_AUTO_TEST_CASE( constructor )
{
  UploadOperationStatusFile file( std::as_bytes( std::span{ rawUploadOperationStatusFile } ) );

  BOOST_CHECK( file.protocolVersion() == Arinc615aVersion::Arinc615a34 );
  BOOST_CHECK( file.status().code() == StatusCode::OperationInProgress );
  BOOST_CHECK( file.status().description() == "XXX" );
  BOOST_CHECK( file.status().counter() == 1 );
  BOOST_CHECK( file.status().exceptionTimer() == 0 );
  BOOST_CHECK( file.status().estimatedTime() == -1 );
  BOOST_CHECK( file.status().listRatio().value() == 10 );

  BOOST_REQUIRE( file.status().loads().size() == 2 );

  auto loadInfo = file.status().loads().begin();

  BOOST_CHECK( loadInfo->headerFilename() == "HF_1" );
  BOOST_CHECK( loadInfo->partNumber() == "PN_1" );
  BOOST_CHECK( loadInfo->ratio().value() == 0 );
  BOOST_CHECK( loadInfo->code() == StatusCode::OperationInProgress );
  BOOST_CHECK( loadInfo->description() == "YYY" );

  ++loadInfo;

  BOOST_CHECK( loadInfo->headerFilename() == "HF_2" );
  BOOST_CHECK( loadInfo->partNumber() == "PN_2" );
  BOOST_CHECK( loadInfo->ratio().value() == 100 );
  BOOST_CHECK( loadInfo->code() == StatusCode::OperationCompleted );
  BOOST_CHECK( loadInfo->description() == "ZZZ" );

  Helper::RawData rawFile2( file );

  UploadOperationStatusFile file2( rawFile2 );

  BOOST_CHECK( file2.protocolVersion() == Arinc615aVersion::Arinc615a34 );
  BOOST_CHECK( file2.status().code() == StatusCode::OperationInProgress );
  BOOST_CHECK( file2.status().description() == "XXX" );
  BOOST_CHECK( file2.status().counter() == 1 );
  BOOST_CHECK( file2.status().exceptionTimer() == 0 );
  BOOST_CHECK( file2.status().estimatedTime() == -1 );
  BOOST_CHECK( file2.status().listRatio().value() == 10 );

  BOOST_REQUIRE( file2.status().loads().size() == 2 );

  auto loadInfo2 = file2.status().loads().begin();

  BOOST_CHECK( loadInfo2->headerFilename() == "HF_1" );
  BOOST_CHECK( loadInfo2->partNumber() == "PN_1" );
  BOOST_CHECK( loadInfo2->ratio().value() == 0 );
  BOOST_CHECK( loadInfo2->code() == StatusCode::OperationInProgress );
  BOOST_CHECK( loadInfo2->description() == "YYY" );

  ++loadInfo2;

  BOOST_CHECK( loadInfo2->headerFilename() == "HF_2" );
  BOOST_CHECK( loadInfo2->partNumber() == "PN_2" );
  BOOST_CHECK( loadInfo2->ratio().value() == 100 );
  BOOST_CHECK( loadInfo2->code() == StatusCode::OperationCompleted );
  BOOST_CHECK( loadInfo2->description() == "ZZZ" );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
