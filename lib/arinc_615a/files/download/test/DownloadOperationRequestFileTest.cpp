// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of unit tests for class Arinc615a::Files::DownloadOperationRequestFile.
 **/

#include <arinc_615a/files/download/DownloadOperationRequestFile.hpp>

#include <helper/RawData.hpp>

#include <boost/test/unit_test.hpp>

namespace Arinc615a::Files {

BOOST_AUTO_TEST_SUITE( Arinc615aTest )
BOOST_AUTO_TEST_SUITE( FilesTest )
BOOST_AUTO_TEST_SUITE( DownloadOperationRequestFileTest )

//! Raw Download Operation Request Test File
static const uint8_t rawDownloadOperationRequestFile[]{
  0,0,0,29,

  0x41, 0x34,

  0,2,

  /* File 1 */
  7,
  'F', 'I', 'L', 'E', '_', '1', 0,

  /* File 2 */
  7,
  'F', 'I', 'L', 'E', '_', '2', 0,


  4,
  'T', 'E', 'S', 'T'
};

BOOST_AUTO_TEST_CASE( constructor)
{
  using Helper::operator ""_b;

  DownloadOperationRequestFile file( std::as_bytes( std::span{ rawDownloadOperationRequestFile } ) );

  BOOST_CHECK( file.protocolVersion() == Arinc615aVersion::Arinc615a34 );

  BOOST_REQUIRE( file.files().size() == 2);

  auto downloadInfo = file.files().begin();

  BOOST_CHECK( *downloadInfo == "FILE_1");

  ++downloadInfo;

  BOOST_CHECK( *downloadInfo == "FILE_2");

  Helper::RawData udata{ 'T'_b, 'E'_b, 'S'_b, 'T'_b };
  BOOST_CHECK( file.userDefinedData() == udata );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
