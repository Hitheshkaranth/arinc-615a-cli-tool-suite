// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615a::Information::UploadLoadStatus.
 **/

#include "UploadLoadStatus.hpp"

namespace Arinc615a::Information {

UploadLoadStatus::UploadLoadStatus(
  std::string headerFilename,
  std::string partNumber ) :
  headerFilenameV{ std::move( headerFilename ) },
  partNumberV{ std::move( partNumber ) }
{
}

UploadLoadStatus::UploadLoadStatus(
  std::string headerFilename,
  std::string partNumber,
  const Ratio ratio,
  const StatusCode code,
  std::string description ) :
  headerFilenameV{ std::move( headerFilename ) },
  partNumberV{ std::move( partNumber ) },
  ratioV{ ratio },
  codeV{ code },
  descriptionV{ std::move( description ) }
{
}

std::string_view UploadLoadStatus::headerFilename() const noexcept
{
  return headerFilenameV;
}

void UploadLoadStatus::headerFilename( std::string headerFilename )
{
  headerFilenameV = std::move( headerFilename );
}

std::string_view UploadLoadStatus::partNumber() const noexcept
{
  return partNumberV;
}

void UploadLoadStatus::partNumber( std::string partNumber )
{
  partNumberV = std::move( partNumber );
}

const Ratio& UploadLoadStatus::ratio() const noexcept
{
  return ratioV;
}

Ratio& UploadLoadStatus::ratio() noexcept
{
  return ratioV;
}

void UploadLoadStatus::ratio( Ratio ratio )
{
  ratioV = ratio;
}

StatusCode UploadLoadStatus::code() const noexcept
{
  return codeV;
}

void UploadLoadStatus::code( const StatusCode code )
{
  codeV = code;
  descriptionV.clear();
}

std::string_view UploadLoadStatus::description() const noexcept
{
  return descriptionV;
}

void UploadLoadStatus::description( std::string description )
{
  descriptionV = std::move( description);
}

void UploadLoadStatus::set( Ratio ratio, StatusCode code, std::string description )
{
  ratioV = std::move( ratio );
  codeV = code;
  descriptionV = std::move( description );
}

}
