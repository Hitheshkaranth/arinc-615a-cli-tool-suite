// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Implementation of Unit Tests of Module Arinc615a::Tftp::Arinc615aOptions.
 **/

#include <arinc_615a/tftp/Arinc615aOptions.hpp>

#include <helper/RawData.hpp>

#include <boost/test/unit_test.hpp>

#include <cstdint>

namespace Arinc615a::Tftp {

BOOST_AUTO_TEST_SUITE( Arinc615aTest )
BOOST_AUTO_TEST_SUITE( TftpTest )
BOOST_AUTO_TEST_SUITE( Arinc615aOptionsTest )

//! TftpOptions_name Test
BOOST_AUTO_TEST_CASE( optionName1 )
{
  BOOST_CHECK( Arinc615aOptions_name( KnownOptions::Port ) == "port" );
  BOOST_CHECK( Arinc615aOptions_name( KnownOptions::ChecksumCrc8 ) == "checksum_1" );
  BOOST_CHECK( Arinc615aOptions_name( KnownOptions::ChecksumCrc16 ) == "checksum_2" );
  BOOST_CHECK( Arinc615aOptions_name( KnownOptions::ChecksumCrc32 ) == "checksum_3" );
  BOOST_CHECK( Arinc615aOptions_name( KnownOptions::ChecksumMd5 ) == "checksum_4" );
  BOOST_CHECK( Arinc615aOptions_name( KnownOptions::ChecksumSha1 ) == "checksum_5" );
  BOOST_CHECK( Arinc615aOptions_name( KnownOptions::ChecksumSha256 ) == "checksum_6" );
  BOOST_CHECK( Arinc615aOptions_name( KnownOptions::ChecksumSha512 ) == "checksum_7" );
  BOOST_CHECK( Arinc615aOptions_name( KnownOptions::ChecksumCrc64 ) == "checksum_8" );
  BOOST_CHECK( Arinc615aOptions_name( KnownOptions::PartNumber ) == "part number" );
  // NOLINTNEXTLINE( clang-analyzer-optin.core.EnumCastOutOfRange ): Test
  BOOST_CHECK( Arinc615aOptions_name( static_cast< KnownOptions>( 99 ) ).empty() );
}

//! Arinc615aOptions_name Test
BOOST_AUTO_TEST_CASE( optionName2 )
{
  BOOST_CHECK( Arinc615aOptions_name( Arinc649::CheckValueType::Crc8 ) == "checksum_1" );
  BOOST_CHECK( Arinc615aOptions_name( Arinc649::CheckValueType::Crc16 ) == "checksum_2" );
  BOOST_CHECK( Arinc615aOptions_name( Arinc649::CheckValueType::Crc32 ) == "checksum_3" );
  BOOST_CHECK( Arinc615aOptions_name( Arinc649::CheckValueType::Md5 ) == "checksum_4" );
  BOOST_CHECK( Arinc615aOptions_name( Arinc649::CheckValueType::Sha1 ) == "checksum_5" );
  BOOST_CHECK( Arinc615aOptions_name( Arinc649::CheckValueType::Sha256 ) == "checksum_6" );
  BOOST_CHECK( Arinc615aOptions_name( Arinc649::CheckValueType::Sha512 ) == "checksum_7" );
  BOOST_CHECK( Arinc615aOptions_name( Arinc649::CheckValueType::Crc64 ) == "checksum_8" );

  // NOLINTNEXTLINE( clang-analyzer-optin.core.EnumCastOutOfRange ): Test
  BOOST_CHECK( Arinc615aOptions_name( Arinc649::CheckValueType{ 99 } ).empty() );
}

//! OptionList::checksum Test
BOOST_AUTO_TEST_CASE( checksumTarget )
{
  using Helper::operator ""_b;

  ::Tftp::Packets::Options optionsEmpty{};
  BOOST_CHECK( std::make_pair( true, Arinc649::CheckValue{} ) == Arinc615aOptions_checksum( optionsEmpty ) );
  BOOST_CHECK( optionsEmpty.empty() );

  ::Tftp::Packets::Options options2{ { "key", "value"} , { "checksum_1", "0001" } };
  BOOST_CHECK(
    std::make_pair(
      true,
      Arinc649::CheckValue{ Arinc649::CheckValueType::Crc8, std::vector< std::byte >{ 0x00_b, 0x01_b } } )
    == Arinc615aOptions_checksum( options2 ) );
  BOOST_CHECK( !options2.empty() );
  BOOST_CHECK( !options2.contains( "checksum_1" ) );

  ::Tftp::Packets::Options options3{ { "checksum_8", "0123456789ABCDEF" } };
  BOOST_CHECK(
    std::make_pair(
      true,
      Arinc649::CheckValue{
        Arinc649::CheckValueType::Crc64,
        std::vector< std::byte >{ 0x01_b, 0x23_b, 0x45_b, 0x67_b, 0x89_b, 0xAB_b, 0xCD_b, 0xEF_b } } )
    == Arinc615aOptions_checksum( options3 ) );
  BOOST_CHECK( options3.empty() );
  BOOST_CHECK( !options3.contains( "checksum_8" ) );

  ::Tftp::Packets::Options options4{ { "checksum_8", "0123456789ABCD" } };
  BOOST_CHECK( std::make_pair( false, Arinc649::CheckValue{} ) == Arinc615aOptions_checksum( options4 ) );
  BOOST_CHECK( options4.empty() );
  BOOST_CHECK( !options4.contains( "checksum_8" ) );

  ::Tftp::Packets::Options options5{ { "checksum_1", "0001" }, { "checksum_8", "0123456789ABCDEF" } };
  BOOST_CHECK( std::make_pair( false, Arinc649::CheckValue{} ) == Arinc615aOptions_checksum( options5 ) );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
