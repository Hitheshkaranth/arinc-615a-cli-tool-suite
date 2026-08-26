// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615a::Tftp::Clients::ReadOperationImpl.
 **/

#include "ReadOperationImpl.hpp"

#include <arinc_615a/tftp/Arinc615aOptions.hpp>

#include <tftp/clients/Client.hpp>
#include <tftp/clients/ReadOperation.hpp>

#include <utility>

namespace Arinc615a::Tftp::Clients {

ReadOperationImpl::ReadOperationImpl( ::Tftp::Clients::ClientPtr tftpClient, boost::asio::io_context &ioContext ) :
  OperationImpl{ ioContext },
  tftpClient{ std::move( tftpClient ) }
{
}

ReadOperation& ReadOperationImpl::tftpTimeout( const std::chrono::seconds timeout )
{
  tftpTimeoutV = timeout;
  return *this;
}

ReadOperation& ReadOperationImpl::tftpRetries( const uint16_t retries )
{
  tftpRetriesV = retries;
  return *this;
}

ReadOperation& ReadOperationImpl::dally( const bool dally )
{
  dallyV = dally;
  return *this;
}

ReadOperation& ReadOperationImpl::optionsConfiguration( ::Tftp::TftpOptionsConfiguration optionsConfiguration )
{
  tftpOptionsConfigurationV = std::move( optionsConfiguration );
  return *this;
}

ReadOperation& ReadOperationImpl::dlpRetries( const uint16_t retries )
{
  OperationImpl::dlpRetries( retries );
  return *this;
}

ReadOperation& ReadOperationImpl::operationDeferredHandler( OperationDeferredHandler handler )
{
  OperationImpl::operationDeferredHandler( std::move( handler ) );
  return *this;
}

ReadOperation& ReadOperationImpl::optionNegotiationHandler( OptionNegotiationHandler handler )
{
  OperationImpl::optionNegotiationHandler( std::move( handler ) );
  return *this;
}

ReadOperation& ReadOperationImpl::completionHandler( OperationCompletedHandler handler )
{
  OperationImpl::completionHandler( std::move( handler ) );
  return *this;
}

ReadOperation& ReadOperationImpl::dataHandler( ReceiveDataHandlerPtr handler )
{
  dataHandlerV = std::move( handler );
  return *this;
}

ReadOperation& ReadOperationImpl::filename( std::string filename )
{
  filenameV = std::move( filename );
  return *this;
}

ReadOperation& ReadOperationImpl::portOption( std::optional< uint16_t > portOption )
{
  portOptionV = std::move( portOption );
  return *this;
}

ReadOperation& ReadOperationImpl::partNumberOption( std::string partNumberOption )
{
  partNumberOptionV = std::move( partNumberOption );
  return *this;
}

ReadOperation& ReadOperationImpl::checksumOption( Arinc649::CheckValue checksumOption )
{
  checksumOptionV = std::move( checksumOption );
  return *this;
}

ReadOperation& ReadOperationImpl::remote( boost::asio::ip::udp::endpoint remote )
{
  remoteV = std::move( remote );
  return *this;
}

ReadOperation& ReadOperationImpl::local( boost::asio::ip::address local )
{
  localV = std::move( local );
  return *this;
}

void ReadOperationImpl::request()
{
  OperationImpl::request();
}

void ReadOperationImpl::gracefulAbort( const ::Tftp::Packets::ErrorCode errorCode, std::string errorMessage )
{
  OperationImpl::gracefulAbort( errorCode, std::move( errorMessage ) );
}

void ReadOperationImpl::abort()
{
  OperationImpl::abort();
}

const ::Tftp::Packets::ErrorInformation& ReadOperationImpl::errorInformation() const
{
  return OperationImpl::errorInformation();
}

::Tftp::Clients::OperationPtr ReadOperationImpl::tftpOperation()
{
  ::Tftp::Packets::Options additionalOptions{};

  // Add Port Option
  if ( portOptionV )
  {
    additionalOptions.try_emplace(
      std::string{ Arinc615aOptions_name( KnownOptions::Port ) },
      std::to_string( *portOptionV ) );
  }

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

  auto operation{ tftpClient->readOperation() };

  operation
    ->tftpTimeout( tftpTimeoutV )
    .tftpRetries( tftpRetriesV )
    .dally( dallyV )
    .optionsConfiguration( tftpOptionsConfigurationV )
    .optionNegotiationHandler(
      std::bind_front( &ReadOperationImpl::handleOptionNegotiation, static_cast< OperationImpl * >( this ) ) )
    .completionHandler(
      std::bind_front( &ReadOperationImpl::handleCompletion, static_cast< OperationImpl * >( this ) ) )
    .dataHandler( dataHandlerV )
    .filename( filenameV )
    .mode( ::Tftp::Packets::TransferMode::OCTET )
    .additionalOptions( std::move( additionalOptions ) )
    .remote( remoteV )
    .local( boost::asio::ip::udp::endpoint{ localV, 0 } );

  return operation;
}

}
