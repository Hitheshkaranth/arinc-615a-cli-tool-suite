// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615a::Target::MediaDefinedDownloadOperationImpl.
 **/

#include "MediaDefinedDownloadOperationImpl.hpp"

#include <arinc_615a/files/download/DownloadOperationRequestFile.hpp>
#include <arinc_615a/files/download/DownloadOperationStatusFile.hpp>

#include <arinc_615a/files/ProtocolFilename.hpp>
#include <arinc_615a/files/ProtocolFileStatistic.hpp>

#include <arinc_615a/target/OperationConfiguration.hpp>

#include <arinc_615a/tftp/clients/Client.hpp>
#include <arinc_615a/tftp/clients/WriteOperation.hpp>

#include <arinc_615a/tftp/servers/Server.hpp>
#include <arinc_615a/tftp/servers/WriteOperation.hpp>

#include <arinc_615a/tftp/Arinc615aOptions.hpp>

#include <arinc_615a/Arinc615aException.hpp>
#include <arinc_615a/StatusCode.hpp>

#include <tftp/files/MemoryFile.hpp>

#include <tftp/packets/TftpOptions.hpp>

#include <spdlog/spdlog.h>

#include <boost/exception/all.hpp>

namespace Arinc615a::Target {

MediaDefinedDownloadOperationImpl::MediaDefinedDownloadOperationImpl(
  boost::asio::io_context &ioContext,
  Arinc615aConfiguration dlConfiguration,
  Arinc615aVersion protocolVersion,
  Tftp::Servers::ServerPtr tftpServer,
  MediaDefinedDownloadOperationConfiguration operationConfiguration ) :
  OperationImpl{
    ioContext,
    std::move( dlConfiguration ),
    protocolVersion,
    std::move( tftpServer ),
    operationConfiguration.handler,
    std::move( operationConfiguration.targetId ),
    operationConfiguration.statusTransmissionRate },
  handler{ operationConfiguration.handler }
{
  protocolFileLogger()
    .operation( OperationType::MediaDefinedDownload )
    .loggingEnabled( configuration().protocolFileLogging );
}

void MediaDefinedDownloadOperationImpl::start(
  boost::asio::ip::udp::endpoint hostAddress,
  ::Tftp::Packets::TftpOptions clientTftpOptions,
  const std::optional< uint16_t > port )
{
  doStart( std::move( hostAddress ), std::move( clientTftpOptions ), port );
}

void MediaDefinedDownloadOperationImpl::accepted()
{
  // set the status
  status( StatusCode::OperationAccepted );

  // force status transmission
  triggerStatusTransmission();
}

void MediaDefinedDownloadOperationImpl::inProgress(
  const bool immediateTransmission,
  const int16_t estimatedTime,
  std::string description )
{
  // set the status
  status(
    description.empty() ? StatusCode::OperationInProgress : StatusCode::OperationInProgressAdditionalInfo,
    std::move( description ),
    estimatedTime );

  // If immediate transmission is requested
  if ( immediateTransmission )
  {
    // force status transmission
    triggerStatusTransmission();
  }
}

void MediaDefinedDownloadOperationImpl::inProgress(
  const bool immediateTransmission,
  const int16_t estimatedTime,
  std::string description,
  const Information::Ratio listRatio )
{
  std::unique_lock lock{ statusMutex };

  statusV.set(
    description.empty() ? StatusCode::OperationInProgress : StatusCode::OperationInProgressAdditionalInfo,
    std::move( description ) );

  statusV.estimatedTime( estimatedTime );

  statusV.listRatio( listRatio );

  lock.unlock();

  // If immediate transmission is requested
  if ( immediateTransmission )
  {
    // force status transmission
    triggerStatusTransmission();
  }
}

void MediaDefinedDownloadOperationImpl::fileAccepted( std::string filename )
{
  std::unique_lock lock{ statusMutex };

  auto &file{ statusV.file( std::move( filename ) ) };
  file.set( StatusCode::OperationAccepted );
}

void MediaDefinedDownloadOperationImpl::fileInProgress( std::string_view filename, std::string description )
{
  std::unique_lock lock{ statusMutex };

  auto &file{ statusV.file( filename ) };
  file.set(
    description.empty() ? StatusCode::OperationInProgress : StatusCode::OperationInProgressAdditionalInfo,
    std::move( description ) );
}

void MediaDefinedDownloadOperationImpl::fileFinished(
  std::string_view filename,
  FinalStatus finalStatus,
  std::string description )
{
  std::unique_lock lock{ statusMutex };

  auto &file{ statusV.file( filename ) };
  file.set( statusCode( finalStatus ), std::move( description ) );
}

void MediaDefinedDownloadOperationImpl::filesFinished( const FinalStatus finalStatus, std::string_view description )
{
  std::unique_lock lock{ statusMutex };

  for ( auto &fileStatus : statusV.files() )
  {
    switch ( fileStatus.code() )
    {
      using enum Arinc615a::StatusCode;

      case OperationNotAccepted:
      case OperationNotSupported:
      case OperationCompleted:
      case OperationAbortedByTargetHw:
      case OperationAbortedByDlp:
      case OperationAbortedByOperator:
      case LoadPartNumberOrDownloadFileFailed:
        // Don't update this file status.
        continue;

      default:
        break;
    }

    fileStatus.set( statusCode( finalStatus ), std::string{ description } );
  }
}

Tftp::Clients::OperationPtr MediaDefinedDownloadOperationImpl::transferFile(
  Arinc615aOptionNegotiationHandler optionNegotiationHandler,
  Tftp::Clients::OperationCompletedHandler completionHandler,
  Tftp::TransmitDataHandlerPtr dataHandler,
  std::string filename,
  std::string partNumber,
  Arinc649::CheckValue checkValue )
{
  SPDLOG_INFO( "Send file {}", filename );

  // check for the part number option
  if ( ( !partNumber.empty() ) && ( Arinc615aVersion::Arinc615a34 != protocolVersion() ) )
  {
    SPDLOG_INFO( "Part Number option requested when not in ARINC 615A-3/4 mode" );
  }

  // check for the checksum option
  if ( ( Arinc649::CheckValue::NoCheckValue != checkValue ) && ( Arinc615aVersion::Arinc615a34 != protocolVersion() ) )
  {
    SPDLOG_INFO( "Checksum option requested when not in ARINC 615A-3/4 mode" );
  }

  return tftpClientWriteOperation(
    {}, // Operation deferred is ignored
    std::bind_front(
      &MediaDefinedDownloadOperationImpl::fileOptionNegotiation,
      this,
      std::move( optionNegotiationHandler ) ),
    std::move( completionHandler ),
    std::move( dataHandler ),
    std::move( filename ),
    std::move( partNumber ),
    std::move( checkValue ) );
}

void MediaDefinedDownloadOperationImpl::finished( const FinalStatus finalStatus, std::string description )
{
  // set final status
  status( statusCode( finalStatus ), std::move( description ) );

  // force status transmission
  triggerStatusTransmission();
}

void MediaDefinedDownloadOperationImpl::tftpRequest(
  boost::asio::ip::udp::endpoint remote,
  const Tftp::RequestType requestType,
  std::string filename,
  ::Tftp::Packets::TftpOptions clientTftpOptions,
  Tftp::Arinc615aOptions clientArinc615aOptions )
{
  // validate host address against received request
  if ( hostAddress() != remote.address() )
  {
    SPDLOG_WARN(
      "TFTP request from unknown remote: {}:{} expected {}",
      remote.address().to_string(),
      remote.port(),
      hostAddress().to_string() );

    tftpServerErrorOperation(
      remote,
      ::Tftp::Packets::ErrorCode::IllegalTftpOperation,
      "Request from this address not expected" );

    return;
  }

  // Only Write Request (of Request File) is allowed
  if ( Tftp::RequestType::Write != requestType )
  {
    tftpServerErrorOperation( remote, ::Tftp::Packets::ErrorCode::IllegalTftpOperation, "Only WRQ allowed" );

    return;
  }

  // check file type - only protocol files are written to host TFTP server
  if ( Files::ProtocolFileType::MediaDefinedDownloadRequest != Files::ProtocolFilename{ filename }.fileType() )
  {
    SPDLOG_WARN( "Unexpected file received" );

    tftpServerErrorOperation( remote, ::Tftp::Packets::ErrorCode::FileNotFound, "Wrong filename" );

    return;
  }

  requestFileRequest( std::move( remote ), std::move( clientTftpOptions ), std::move( clientArinc615aOptions ) );

  // Update Statistic
  Files::ProtocolFileStatistic::globalReceive().protocolFile( Files::ProtocolFileType::MediaDefinedDownloadRequest );
}

void MediaDefinedDownloadOperationImpl::status( const StatusCode code, std::string description, const int16_t estimatedTime )
{
  std::unique_lock lock{ statusMutex };

  statusV.set( code, std::move( description ) );
  statusV.estimatedTime( estimatedTime );
}

void MediaDefinedDownloadOperationImpl::statusFile()
{
  std::unique_lock lock{ statusMutex };

  // if status has not been sent previously, send accepted status, set Accepted Status
  if ( Arinc615a::StatusCode::Invalid == statusV.code() )
  {
    SPDLOG_INFO( "Force sending of acceptance status" );
    statusV.code( Arinc615a::StatusCode::OperationAccepted );
  }

  const Files::DownloadOperationStatusFile statusFile{ protocolVersion(), statusV };

  const auto file{ std::make_shared< ::Tftp::Files::MemoryFile >( static_cast< Helper::RawData >( statusFile ) ) };
  assert( file );

  protocolFileLogger().transmitProtocolFile(
    protocolFilename( Files::ProtocolFileType::DownloadStatus ),
    file->data() );

  if ( statusOperation )
  {
    SPDLOG_WARN( "Status Operation not empty" );
  }
  lock.unlock();

  // send the protocol file
  statusOperation = protocolFileOperation(
    Files::ProtocolFileType::DownloadStatus,
    {}, // Operation deferred is ignored
    std::bind_front( &MediaDefinedDownloadOperationImpl::statusFileCompleted, this, statusV ),
    file );
  assert( statusOperation );

  statusOperation->request();

  // increment counter for next status file transmission
  statusV.incrementCounter();

  triggerStatusTransmissionTimer();
}

void MediaDefinedDownloadOperationImpl::statusFileCompleted(
  Information::DownloadStatus sentStatus,
  const Tftp::TransferStatus transferStatus )
{
  statusOperation.reset();

  SPDLOG_INFO( "Send status completed" );

  switch ( transferStatus )
  {
    using enum Tftp::TransferStatus;

    case Successful:
      break;

    case OperationAbortedByDlp:
      handler.abortRequest( AbortRequest::AbortByDlp );
      return;

    case OperationAbortedByOperator:
      handler.abortRequest( AbortRequest::AbortByOperator );
      return;

    default:
      SPDLOG_ERROR( "Sending of status failed" );

      // finalise operation
      finalise( FinalStatus::AbortedByTargetHardware, "Status transmission failed" );

      return;
  }

  // call handler to inform
  handler.status( sentStatus );

  switch ( sentStatus.code() )
  {
    using enum StatusCode;

    // First transmission of status -> set to in progress
    case OperationAccepted:
    case OperationInProgress:
    case OperationInProgressAdditionalInfo:
      triggerStatusTransmissionTimer();
      break;

    default:
      // finalise operation
      finalise( finalStatus( sentStatus.code() ), sentStatus.description() );
      break;
  }
}

void MediaDefinedDownloadOperationImpl::requestFileRequest(
  boost::asio::ip::udp::endpoint remote,
  ::Tftp::Packets::TftpOptions clientTftpOptions,
  Tftp::Arinc615aOptions clientArinc615aOptions )
{
  SPDLOG_INFO( "Receive download request file request file" );

  try
  {
    // no ARINC 615A Options (Checksum or port) expected - discard all
    if ( clientArinc615aOptions )
    {
      SPDLOG_INFO( "Received unexpected ARINC 615A options: {}", Arinc615aOptions_toString( clientArinc615aOptions ) );
    }

    // create TFTP operation for file transfer - reception of request file is WRQ
    auto memFile{ std::make_shared< ::Tftp::Files::MemoryFile>() };
    assert( memFile );

    // TFTP server response to write request
    requestFileOperation = tftpServerWriteOperation();
    assert( requestFileOperation );

    ( *requestFileOperation )
      .tftpTimeout( configuration().tftpConfiguration.tftpTimeout )
      .tftpRetries( configuration().tftpConfiguration.tftpRetries )
      .dally( configuration().tftpConfiguration.dally )
      .optionsConfiguration( configuration().tftpOptionsConfiguration )
      .completionHandler( std::bind_front( &MediaDefinedDownloadOperationImpl::requestFileCompleted, this, memFile ) )
      .dataHandler( memFile )
      .remote( std::move( remote ) )
      .local( configuration().localInterfaceAddress )
      .clientOptions( std::move( clientTftpOptions ) );

    requestFileOperation->start();
  }
  catch ( const Arinc615aException &e )
  {
    SPDLOG_ERROR( "Error receive answer file: {}", boost::diagnostic_information( e ) );
  }
}

void MediaDefinedDownloadOperationImpl::requestFileCompleted(
  const ::Tftp::Files::MemoryFilePtr rawFile,
  const ::Tftp::TransferStatus transferStatus )
{
  requestFileOperation.reset();

  SPDLOG_INFO( "Receive download request file completed" );

  if ( transferStatus != ::Tftp::TransferStatus::Successful )
  {
    finished( FinalStatus::AbortedByTargetHardware, "Receive dowmload request file failed" );
    return;
  }

  try
  {
    protocolFileLogger().receivedProtocolFile(
      protocolFilename( Files::ProtocolFileType::MediaDefinedDownloadRequest ),
      rawFile->data() );

    // decode received data as download request file
    Files::DownloadOperationRequestFile requestFile{ rawFile->data() };

    // Check Received Protocol Version
    if ( requestFile.protocolVersion() != protocolVersion() )
    {
      SPDLOG_ERROR( "Receive download request file protocol version invalid" );

      finished( FinalStatus::AbortedByTargetHardware, "Request file protocol version invalid" );
      return;
    }

    // call handler
    handler.downloadingRequest( requestFile.files(), requestFile.userDefinedData() );
  }
  catch ( const Arinc615aException &e )
  {
    SPDLOG_ERROR( "Receive download request file format error: {}", boost::diagnostic_information( e ) );

    finished( FinalStatus::AbortedByTargetHardware, "Request file format error" );
  }
}

bool MediaDefinedDownloadOperationImpl::fileOptionNegotiation(
  Arinc615aOptionNegotiationHandler optionNegotiationHandler,
  const Tftp::Arinc615aOptions &options )
{
  if ( options.port )
  {
    SPDLOG_ERROR( "Received unexpected ARINC 615A Port Option" );
    return false;
  }

  // call handler for the checksum option
  return optionNegotiationHandler( options.partNumber, options.checksum );
}

}
