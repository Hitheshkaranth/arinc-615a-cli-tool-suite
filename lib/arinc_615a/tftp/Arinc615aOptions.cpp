// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Struct Arinc615a::Tftp::Arinc615aOptions.
 **/

#include "Arinc615aOptions.hpp"

#include <arinc_649/Arinc649Exception.hpp>

#include <ostream>
#include <format>

namespace Arinc615a::Tftp {

::Tftp::Packets::Options Arinc615aOptions::options() const
{
  ::Tftp::Packets::Options tftpOptions;

  // ARINC 615A Port Option
  if ( port )
  {
    tftpOptions.try_emplace( std::string{ Arinc615aOptions_name( KnownOptions::Port ) }, std::to_string( *port ) );
  }

  // ARINC 615A Part Number Option
  if ( !partNumber.empty() )
  {
    tftpOptions.try_emplace( std::string{ Arinc615aOptions_name( KnownOptions::PartNumber ) }, partNumber );
  }

  // ARINC 615A Checksum Option
  if ( checksum != Arinc649::CheckValue::NoCheckValue )
  {
    tftpOptions.try_emplace( std::string{ Arinc615aOptions_name( checksum.type() ) }, checksum.toString() );
  }

  return tftpOptions;
}

std::string_view Arinc615aOptions_name( const KnownOptions option ) noexcept
{
  switch ( option )
  {
    case KnownOptions::PartNumber:
      return "part number";

    case KnownOptions::ChecksumCrc8:
      return "checksum_1";

    case KnownOptions::ChecksumCrc16:
      return "checksum_2";

    case KnownOptions::ChecksumCrc32:
      return "checksum_3";

    case KnownOptions::ChecksumMd5:
      return "checksum_4";

    case KnownOptions::ChecksumSha1:
      return "checksum_5";

    case KnownOptions::ChecksumSha256:
      return "checksum_6";

    case KnownOptions::ChecksumSha512:
      return "checksum_7";

    case KnownOptions::ChecksumCrc64:
      return "checksum_8";

    case KnownOptions::Port:
      return "port";

    default:
      return {};
  }
}

std::string_view Arinc615aOptions_name( const Arinc649::CheckValueType type ) noexcept
{
  switch ( type )
  {
    using enum Arinc649::CheckValueType;

    case Crc8:
      return "checksum_1";

    case Crc16:
      return "checksum_2";

    case Crc32:
      return "checksum_3";

    case Md5:
      return "checksum_4";

    case Sha1:
      return "checksum_5";

    case Sha256:
      return "checksum_6";

    case Sha512:
      return "checksum_7";

    case Crc64:
      return "checksum_8";

    default:
      return {};
  }
}

std::string Arinc615aOptions_toString( const Arinc615aOptions &options )
{
  if ( !options )
  {
    return "(NONE)";
  }

  std::string retStr;

  if ( options.port )
  {
    retStr += std::format( "[{}:{}]", Arinc615aOptions_name( KnownOptions::Port ), std::to_string( *options.port ) );
  }

  if ( !options.partNumber.empty() )
  {
    retStr += std::format( "[{}:{}]", Arinc615aOptions_name( KnownOptions::PartNumber ), options.partNumber );
  }

  if ( options.checksum.type() != Arinc649::CheckValueType::NotUsed )
  {
    retStr += std::format( "[{}:{}]", Arinc615aOptions_name( options.checksum.type() ), options.checksum.toString() );
  }

  return retStr;
}

std::pair< bool, Arinc649::CheckValue > Arinc615aOptions_checksum( ::Tftp::Packets::Options &options )
{
  try
  {
    // Initialise return value: success, no checksum option found.
    std::pair< bool, Arinc649::CheckValue > retVal{ true, {} };

    for ( const auto type :
      { Arinc649::CheckValueType::Crc8,
        Arinc649::CheckValueType::Crc16,
        Arinc649::CheckValueType::Crc32,
        Arinc649::CheckValueType::Md5,
        Arinc649::CheckValueType::Sha1,
        Arinc649::CheckValueType::Sha256,
        Arinc649::CheckValueType::Sha512,
        Arinc649::CheckValueType::Crc64 } )
    {
      // Try to extract the checksum option
      if (
        // TODO remove std::string generation if P2077R3 is implemented within stdlibc++ (GCC)
        auto option{ options.extract( std::string{ Arinc615aOptions_name( type ) } ) };
        option )
      {
        if ( retVal != std::pair< bool, Arinc649::CheckValue >{ true, {} } )
        {
          // Already some checksum option found
          return { false, {} };
        }

        // use check value if it is valid
        Arinc649::CheckValue checkValue{ type, option.mapped() };

        if ( !checkValue )
        {
          // checksum option is invalid
          return { false, {} };
        }

        retVal = { true, std::move( checkValue ) };
      }
    }

    return retVal;
  }
  catch ( const Arinc649::Arinc649Exception& )
  {
    //! @todo Check if we really need exceptions
    return { false, {} };
  }
}

std::ostream &operator<<( std::ostream &stream, const Arinc615aOptions &options )
{
  return ( stream << Arinc615aOptions_toString( options ) );
}

}
