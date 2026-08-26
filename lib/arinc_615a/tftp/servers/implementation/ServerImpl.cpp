// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Tftp::Servers::ServerImpl.
 **/

#include "ServerImpl.hpp"

#include <arinc_615a/tftp/servers/implementation/ReadOperationImpl.hpp>
#include <arinc_615a/tftp/servers/implementation/WriteOperationImpl.hpp>

#include <arinc_615a/tftp/Arinc615aOptions.hpp>
#include <arinc_615a/tftp/ErrorMessage.hpp>

#include <tftp/servers/Server.hpp>
#include <tftp/servers/ReadOperation.hpp>
#include <tftp/servers/WriteOperation.hpp>

#include <tftp/packets/Options.hpp>

#include <spdlog/spdlog.h>

namespace Arinc615a::Tftp::Servers {

ServerImpl::ServerImpl( boost::asio::io_context &ioContext ):
  baseTftpServerV{ ::Tftp::Servers::Server::instance( ioContext ) }
{
  assert( baseTftpServerV );
  baseTftpServerV->requestHandler( std::bind_front( &ServerImpl::tftpRequestHandler, this ) );
}

ServerImpl::~ServerImpl() = default;

Server& ServerImpl::requestHandler( ReceivedTftpRequestHandler handler )
{
  tftpRequestHandlerV = std::move( handler );
  return *this;
}

Server& ServerImpl::serverAddress(
  boost::asio::ip::udp::endpoint serverAddress )
{
  assert( baseTftpServerV );
  baseTftpServerV->serverAddress( std::move( serverAddress ) );
  return *this;
}

boost::asio::ip::udp::endpoint ServerImpl::localEndpoint() const
{
  assert( baseTftpServerV );
  return baseTftpServerV->localEndpoint();
}

void ServerImpl::start()
{
  assert( baseTftpServerV );
  baseTftpServerV->start();
}

void ServerImpl::stop()
{
  assert( baseTftpServerV );
  baseTftpServerV->stop();
}

ReadOperationPtr ServerImpl::readOperation()
{
  assert( baseTftpServerV );
  auto operation{ std::make_shared< ReadOperationImpl >( baseTftpServerV->readOperation() ) };
  assert( operation );
  return operation;
}

WriteOperationPtr ServerImpl::writeOperation()
{
  assert( baseTftpServerV );
  auto operation{ std::make_shared< WriteOperationImpl >( baseTftpServerV->writeOperation() ) };
  assert( operation );
  return operation;
}

void ServerImpl::errorOperation(
  const boost::asio::ip::udp::endpoint &remote,
  const ::Tftp::Packets::ErrorCode errorCode,
  std::string errorMessage )
{
  assert( baseTftpServerV );
  baseTftpServerV->errorOperation(
    remote,
    boost::asio::ip::udp::endpoint{ localEndpoint().address(), 0 },
    errorCode,
    std::move( errorMessage ) );
}

void ServerImpl::waitOperation( const boost::asio::ip::udp::endpoint &remote, const std::chrono::seconds waitTime )
{
  assert( baseTftpServerV );
  // send error packet wit WAIT Error
  baseTftpServerV->errorOperation(
    remote,
    boost::asio::ip::udp::endpoint{ localEndpoint().address(), 0 },
    ::Tftp::Packets::ErrorCode::NotDefined,
    ErrorMessage_wait( waitTime ) );
}

void ServerImpl::abortOperation( const boost::asio::ip::udp::endpoint &remote, const StatusCode status )
{
  assert( baseTftpServerV );
  // send error packet wit ABORT Error
  baseTftpServerV->errorOperation(
    remote,
    boost::asio::ip::udp::endpoint{ localEndpoint().address(), 0 },
    ::Tftp::Packets::ErrorCode::NotDefined,
    ErrorMessage_abort( status ) );
}

void ServerImpl::tftpRequestHandler(
  const boost::asio::ip::udp::endpoint &remote,
  const RequestType requestType,
  std::string_view filename,
  const ::Tftp::Packets::TransferMode mode,
  const ::Tftp::Packets::TftpOptions &clientOptions,
  const ::Tftp::Packets::Options &additionalClientOptions )
{
  // check transfer mode (only OCTET accepted)
  if ( mode != ::Tftp::Packets::TransferMode::OCTET )
  {
    SPDLOG_ERROR( "Invalid transfer mode" );

    errorOperation( remote, ::Tftp::Packets::ErrorCode::IllegalTftpOperation, "Wrong transfer mode" );

    return;
  }

  auto remainingAdditionalClientOptions{ additionalClientOptions };
  Arinc615aOptions options{};

  // Port Option
  auto [ portValid, portValue ] = ::Tftp::Packets::Options_getOption< uint16_t >(
    remainingAdditionalClientOptions,
    Arinc615aOptions_name( KnownOptions::Port ) );

  if ( portValue )
  {
    options.port = portValue;
  }

  // Part Number
  // TODO remove std::string generation if P2077R3 is implemented within stdlibc++ (GCC)
  auto partNumber{ remainingAdditionalClientOptions.extract(
    std::string{ Arinc615aOptions_name( KnownOptions::PartNumber ) } ) };
  if ( partNumber )
  {
    options.partNumber = std::move( partNumber.mapped() );
  }

  // Checksum Option
  auto [ checksumValid, checksumValue ] = Arinc615aOptions_checksum( remainingAdditionalClientOptions );

  options.checksum = std::move( checksumValue );

  if ( !remainingAdditionalClientOptions.empty() )
  {
    SPDLOG_INFO(
      "There are additional options ignored: {}",
      ::Tftp::Packets::Options_toString( remainingAdditionalClientOptions ) );
  }

  // Call TFTP Request Handler
  tftpRequestHandlerV(
    remote,
    requestType,
    filename,
    clientOptions,
    options );
}

}
