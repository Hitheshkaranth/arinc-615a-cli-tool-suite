// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Module Arinc615a::Files String.
 **/

#include "String.hpp"

#include <arinc_615a/Arinc615aException.hpp>

#include <helper/Exception.hpp>
#include <helper/SafeCast.hpp>

#include <boost/throw_exception.hpp>

namespace Arinc615a::Files {

std::tuple< Helper::ConstRawDataSpan, std::string_view > String_decode( Helper::ConstRawDataSpan rawData )
{
  if ( rawData.empty() )
  {
    BOOST_THROW_EXCEPTION( Arinc615aException{} << Helper::AdditionalInfo{ "invalid start and end supplied" } );
  }

  // decode string length
  uint8_t stringLength{};
  std::tie( rawData, stringLength ) = Helper::RawData_getInt< uint8_t>( rawData );

  // check string length
  if ( rawData.size() < stringLength )
  {
    BOOST_THROW_EXCEPTION( Arinc615aException{} << Helper::AdditionalInfo{ "string length inconsistent" } );
  }

  // clear the string
  std::string_view decodedString;

  // extract string if length is non-null
  if ( stringLength > 0 )
  {
    // decode string
    std::tie( rawData, decodedString ) = Helper::RawData_getString( rawData, stringLength );

    // search for terminating 0-character
    const size_t nullTermPos{ decodedString.find( '\0' ) };
    if ( std::string::npos == nullTermPos )
    {
      BOOST_THROW_EXCEPTION( Arinc615aException{} << Helper::AdditionalInfo{ "string not NULL terminated" } );
    }

    // resize string to actual length
    decodedString = decodedString.substr( 0, nullTermPos );
  }

  return { rawData, decodedString };
}

Helper::RawData String_encode( const std::string_view stringToEncode, const uint8_t fixedLength )
{
  // raw string size is string size + terminating NULL-character - in case of empty string length is 0
  const auto rawStringSize{ stringToEncode.empty() ? 0U : stringToEncode.size() + 1U };

  if ( ( rawStringSize >= 255U ) || ( ( fixedLength != 0U ) && ( rawStringSize > fixedLength ) ) )
  {
    BOOST_THROW_EXCEPTION( Arinc615aException{} << Helper::AdditionalInfo{ "string too long" } );
  }

  // size of raw string is length field (1 byte) + ( fixed length or rawStringSize (incl. NULL-char) )
  Helper::RawData rawString( fixedLength != 0 ? 1U + fixedLength : 1U + rawStringSize );

  // length
  auto remaining{ Helper::RawData_setInt< uint8_t >(
    rawString,
    fixedLength != 0U ? fixedLength : Helper::safeCast< uint8_t >( rawStringSize ) ) };

  // copy string
  remaining = Helper::RawData_setString( remaining, stringToEncode );
  if ( !stringToEncode.empty() )
  {
    // add trailing '0'
    remaining.back() = std::byte{ 0U };
  }

  return rawString;
}

}
