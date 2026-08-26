// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615a::Files::InformationOperationStatusFile.
 **/

#include "InformationOperationStatusFile.hpp"

#include <arinc_615a/files/String.hpp>

#include <arinc_615a/Arinc615aException.hpp>
#include <arinc_615a/StatusCode.hpp>

#include <helper/Exception.hpp>

#include <boost/throw_exception.hpp>

#include <utility>

namespace Arinc615a::Files {

InformationOperationStatusFile::InformationOperationStatusFile(
  Arinc615aVersion const protocolVersion,
  Information::InformationStatus status ) :
  ProtocolFile{ protocolVersion },
  statusV{ std::move( status ) }
{
}

InformationOperationStatusFile::InformationOperationStatusFile( Helper::ConstRawDataSpan rawData )
{
  decode( rawData );
}

InformationOperationStatusFile& InformationOperationStatusFile::operator=( Helper::ConstRawDataSpan rawData )
{
  decode( rawData );
  return *this;
}

const Information::InformationStatus& InformationOperationStatusFile::status() const
{
  return statusV;
}

Information::InformationStatus& InformationOperationStatusFile::status()
{
  return statusV;
}

void InformationOperationStatusFile::status( Information::InformationStatus status )
{
  statusV = std::move( status );
}

Helper::RawData InformationOperationStatusFile::encode() const
{
  // counter + status code + exception timer + estimated time (each 16 bit)
  Helper::RawData rawData( HeaderSize + ( 4U * sizeof( uint16_t ) ) );

  // skip header - it is filled finally
  auto nextData{ Helper::RawDataSpan{ rawData }.subspan( HeaderSize ) };

  // counter
  nextData = Helper::RawData_setInt( nextData, statusV.counter() );

  // status code
  nextData = Helper::RawData_setInt( nextData, std::to_underlying( statusV.code() ) );

  // exception timer
  nextData = Helper::RawData_setInt( nextData, statusV.exceptionTimer() );

  // estimated time
  nextData = Helper::RawData_setInt( nextData, statusV.estimatedTime() );
  assert( nextData.empty() );

  // status description
  const auto rawStatusDescription{ String_encode( statusV.description() ) };
  rawData.insert( rawData.end(), rawStatusDescription.begin(), rawStatusDescription.end() );

  // insert header
  insertHeader( rawData );

  return rawData;
}

void InformationOperationStatusFile::decode( Helper::ConstRawDataSpan rawData )
{
  // check minimum data size
  if ( rawData.size() < ( HeaderSize + 9U ) )
  {
    BOOST_THROW_EXCEPTION( Arinc615aException{} << Helper::AdditionalInfo{ "Protocol file to small" } );
  }

  auto remainingData{ decodeHeader( rawData ) };

  // counter
  uint16_t counter;
  std::tie( remainingData, counter ) = Helper::RawData_getInt< uint16_t >( remainingData );
  statusV.counter( counter );

  // status code
  uint16_t intStatusCode;
  std::tie( remainingData, intStatusCode ) = Helper::RawData_getInt< uint16_t >( remainingData );
  statusV.code( statusCode( intStatusCode ) );

  // exception timer
  uint16_t exceptionTimer;
  std::tie( remainingData, exceptionTimer ) = Helper::RawData_getInt< uint16_t >( remainingData );
  statusV.exceptionTimer( exceptionTimer );

  // estimated time
  int16_t estimatedTime;
  std::tie( remainingData, estimatedTime ) = Helper::RawData_getInt< int16_t >( remainingData );
  statusV.estimatedTime( estimatedTime );

  // status description
  std::string statusDescription;
  std::tie( remainingData, statusDescription ) = String_decode( remainingData );
  statusV.description( std::move( statusDescription ) );

  // Final Check for additional data
  if ( !remainingData.empty() )
  {
    BOOST_THROW_EXCEPTION( Arinc615aException{} << Helper::AdditionalInfo{ "More data than expected" } );
  }
}

}
