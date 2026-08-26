// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615a::Information::DownloadStatus.
 **/

#include "DownloadStatus.hpp"

#include <algorithm>

namespace Arinc615a::Information {

DownloadStatus::DownloadStatus(
  const uint16_t counter,
  const uint16_t exceptionTimer,
  const int16_t estimatedTime,
  const StatusCode code,
  std::string description,
  Ratio listRatio,
  DownloadFilesStatus filesStatus ) :
  Status{ counter, exceptionTimer, estimatedTime, code, std::move( description ) },
  listRatioV{ listRatio },
  filesStatusV{ std::move( filesStatus ) }
{
}

const Ratio& DownloadStatus::listRatio() const noexcept
{
  return listRatioV;
}

Ratio& DownloadStatus::listRatio() noexcept
{
  return listRatioV;
}

void DownloadStatus::listRatio( const Ratio listRatio)
{
  listRatioV = listRatio;
}

const DownloadFilesStatus& DownloadStatus::files() const noexcept
{
  return filesStatusV;
}

DownloadFilesStatus& DownloadStatus::files() noexcept
{
  return filesStatusV;
}

void DownloadStatus::files( DownloadFilesStatus filesStatus )
{
  filesStatusV = std::move( filesStatus );
}

DownloadFileStatus& DownloadStatus::file( std::string filename )
{
  return filesStatusV.emplace_back( std::move( filename ) );
}

DownloadFileStatus& DownloadStatus::file( std::string_view filename )
{
  const auto fileIt = std::ranges::find_if( filesStatusV, [ &filename ]( const DownloadFileStatus &file ) {
    return file.filename() == filename;
  } );

  // If the file status is not present, add and return a new file status
  if ( fileIt == filesStatusV.end() )
  {
    return file( std::string{ filename } );
  }

  return *fileIt;
}

}
