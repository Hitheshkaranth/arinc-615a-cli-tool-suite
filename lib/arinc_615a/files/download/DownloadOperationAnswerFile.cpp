// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615a::Files::DownloadOperationAnswerFile.
 **/

#include "DownloadOperationAnswerFile.hpp"

#include <arinc_615a/files/String.hpp>

#include <arinc_615a/Arinc615aException.hpp>

#include <helper/Exception.hpp>
#include <helper/RawData.hpp>
#include <helper/SafeCast.hpp>

#include <spdlog/spdlog.h>

#include <boost/throw_exception.hpp>

namespace Arinc615a::Files {

DownloadOperationAnswerFile::DownloadOperationAnswerFile(
  Arinc615aVersion const protocolVersion,
  Information::DownloadFiles files ) :
  ProtocolFile{ protocolVersion },
  filesV{ std::move( files ) }
{
}

DownloadOperationAnswerFile::DownloadOperationAnswerFile( Helper::ConstRawDataSpan rawData )
{
  decode( rawData );
}

DownloadOperationAnswerFile& DownloadOperationAnswerFile::operator=( Helper::ConstRawDataSpan rawData )
{
  decode( rawData );
  return *this;
}

const Information::DownloadFiles& DownloadOperationAnswerFile::files() const
{
  return filesV;
}

Information::DownloadFiles& DownloadOperationAnswerFile::files()
{
  return filesV;
}

void DownloadOperationAnswerFile::files( Information::DownloadFiles files )
{
  filesV = std::move( files );
}

Helper::RawData DownloadOperationAnswerFile::encode() const
{
  Helper::RawData rawData( MinimumSize );

  // skip header - it is filled finally
  auto nextData{ Helper::RawDataSpan{ rawData }.subspan( HeaderSize ) };

  // the number of files must not exceed the field maximum value
  if ( filesV.size() > std::numeric_limits< uint16_t >::max() )
  {
    BOOST_THROW_EXCEPTION( Arinc615aException()
      << Helper::AdditionalInfo{ "More files than allowed" } );
  }

  // number of files
  nextData = Helper::RawData_setInt( nextData, Helper::safeCast< uint16_t >( filesV.size() ) );
  assert( nextData.empty() );

  // iterate over files
  for ( const auto &file : filesV )
  {
    // filename
    const auto filename{ String_encode( file ) };

    // add filename to the file
    rawData.insert( rawData.end(), filename.begin(), filename.end() );
  }

  // insert header
  insertHeader( rawData );

  return rawData;
}

void DownloadOperationAnswerFile::decode( Helper::ConstRawDataSpan rawData )
{
  // check minimum data size
  if ( rawData.size() < MinimumSize )
  {
    BOOST_THROW_EXCEPTION( Arinc615aException{} << Helper::AdditionalInfo{ "Protocol file to small" } );
  }

  auto remainingData{ decodeHeader( rawData ) };

  // number of files
  uint16_t numberOfFiles;
  std::tie( remainingData, numberOfFiles ) = Helper::RawData_getInt< uint16_t >( remainingData );

  if ( 0U == numberOfFiles )
  {
    SPDLOG_WARN( "Invalid number of files (0)" );
  }

  // iterate over files
  for ( uint16_t fileIndex{ 0U }; fileIndex < numberOfFiles; ++fileIndex )
  {
    // filename
    std::string filename;
    std::tie( remainingData, filename ) = String_decode( remainingData );
    if ( filename.empty() )
    {
      SPDLOG_WARN( "filename is empty" );
    }

    filesV.emplace_back( std::move( filename ) );
  }

  // Final Check for additional data
  if ( !remainingData.empty() )
  {
    BOOST_THROW_EXCEPTION( Arinc615aException{} << Helper::AdditionalInfo{ "More data then expected" } );
  }
}

}
