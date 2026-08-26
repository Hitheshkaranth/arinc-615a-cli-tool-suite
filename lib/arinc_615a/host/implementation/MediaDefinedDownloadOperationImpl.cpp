// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615a::Host::MediaDefinedDownloadOperationImpl.
 **/

#include "MediaDefinedDownloadOperationImpl.hpp"
#include "ProtocolImpl.hpp"

#include <arinc_615a/host/OperationConfiguration.hpp>
#include <arinc_615a/host/MediaDefinedDownloadOperationHandler.hpp>

#include <arinc_615a/files/download/DownloadOperationRequestFile.hpp>

#include <arinc_615a/files/ProtocolFilename.hpp>
#include <arinc_615a/files/ProtocolFileStatistic.hpp>

#include <arinc_615a/tftp/clients/Client.hpp>
#include <arinc_615a/tftp/clients/WriteOperation.hpp>

#include <arinc_615a/tftp/servers/Server.hpp>

#include <arinc_615a/tftp/Arinc615aOptions.hpp>

#include <tftp/files/MemoryFile.hpp>

#include <tftp/packets/TftpOptions.hpp>

#include <spdlog/spdlog.h>

namespace Arinc615a::Host {

MediaDefinedDownloadOperationImpl::MediaDefinedDownloadOperationImpl(
  boost::asio::io_context &ioContext,
  MediaDefinedDownloadOperationConfiguration operationConfiguration ) :
  DownloadOperationImpl{
    ioContext,
    std::move( operationConfiguration.dataLoaderConfiguration ),
    operationConfiguration.handler,
    std::move( operationConfiguration.targetAddress ),
    std::move( operationConfiguration.targetId ),
    operationConfiguration.dlpTimeout,
    operationConfiguration.portOption },
  handlerV{ operationConfiguration.handler }
{
  protocolFileLogger()
    .loggingEnabled( configuration().protocolFileLogging )
    .operation( OperationType::MediaDefinedDownload );
}

MediaDefinedDownloadOperationImpl::~MediaDefinedDownloadOperationImpl() = default;

void MediaDefinedDownloadOperationImpl::start()
{
  requestSentV = false;
  // send initialisation
  initialise( Files::ProtocolFileType::MediaDefinedDownloadInitialization );
}

void MediaDefinedDownloadOperationImpl::abort( const AbortReason reason )
{
  doAbort( reason );
}

void MediaDefinedDownloadOperationImpl::terminate( const AbortReason reason )
{
  doTerminate( reason );
}

Tftp::Servers::WriteOperationPtr MediaDefinedDownloadOperationImpl::fileTransfer(
  Tftp::ReceiveDataHandlerPtr dataHandler,
  boost::asio::ip::udp::endpoint remote,
  ::Tftp::Packets::TftpOptions clientTftpOptions,
  std::string partNumber,
  Arinc649::CheckValue checkValue )
{
  return doFileTransfer(
    std::move( dataHandler ),
    std::move( remote ),
    std::move( clientTftpOptions ),
    std::move( partNumber ),
    std::move( checkValue ) );
}

void MediaDefinedDownloadOperationImpl::waitOperation(
  const boost::asio::ip::udp::endpoint &remote,
  const std::chrono::seconds waitTime )
{
  doWaitOperation( remote, waitTime );
}

void MediaDefinedDownloadOperationImpl::errorOperation(
  const boost::asio::ip::udp::endpoint &remote,
  const ::Tftp::Packets::ErrorCode errorCode,
  std::string errorMessage )
{
  doErrorOperation( remote, errorCode, std::move( errorMessage ) );
}

void MediaDefinedDownloadOperationImpl::request( Information::DownloadFiles files, Helper::RawData userDefinedData )
{
  if ( requestSentV )
  {
    SPDLOG_WARN( "Request can be only transmitted once" );
    return;
  }

  Files::DownloadOperationRequestFile requestFile{
    protocolVersion(),
    std::move( files ),
    std::move( userDefinedData ) };

  const auto file{ std::make_shared< ::Tftp::Files::MemoryFile >( static_cast< Helper::RawData >( requestFile ) ) };
  assert( file );

  protocolFileLogger().transmitProtocolFile(
    protocolFilename( Files::ProtocolFileType::MediaDefinedDownloadRequest ),
    file->data() );

  // Perform write operation of the download request file
  requestOperationV = tftpClient().writeOperation();
  assert( requestOperationV );

  requestOperationV
    ->tftpTimeout( configuration().tftpConfiguration.tftpTimeout )
    .tftpRetries( configuration().tftpConfiguration.tftpRetries )
    .optionsConfiguration( configuration().tftpOptionsConfiguration )
    .dlpRetries( configuration().dlpRetries )
    .optionNegotiationHandler( std::bind_front( &MediaDefinedDownloadOperationImpl::requestOptionsNegotiation, this ) )
    .completionHandler( std::bind_front( &MediaDefinedDownloadOperationImpl::requestCompleted, this ) )
    .dataHandler( file )
    .filename(
      static_cast< std::string >(
        Files::ProtocolFilename{ targetId(), Files::ProtocolFileType::MediaDefinedDownloadRequest } ) )
    .remote( boost::asio::ip::udp::endpoint{ targetAddress(), configuration().tftpConfiguration.tftpServerPort } )
    .local( configuration().localInterfaceAddress );

  requestOperationV->request();

  // Update Statistic
  Files::ProtocolFileStatistic::globalTransmit().protocolFile( Files::ProtocolFileType::MediaDefinedDownloadRequest );
}

void MediaDefinedDownloadOperationImpl::tftpRequest(
  boost::asio::ip::udp::endpoint remote,
  const Tftp::RequestType requestType,
  std::string filename,
  ::Tftp::Packets::TftpOptions clientTftpOptions,
  Tftp::Arinc615aOptions clientArinc615aOptions )
{
  // check request
  if ( !checkRequest( filename, remote ) )
  {
    // Check of request failed - already reported - exit now
    return;
  }

  switch ( requestType )
  {
    using enum ::Tftp::RequestType;

    case Read:
      SPDLOG_ERROR( "Unexpected read request" );
      tftpServer().errorOperation( remote, ::Tftp::Packets::ErrorCode::IllegalTftpOperation, "Unexpected read request" );
      break;

    case Write:
      // check file type
      // NOLINTNEXTLINE( readability-trivial-switch ): Keep for better protocol file readability
      switch ( Files::ProtocolFilename{ filename }.fileType() )
      {
        using enum Files::ProtocolFileType;

        case DownloadStatus:
          // Handle the status file
          statusFileRequest( std::move( remote ), std::move( clientTftpOptions ), std::move( clientArinc615aOptions ) );

          // Update Statistic
          Files::ProtocolFileStatistic::globalReceive().protocolFile( DownloadStatus );
          break;

        default:
          // All other files are handled as data files
          SPDLOG_INFO(
            "Data File Request '{}' Part Number '{}' Checksum '{}'",
            filename,
            clientArinc615aOptions.partNumber,
            clientArinc615aOptions.checksum.toString() );

          // handlerV directly handles the file request (Must create TFTP operation and execute them)
          handlerV.fileRequest(
            std::move( remote ),
            std::move( filename ),
            std::move( clientTftpOptions ),
            std::move( clientArinc615aOptions.partNumber ),
            std::move( clientArinc615aOptions.checksum ) );
          break;
      }
      break;

    default:
      SPDLOG_ERROR( "Invalid request" );
      break;
  }
}

bool MediaDefinedDownloadOperationImpl::requestOptionsNegotiation( const Tftp::Arinc615aOptions &serverOptions )
{
  if ( serverOptions )
  {
    // no ARINC 615A Options (Checksum or port) expected - reject
    SPDLOG_ERROR( "Received unexpected ARINC 615A options: {}", Arinc615aOptions_toString( serverOptions ) );
    return false;
  }

  return true;
}

void MediaDefinedDownloadOperationImpl::requestCompleted( const Tftp::TransferStatus status )
{
  requestOperationV.reset();

  SPDLOG_INFO( "Request file transmission completed" );

  if ( Tftp::TransferStatus::Successful != status )
  {
    SPDLOG_ERROR( "Download request file could not be transmitted" );
    abort( AbortReason::Protocol );
    return;
  }

  requestSentV = true;
}

}
