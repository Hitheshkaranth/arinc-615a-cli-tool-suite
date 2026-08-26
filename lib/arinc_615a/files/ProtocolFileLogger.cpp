// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615a::Files::ProtocolFileLogger.
 **/

#include "ProtocolFileLogger.hpp"

#include <arinc_615a/OperationTypeDescription.hpp>

#include <spdlog/spdlog.h>

#include <chrono>
#include <format>
#include <fstream>

namespace Arinc615a::Files {

ProtocolFileLogger& ProtocolFileLogger::loggingDirectory( std::filesystem::path loggingDirectory )
{
  SPDLOG_INFO( "Protocol file logging directory: {}", loggingDirectory.string() );
  loggingDirectoryV = std::move( loggingDirectory );
  return *this;
}

ProtocolFileLogger& ProtocolFileLogger::loggingEnabled( const bool loggingEnabled )
{
  loggingEnabledV = loggingEnabled;
  return *this;
}

ProtocolFileLogger& ProtocolFileLogger::operation( const OperationType operation )
{
  operationV = operation;
  return *this;
}

void ProtocolFileLogger::receivedProtocolFile( std::string_view filename, Helper::ConstRawDataSpan file )
{
  if ( !loggingEnabledV )
  {
    return;
  }

  logProtocolFile( "RX", filename, file );
}

void ProtocolFileLogger::transmitProtocolFile( std::string_view filename, Helper::ConstRawDataSpan file )
{
  if ( !loggingEnabledV )
  {
    return;
  }

  logProtocolFile( "TX", filename, file );
}

void ProtocolFileLogger::logProtocolFile(
  std::string_view prefix,
  std::string_view filename,
  Helper::ConstRawDataSpan file )
{
  auto protocolFileLoggingFilename{
    loggingDirectoryV
    / std::format(
      "{:%FT%H-%M-%S%z}_{}_{}_{}",
      std::chrono::system_clock::now(),
      OperationTypeDescription::instance().name( operationV ),
      prefix,
      filename ) };

  SPDLOG_INFO( "Log protocol file to {}", protocolFileLoggingFilename.string() );

  std::ofstream fileStream{ protocolFileLoggingFilename, std::ios::out | std::ios::binary };

  if ( !fileStream )
  {
    SPDLOG_ERROR( "Could not open file for writing" );
    return;
  }

  fileStream.write( reinterpret_cast< const char * >( file.data() ), static_cast< std::streamsize >( file.size() ) );
  fileStream.close();
}

}
