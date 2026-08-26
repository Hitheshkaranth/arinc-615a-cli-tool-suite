// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of unit tests of class Arinc615a::Files::ProtocolFilename.
 **/

#include <arinc_615a/files/ProtocolFilename.hpp>

#include <boost/test/unit_test.hpp>

namespace Arinc615a::Files {

BOOST_AUTO_TEST_SUITE( Arinc615aTest )
BOOST_AUTO_TEST_SUITE( FilesTest )
BOOST_AUTO_TEST_SUITE( ProtocolFilenameTest )

//! split test
BOOST_AUTO_TEST_CASE( split )
{
  const auto [ targetId, postion ]{ ProtocolFilename::split( "1234_ABC.LUS" ) };

  BOOST_CHECK( targetId == "1234_ABC" );
  BOOST_CHECK( postion == "LUS" );

  const auto [ targetId2, postion2 ]{ ProtocolFilename::split( "1234_ABC_LUS" ) };

  BOOST_CHECK( targetId2.empty() );
  BOOST_CHECK( postion2.empty() );
}

//! is protocol filename test
BOOST_AUTO_TEST_CASE( isProtocolFilename )
{
  BOOST_CHECK( ProtocolFilename::isProtocolFilename( "1234_ABC.LUS" ) );
  BOOST_CHECK( !ProtocolFilename::isProtocolFilename( "1234_ABC" ) );
  BOOST_CHECK( !ProtocolFilename::isProtocolFilename( "1234_ABC.XXX" ) );
  BOOST_CHECK( ProtocolFilename::isProtocolFilename( "1234_.LUS" ) );
  BOOST_CHECK( !ProtocolFilename::isProtocolFilename( "1234.LUS" ) );
}

//! Get extension test
BOOST_AUTO_TEST_CASE( extension )
{
  BOOST_CHECK( ProtocolFilename::extension( ProtocolFileType::DownloadStatus ) == "LNS" );
  BOOST_CHECK( ProtocolFilename::extension( ProtocolFileType::UploadInitialization ) == "LUI" );
  BOOST_CHECK( ProtocolFilename::extension( ProtocolFileType::DownloadStatus ) == "LNS" );
  BOOST_CHECK( ProtocolFilename::extension( ProtocolFileType::Invalid ).empty() );
}

//! get file type test
BOOST_AUTO_TEST_CASE( fileType )
{
  BOOST_CHECK( ProtocolFilename::fileType( "LUS" ) == ProtocolFileType::UploadStatus );
  BOOST_CHECK( ProtocolFilename::fileType( "LUI" ) == ProtocolFileType::UploadInitialization );
  BOOST_CHECK( ProtocolFilename::fileType( "LNS" ) == ProtocolFileType::DownloadStatus );
  BOOST_CHECK( ProtocolFilename::fileType( "XXX" ) == ProtocolFileType::Invalid );
}

//! constructor test
BOOST_AUTO_TEST_CASE( constructor1 )
{
  ProtocolFilename file( "1234", "ABC", ProtocolFileType::UploadStatus );

  BOOST_CHECK( file.targetId() == "1234_ABC" );
  BOOST_CHECK( file.thwId() == "1234" );
  BOOST_CHECK( file.position() == "ABC" );
  BOOST_CHECK( file.fileType() == ProtocolFileType::UploadStatus );
}

//! constructor test
BOOST_AUTO_TEST_CASE( constructor2 )
{
  ProtocolFilename file( "1234_ABC", ProtocolFileType::UploadStatus );

  BOOST_CHECK( file.targetId() == "1234_ABC" );
  BOOST_CHECK( file.thwId() == "1234" );
  BOOST_CHECK( file.position() == "ABC" );
  BOOST_CHECK( file.fileType() == ProtocolFileType::UploadStatus );
}

//! constructor test
BOOST_AUTO_TEST_CASE( constructor3 )
{
  ProtocolFilename file( TargetId( "1234_ABC" ), ProtocolFileType::UploadStatus );

  BOOST_CHECK( file.targetId() == "1234_ABC" );
  BOOST_CHECK( file.thwId() == "1234" );
  BOOST_CHECK( file.position() == "ABC" );
  BOOST_CHECK( file.fileType() == ProtocolFileType::UploadStatus );
}

//! constructor test
BOOST_AUTO_TEST_CASE( constructor4 )
{
  ProtocolFilename file( "1234_ABC.LUS" );

  BOOST_CHECK( file.targetId() == "1234_ABC" );
  BOOST_CHECK( file.thwId() == "1234" );
  BOOST_CHECK( file.position() == "ABC" );
  BOOST_CHECK( file.fileType() == ProtocolFileType::UploadStatus );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
