// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Unit Tests for Class Arinc615a::Files::FileInitializationFile.
 **/

#include <arinc_615a/files/InitializationFile.hpp>

#include <arinc_615a/Arinc615aException.hpp>

#include <helper/RawData.hpp>

#include <boost/test/unit_test.hpp>

#include <array>

namespace Arinc615a::Files {

BOOST_AUTO_TEST_SUITE( Arinc615aTest )
BOOST_AUTO_TEST_SUITE( FilesTest )
BOOST_AUTO_TEST_SUITE( InitializationFileTest )

//! Raw Initialisation File
static const uint8_t initializationFileRaw[]{
  0, 0, 0, 13, // length

  0x41, 0x34, // protocol version

  0x00, 0x01, // response code

  4,               // status message length
  'X', 'X', 'X', 0 // status message
};

//! Default Constructor Test
BOOST_AUTO_TEST_CASE( constructorDefault)
{
  InitializationFile file{};
  BOOST_CHECK( file.protocolVersion() == Arinc615aVersion::Arinc615a34 );
  BOOST_CHECK( file.response().code() == OperationAcceptanceStatusCode::OperationAccepted );
  BOOST_CHECK( file.response().description().empty() );
}

BOOST_AUTO_TEST_CASE( constructor1)
{
  using Helper::operator ""_b;

  InitializationFile file{ std::as_bytes( std::span{ initializationFileRaw } ) };

  BOOST_CHECK( file.protocolVersion() == Arinc615aVersion::Arinc615a34 );
  BOOST_CHECK( file.response().code() == OperationAcceptanceStatusCode::OperationAccepted);
  BOOST_CHECK( file.response().description() == "XXX");

  BOOST_CHECK( (const_cast< const InitializationFile&>( file).response()
    == Information::InitializationResponse{
    OperationAcceptanceStatusCode::OperationAccepted,
    "XXX" } ) );

  BOOST_CHECK_THROW(
    (InitializationFile{ Helper::RawData{ 0_b, 0_b, 0_b, 4_b } } ),
    Arinc615aException );

  BOOST_CHECK_THROW(
    (InitializationFile{ Helper::RawData{
      0_b, 0_b, 0_b, 7_b,
      0x41_b, 0x34_b } } ),
    Arinc615aException );

  BOOST_CHECK_THROW(
    (InitializationFile{ Helper::RawData{
      0_b, 0_b, 0_b, 8_b,
      0x41_b, 0x34_b,
      0x00_b, 0x01_b } } ),
    Arinc615aException);

  BOOST_CHECK_THROW(
    (InitializationFile{ Helper::RawData{
      0_b, 0_b, 0_b, 15_b,

      0x41_b, 0x34_b,

      0x00_b, 0x01_b,

      4_b,
      'X'_b, 'X'_b, 'X'_b, 0_b,

      0xde_b, 0xad_b} } ),
    Arinc615aException );
}

BOOST_AUTO_TEST_CASE( constructor2 )
{
  using Helper::operator ""_b;

  InitializationFile file{
    Arinc615aVersion::Arinc615a2,
    Information::InitializationResponse{
      OperationAcceptanceStatusCode::OperationDenied,
      "XXX" } };

  BOOST_CHECK( file.protocolVersion() == Arinc615aVersion::Arinc615a2);
  BOOST_CHECK( file.response().code() == OperationAcceptanceStatusCode::OperationDenied);
  BOOST_CHECK( file.response().description() == "XXX");

  BOOST_CHECK(
    ( static_cast< Helper::RawData>( file)
    == Helper::RawData{
      0_b, 0_b, 0_b, 13_b,

      0x41_b, 0x33_b,

      0x10_b, 0x00_b,

      4_b,
      'X'_b, 'X'_b, 'X'_b, 0_b
  } ) );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
