// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615a::Target::OperationImpl.
 **/

#include "OperationImpl.hpp"

#include <arinc_615a/files/InitializationFile.hpp>
#include <arinc_615a/files/ProtocolFilename.hpp>
#include <arinc_615a/files/ProtocolFileTypeDescription.hpp>
#include <arinc_615a/files/ProtocolFileStatistic.hpp>

#include <arinc_615a/tftp/clients/Client.hpp>
#include <arinc_615a/tftp/clients/ReadOperation.hpp>
#include <arinc_615a/tftp/clients/WriteOperation.hpp>

#include <arinc_615a/tftp/servers/Server.hpp>
#include <arinc_615a/tftp/servers/ReadOperation.hpp>

#include <arinc_615a/tftp/Arinc615aOptions.hpp>

#include <arinc_615a/information/Status.hpp>

#include <arinc_615a/Arinc615aException.hpp>

#include <tftp/files/MemoryFile.hpp>

#include <tftp/packets/TftpOptions.hpp>

#include <spdlog/spdlog.h>

#include <utility>

namespace Arinc615a::Target {

OperationImpl::OperationImpl(
  boost::asio::io_context &ioContext,
  Arinc615aConfiguration configuration,
  const Arinc615aVersion protocolVersion,
  Tftp::Servers::ServerPtr tftpServer,
  OperationHandler &handler,
  TargetId targetId,
  const std::chrono::seconds statusTransmissionRate ):
  configurationV{ std::move( configuration ) },
  protocolVersionV{ protocolVersion },
  tftpServerV{ std::move( tftpServer ) },
  tftpClientV{ Tftp::Clients::Client::instance( ioContext ) },
  handlerV{ handler },
  targetIdV{ std::move( targetId ) },
  statusTransmissionRateV{ statusTransmissionRate },
  hostPortV{ configurationV.tftpConfiguration.tftpServerPort },
  statusTransmissionTimerV{ ioContext }
{
}

OperationImpl::~OperationImpl() = default;

const Arinc615aConfiguration& OperationImpl::configuration() const noexcept
{
  return configurationV;
}

Arinc615aVersion OperationImpl::protocolVersion() const noexcept
{
  return protocolVersionV;
}

Files::ProtocolFileLogger& OperationImpl::protocolFileLogger()
{
  return protocolFileLoggerV;
}

std::string OperationImpl::protocolFilename( const Files::ProtocolFileType fileType ) const
{
  return static_cast< std::string >( Files::ProtocolFilename{ targetIdV, fileType } );
}

void OperationImpl::doStart(
  boost::asio::ip::udp::endpoint hostAddress,
  ::Tftp::Packets::TftpOptions clientTftpOptions,
  const std::optional< uint16_t > port )
{
  // store host address
  hostAddressV = hostAddress.address();
  // store host port if provided
  if ( port )
  {
    hostPortV = *port;
  }

  // Send initialisation response
  sendInitFile( std::move( hostAddress ), std::move( clientTftpOptions ), port );
}

void OperationImpl::tftpServerErrorOperation(
  const boost::asio::ip::udp::endpoint &remote,
  const ::Tftp::Packets::ErrorCode errorCode,
  std::string errorMessage )
{
  tftpServerV->errorOperation( remote, errorCode, std::move( errorMessage ) );
}

Tftp::Servers::WriteOperationPtr OperationImpl::tftpServerWriteOperation()
{
  auto operation{ tftpServerV->writeOperation() };
  assert( operation );
  return operation;
}

Tftp::Clients::ReadOperationPtr OperationImpl::tftpClientReadOperation(
  Tftp::Clients::OperationDeferredHandler operationDeferredHandler,
  Tftp::Clients::OptionNegotiationHandler optionNegotiationHandler,
  Tftp::Clients::OperationCompletedHandler completionHandler,
  Tftp::ReceiveDataHandlerPtr dataHandler,
  std::string filename,
  std::string partNumberOption,
  Arinc649::CheckValue checksumOption )
{
  auto operation{ tftpClientV->readOperation() };
  assert( operation );

  (*operation)
    .tftpTimeout( configurationV.tftpConfiguration.tftpTimeout )
    .tftpRetries( configurationV.tftpConfiguration.tftpRetries )
    .dally( configurationV.tftpConfiguration.dally )
    .optionsConfiguration( configurationV.tftpOptionsConfiguration )
    .dlpRetries( configurationV.dlpRetries )
    .operationDeferredHandler( std::move( operationDeferredHandler ) )
    .optionNegotiationHandler( std::move( optionNegotiationHandler ) )
    .completionHandler( std::move( completionHandler ) )
    .dataHandler( std::move( dataHandler ) )
    .filename( std::move( filename ) )
    .partNumberOption( std::move( partNumberOption ) )
    .checksumOption( std::move( checksumOption ) )
    .remote( boost::asio::ip::udp::endpoint{ hostAddressV, hostPortV } )
    .local( configurationV.localInterfaceAddress );

  return operation;
}

Tftp::Clients::WriteOperationPtr OperationImpl::tftpClientWriteOperation(
  Tftp::Clients::OperationDeferredHandler operationDeferredHandler,
  Tftp::Clients::OptionNegotiationHandler optionNegotiationHandler,
  Tftp::Clients::OperationCompletedHandler completionHandler,
  Tftp::TransmitDataHandlerPtr dataHandler,
  std::string filename,
  std::string partNumberOption,
  Arinc649::CheckValue checksumOption )
{
  auto operation{ tftpClientV->writeOperation() };

  (*operation)
    .tftpTimeout( configuration().tftpConfiguration.tftpTimeout )
    .tftpRetries( configuration().tftpConfiguration.tftpRetries )
    .optionsConfiguration( configurationV.tftpOptionsConfiguration )
    .dlpRetries( configurationV.dlpRetries )
    .operationDeferredHandler( std::move( operationDeferredHandler ) )
    .optionNegotiationHandler( std::move( optionNegotiationHandler ) )
    .completionHandler( std::move( completionHandler ) )
    .dataHandler( std::move( dataHandler ) )
    .filename( std::move( filename ) )
    .partNumberOption( std::move( partNumberOption ) )
    .checksumOption( std::move( checksumOption ) )
    .remote( boost::asio::ip::udp::endpoint{ hostAddressV, hostPortV  } )
    .local( configurationV.localInterfaceAddress );

  return operation;
}

void OperationImpl::sendInitFile(
  boost::asio::ip::udp::endpoint hostAddress,
  ::Tftp::Packets::TftpOptions clientTftpOptions,
  const std::optional< uint16_t > port )
{
  SPDLOG_INFO( "Send initialisation file with status 'Operation Accepted'" );

  try
  {
    // create the initialisation file
    Arinc615a::Files::InitializationFile initFile{
      protocolVersionV,
      Information::InitializationResponse{ OperationAcceptanceStatusCode::OperationAccepted } };

    const auto file{ std::make_shared< ::Tftp::Files::MemoryFile >( static_cast< Helper::RawData >( initFile ) ) };
    assert( file );

    protocolFileLoggerV.transmitProtocolFile( std::format( "{}.INIT", targetIdV ), file->data() );

    // TFTP server response to read request
    initialisationOperationV = tftpServerV->readOperation();
    assert( initialisationOperationV );

    ( *initialisationOperationV )
      .tftpTimeout( configurationV.tftpConfiguration.tftpTimeout )
      .tftpRetries( configurationV.tftpConfiguration.tftpRetries )
      .completionHandler( std::bind_front( &OperationImpl::sendInitFileComplete, this ) )
      .dataHandler( file )
      .remote( std::move( hostAddress ) )
      .local( configuration().localInterfaceAddress )
      .clientOptions( std::move( clientTftpOptions ) )
      .negotiatedArinc615aOptions(
        Tftp::Arinc615aOptions{
          .port = port,
          .partNumber = {},
          .checksum = Arinc649::CheckValue::NoCheckValue } );

    initialisationOperationV->start();
  }
  catch ( const Arinc615aException &e )
  {
    SPDLOG_ERROR( "Error send initialisation file: {}", e.what() );
  }
}

void OperationImpl::sendInitFileComplete( const ::Tftp::TransferStatus transferStatus )
{
  SPDLOG_INFO( "Send initialisation file completed" );

  initialisationOperationV.reset();

  if ( transferStatus != ::Tftp::TransferStatus::Successful )
  {
    // finalise operation
    finalise( FinalStatus::AbortedByTargetHardware, "Initialisation file transmission failed" );
    return;
  }

  // trigger status transmission
  triggerStatusTransmissionTimer();

  // call handler
  handlerV.initialised();
}

Tftp::Clients::WriteOperationPtr OperationImpl::protocolFileOperation(
  const Files::ProtocolFileType fileType,
  Tftp::Clients::OperationDeferredHandler operationDeferredHandler,
  Tftp::Clients::OperationCompletedHandler completionHandler,
  Tftp::TransmitDataHandlerPtr dataHandler,
  Arinc649::CheckValue checkValue )
{
  SPDLOG_INFO( "Send protocol file {}", Files::ProtocolFileTypeDescription::instance().name( fileType ) );

  // check for the checksum option
  if ( ( Arinc649::CheckValue::NoCheckValue != checkValue ) && ( Arinc615aVersion::Arinc615a34 != protocolVersion() ) )
  {
    SPDLOG_INFO( "Checksum option requested when not in ARINC 615A-3/4 mode" );
  }

  bool handleAbort{ false };

  switch ( fileType )
  {
    using enum Arinc615a::Files::ProtocolFileType;

    case LoadConfigurationStatus:
    case UploadStatus:
    case DownloadStatus:
      // for status files abort handling is activated
      handleAbort = true;
      break;

    default:
      break;
  }

  // Update Statistic
  Files::ProtocolFileStatistic::globalTransmit().protocolFile( fileType );

  auto operation{ tftpClientV->writeOperation() };

  operation
    ->tftpTimeout( configurationV.tftpConfiguration.tftpTimeout )
    .tftpRetries( configurationV.tftpConfiguration.tftpRetries )
    .optionsConfiguration( configurationV.tftpOptionsConfiguration )
    .dlpRetries( configurationV.dlpRetries )
    .handleAbort( handleAbort )
    .operationDeferredHandler( std::move( operationDeferredHandler ) )
    .optionNegotiationHandler( std::bind_front( &OperationImpl::protocolFileOptionsNegotiation, this, checkValue ) )
    .completionHandler( std::move( completionHandler ) )
    .dataHandler( std::move( dataHandler ) )
    .filename( protocolFilename( fileType ) )
    .checksumOption( std::move( checkValue ) )
    .remote( boost::asio::ip::udp::endpoint{ hostAddressV, hostPortV } )
    .local( configurationV.localInterfaceAddress );

  return operation;
}

void OperationImpl::triggerStatusTransmission()
{
  // start timer for time-out handling
  statusTransmissionTimerV.expires_after( std::chrono::seconds::zero() );

  // connect time-out operation to timer expiration
  statusTransmissionTimerV.async_wait( std::bind_front( &OperationImpl::statusTransmissionTimerHandler, this ) );
}

void OperationImpl::triggerStatusTransmissionTimer()
{
  // start timer for time-out handling
  statusTransmissionTimerV.expires_after( statusTransmissionRateV );

  // connect time-out operation to timer expiration
  statusTransmissionTimerV.async_wait( std::bind_front( &OperationImpl::statusTransmissionTimerHandler, this ) );
}

void OperationImpl::finalise( FinalStatus finalStatus, std::string_view description )
{
  SPDLOG_INFO( "ARINC 615A Target Operation finished" );

  // inform handler
  handlerV.finished( finalStatus, description );

  // Stop DLP Timer
  statusTransmissionTimerV.cancel();
}

const boost::asio::ip::address& OperationImpl::hostAddress() const
{
  return hostAddressV;
}

void OperationImpl::statusTransmissionTimerHandler( const boost::system::error_code& errorCode )
{
  // operation aborted
  if ( boost::asio::error::operation_aborted == errorCode )
  {
    SPDLOG_INFO( "Status timer aborted" );
    return;
  }

  if ( errorCode )
  {
    // internal (timer) error occurred
    SPDLOG_ERROR( "timer error: {}", errorCode.message() );
  }

  SPDLOG_INFO( "Status transmission timeout" );

  // send status
  statusFile();
}

bool OperationImpl::protocolFileOptionsNegotiation(
  [[maybe_unused]] const Arinc649::CheckValue &providedCheckValue,
  const Tftp::Arinc615aOptions &serverOptions )
{
  // ARINC 615A Port Option is only provided on Initialisation File Request.
  if ( serverOptions.port )
  {
    SPDLOG_ERROR( "Received unexpected ARINC 615A Port Option for protocol file" );
    return false;
  }

  // ARINC 615A Part Number Option isn't provided for Protocol Files
  if ( !serverOptions.partNumber.empty() )
  {
    SPDLOG_ERROR( "Received unexpected ARINC 615A Part Number Option for protocol file" );
    return false;
  }

  if ( Arinc649::CheckValue::NoCheckValue == providedCheckValue )
  {
    if ( Arinc649::CheckValue::NoCheckValue != serverOptions.checksum )
    {
      SPDLOG_ERROR( "Received unexpected ARINC 615A Checksum Option for protocol file" );
      return false;
    }
  }
  else
  {
    if ( providedCheckValue != serverOptions.checksum )
    {
      SPDLOG_ERROR( "Received ARINC 615A Checksum Option differs for protocol file" );
      return false;
    }
  }

  return true;
}

}
