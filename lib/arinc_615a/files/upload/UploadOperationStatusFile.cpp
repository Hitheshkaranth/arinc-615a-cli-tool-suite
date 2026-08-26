// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615a::Files::UploadOperationStatusFile.
 **/

#include "UploadOperationStatusFile.hpp"

#include <arinc_615a/files/Ratio.hpp>
#include <arinc_615a/files/String.hpp>

#include <arinc_615a/Arinc615aException.hpp>
#include <arinc_615a/StatusCode.hpp>

#include <helper/Exception.hpp>
#include <helper/SafeCast.hpp>

#include <spdlog/spdlog.h>

#include <boost/throw_exception.hpp>

#include <utility>

namespace Arinc615a::Files {

UploadOperationStatusFile::UploadOperationStatusFile(
  Arinc615aVersion const protocolVersion,
  Information::UploadStatus status ) :
  ProtocolFile{ protocolVersion },
  statusV{ std::move( status ) }
{
}

UploadOperationStatusFile::UploadOperationStatusFile( Helper::ConstRawDataSpan rawData )
{
  decode( rawData );
}

UploadOperationStatusFile& UploadOperationStatusFile::operator=( Helper::ConstRawDataSpan rawData )
{
  decode( rawData );
  return *this;
}

const Information::UploadStatus& UploadOperationStatusFile::status() const
{
  return statusV;
}

Information::UploadStatus& UploadOperationStatusFile::status()
{
  return statusV;
}

void UploadOperationStatusFile::status( Information::UploadStatus status )
{
  statusV = std::move( status );
}

Helper::RawData UploadOperationStatusFile::encode() const
{
  // Reserver Size for header and status code
  Helper::RawData rawData( HeaderSize + sizeof( uint16_t ) );

  // skip header - it is filled finally
  auto nextData{ Helper::RawDataSpan{ rawData }.subspan( HeaderSize ) };

  // status code
  Helper::RawData_setInt( nextData, std::to_underlying( statusV.code() ) );

  // status description
  const auto rawDescription{ String_encode( statusV.description() ) };
  rawData.insert( rawData.end(), rawDescription.begin(), rawDescription.end() );

  // reserve and resize buffer for status counter, exception timer, and estimated time
  rawData.resize( rawData.size() + ( 3UZ * sizeof( uint16_t ) ) );
  nextData = Helper::RawDataSpan{ rawData }.last( 3UZ * sizeof( uint16_t ) );

  // counter
  nextData = Helper::RawData_setInt( nextData, statusV.counter() );

  // exception timer
  nextData = Helper::RawData_setInt( nextData, statusV.exceptionTimer() );

  // estimated time
  nextData = Helper::RawData_setInt( nextData, statusV.estimatedTime() );
  assert( nextData.empty() );

  // load list ratio
  const auto loadListRatio{ Ratio_encode( statusV.listRatio() ) };
  rawData.insert( rawData.end(), loadListRatio.begin(), loadListRatio.end() );

  // resize buffer for number of header files
  rawData.resize( rawData.size() + sizeof( uint16_t ) );
  nextData = Helper::RawDataSpan{ rawData }.last( sizeof( uint16_t ) );

  // the number of loads must not exceed the field max value
  if ( statusV.loads().size() > std::numeric_limits< uint16_t >::max() )
  {
    BOOST_THROW_EXCEPTION( Arinc615aException{}
      << Helper::AdditionalInfo{ "More Load Headers than allowed" } );
  }

  // number of header files
  Helper::RawData_setInt( nextData, Helper::safeCast< uint16_t >( statusV.loads().size() ) );

  // add each header file status
  for ( const auto &headerFile : statusV.loads() )
  {
    // Header filename
    const auto rawHeaderFilename{ String_encode( headerFile.headerFilename() ) };
    rawData.insert( rawData.end(), rawHeaderFilename.begin(), rawHeaderFilename.end() );

    // Load part number
    const auto rawLoadPartNumber{ String_encode( headerFile.partNumber() ) };
    rawData.insert( rawData.end(), rawLoadPartNumber.begin(), rawLoadPartNumber.end() );

    // load ratio
    const auto loadRatio{ Ratio_encode( headerFile.ratio() ) };
    rawData.insert( rawData.end(), loadRatio.begin(), loadRatio.end() );

    // resize buffer for load status code
    rawData.resize( rawData.size() + sizeof( uint16_t ) );
    nextData = Helper::RawDataSpan{ rawData }.last( sizeof( uint16_t ) );

    // load status code
    nextData = Helper::RawData_setInt( nextData, std::to_underlying( headerFile.code() ) );
    assert( nextData.empty() );

    // Load status description
    const auto rawLoadDescription{ String_encode( headerFile.description() ) };
    rawData.insert( rawData.end(), rawLoadDescription.begin(), rawLoadDescription.end() );
  }

  // insert header
  insertHeader( rawData );

  return rawData;
}

void UploadOperationStatusFile::decode( Helper::ConstRawDataSpan rawData )
{
  // check minimum data size
  if ( rawData.size() < ( HeaderSize + 9UZ ) )
  {
    BOOST_THROW_EXCEPTION( Arinc615aException{} << Helper::AdditionalInfo{ "Protocol file to small" } );
  }

  auto remainingData{ decodeHeader( rawData ) };

  // status code
  uint16_t intStatusCode;
  std::tie( remainingData, intStatusCode ) = Helper::RawData_getInt< uint16_t >( remainingData );
  statusV.code( statusCode( intStatusCode ) );

  // status description
  std::string statusDescription;
  std::tie( remainingData, statusDescription ) = String_decode( remainingData );
  statusV.description( std::move( statusDescription ) );

  // counter
  uint16_t counter;
  std::tie( remainingData, counter ) = Helper::RawData_getInt< uint16_t >( remainingData );
  statusV.counter( counter );

  // exception timer
  uint16_t exceptionTimer;
  std::tie( remainingData, exceptionTimer ) = Helper::RawData_getInt< uint16_t >( remainingData );
  statusV.exceptionTimer( exceptionTimer );

  // estimated time
  int16_t estimatedTime;
  std::tie( remainingData, estimatedTime ) = Helper::RawData_getInt< int16_t >( remainingData );
  statusV.estimatedTime( estimatedTime );

  // load list ratio
  Information::Ratio loadListRatio;
  std::tie( remainingData, loadListRatio) = Ratio_decode( remainingData );
  statusV.listRatio( loadListRatio );

  // number of header files
  uint16_t numberOfHeaderFiles;
  std::tie( remainingData, numberOfHeaderFiles ) = Helper::RawData_getInt< uint16_t >( remainingData );

  Information::UploadLoadsStatus loadsStatus;

  // iterate over header files
  for ( uint16_t headerFileIndex{ 0U }; headerFileIndex < numberOfHeaderFiles; ++headerFileIndex )
  {
    try
    {
      // header filename
      std::string headerFilename;
      std::tie( remainingData, headerFilename ) = String_decode( remainingData );
      if ( headerFilename.empty() )
      {
        SPDLOG_WARN( "header filename is empty" );
      }

      // load part number
      std::string loadPartNumber;
      std::tie( remainingData, loadPartNumber ) = String_decode( remainingData );
      if ( loadPartNumber.empty() )
      {
        SPDLOG_WARN( "load part number is empty" );
      }

      // load ratio
      Information::Ratio loadRatio;
      std::tie( remainingData, loadRatio ) = Ratio_decode( remainingData );

      // load status code
      uint16_t intLoadStatusCode;
      std::tie( remainingData, intLoadStatusCode ) = Helper::RawData_getInt< uint16_t >( remainingData );

      // load status description
      std::string loadStatusDescription;
      std::tie( remainingData, loadStatusDescription ) = String_decode( remainingData );

      loadsStatus.emplace_back(
        std::move( headerFilename ),
        std::move( loadPartNumber ),
        std::move( loadRatio ),
        statusCode( intLoadStatusCode ),
        std::move( loadStatusDescription ) );
    }
    catch ( const std::invalid_argument & )
    {
      BOOST_THROW_EXCEPTION( Arinc615aException{} << Helper::AdditionalInfo{ "Decoding Load Ratio" } );
    }
  }

  statusV.loads( std::move( loadsStatus ) );

  // Final Check for additional data
  if ( !remainingData.empty() )
  {
    BOOST_THROW_EXCEPTION( Arinc615aException{} << Helper::AdditionalInfo{ "More data then expected" } );
  }
}

}
