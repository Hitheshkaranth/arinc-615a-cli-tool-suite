// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615a::Tftp::Servers::ReadOperationImpl.
 **/

#include "ReadOperationImpl.hpp"

#include <arinc_615a/tftp/Arinc615aOptions.hpp>

#include <tftp/servers/ReadOperation.hpp>

#include <tftp/packets/TftpOptions.hpp>

#include <tftp/TftpOptionsConfiguration.hpp>

namespace Arinc615a::Tftp::Servers {

ReadOperationImpl::ReadOperationImpl( ::Tftp::Servers::ReadOperationPtr operation ) :
  operationV{ std::move( operation ) }
{
}

ReadOperation& ReadOperationImpl::tftpTimeout(
  const std::chrono::seconds timeout )
{
  assert( operationV );
  operationV->tftpTimeout( timeout );
  return *this;
}

ReadOperation& ReadOperationImpl::tftpRetries( const uint16_t retries )
{
  assert( operationV );
  operationV->tftpRetries( retries );
  return *this;
}

ReadOperation& ReadOperationImpl::optionsConfiguration( ::Tftp::TftpOptionsConfiguration optionsConfiguration )
{
  assert( operationV );
  operationV->optionsConfiguration( std::move( optionsConfiguration ) );
  return *this;
}

ReadOperation& ReadOperationImpl::completionHandler( OperationCompletedHandler handler )
{
  assert( operationV );
  operationV->completionHandler( std::move( handler ) );
  return *this;
}

ReadOperation& ReadOperationImpl::dataHandler( TransmitDataHandlerPtr handler )
{
  assert( operationV );
  operationV->dataHandler( std::move( handler ) );
  return *this;
}

ReadOperation& ReadOperationImpl::remote( boost::asio::ip::udp::endpoint remote )
{
  assert( operationV );
  operationV->remote( std::move( remote ) );
  return *this;
}

ReadOperation& ReadOperationImpl::local( boost::asio::ip::address local )
{
  assert( operationV );
  operationV->local( boost::asio::ip::udp::endpoint{ std::move( local ), 0 } );
  return *this;
}

ReadOperation& ReadOperationImpl::clientOptions( ::Tftp::Packets::TftpOptions clientOptions )
{
  assert( operationV );
  operationV->clientOptions( clientOptions );
  return *this;
}

ReadOperation& ReadOperationImpl::negotiatedArinc615aOptions( Arinc615aOptions options )
{
  assert( operationV );
  operationV->additionalNegotiatedOptions( options.options() );
  return *this;
}

void ReadOperationImpl::start()
{
  assert( operationV );
  operationV->start();
}

void ReadOperationImpl::gracefulAbort( const ::Tftp::Packets::ErrorCode errorCode, std::string errorMessage )
{
  assert( operationV );
  operationV->gracefulAbort( errorCode, std::move( errorMessage ) );
}

void ReadOperationImpl::abort()
{
  assert( operationV );
  operationV->abort();
}

const ::Tftp::Packets::ErrorInformation& ReadOperationImpl::errorInformation() const
{
  assert( operationV );
  return operationV->errorInformation();
}

}
