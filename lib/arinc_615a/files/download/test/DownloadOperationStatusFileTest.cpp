// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Unit Tests for Class Arinc615a::Files::DownloadOperationStatusFile.
 **/

#include <arinc_615a/files/download/DownloadOperationStatusFile.hpp>

#include <boost/test/unit_test.hpp>

namespace Arinc615a::Files {

BOOST_AUTO_TEST_SUITE( Arinc615aTest )
BOOST_AUTO_TEST_SUITE( FilesTest )
BOOST_AUTO_TEST_SUITE( DownloadOperationStatusFileTest )

//! Download Operation Status File Test Data
static const uint8_t rawFile[]{
  // file length
  0,0,0,54,

  // version
  0x41, 0x34,

  // status code
  0x00, 0x02,

  // status description (length, description)
  4,
  'X', 'X', 'X', 0,

  // counter
  0x00, 0x01,

  // exception timer
  0x00, 0x00,

  // estimated time
  0xFF, 0xFF,

  // download list ratio
  ' ', '1', '0',

  // number of files
  0, 2,

  /* File 1 */
  7, // string length
  'F', 'I', 'L', 'E', '_', '1', 0, // file name

  0x00, 0x02, // file status

  // file status description
  4,
  'Y', 'Y', 'Y', 0,


  /* File 2 */
  7, // string length
  'F', 'I', 'L', 'E', '_', '2', 0, // file name

  0x00, 0x03, // file status

  // file status description
  4,
  'Z', 'Z', 'Z', 0,
};

//! Constructor Test
BOOST_AUTO_TEST_CASE( constructor1)
{
  DownloadOperationStatusFile file( std::as_bytes( std::span{ rawFile } ) );

  BOOST_CHECK( file.protocolVersion() == Arinc615aVersion::Arinc615a34 );
  BOOST_CHECK( file.status().code() == StatusCode::OperationInProgress );
  BOOST_CHECK( file.status().description() == "XXX" );
  BOOST_CHECK( file.status().counter() == 1 );
  BOOST_CHECK( file.status().exceptionTimer() == 0 );
  BOOST_CHECK( file.status().estimatedTime() == -1 );
  BOOST_CHECK( file.status().listRatio().value() == 10 );

  BOOST_REQUIRE( file.status().files().size() == 2 );

  auto fileInfo = file.status().files().begin();

  BOOST_CHECK( fileInfo->filename() == "FILE_1");
  BOOST_CHECK( fileInfo->code() == StatusCode::OperationInProgress);
  BOOST_CHECK( fileInfo->description() == "YYY");

  ++fileInfo;

  BOOST_CHECK( fileInfo->filename() == "FILE_2");
  BOOST_CHECK( fileInfo->code() == StatusCode::OperationCompleted);
  BOOST_CHECK( fileInfo->description() == "ZZZ" );
}

//! Constructor Test
BOOST_AUTO_TEST_CASE( constructor2 )
{
  DownloadOperationStatusFile file{
    Arinc615aVersion::Arinc615a34,
    {
      1,
      0,
      -1,
      StatusCode::OperationInProgress,
      "XXX",
      Information::Ratio{ 10 },
      {
        { "FILE_1", StatusCode::OperationInProgress, "YYY" },
        { "FILE_2", StatusCode::OperationCompleted, "ZZZ" }
      }
    } };

  BOOST_CHECK( file.protocolVersion() == Arinc615aVersion::Arinc615a34 );
  BOOST_CHECK( file.status().code() == StatusCode::OperationInProgress );
  BOOST_CHECK( file.status().description() == "XXX" );
  BOOST_CHECK( file.status().counter() == 1 );
  BOOST_CHECK( file.status().exceptionTimer() == 0 );
  BOOST_CHECK( file.status().estimatedTime() == -1 );
  BOOST_CHECK( file.status().listRatio().value() == 10 );

  BOOST_REQUIRE( file.status().files().size() == 2 );

  auto fileInfo{ file.status().files().begin() };

  BOOST_CHECK( fileInfo->filename() == "FILE_1" );
  BOOST_CHECK( fileInfo->code() == StatusCode::OperationInProgress );
  BOOST_CHECK( fileInfo->description() == "YYY" );

  ++fileInfo;

  BOOST_CHECK( fileInfo->filename() == "FILE_2" );
  BOOST_CHECK( fileInfo->code() == StatusCode::OperationCompleted );
  BOOST_CHECK( fileInfo->description() == "ZZZ" );

  BOOST_CHECK( std::ranges::equal( std::as_bytes( std::span{ rawFile } ), static_cast< Helper::RawData >( file ) ) );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
