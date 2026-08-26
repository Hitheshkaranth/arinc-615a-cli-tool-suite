// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615a::Information::UploadStatus.
 **/

#include "UploadStatus.hpp"

#include <algorithm>

namespace Arinc615a::Information {

UploadStatus::UploadStatus(
  const uint16_t counter,
  const uint16_t exceptionTimer,
  const int16_t estimatedTime,
  const StatusCode code,
  std::string description,
  const Ratio listRatio,
  UploadLoadsStatus loadsStatus ) :
  Status{ counter, exceptionTimer, estimatedTime, code, std::move( description ) },
  listRatioV{ listRatio },
  loadsStatusV{ std::move( loadsStatus ) }
{
}

const Ratio& UploadStatus::listRatio() const noexcept
{
  return listRatioV;
}

Ratio& UploadStatus::listRatio() noexcept
{
  return listRatioV;
}

void UploadStatus::listRatio( const Ratio listRatio )
{
  listRatioV = listRatio;
}

const UploadLoadsStatus& UploadStatus::loads() const noexcept
{
  return loadsStatusV;
}

UploadLoadsStatus& UploadStatus::loads() noexcept
{
  return loadsStatusV;
}

void UploadStatus::loads( UploadLoadsStatus loads )
{
  loadsStatusV = std::move( loads );
}

UploadLoadStatus& UploadStatus::load( std::string headerFilename, std::string partNumber )
{
  return loadsStatusV.emplace_back( std::move( headerFilename ), std::move( partNumber ) );
}

UploadLoadStatus& UploadStatus::load( std::string_view headerFilename )
{
  const auto loadIt = std::ranges::find_if( loadsStatusV, [ &headerFilename ]( const UploadLoadStatus &load ) {
    return load.headerFilename() == headerFilename;
  } );

  // If load is not present, add and return the new status
  if ( loadIt == loadsStatusV.end() )
  {
    return load( std::string{ headerFilename }, "" );
  }

  return *loadIt;
}

}
