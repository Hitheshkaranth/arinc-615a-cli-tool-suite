// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615a::Host::OperationImpl.
 **/

#include "OperationImpl.hpp"

#include <arinc_615a/host/implementation/ProtocolImpl.hpp>
#include <arinc_615a/host/OperationHandler.hpp>

#include <arinc_615a/files/InitializationFile.hpp>
#include <arinc_615a/files/ProtocolFileStatistic.hpp>
#include <arinc_615a/files/ProtocolFileTypeDescription.hpp>
#include <arinc_615a/files/ProtocolFilename.hpp>

#include <arinc_615a/tftp/clients/Client.hpp>
#include <arinc_615a/tftp/clients/ReadOperation.hpp>

#include <arinc_615a/tftp/servers/Server.hpp>

#include <arinc_615a/tftp/Arinc615aOptions.hpp>

#include <arinc_615a/Arinc615aException.hpp>
#include <arinc_615a/OperationTypeDescription.hpp>
#include <arinc_615a/StatusCodeDescription.hpp>

#include <tftp/files/MemoryFile.hpp>

#include <tftp/packets/TftpOptions.hpp>

#include <tftp/TftpException.hpp>

#include <spdlog/spdlog.h>

#include <boost/exception/all.hpp>

#include <boost/bind/bind.hpp>

#include <format>

namespace Arinc615a::Host {

OperationImpl::OperationImpl(
  boost::asio::io_context &ioContext,
  Arinc615aConfiguration configuration,
  OperationHandler &handler,
  boost::asio::ip::address targetAddress,
  TargetId targetId,
  std::chrono::seconds dlpTimeout,
  bool portOption ) :
  configurationV{ std::move( configuration ) },
  handlerV{ handler },
  targetAddressV{ std::move( targetAddress ) },
  targetIdV{ std::move( targetId ) },
  dlpTimeoutV{ dlpTimeout },
  portOptionV{ portOption },
  tftpClientV{ Tftp::Clients::Client::instance( ioContext ) },
  tftpServerV{ Tftp::Servers::Server::instance( ioContext ) },
  timerV{ ioContext }
{
  ( *tftpServerV )
    .serverAddress( boost::asio::ip::udp::endpoint{
      configurationV.localInterfaceAddress,
      ( portOptionV ? uint16_t{ 0U } : configurationV.tftpConfiguration.tftpServerPort ) } )
    .requestHandler( std::bind_front( &OperationImpl::receivedTftpRequest, this ) );
}

OperationImpl::~OperationImpl() noexcept = default;

void OperationImpl::doAbort( const Operation::AbortReason reason )
{
  if ( Operation::AbortReason::NoAbort != abortReasonV )
  {
    SPDLOG_INFO( "Already aborted - skip" );
    return;
  }

  SPDLOG_INFO( "Operation abort received" );

  abortReasonV = reason;

  // if initialisation is in-progress, abort them immediately
  if ( initialisationOperationV )
  {
    initialisationOperationV->gracefulAbort( ::Tftp::Packets::ErrorCode::NotDefined, "Abort" );
  }
}

void OperationImpl::doTerminate( const Operation::AbortReason reason )
{
  SPDLOG_INFO( "Terminate Operation" );

  // if initialisation is in-progress, abort them immediately
  if ( initialisationOperationV )
  {
    initialisationOperationV->gracefulAbort( ::Tftp::Packets::ErrorCode::NotDefined, "Abort" );
  }

  StatusCode statusCode;
  switch ( reason )
  {
    using enum StatusCode;
    using enum Operation::AbortReason;

    case Operator:
      statusCode = OperationAbortedByOperator;
      break;

    case Protocol:
      statusCode = OperationAbortedByDlp;
      break;

    default:
      SPDLOG_WARN( "Invalid abort reason" );
      statusCode = OperationAbortedByDlp;
      break;
  }

  finished( statusCode, "Terminate Request" );
}

const Arinc615aConfiguration& OperationImpl::configuration() const
{
  return configurationV;
}

Tftp::Clients::Client & OperationImpl::tftpClient()
{
  return *tftpClientV;
}

Tftp::Servers::Server& OperationImpl::tftpServer()
{
  return *tftpServerV;
}

Files::ProtocolFileLogger& OperationImpl::protocolFileLogger()
{
  return protocolFileLoggerV;
}

void OperationImpl::triggerDlpTimeout( const std::chrono::seconds exceptionTime )
{
  // start timer for time-out handling (the maximum value of exception time and
  // configured DLP timeout
  timerV.expires_after( std::max( exceptionTime, dlpTimeoutV ) );

  // connect time-out operation to timer expiration
  timerV.async_wait( std::bind_front( &OperationImpl::timerHandler, this ) );
}

void OperationImpl::finished( const StatusCode status, std::string_view description )
{
  // Inform handler
  handlerV.finished( status, description );

  // cancel timer
  timerV.cancel();

  // stop TFTP Server
  tftpServerV->stop();
}

std::string OperationImpl::protocolFilename( const Files::ProtocolFileType fileType ) const
{
  return static_cast< std::string > ( Files::ProtocolFilename{ targetIdV, fileType } );
}

TargetId OperationImpl::targetId() const
{
  return targetIdV;
}

boost::asio::ip::address OperationImpl::targetAddress() const
{
  return targetAddressV;
}

bool OperationImpl::isAborted( const boost::asio::ip::udp::endpoint &remote )
{
  if ( abortReasonV == Operation::AbortReason::NoAbort )
  {
    return false;
  }

  SPDLOG_INFO( "Handle abort request" );

  // the status file is responded with an error - the target will send a proper status file
  StatusCode statusCode;

  switch ( abortReasonV )
  {
    using enum Arinc615a::StatusCode;

    case Operation::AbortReason::Operator:
      statusCode = OperationAbortedByOperator;
      break;

    case Operation::AbortReason::Protocol:
      statusCode = OperationAbortedByDlp;
      break;

    default:
      SPDLOG_WARN( "Invalid abort reason" );
      statusCode = OperationAbortedByDlp;
      break;
  }

  // send ABORT error message
  tftpServerV->abortOperation( remote, statusCode );

  // reset abort reason
  abortReasonV = Operation::AbortReason::NoAbort;

  // re-trigger DLP timeout
  triggerDlpTimeout();

  return true;
}

bool OperationImpl::checkRequest( const std::string_view filename, const boost::asio::ip::udp::endpoint &remote )
{
  if ( Files::ProtocolFilename::isProtocolFilename( filename ) )
  {
    if ( const Files::ProtocolFilename protocolFilename{ filename }; protocolFilename.targetId() != targetId() )
    {
      // Reject other files
      SPDLOG_ERROR( "Wrong target ID for given protocol file" );

      tftpServerV->errorOperation(
        remote,
        ::Tftp::Packets::ErrorCode::FileNotFound,
        "Wrong target ID for protocol file" );

      return false;
    }
  }

  return true;
}

void OperationImpl::initialise( const Files::ProtocolFileType fileType )
{
  SPDLOG_INFO( "Request initialisation file '{}'", Files::ProtocolFileTypeDescription::instance().name( fileType ) );

  try
  {
    // Start the TFTP server - wait for incoming messages
    tftpServerV->start();

    auto memFile{ std::make_shared< ::Tftp::Files::MemoryFile >() };
    assert( memFile );

    // Perform read operation of the initialisation file
    initialisationOperationV = tftpClientV->readOperation();
    assert( initialisationOperationV );

    initialisationOperationV
      ->tftpTimeout( configurationV.tftpConfiguration.tftpTimeout )
      .tftpRetries( configurationV.tftpConfiguration.tftpRetries )
      .dally( configurationV.tftpConfiguration.dally )
      .optionsConfiguration( configurationV.tftpOptionsConfiguration )
      .dlpRetries( configurationV.dlpRetries )
      .operationDeferredHandler( std::bind_front( &OperationImpl::initialisationFileDeferred, this ) )
      .optionNegotiationHandler( std::bind_front( &OperationImpl::initialisationFileOptionsNegotiation, this ) )
      .completionHandler( std::bind_front( &OperationImpl::initialisationFileCompleted, this, fileType, memFile ) )
      .dataHandler( memFile )
      .filename( protocolFilename( fileType ) )
      .portOption(
        portOptionV
          ? std::optional< uint16_t >{ tftpServerV->localEndpoint().port() }
          : std::optional< uint16_t >{ std::nullopt } )
      .remote( boost::asio::ip::udp::endpoint{ targetAddress(), configurationV.tftpConfiguration.tftpServerPort } )
      .local( configurationV.localInterfaceAddress );

    initialisationOperationV->request();

    // Update Statistic
    Files::ProtocolFileStatistic::globalReceive().protocolFile( fileType );
  }
  catch ( const Arinc615aException &e )
  {
    SPDLOG_ERROR( "Error receive initialisation file: {}", e.what() );
  }
}

Arinc615aVersion OperationImpl::protocolVersion() const noexcept
{
  return protocolVersionV;
}

void OperationImpl::receivedTftpRequest(
  boost::asio::ip::udp::endpoint remote,
  const Tftp::RequestType requestType,
  const std::string_view filename,
  const ::Tftp::Packets::TftpOptions &clientTftpOptions,
  const Tftp::Arinc615aOptions &clientArinc615aOptions )
{
  SPDLOG_INFO( "Received TFTP request" );

  // call operation request handler
  tftpRequest( std::move( remote ), requestType, std::string{ filename }, clientTftpOptions, clientArinc615aOptions );
}

void OperationImpl::timerHandler( const boost::system::error_code &errorCode )
{
  // operation aborted (packet received)
  if ( boost::asio::error::operation_aborted == errorCode )
  {
    return;
  }

  // internal (timer) error occurred
  if ( errorCode )
  {
    SPDLOG_ERROR( "timer error: {}", errorCode.message() );

    finished( StatusCode::OperationAbortedByDlp, "Internal timer error" );

    return;
  }

  SPDLOG_ERROR( "DLP timeout" );

  //! @todo cancel active transfers

  finished( StatusCode::OperationAbortedByDlp, "DLP Timeout" );
}

void OperationImpl::initialisationFileDeferred( const std::chrono::seconds waitTime )
{
  handlerV.initialisationDeferred( waitTime );
}

bool OperationImpl::initialisationFileOptionsNegotiation( const Tftp::Arinc615aOptions &options )
{
  if ( options.checksum != Arinc649::CheckValue::NoCheckValue )
  {
    SPDLOG_ERROR( "Received checksum option, when never requested" );
    return false;
  }

  if ( options.port )
  {
    if ( !portOptionV )
    {
      SPDLOG_ERROR( "Port option received, when not requested" );
      return false;
    }

    if ( *options.port != tftpServerV->localEndpoint().port() )
    {
      SPDLOG_ERROR( "Port option unequal to requested" );
      return false;
    }
  }
  else
  {
    if ( portOptionV )
    {
      SPDLOG_ERROR( "Port option not accepted - Operation must be restarted with default port" );
      return false;
    }
  }

  return true;
}

void OperationImpl::initialisationFileCompleted(
  const Files::ProtocolFileType fileType,
  ::Tftp::Files::MemoryFilePtr rawInitialisationFile,
  const Tftp::TransferStatus status )
{
  // clean-up initialisation operation
  initialisationOperationV.reset();

  SPDLOG_INFO( "Initialisation file completion handler" );

  if ( Tftp::TransferStatus::Successful != status )
  {
    SPDLOG_ERROR( "Initialisation File could not be received" );

    finished( StatusCode::OperationAbortedByDlp, "Initialisation File could not be received" );
    return;
  }

  try
  {
    protocolFileLoggerV.receivedProtocolFile( protocolFilename( fileType ), rawInitialisationFile->data() );

    // Decode received data as an initialisation file
    Files::InitializationFile initFile{ rawInitialisationFile->data() };

    SPDLOG_INFO(
      "Initialisation Response: {} '{}'",
      StatusCodeDescription::instance().name( static_cast< StatusCode >( initFile.response().code() ) ),
      initFile.response().description() );

    // call handler with status information
    handlerV.initialisationResponse( initFile.response() );

    switch ( initFile.response().code() )
    {
      using enum OperationAcceptanceStatusCode;

      case OperationAccepted:
        // store protocol version for later transfer of protocol files
        protocolVersionV = initFile.protocolVersion();

        // trigger the DLP timeout timer
        triggerDlpTimeout();
        break;

      case OperationDenied:
        SPDLOG_ERROR( "Operation denied: {}", initFile.response().description() );

        finished( static_cast< StatusCode >( initFile.response().code() ), initFile.response().description() );
        break;

      case OperationNotSupported:
        SPDLOG_ERROR( "Operation not supported: {}", initFile.response().description() );

        finished( static_cast< StatusCode >( initFile.response().code() ), initFile.response().description() );
        break;

      default:
        SPDLOG_ERROR( "Unknown acceptance code" );

        finished( StatusCode::OperationAbortedByDlp, "unknown status code" );
        break;
    }
  }
  catch ( const Arinc615aException &e )
  {
    SPDLOG_ERROR( "Decoding/Handling initialisation file: {}", boost::diagnostic_information( e ) );

    finished( StatusCode::OperationAbortedByDlp, "Initialisation File could not be received" );
  }
}

}
