// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615a::Target::OperatorDefinedDownloadOperationImpl.
 **/

#include "OperatorDefinedDownloadOperationImpl.hpp"

#include <arinc_615a/files/download/DownloadOperationAnswerFile.hpp>
#include <arinc_615a/files/download/DownloadOperationListFile.hpp>
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

OperatorDefinedDownloadOperationImpl::OperatorDefinedDownloadOperationImpl(
  boost::asio::io_context &ioContext,
  Arinc615aConfiguration dlConfiguration,
  Arinc615aVersion protocolVersion,
  Tftp::Servers::ServerPtr tftpServer,
  OperatorDefinedDownloadOperationConfiguration operationConfiguration ) :
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
    .operation( OperationType::OperatorDefinedDownload )
    .loggingEnabled( configuration().protocolFileLogging );
}

void OperatorDefinedDownloadOperationImpl::start(
  boost::asio::ip::udp::endpoint hostAddress,
  ::Tftp::Packets::TftpOptions clientTftpOptions,
  const std::optional< uint16_t > port )
{
  doStart( std::move( hostAddress ), std::move( clientTftpOptions ), port );
}

void OperatorDefinedDownloadOperationImpl::accepted()
{
  // set the status
  status( StatusCode::OperationAccepted );

  // force status transmission
  triggerStatusTransmission();
}

void OperatorDefinedDownloadOperationImpl::inProgress(
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

void OperatorDefinedDownloadOperationImpl::inProgress(
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

void OperatorDefinedDownloadOperationImpl::fileAccepted( std::string filename )
{
  std::unique_lock lock{ statusMutex };

  auto &file{ statusV.file( std::move( filename ) ) };
  file.set( StatusCode::OperationAccepted );
}

void OperatorDefinedDownloadOperationImpl::fileInProgress( std::string_view filename, std::string description )
{
  std::unique_lock lock{ statusMutex };

  auto &file{ statusV.file( filename ) };
  file.set(
    description.empty() ? StatusCode::OperationInProgress : StatusCode::OperationInProgressAdditionalInfo,
    std::move( description ) );
}

void OperatorDefinedDownloadOperationImpl::fileFinished(
  std::string_view filename,
  FinalStatus finalStatus,
  std::string description )
{
  std::unique_lock lock{ statusMutex };

  auto &file{ statusV.file( filename ) };
  file.set( statusCode( finalStatus ), std::move( description ) );
}

void OperatorDefinedDownloadOperationImpl::filesFinished( const FinalStatus finalStatus, std::string_view description )
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

Tftp::Clients::OperationPtr OperatorDefinedDownloadOperationImpl::transferFile(
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
      &OperatorDefinedDownloadOperationImpl::fileOptionNegotiation,
      this,
      std::move( optionNegotiationHandler ) ),
    std::move( completionHandler ),
    std::move( dataHandler ),
    std::move( filename ),
    std::move( partNumber ),
    std::move( checkValue ) );
}

void OperatorDefinedDownloadOperationImpl::finished( const FinalStatus finalStatus, std::string description )
{
  // set final status
  status( statusCode( finalStatus ), std::move( description ) );

  // force status transmission
  triggerStatusTransmission();
}

void OperatorDefinedDownloadOperationImpl::tftpRequest(
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

  // Only Write Request (of Answer File) is allowed
  if ( Tftp::RequestType::Write != requestType )
  {
    tftpServerErrorOperation( remote, ::Tftp::Packets::ErrorCode::IllegalTftpOperation, "Only WRQ allowed" );

    return;
  }

  // check file type - only protocol files are written to host TFTP server
  if ( Files::ProtocolFileType::OperatorDefinedDownloadAnswer != Files::ProtocolFilename{ filename }.fileType() )
  {
    SPDLOG_WARN( "Unexpected file received" );

    tftpServerErrorOperation( remote, ::Tftp::Packets::ErrorCode::FileNotFound, "Wrong filename" );

    return;
  }

  answerFileRequest( std::move( remote ), std::move( clientTftpOptions ), std::move( clientArinc615aOptions ) );

  // Update Statistic
  Files::ProtocolFileStatistic::globalReceive().protocolFile( Files::ProtocolFileType::OperatorDefinedDownloadAnswer );
}

void OperatorDefinedDownloadOperationImpl::downloadingList( Information::DownloadFilesInformation files )
{
  const Files::DownloadOperationListFile listFile( protocolVersion(), std::move( files ) );

  auto file{ std::make_shared< ::Tftp::Files::MemoryFile >( static_cast< Helper::RawData >( listFile ) ) };
  assert( file );

  protocolFileLogger().transmitProtocolFile(
    protocolFilename( Files::ProtocolFileType::OperatorDefinedDownloadList ),
    file->data() );

  // send the protocol file
  downloadingListOperation = protocolFileOperation(
    Files::ProtocolFileType::OperatorDefinedDownloadList,
    {}, // Operation deferred is ignored
    std::bind_front( &OperatorDefinedDownloadOperationImpl::downloadingListCompleted, this ),
    file );
  assert( downloadingListOperation );

  downloadingListOperation->request();
}

void OperatorDefinedDownloadOperationImpl::status(
  const StatusCode code,
  std::string description,
  const int16_t estimatedTime )
{
  std::unique_lock lock{ statusMutex };

  statusV.set( code, std::move( description ) );
  statusV.estimatedTime( estimatedTime );
}

void OperatorDefinedDownloadOperationImpl::statusFile()
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
    std::bind_front( &OperatorDefinedDownloadOperationImpl::statusFileCompleted, this, statusV ),
    file );
  assert( statusOperation );

  statusOperation->request();

  // increment counter for next status file transmission
  statusV.incrementCounter();

  triggerStatusTransmissionTimer();
}

void OperatorDefinedDownloadOperationImpl::statusFileCompleted(
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

void OperatorDefinedDownloadOperationImpl::answerFileRequest(
  boost::asio::ip::udp::endpoint remote,
  ::Tftp::Packets::TftpOptions clientTftpOptions,
  Tftp::Arinc615aOptions clientArinc615aOptions )
{
  SPDLOG_INFO( "Receive download answer file request file" );

  try
  {
    // no ARINC 615A Options (Checksum or port) expected - discard all
    if ( clientArinc615aOptions )
    {
      SPDLOG_INFO( "Received unexpected ARINC 615A options: {}", Arinc615aOptions_toString( clientArinc615aOptions ) );
    }

    // create TFTP operation for file transfer - reception of answer file is WRQ
    auto memFile{ std::make_shared< ::Tftp::Files::MemoryFile>() };
    assert( memFile );

    // TFTP server response to write request
    answerFileOperation = tftpServerWriteOperation();
    assert( answerFileOperation );

    ( *answerFileOperation )
      .tftpTimeout( configuration().tftpConfiguration.tftpTimeout )
      .tftpRetries( configuration().tftpConfiguration.tftpRetries )
      .dally( configuration().tftpConfiguration.dally )
      .optionsConfiguration( configuration().tftpOptionsConfiguration )
      .completionHandler( std::bind_front( &OperatorDefinedDownloadOperationImpl::answerFileCompleted, this, memFile ) )
      .dataHandler( memFile )
      .remote( std::move( remote ) )
      .local( configuration().localInterfaceAddress )
      .clientOptions( std::move( clientTftpOptions ) );

    answerFileOperation->start();
  }
  catch ( const Arinc615aException &e )
  {
    SPDLOG_ERROR( "Error receive answer file: {}", boost::diagnostic_information( e ) );
  }
}

void OperatorDefinedDownloadOperationImpl::answerFileCompleted(
  const ::Tftp::Files::MemoryFilePtr rawFile,
  const ::Tftp::TransferStatus transferStatus )
{
  answerFileOperation.reset();

  SPDLOG_INFO( "Receive download answer file completed" );

  if ( transferStatus != ::Tftp::TransferStatus::Successful )
  {
    finished( FinalStatus::AbortedByTargetHardware, "Receive download answer file failed" );
    return;
  }

  try
  {
    protocolFileLogger().receivedProtocolFile(
      protocolFilename( Files::ProtocolFileType::OperatorDefinedDownloadAnswer ),
      rawFile->data() );

    // decode received data as download answer file
    Files::DownloadOperationAnswerFile answerFile{ rawFile->data() };

    // Check Received Protocol Version
    if ( answerFile.protocolVersion() != protocolVersion() )
    {
      SPDLOG_ERROR( "Receive download answer file protocol version invalid" );

      finished( FinalStatus::AbortedByTargetHardware, "Answer file protocol version invalid" );
      return;
    }

    // call handler
    handler.downloadingAnswer( answerFile.files() );
  }
  catch ( const Arinc615aException &e )
  {
    SPDLOG_ERROR( "Receive download answer file format error: {}", boost::diagnostic_information( e ) );

    finished( FinalStatus::AbortedByTargetHardware, "Answer file format error" );
  }
}

void OperatorDefinedDownloadOperationImpl::downloadingListCompleted( const Tftp::TransferStatus transferStatus )
{
  downloadingListOperation.reset();

  SPDLOG_INFO( "Send downloading list completed" );

  if ( transferStatus != Tftp::TransferStatus::Successful )
  {
    finished( FinalStatus::AbortedByTargetHardware, "Target Information transmission failed" );
    return;
  }
}

bool OperatorDefinedDownloadOperationImpl::fileOptionNegotiation(
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
