// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615a::Tftp::Clients::WriteOperationImpl.
 **/

#include "WriteOperationImpl.hpp"

#include <arinc_615a/tftp/Arinc615aOptions.hpp>

#include <tftp/clients/Client.hpp>
#include <tftp/clients/WriteOperation.hpp>

#include <utility>

namespace Arinc615a::Tftp::Clients {

WriteOperationImpl::WriteOperationImpl( ::Tftp::Clients::ClientPtr tftpClient, boost::asio::io_context &ioContext ) :
  OperationImpl{ ioContext },
  tftpClient{ std::move( tftpClient ) }
{
}

WriteOperation& WriteOperationImpl::tftpTimeout( const std::chrono::seconds timeout )
{
  tftpTimeoutV = timeout;
  return *this;
}

WriteOperation& WriteOperationImpl::tftpRetries( const uint16_t retries )
{
  tftpRetriesV = retries;
  return *this;
}

WriteOperation& WriteOperationImpl::optionsConfiguration( ::Tftp::TftpOptionsConfiguration optionsConfiguration )
{
  tftpOptionsConfigurationV = std::move( optionsConfiguration );
  return *this;
}

WriteOperation& WriteOperationImpl::dlpRetries( const uint16_t retries )
{
  OperationImpl::dlpRetries( retries );
  return *this;
}

WriteOperation& WriteOperationImpl::handleAbort( const bool handleAbort )
{
  OperationImpl::handleAbort( handleAbort );
  return *this;
}

WriteOperation& WriteOperationImpl::operationDeferredHandler( OperationDeferredHandler handler )
{
  OperationImpl::operationDeferredHandler( std::move( handler ) );
  return *this;
}

WriteOperation& WriteOperationImpl::optionNegotiationHandler( OptionNegotiationHandler handler )
{
  OperationImpl::optionNegotiationHandler( std::move( handler ) );
  return *this;
}

WriteOperation& WriteOperationImpl::completionHandler( OperationCompletedHandler handler )
{
  OperationImpl::completionHandler( std::move( handler ) );
  return *this;
}

WriteOperation& WriteOperationImpl::dataHandler( TransmitDataHandlerPtr handler )
{
  dataHandlerV = std::move( handler );
  return *this;
}

WriteOperation& WriteOperationImpl::filename( std::string filename )
{
  filenameV = std::move( filename );
  return *this;
}

WriteOperation& WriteOperationImpl::partNumberOption( std::string partNumberOption )
{
  partNumberOptionV = std::move( partNumberOption );
  return *this;
}

WriteOperation& WriteOperationImpl::checksumOption( Arinc649::CheckValue checksumOption )
{
  checksumOptionV = std::move( checksumOption );
  return *this;
}

WriteOperation& WriteOperationImpl::remote( boost::asio::ip::udp::endpoint remote )
{
  remoteV = std::move( remote );
  return *this;
}

WriteOperation& WriteOperationImpl::local( boost::asio::ip::address local )
{
  localV = std::move( local );
  return *this;
}

void WriteOperationImpl::request()
{
  OperationImpl::request();
}

void WriteOperationImpl::gracefulAbort( const ::Tftp::Packets::ErrorCode errorCode, std::string errorMessage )
{
  OperationImpl::gracefulAbort( errorCode, std::move( errorMessage ) );
}

void WriteOperationImpl::abort()
{
  OperationImpl::abort();
}

const ::Tftp::Packets::ErrorInformation& WriteOperationImpl::errorInformation() const
{
  return OperationImpl::errorInformation();
}

::Tftp::Clients::OperationPtr WriteOperationImpl::tftpOperation()
{
  ::Tftp::Packets::Options additionalOptions{};

  // Add Part Number Option
  if ( !partNumberOptionV.empty() )
  {
    additionalOptions.try_emplace(
      std::string{ Arinc615aOptions_name( KnownOptions::PartNumber ) },
      partNumberOptionV );
  }

  // Add Checksum Option
  if ( Arinc649::CheckValueType::NotUsed != checksumOptionV.type() )
  {
    additionalOptions.try_emplace(
      std::string{ Arinc615aOptions_name( checksumOptionV.type() ) },
      checksumOptionV.toString() );
  }

  auto operation{ tftpClient->writeOperation() };

  operation
    ->tftpTimeout( tftpTimeoutV )
    .tftpRetries( tftpRetriesV )
    .optionsConfiguration( tftpOptionsConfigurationV )
    .optionNegotiationHandler(
      std::bind_front( &WriteOperationImpl::handleOptionNegotiation, static_cast< OperationImpl * >( this ) ) )
    .completionHandler(
      std::bind_front( &WriteOperationImpl::handleCompletion, static_cast< OperationImpl * >( this ) ) )
    .dataHandler( dataHandlerV )
    .filename( filenameV )
    .mode( ::Tftp::Packets::TransferMode::OCTET )
    .additionalOptions( std::move( additionalOptions ) )
    .remote( remoteV )
    .local( boost::asio::ip::udp::endpoint{ localV, 0 } );

  return operation;
}

}
