// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615a::Find::Clients::QueryImpl.
 **/

#include "QueryImpl.hpp"

#include <arinc_615a/find/TargetInformation.hpp>

#include <arinc_615a/find/clients/FindClientException.hpp>

#include <arinc_615a/find/packets/Packet.hpp>
#include <arinc_615a/find/packets/PacketStatistic.hpp>

#include <helper/Exception.hpp>

#include <spdlog/spdlog.h>

#include <boost/exception/all.hpp>

#include <format>

namespace Arinc615a::Find::Clients {

QueryImpl::QueryImpl( boost::asio::io_context &ioContext ):
  timerV{ ioContext },
  socketV{ ioContext },
  rawReceivePacketV( Packets::Packet::MaxPacketSize )
{
}

QueryImpl::~QueryImpl() noexcept = default;

Query& QueryImpl::responseHandler( ResponseHandler responseHandler )
{
  responseHandlerV = std::move( responseHandler );
  return *this;
}

Query& QueryImpl::completionHandler( CompletionHandler completionHandler )
{
  completionHandlerV = std::move( completionHandler );
  return *this;
}

Query& QueryImpl::localAddress( boost::asio::ip::address localAddress )
{
  localAddressV = std::move( localAddress );
  return *this;
}

Query& QueryImpl::remoteAddress( boost::asio::ip::address remoteAddress )
{
  remoteAddressV = std::move( remoteAddress );
  return *this;
}

Query& QueryImpl::port( const uint16_t port )
{
  portV = port;
  return *this;
}

Query& QueryImpl::dynamicLocalPort( const bool dynamicLocalPort )
{
  dynamicLocalPortV = dynamicLocalPort;
  return *this;
}

Query& QueryImpl::timeout( const std::chrono::seconds timeout )
{
  timeoutV = timeout;
  return *this;
}

void QueryImpl::start()
{
  try
  {
    const boost::asio::ip::udp::endpoint remote{ remoteAddressV, portV };

    const boost::asio::ip::udp::endpoint local{ localAddressV, dynamicLocalPortV ? uint16_t{ 0U } : portV };

    socketV.open( remote.protocol() );
    socketV.set_option( boost::asio::ip::udp::socket::broadcast( true ) );
    socketV.bind( local );

    // send request packet
    send( remote, Packets::Packet{ Packets::Opcode::InformationRequest, 1U } );

    // initiate receive responses
    receive();

    // start timer for time-out handling
    timerV.expires_after( timeoutV );

    // connect time-out operation to timer expiration
    timerV.async_wait( std::bind_front( &QueryImpl::timerHandler, this ) );
  }
  catch ( const boost::system::system_error &err )
  {
    if ( completionHandlerV )
    {
      completionHandlerV();
    }

    BOOST_THROW_EXCEPTION( FindClientException() << Helper::AdditionalInfo{ err.what() } );
  }
}

void QueryImpl::abort()
{
  try
  {
    socketV.cancel();
    timerV.cancel();

    socketV.close();
  }
  catch ( const boost::system::system_error &err )
  {
    if ( completionHandlerV )
    {
      completionHandlerV();
    }

    BOOST_THROW_EXCEPTION( FindClientException() << Helper::AdditionalInfo{ err.what() } );
  }

  if ( completionHandlerV )
  {
    completionHandlerV();
  }
}

void QueryImpl::send( const boost::asio::ip::udp::endpoint &remote, const Packets::Packet &packet )
{
  SPDLOG_INFO( "Send FIND packet to {}:{}", remote.address().to_string(), remote.port() );

  auto rawPacket{ packet.encode() };

  // Update statistic
  Packets::PacketStatistic::globalTransmit().packet( packet.opcode(), rawPacket.size() );

  // send request packet
  socketV.send_to( boost::asio::buffer( rawPacket ), remote );
}

void QueryImpl::receive()
{
  // resize packet to hold maximum amount of data
  rawReceivePacketV.resize( Packets::Packet::MaxPacketSize );

  // call receive operation
  socketV.async_receive_from(
    boost::asio::buffer( rawReceivePacketV ),
    receiveRemoteEndpointV,
    std::bind_front( &QueryImpl::receiveHandler, this ) );
}

void QueryImpl::timerHandler( const boost::system::error_code &error )
{
  if ( boost::asio::error::operation_aborted == error )
  {
    // time-out waiting aborted
    return;
  }

  if ( error )
  {
    // timer error
    SPDLOG_ERROR( "timer error" );
  }

  try
  {
    socketV.cancel();
    socketV.close();
  }
  catch ( const boost::system::system_error &err )
  {
    if ( completionHandlerV )
    {
      completionHandlerV();
    }

    BOOST_THROW_EXCEPTION( FindClientException() << Helper::AdditionalInfo{ err.what() } );
  }

  if ( completionHandlerV )
  {
    completionHandlerV();
  }
}

void QueryImpl::receiveHandler( const boost::system::error_code &error, const std::size_t bytesTransferred )
{
  if ( boost::asio::error::operation_aborted == error )
  {
    // read operation aborted
    return;
  }

  if ( error )
  {
    // receive error
    SPDLOG_ERROR( "Error when receiving message - error code {}: {}", error.value(), error.message() );
  }
  else
  {
    // data received
    packet( receiveRemoteEndpointV, { rawReceivePacketV.begin(), bytesTransferred } );
  }

  // restart receive operation
  receive();
}

void QueryImpl::informationRequestPacket(
  const boost::asio::ip::udp::endpoint &remote,
  [[maybe_unused]] const Packets::Packet &request )
{
  SPDLOG_INFO( "Ignoring unexpected FIND information request packet received from: {}", remote.address().to_string() );
}

void QueryImpl::informationAnswerPacket( const boost::asio::ip::udp::endpoint &remote, const Packets::Packet &answer )
{
  if ( answer.numberOfParameters() != std::to_underlying( Packets::ParameterList::Last ) )
  {
    SPDLOG_WARN(
      "invalid number of parameters actual: {}, expected {}",
      answer.numberOfParameters(),
      std::to_underlying( Packets::ParameterList::Last ) );

    return;
  }

  TargetInformation targetInformation{
    answer.parameter( std::to_underlying( Packets::ParameterList::ThwId ) ),
    answer.parameter( std::to_underlying( Packets::ParameterList::ThwTypeName ) ),
    answer.parameter( std::to_underlying( Packets::ParameterList::ThwPosition ) ),
    answer.parameter( std::to_underlying( Packets::ParameterList::LiteralName ) ),
    answer.parameter( std::to_underlying( Packets::ParameterList::ManufacturerCode ) ) };

  if ( responseHandlerV )
  {
    responseHandlerV( remote.address(), targetInformation );
  }
}

void QueryImpl::invalidPacket(
  const boost::asio::ip::udp::endpoint &remote,
  [[maybe_unused]] Helper::ConstRawDataSpan rawPacket )
{
  SPDLOG_WARN( "Ignoring invalid packet received from: {}", remote.address().to_string() );
}

}
