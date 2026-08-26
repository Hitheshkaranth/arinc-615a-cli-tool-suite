// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615a::Target::UploadOperationImpl.
 **/

#include "UploadOperationImpl.hpp"

#include <arinc_615a/target/UploadOperationHandler.hpp>
#include <arinc_615a/target/OperationConfiguration.hpp>

#include <arinc_615a/files/upload/UploadOperationStatusFile.hpp>
#include <arinc_615a/files/upload/UploadOperationRequestFile.hpp>

#include <arinc_615a/files/ProtocolFilename.hpp>
#include <arinc_615a/files/ProtocolFileStatistic.hpp>

#include <arinc_615a/tftp/clients/ReadOperation.hpp>
#include <arinc_615a/tftp/clients/WriteOperation.hpp>

#include <arinc_615a/tftp/servers/Server.hpp>
#include <arinc_615a/tftp/servers/WriteOperation.hpp>

#include <arinc_615a/tftp/Arinc615aOptions.hpp>

#include <arinc_615a/information/UploadLoad.hpp>

#include <arinc_615a/Arinc615aException.hpp>
#include <arinc_615a/StatusCode.hpp>

#include <tftp/files/MemoryFile.hpp>

#include <tftp/packets/TftpOptions.hpp>

#include <spdlog/spdlog.h>

#include <boost/exception/all.hpp>

namespace Arinc615a::Target {

UploadOperationImpl::UploadOperationImpl(
  boost::asio::io_context &ioContext,
  Arinc615aConfiguration dlConfiguration,
  Arinc615aVersion protocolVersion,
  Tftp::Servers::ServerPtr tftpServer,
  UploadOperationConfiguration operationConfiguration ) :
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
    .operation( OperationType::Upload )
    .loggingEnabled( configuration().protocolFileLogging );
}

void UploadOperationImpl::start(
  boost::asio::ip::udp::endpoint hostAddress,
  ::Tftp::Packets::TftpOptions clientTftpOptions,
  const std::optional< uint16_t > port )
{
  doStart( std::move( hostAddress ), std::move( clientTftpOptions ), port );
}

void UploadOperationImpl::accepted()
{
  // set the status
  status( StatusCode::OperationAccepted );

  // force status transmission
  triggerStatusTransmission();
}

void UploadOperationImpl::inProgress(
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

void UploadOperationImpl::inProgress(
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

void UploadOperationImpl::loadAccepted( std::string headerFilename, std::string partNumber )
{
  std::unique_lock lock{ statusMutex };

  auto &load{ statusV.load( std::move( headerFilename ), std::move( partNumber ) ) };
  load.set( Information::Ratio{ 0 }, StatusCode::OperationAccepted );
}

void UploadOperationImpl::loadInProgress(
  std::string_view headerFilename,
  Information::Ratio ratio,
  std::string description )
{
  std::unique_lock lock{ statusMutex };

  auto &load{ statusV.load( headerFilename ) };
  load.set(
    ratio,
    description.empty() ? StatusCode::OperationInProgress : StatusCode::OperationInProgressAdditionalInfo,
    std::move( description ) );
}

void UploadOperationImpl::loadFinished(
  const std::string_view headerFilename,
  const FinalStatus finalStatus,
  std::string description )
{
  std::unique_lock lock{ statusMutex };

  auto &load{ statusV.load( headerFilename ) };
  load.set( load.ratio(), statusCode( finalStatus ), std::move( description ) );
}

void UploadOperationImpl::loadsFinished( const FinalStatus finalStatus, std::string_view description )
{
  std::unique_lock lock{ statusMutex };

  for ( auto &loadStatus : statusV.loads() )
  {
    switch ( loadStatus.code() )
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

    loadStatus.set( loadStatus.ratio(), statusCode( finalStatus ), std::string{ description } );
  }
}

Tftp::Clients::OperationPtr UploadOperationImpl::transferFile(
  Arinc615aOptionNegotiationHandler optionNegotiationHandler,
  Tftp::Clients::OperationCompletedHandler completionHandler,
  Tftp::ReceiveDataHandlerPtr dataHandler,
  std::string filename,
  std::string partNumber,
  Arinc649::CheckValue checkValue )
{
  SPDLOG_INFO( "Receive file '{}'", filename );

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

  return tftpClientReadOperation(
    {}, // Operation deferred is ignored
    std::bind_front( &UploadOperationImpl::fileOptionNegotiation, this, std::move( optionNegotiationHandler ) ),
    std::move( completionHandler ),
    std::move( dataHandler ),
    std::move( filename ),
    std::move( partNumber ),
    std::move( checkValue ) );
}

void UploadOperationImpl::finished( const FinalStatus finalStatus, std::string description )
{
  // set final status
  status( statusCode( finalStatus ), std::move( description ) );

  // force status transmission
  triggerStatusTransmission();
}

void UploadOperationImpl::tftpRequest(
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
      "Request from this address isn't expected" );

    return;
  }

  // Only Write Request (of Request File) is allowed
  if ( Tftp::RequestType::Write != requestType )
  {
    tftpServerErrorOperation( remote, ::Tftp::Packets::ErrorCode::IllegalTftpOperation, "Only WRQ allowed" );

    return;
  }

  // check file type - only protocol files are written to host TFTP server
  if ( Files::ProtocolFileType::UploadRequest != Files::ProtocolFilename{ filename }.fileType() )
  {
    SPDLOG_WARN( "Unexpected file received" );

    tftpServerErrorOperation( remote, ::Tftp::Packets::ErrorCode::FileNotFound, "Wrong filename" );

    return;
  }

  requestFileRequest( std::move( remote ), std::move( clientTftpOptions ), std::move( clientArinc615aOptions ) );

  // Update Statistic
  Files::ProtocolFileStatistic::globalReceive().protocolFile( Files::ProtocolFileType::UploadRequest );
}

void UploadOperationImpl::status( const StatusCode code, std::string description, const int16_t estimatedTime )
{
  std::unique_lock lock{ statusMutex };

  statusV.set( code, std::move( description ) );
  statusV.estimatedTime( estimatedTime );
}

void UploadOperationImpl::statusFile()
{
  std::unique_lock lock{ statusMutex };

  // if status has not been sent previously, send accepted status, set Accepted Status
  if ( Arinc615a::StatusCode::Invalid == statusV.code() )
  {
    SPDLOG_INFO( "Force sending of acceptance status" );
    statusV.code( Arinc615a::StatusCode::OperationAccepted );
  }

  const Files::UploadOperationStatusFile statusFile{ protocolVersion(), statusV };

  const auto file{ std::make_shared< ::Tftp::Files::MemoryFile >( static_cast< Helper::RawData >( statusFile ) ) };
  assert( file );

  protocolFileLogger().transmitProtocolFile( protocolFilename( Files::ProtocolFileType::UploadStatus ), file->data() );

  if ( statusOperation )
  {
    SPDLOG_WARN( "Status Operation not empty" );
  }
  lock.unlock();

  // send the protocol file
  statusOperation = protocolFileOperation(
    Files::ProtocolFileType::UploadStatus,
    {}, // Operation deferred is ignored
    std::bind_front( &UploadOperationImpl::statusFileCompleted, this, statusV ),
    file );
  assert( statusOperation );

  statusOperation->request();

  // increment counter for next status file transmission
  statusV.incrementCounter();

  triggerStatusTransmissionTimer();
}

void UploadOperationImpl::statusFileCompleted(
  Information::UploadStatus sentStatus,
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

void UploadOperationImpl::requestFileRequest(
  boost::asio::ip::udp::endpoint remote,
  ::Tftp::Packets::TftpOptions clientTftpOptions,
  Tftp::Arinc615aOptions clientArinc615aOptions )
{
  SPDLOG_INFO( "Receive upload request file" );

  try
  {
    // no ARINC 615A Options (Checksum or port) expected - discard all
    if ( clientArinc615aOptions )
    {
      SPDLOG_INFO(
        "Received unexpected ARINC 615A options: {}",
        Tftp::Arinc615aOptions_toString( clientArinc615aOptions ) );
    }

    // create TFTP operation for file transfer - reception of request file is WRQ
    auto memFile{ std::make_shared< ::Tftp::Files::MemoryFile>() };
    assert( memFile );

    // TFTP server response to write request
    requestFileOperation = tftpServerWriteOperation();
    assert( requestFileOperation );

    requestFileOperation
      ->tftpTimeout( configuration().tftpConfiguration.tftpTimeout )
      .tftpRetries( configuration().tftpConfiguration.tftpRetries )
      .dally( configuration().tftpConfiguration.dally )
      .optionsConfiguration( configuration().tftpOptionsConfiguration )
      .completionHandler( std::bind_front( &UploadOperationImpl::requestFileCompleted, this, memFile ) )
      .dataHandler( memFile )
      .remote( std::move( remote ) )
      .local( configuration().localInterfaceAddress )
      .clientOptions( std::move( clientTftpOptions ) );

    requestFileOperation->start();
  }
  catch ( const Arinc615aException &e )
  {
    SPDLOG_ERROR( "Error receive request file: {}", boost::diagnostic_information( e ) );
  }
}

void UploadOperationImpl::requestFileCompleted(
  const ::Tftp::Files::MemoryFilePtr rawFile,
  const ::Tftp::TransferStatus transferStatus )
{
  requestFileOperation.reset();

  SPDLOG_INFO( "Receive upload request file completed" );

  if ( transferStatus != ::Tftp::TransferStatus::Successful )
  {
    finished( FinalStatus::AbortedByTargetHardware, "Request file transmission failed" );
    return;
  }

  try
  {
    protocolFileLogger().receivedProtocolFile(
      protocolFilename( Files::ProtocolFileType::UploadRequest ),
      rawFile->data() );

    // decode received data as upload request file
    Files::UploadOperationRequestFile requestFile{ rawFile->data() };

    // Check Received Protocol Version
    if ( requestFile.protocolVersion() != protocolVersion() )
    {
      SPDLOG_ERROR( "Receive upload request file protocol version invalid" );

      finished( FinalStatus::AbortedByTargetHardware, "Request file protocol version invalid" );
      return;
    }

    // call handler
    handler.loadList( requestFile.loads() );
  }
  catch ( const Arinc615aException &e )
  {
    SPDLOG_ERROR( "Receive upload request file format error: {}", boost::diagnostic_information( e ) );

    finished( FinalStatus::AbortedByTargetHardware, "Request file format error" );
  }
}

bool UploadOperationImpl::fileOptionNegotiation(
  Arinc615aOptionNegotiationHandler optionNegotiationHandler,
  const Tftp::Arinc615aOptions &options )
{
  if ( options.port )
  {
    SPDLOG_ERROR( "Received unexpected ARINC 615A Port Option" );
    return false;
  }

  // call handler for part number and checksum options
  return optionNegotiationHandler( options.partNumber, options.checksum );
}

}
