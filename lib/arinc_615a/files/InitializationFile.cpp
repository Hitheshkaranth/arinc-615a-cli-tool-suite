// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615a::Files::InitializationFile.
 **/

#include "InitializationFile.hpp"

#include <arinc_615a/files/String.hpp>

#include <arinc_615a/Arinc615aException.hpp>
#include <arinc_615a/StatusCode.hpp>

#include <helper/Exception.hpp>

#include <boost/throw_exception.hpp>

#include <utility>

namespace Arinc615a::Files {

InitializationFile::InitializationFile(
  Arinc615aVersion const protocolVersion,
  Information::InitializationResponse response ) :
  ProtocolFile{ protocolVersion },
  responseV{ std::move( response ) }
{
}

InitializationFile::InitializationFile( Helper::ConstRawDataSpan rawData )
{
  decode( rawData );
}

InitializationFile& InitializationFile::operator=( Helper::ConstRawDataSpan rawData )
{
  decode( rawData );
  return *this;
}

const Information::InitializationResponse& InitializationFile::response() const
{
  return responseV;
}

Information::InitializationResponse& InitializationFile::response()
{
  return responseV;
}

void InitializationFile::response( Information::InitializationResponse response )
{
  responseV = std::move( response );
}

Helper::RawData InitializationFile::encode() const
{
  Helper::RawData rawData( HeaderSize + 2U );

  // skip header - it is filled finally
  auto nextData{ Helper::RawDataSpan{ rawData }.subspan( HeaderSize ) };

  // status code
  nextData = Helper::RawData_setInt( nextData, std::to_underlying( responseV.code() ) );
  assert( nextData.empty() );

  // status message
  const auto rawMessage{ String_encode( responseV.description() ) };
  rawData.insert( rawData.end(), rawMessage.begin(), rawMessage.end() );

  // insert header
  insertHeader( rawData );

  return rawData;
}

void InitializationFile::decode( Helper::ConstRawDataSpan rawData )
{
  // check minimum data size
  if ( rawData.size() < MinFileSize )
  {
    BOOST_THROW_EXCEPTION( Arinc615aException{} << Helper::AdditionalInfo{ "Protocol file to small" } );
  }

  auto remainingData{ decodeHeader( rawData ) };

  // Status code
  std::underlying_type_t< OperationAcceptanceStatusCode > intStatusCode;
  std::tie( remainingData, intStatusCode ) =
    Helper::RawData_getInt< std::underlying_type_t< OperationAcceptanceStatusCode > >( remainingData );
  responseV.code( operationAcceptanceStatusCode( intStatusCode ) );

  // Status description
  std::string statusDescription;
  std::tie( remainingData, statusDescription ) = String_decode( remainingData );
  responseV.description( std::move( statusDescription ) );

  // Final Check for additional data
  if ( !remainingData.empty() )
  {
    BOOST_THROW_EXCEPTION( Arinc615aException{} << Helper::AdditionalInfo{ "More data then expected" } );
  }
}

}
