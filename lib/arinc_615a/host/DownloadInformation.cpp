// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Struct Arinc615a::Host::DownloadInformation.
 **/

#include "DownloadInformation.hpp"

#include <arinc_649/CheckValueGenerator.hpp>

#include <spdlog/spdlog.h>

#include <format>
#include <fstream>
#include <ranges>

namespace Arinc615a::Host {

void DownloadInformation::start( TargetId targetId, boost::asio::ip::address targetAddress )
{
  std::unique_lock lock{ mutexV };

  targetIdV = std::move( targetId );
  targetAddressV = std::move( targetAddress );
  startV = std::chrono::floor< std::chrono::seconds >( std::chrono::system_clock::now() );
}

std::filesystem::path DownloadInformation::downloadDataPath( const std::filesystem::path &basePath ) const
{
  std::unique_lock lock{ mutexV };

  const auto downloadDataName{ std::format(
    "DNLD_DATA_{:}_{:L%Y%m%d_%H%M%S%Z}",
    static_cast< std::string >( targetIdV ),
    std::chrono::zoned_time( std::chrono::current_zone(), std::chrono::floor< std::chrono::seconds >( startV ) ) ) };

  return basePath / downloadDataName;
}

std::filesystem::path DownloadInformation::downloadInformationPath( const std::filesystem::path &basePath ) const
{
  std::unique_lock lock{ mutexV };

  const auto downloadInfoName{ std::format(
    "DNLD_INFO_{:}_{:L%Y%m%d_%H%M%S%Z}.txt",
    static_cast< std::string >( targetIdV ),
    std::chrono::zoned_time( std::chrono::current_zone(), std::chrono::floor< std::chrono::seconds >( startV ) ) ) };

  return basePath / downloadInfoName;
}

void DownloadInformation::files( std::list< std::string > files )
{
  std::unique_lock lock{ mutexV };

  filesV = std::move( files );
}

void DownloadInformation::fileStart(
  const std::string_view filename,
  std::filesystem::path filePath,
  std::string partNumber,
  Arinc649::CheckValue checkValue )
{
  std::unique_lock lock{ mutexV };

  filesInformationV.insert_or_assign(
    std::string{ filename },
    FileTransferInformation{
      .start = std::chrono::floor< std::chrono::seconds >( std::chrono::system_clock::now() ),
      .filePath = std::move( filePath ),
      .partNumber = std::move( partNumber ),
      .checkValue = std::move( checkValue ) } );
}

void DownloadInformation::fileFinished( std::string_view filename, std::string status, std::size_t fileSize )
{
  std::unique_lock lock{ mutexV };

  auto &fileInformation{ filesInformationV[ std::string{ filename } ] };

  fileInformation.end = std::chrono::floor< std::chrono::seconds >( std::chrono::system_clock::now() );
  fileInformation.fileStatus = std::move( status );
  fileInformation.fileSize = fileSize;
}

void DownloadInformation::finished( std::string status )
{
  std::unique_lock lock{ mutexV };

  std::size_t overallSize{ 0U };
  for ( const auto &fileStatus : filesInformationV | std::views::values )
  {
    overallSize += fileStatus.fileSize;
  }

  endV = std::chrono::floor< std::chrono::seconds >( std::chrono::system_clock::now() );
  overallStatusV = std::make_tuple( std::move( status ), overallSize );
}

void DownloadInformation::save( const std::filesystem::path &downloadInformationPath, const bool checkIntegrity )
{
  std::unique_lock lock{ mutexV };

  SPDLOG_INFO( "Write Download Information to {}", downloadInformationPath.string() );

  std::fstream downloadInformationStream{ downloadInformationPath, std::ios::out | std::ios::trunc };

  if ( !downloadInformationStream )
  {
    SPDLOG_ERROR( "Could not open {}", downloadInformationPath.string() );
  }

  downloadInformationStream
    << std::format( "{:25} {}\n", "Target ID:", static_cast< std::string >( targetIdV ) )
    << std::format( "{:25} {}\n", "Target Address:", targetAddressV.to_string() )
    << "Requested Files:\n";
  for ( const auto &file : filesV )
  {
    downloadInformationStream << "  " << file << "\n";
  }

  downloadInformationStream
    << std::format(
         "{:25} {:L%F %T %Z}\n",
         "Operation Start:",
         std::chrono::zoned_time( std::chrono::current_zone(), std::chrono::floor< std::chrono::seconds >( startV ) ) )
    << std::format(
         "{:25} {:L%F %T %Z}\n",
         "Operation End:",
         std::chrono::zoned_time( std::chrono::current_zone(), std::chrono::floor< std::chrono::seconds >( endV ) ) );

  for ( const auto &[ file, status ] : filesInformationV )
  {
    downloadInformationStream
      << "File Transfer:\n"
      << std::format( "  {:23} {}\n", "Filename:", file )
      << std::format(
           "  {:23} {}\n",
           "File path:",
           status.filePath.lexically_proximate( downloadInformationPath.parent_path() ).string() )
      << std::format(
           "  {:23} {:L%F %T %Z}\n",
           "Transfer Start:",
           std::chrono::zoned_time(
             std::chrono::current_zone(),
             std::chrono::floor< std::chrono::seconds >( status.start ) ) )
      << std::format(
           "  {:23} {:L%F %T %Z}\n",
           "Transfer End:",
           std::chrono::zoned_time(
             std::chrono::current_zone(),
             std::chrono::floor< std::chrono::seconds >( status.end ) ) )
      << std::format( "  {:23} {}\n", "Transfer Status:", status.fileStatus )
      << std::format( "  {:23} {}\n", "Transfer Size:", status.fileSize );

    if ( !status.partNumber.empty() )
    {
      downloadInformationStream << std::format( "  {:23} {}\n", "Part Number:", status.partNumber );
    }

    if ( status.checkValue != Arinc649::CheckValue::NoCheckValue )
    {
      downloadInformationStream << std::format( "  {:23} {}\n", "Check Value:", status.checkValue );

      if ( checkIntegrity )
      {
        auto calculatedCheckValue{ Arinc649::CheckValueGenerator::checkValue( status.checkValue.type(), status.filePath ) };

        downloadInformationStream << std::format(
          "  {:23} {} *{}*\n",
          "Calculated Check Value:",
          calculatedCheckValue,
          ( status.checkValue == calculatedCheckValue ) ? "PASSED" : "FAILED" );
      }
    }
  }

  downloadInformationStream
    << std::format( "{:25} {}\n", "Overall Status:", std::get< 0 >( overallStatusV ) )
    << std::format( "{:25} {} Bytes\n", "Overall Size:", std::get< 1 >( overallStatusV ) );

  downloadInformationStream.flush();
  downloadInformationStream.close();
}

}
