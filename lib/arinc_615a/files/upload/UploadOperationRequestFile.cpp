// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615a::Files::UploadOperationRequestFile.
 **/

#include "UploadOperationRequestFile.hpp"

#include <arinc_615a/files/String.hpp>

#include <arinc_615a/Arinc615aException.hpp>

#include <helper/Exception.hpp>
#include <helper/RawData.hpp>
#include <helper/SafeCast.hpp>

#include <spdlog/spdlog.h>

#include <boost/throw_exception.hpp>

namespace Arinc615a::Files {

UploadOperationRequestFile::UploadOperationRequestFile(
  Arinc615aVersion const protocolVersion,
  Information::UploadLoads loads ) :
  ProtocolFile{ protocolVersion },
  loadsV{ std::move( loads ) }
{
}

UploadOperationRequestFile::UploadOperationRequestFile( Helper::ConstRawDataSpan rawData )
{
  decode( rawData );
}

UploadOperationRequestFile& UploadOperationRequestFile::operator=( Helper::ConstRawDataSpan rawData )
{
  decode( rawData );
  return *this;
}

const Information::UploadLoads& UploadOperationRequestFile::loads() const
{
  return loadsV;
}

Information::UploadLoads& UploadOperationRequestFile::loads()
{
  return loadsV;
}

void UploadOperationRequestFile::loads( Information::UploadLoads loads )
{
  loadsV = std::move( loads );
}

Helper::RawData UploadOperationRequestFile::encode() const
{
  Helper::RawData rawData( MinimumSize );

  // skip header - it is filled finally
  auto nextData{ Helper::RawDataSpan{ rawData }.subspan( HeaderSize ) };

  // the number of loads must not exceed the field maximum value
  if ( loadsV.size() > std::numeric_limits< uint16_t >::max() )
  {
    BOOST_THROW_EXCEPTION( Arinc615aException{} << Helper::AdditionalInfo{ "More loads than allowed" } );
  }

  // number of header files
  nextData = Helper::RawData_setInt( nextData, Helper::safeCast< uint16_t >( loadsV.size() ) );
  assert( nextData.empty() );

  // iterate over header files
  for ( const auto &load : loadsV )
  {
    // header filename
    const auto headerFilename{ String_encode( load.headerFilename ) };
    // load part number name
    const auto loadPartNumberName{ String_encode( load.partNumber ) };

    // reserve memory to hold new content
    rawData.reserve( headerFilename.size() + loadPartNumberName.size() );

    // add header filename to file
    rawData.insert( rawData.end(), headerFilename.begin(), headerFilename.end() );

    // add load part number name to file
    rawData.insert( rawData.end(), loadPartNumberName.begin(), loadPartNumberName.end() );
  }

  // insert header
  insertHeader( rawData );

  return rawData;
}

void UploadOperationRequestFile::decode( Helper::ConstRawDataSpan rawData )
{
  // check minimum data size
  if ( rawData.size() < MinimumSize )
  {
    BOOST_THROW_EXCEPTION( Arinc615aException{} << Helper::AdditionalInfo{ "Protocol file to small" } );
  }

  auto remainingData{ decodeHeader( rawData ) };

  // number of header files
  uint16_t numberOfHeaderFiles;
  std::tie( remainingData, numberOfHeaderFiles ) = Helper::RawData_getInt< uint16_t >( remainingData );

  if ( 0U == numberOfHeaderFiles )
  {
    SPDLOG_WARN( "Invalid number of header files (0)" );
  }

  // iterate over header files
  for ( uint16_t headerFileIndex{ 0U }; headerFileIndex < numberOfHeaderFiles; ++headerFileIndex )
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

    // Add load header info to the loads list
    loadsV.emplace_back( std::move( headerFilename ), std::move( loadPartNumber ) );
  }

  // Final Check for additional data
  if ( !remainingData.empty() )
  {
    BOOST_THROW_EXCEPTION( Arinc615aException{} << Helper::AdditionalInfo{ "More data then expected" } );
  }
}

}
