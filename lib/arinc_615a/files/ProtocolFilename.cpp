// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615a::Files::ProtocolFilename.
 **/

#include "ProtocolFilename.hpp"

#include <format>

namespace Arinc615a::Files {

std::tuple< std::string_view, std::string_view> ProtocolFilename::split( std::string_view filename )
{
  const auto pointPos{ filename.find( '.' ) };

  // point not found
  if ( std::string_view::npos == pointPos )
  {
    return {};
  }

  return std::make_tuple( filename.substr( 0, pointPos ), filename.substr( pointPos + 1, std::string_view::npos ) );
}

bool ProtocolFilename::isProtocolFilename( const std::string_view filename )
{
  const auto [ targetId, extension ] = split( filename );

  return ( TargetId::isTargetId( targetId ) && ( fileType( extension ) != ProtocolFileType::Invalid ) );
}


std::string_view ProtocolFilename::extension( const ProtocolFileType fileType )
{
  const auto &index{ extensionTypeInfoIndex().get< ByType>() };

  const auto element{ index.find( fileType ) };

  if ( element == index.end() )
  {
    return {};
  }

  return element->extension;
}

ProtocolFileType ProtocolFilename::fileType( std::string_view extension )
{
  const auto &index{ extensionTypeInfoIndex().get< ByExtension>() };

  auto element{ index.find( extension ) };

  if ( element == index.end() )
  {
    return ProtocolFileType::Invalid;
  }

  return element->type;
}

ProtocolFilename::ProtocolFilename( std::string thwId, std::string position, ProtocolFileType fileType ) :
  targetIdV{ std::move( thwId ), std::move( position ) },
  fileTypeV{ fileType }
{
}

ProtocolFilename::ProtocolFilename( std::string_view targetId, const ProtocolFileType fileType ) :
  targetIdV{ targetId },
  fileTypeV{ fileType }
{
}

ProtocolFilename::ProtocolFilename( TargetId targetId, ProtocolFileType fileType ) :
  targetIdV{ std::move( targetId ) },
  fileTypeV{ fileType }
{
}

ProtocolFilename::ProtocolFilename( std::string_view filename )
{
  const auto [ targetId, extension ] = split( filename );
  targetIdV = targetId;
  fileTypeV = fileType( extension );
}

const TargetId& ProtocolFilename::targetId() const
{
  return targetIdV;
}

void ProtocolFilename::targetId( std::string_view targetId )
{
  targetIdV = targetId;
}

void ProtocolFilename::targetId( TargetId targetId )
{
  targetIdV = std::move( targetId );
}

std::string_view ProtocolFilename::thwId() const
{
  return targetIdV.thwId();
}

void ProtocolFilename::thwId( std::string thwId )
{
  targetIdV.thwId( std::move( thwId ) );
}

std::string_view ProtocolFilename::position() const
{
  return targetIdV.position();
}

void ProtocolFilename::position( std::string position )
{
  targetIdV.position( std::move( position ) );
}

ProtocolFileType ProtocolFilename::fileType() const
{
  return fileTypeV;
}

void ProtocolFilename::fileType( const ProtocolFileType fileType )
{
  fileTypeV = fileType;
}

ProtocolFilename::operator std::string() const
{
  return std::format( "{}.{}", targetIdV, extension( fileTypeV ) );
}

ProtocolFilename::operator bool() const noexcept
{
  return targetIdV && ( ProtocolFileType::Invalid != fileTypeV );
}

const ProtocolFilename::ExtensionTypeInfoIndex& ProtocolFilename::extensionTypeInfoIndex()
{
  static const ExtensionTypeInfoIndex extensionTypeInfoIndex{
    { "LCI", ProtocolFileType::LoadConfigurationInitialization },
    { "LCL", ProtocolFileType::LoadConfigurationList },
    { "LCS", ProtocolFileType::LoadConfigurationStatus },

    { "LUI", ProtocolFileType::UploadInitialization },
    { "LUR", ProtocolFileType::UploadRequest },
    { "LUS", ProtocolFileType::UploadStatus },

    { "LND", ProtocolFileType::MediaDefinedDownloadInitialization },
    { "LNR", ProtocolFileType::MediaDefinedDownloadRequest },

    { "LNO", ProtocolFileType::OperatorDefinedDownloadInitialization },
    { "LNL", ProtocolFileType::OperatorDefinedDownloadList },
    { "LNA", ProtocolFileType::OperatorDefinedDownloadAnswer },

    { "LNS", ProtocolFileType::DownloadStatus }
  };
  return extensionTypeInfoIndex;
}

}
