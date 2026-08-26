// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of unit tests for class Arinc615a::Files::DownloadOperationListFile.
 **/

#include <arinc_615a/files/download/DownloadOperationListFile.hpp>

#include <boost/test/unit_test.hpp>

#include <algorithm>

namespace Arinc615a::Files {

BOOST_AUTO_TEST_SUITE( Arinc615aTest )
BOOST_AUTO_TEST_SUITE( FilesTest )
BOOST_AUTO_TEST_SUITE( DownloadOperationListFileTest )

//! Raw Download Operation List File
static constexpr uint8_t rawDownloadOperationListFile[]{
  0,0,0,40,  // File Size

  0x41, 0x34, // File Version

  0, 2, // Number of Files

  /* File 1 */
  7,
  'F', 'I', 'L', 'E', '_', '1', 0,

  7,
  'D', 'A', 'T', 'A', '_', '1', 0,

  /* File 2 */
  7,
  'F', 'I', 'L', 'E', '_', '2', 0,

  7,
  'D', 'A', 'T', 'A', '_', '2', 0,
};

//! DownloadOperationListFile Constructor Test
BOOST_AUTO_TEST_CASE( constructor )
{
  DownloadOperationListFile file( std::as_bytes( std::span{ rawDownloadOperationListFile } ) );

  BOOST_CHECK( file.protocolVersion() == Arinc615aVersion::Arinc615a34 );

  BOOST_REQUIRE( file.files().size() == 2 );

  auto listInfo = file.files().begin();

  BOOST_CHECK( listInfo->filename == "FILE_1" );
  BOOST_CHECK( listInfo->description == "DATA_1" );

  ++listInfo;

  BOOST_CHECK( listInfo->filename == "FILE_2" );
  BOOST_CHECK( listInfo->description == "DATA_2" );
}

//! DownloadOperationListFile Encode Test
BOOST_AUTO_TEST_CASE( encode )
{
  DownloadOperationListFile file;
  BOOST_CHECK( file.protocolVersion() == Arinc615aVersion::Arinc615a34 );
  BOOST_CHECK( file.files().empty() );

  file.files().emplace_back( "FILE_1", "DATA_1" );
  file.files().emplace_back( "FILE_2", "DATA_2" );

  BOOST_CHECK( file.files().size() == 2 );

  auto rawFile( static_cast< Helper::RawData >( file ) );
  BOOST_CHECK( std::ranges::equal( rawFile, std::as_bytes( std::span{ rawDownloadOperationListFile } ) ) );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
