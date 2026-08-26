// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615a::Information::DownloadFileStatus.
 **/

#include "DownloadFileStatus.hpp"

namespace Arinc615a::Information {

DownloadFileStatus::DownloadFileStatus( std::string filename ) :
  filenameV{ std::move( filename ) }
{
}

DownloadFileStatus::DownloadFileStatus( std::string filename, StatusCode code, std::string description ) :
  filenameV{ std::move( filename ) },
  codeV{ code },
  descriptionV{ std::move( description ) }
{
}

std::string_view DownloadFileStatus::filename() const noexcept
{
  return filenameV;
}

void DownloadFileStatus::filename( std::string filename )
{
  filenameV = std::move( filename );
}

StatusCode DownloadFileStatus::code() const noexcept
{
  return codeV;
}

void DownloadFileStatus::code( const StatusCode code )
{
  codeV = code;
  descriptionV.clear();
}

std::string_view DownloadFileStatus::description() const noexcept
{
  return descriptionV;
}

void DownloadFileStatus::description( std::string description )
{
  descriptionV = std::move( description );
}

void DownloadFileStatus::set( const StatusCode code, std::string description )
{
  codeV = code;
  descriptionV = std::move( description );
}

}
