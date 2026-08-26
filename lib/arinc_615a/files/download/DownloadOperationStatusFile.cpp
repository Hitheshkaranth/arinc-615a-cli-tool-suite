// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615a::Files::DownloadOperationStatusFile.
 **/

#include "DownloadOperationStatusFile.hpp"

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

DownloadOperationStatusFile::DownloadOperationStatusFile(
  Arinc615aVersion const protocolVersion,
  Information::DownloadStatus status ) :
  ProtocolFile{ protocolVersion },
  statusV{ std::move( status ) }
{
}

DownloadOperationStatusFile::DownloadOperationStatusFile( const Helper::ConstRawDataSpan rawData )
{
  decode( rawData );
}

DownloadOperationStatusFile& DownloadOperationStatusFile::operator=( const Helper::ConstRawDataSpan rawData )
{
  decode( rawData );
  return *this;
}

const Information::DownloadStatus& DownloadOperationStatusFile::status() const
{
  return statusV;
}

Information::DownloadStatus& DownloadOperationStatusFile::status()
{
  return statusV;
}

void DownloadOperationStatusFile::status( Information::DownloadStatus status )
{
  statusV = std::move( status );
}

Helper::RawData DownloadOperationStatusFile::encode() const
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

  // download list ratio
  const auto downloadListRatio{ Ratio_encode( statusV.listRatio() ) };
  rawData.insert( rawData.end(), downloadListRatio.begin(), downloadListRatio.end() );

  // the number of files must not exceed the field max value
  if ( statusV.files().size() > std::numeric_limits< uint16_t >::max() )
  {
    BOOST_THROW_EXCEPTION( Arinc615aException{} << Helper::AdditionalInfo{ "More files than allowed" } );
  }

  // resize buffer for the number of files field
  rawData.resize( rawData.size() + sizeof( uint16_t ) );
  nextData = Helper::RawDataSpan{ rawData }.last( sizeof( uint16_t ) );

  // number of files
  Helper::RawData_setInt( nextData, Helper::safeCast< uint16_t >( statusV.files().size() ) );

  // add each file status
  for ( const auto &file : statusV.files() )
  {
    // filename
    const auto rawFilename{ String_encode( file.filename() ) };
    rawData.insert( rawData.end(), rawFilename.begin(), rawFilename.end() );

    // resize buffer for file status
    rawData.resize( rawData.size() + sizeof( uint16_t ) );
    nextData = Helper::RawDataSpan{ rawData }.last( sizeof( uint16_t ) );

    // file status code
    nextData = Helper::RawData_setInt( nextData, std::to_underlying( file.code() ) );
    assert( nextData.empty() );

    // file status description
    const auto rawStatusDescription{ String_encode( file.description() ) };
    rawData.insert( rawData.end(), rawStatusDescription.begin(), rawStatusDescription.end() );
  }

  // insert header
  insertHeader( rawData );

  return rawData;
}

void DownloadOperationStatusFile::decode( const Helper::ConstRawDataSpan rawData )
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

  // download list ratio
  Information::Ratio downloadListRatio;
  std::tie( remainingData, downloadListRatio ) = Ratio_decode( remainingData );
  statusV.listRatio( downloadListRatio );

  // number of files
  uint16_t numberOfFiles;
  std::tie( remainingData, numberOfFiles ) = Helper::RawData_getInt< uint16_t >( remainingData );

  // the number of files field could be 0 - therefore, don't check for validity

  Information::DownloadFilesStatus fileStatusList;

  // iterate over files
  for ( uint16_t fileIndex{ 0U }; fileIndex < numberOfFiles; ++fileIndex )
  {
    // filename of the download file
    std::string filename;
    std::tie( remainingData, filename ) = String_decode( remainingData );
    if ( filename.empty() )
    {
      SPDLOG_WARN( "filename is empty" );
    }

    // status code of the download file
    uint16_t intFileStatusCode;
    std::tie( remainingData, intFileStatusCode ) = Helper::RawData_getInt< uint16_t >( remainingData );

    // status description of the download file
    std::string fileStatusDescription;
    std::tie( remainingData, fileStatusDescription ) = String_decode( remainingData );

    fileStatusList.emplace_back(
      std::move( filename ),
      statusCode( intFileStatusCode ),
      std::move( fileStatusDescription ) );
  }

  statusV.files( std::move( fileStatusList ) );

  // Final Check for additional data
  if ( !remainingData.empty() )
  {
    BOOST_THROW_EXCEPTION( Arinc615aException{} << Helper::AdditionalInfo{ "More data then expected" } );
  }
}

}
