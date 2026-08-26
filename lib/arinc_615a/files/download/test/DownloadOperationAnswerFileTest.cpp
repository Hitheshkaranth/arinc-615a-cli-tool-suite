// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of unit tests for class Arinc615a::Files::DownloadOperationAnswerFile.
 **/

#include <arinc_615a/files/download/DownloadOperationAnswerFile.hpp>

#include <boost/test/unit_test.hpp>

namespace Arinc615a::Files {

BOOST_AUTO_TEST_SUITE( Arinc615aTest )
BOOST_AUTO_TEST_SUITE( FilesTest )
BOOST_AUTO_TEST_SUITE( DownloadOperationAnswerFileTest )

//! Raw Download Operation Answer Test File
static const uint8_t rawDownloadOperationAnswerFile[]{
  0,0,0,24,

  0x41, 0x34,

  0,2,

  /* File 1 */
  7,
  'F', 'I', 'L', 'E', '_', '1', 0,

  /* File 2 */
  7,
  'F', 'I', 'L', 'E', '_', '2', 0,
};

//! Constructor Test
BOOST_AUTO_TEST_CASE( constructor )
{
  DownloadOperationAnswerFile file( std::as_bytes( std::span{ rawDownloadOperationAnswerFile } ));

  BOOST_CHECK( file.protocolVersion() == Arinc615aVersion::Arinc615a34 );

  BOOST_REQUIRE( file.files().size() == 2 );

  auto answer( file.files().begin() );

  BOOST_CHECK( *answer == "FILE_1" );

  ++answer;

  BOOST_CHECK( *answer == "FILE_2" );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
