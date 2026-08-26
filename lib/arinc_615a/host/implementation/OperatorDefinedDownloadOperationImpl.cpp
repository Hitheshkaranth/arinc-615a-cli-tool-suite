// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615a::Host::OperatorDefinedDownloadOperationImpl.
 **/

#include "OperatorDefinedDownloadOperationImpl.hpp"
#include "ProtocolImpl.hpp"

#include <arinc_615a/host/OperationConfiguration.hpp>
#include <arinc_615a/host/OperatorDefinedDownloadOperationHandler.hpp>

#include <arinc_615a/files/download/DownloadOperationListFile.hpp>
#include <arinc_615a/files/download/DownloadOperationAnswerFile.hpp>

#include <arinc_615a/files/ProtocolFilename.hpp>
#include <arinc_615a/files/ProtocolFileStatistic.hpp>

#include <arinc_615a/tftp/clients/Client.hpp>
#include <arinc_615a/tftp/clients/WriteOperation.hpp>

#include <arinc_615a/tftp/servers/Server.hpp>
#include <arinc_615a/tftp/servers/WriteOperation.hpp>

#include <arinc_615a/tftp/Arinc615aOptions.hpp>

#include <arinc_615a/Arinc615aException.hpp>

#include <tftp/files/MemoryFile.hpp>

#include <tftp/packets/TftpOptions.hpp>

#include <spdlog/spdlog.h>

#include <boost/exception/all.hpp>

namespace Arinc615a::Host {

OperatorDefinedDownloadOperationImpl::OperatorDefinedDownloadOperationImpl(
  boost::asio::io_context &ioContext,
  OperatorDefinedDownloadOperationConfiguration operationConfiguration ) :
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
    .operation( OperationType::OperatorDefinedDownload );
}

OperatorDefinedDownloadOperationImpl::~OperatorDefinedDownloadOperationImpl() = default;

void OperatorDefinedDownloadOperationImpl::start()
{
  answerSentV = false;
  // send initialisation
  initialise( Files::ProtocolFileType::OperatorDefinedDownloadInitialization );
}

void OperatorDefinedDownloadOperationImpl::abort( const AbortReason reason )
{
  doAbort( reason );
}

void OperatorDefinedDownloadOperationImpl::terminate( const AbortReason reason )
{
  doTerminate( reason );
}

Tftp::Servers::WriteOperationPtr OperatorDefinedDownloadOperationImpl::fileTransfer(
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

void OperatorDefinedDownloadOperationImpl::waitOperation(
  const boost::asio::ip::udp::endpoint &remote,
  const std::chrono::seconds waitTime )
{
  doWaitOperation( remote, waitTime );
}

void OperatorDefinedDownloadOperationImpl::errorOperation(
  const boost::asio::ip::udp::endpoint &remote,
  const ::Tftp::Packets::ErrorCode errorCode,
  std::string errorMessage )
{
  doErrorOperation( remote, errorCode, std::move( errorMessage ) );
}

void OperatorDefinedDownloadOperationImpl::answer( Information::DownloadFiles files )
{
  if ( answerSentV )
  {
    SPDLOG_WARN( "Answer can be only transmitted once" );
    return;
  }

  Files::DownloadOperationAnswerFile answerFile{ protocolVersion(), std::move( files ) };

  const auto file{ std::make_shared< ::Tftp::Files::MemoryFile >( static_cast< Helper::RawData >( answerFile ) ) };
  assert( file );

  protocolFileLogger().transmitProtocolFile(
    protocolFilename( Files::ProtocolFileType::OperatorDefinedDownloadAnswer ),
    file->data() );

  // Perform write operation of the download answer file
  answerOperationV = tftpClient().writeOperation();
  assert( answerOperationV );

  answerOperationV
    ->tftpTimeout( configuration().tftpConfiguration.tftpTimeout )
    .tftpRetries( configuration().tftpConfiguration.tftpRetries )
    .optionsConfiguration( configuration().tftpOptionsConfiguration )
    .dlpRetries( configuration().dlpRetries )
    .optionNegotiationHandler(
      std::bind_front( &OperatorDefinedDownloadOperationImpl::answerOptionsNegotiation, this ) )
    .completionHandler( std::bind_front( &OperatorDefinedDownloadOperationImpl::answerCompleted, this ) )
    .dataHandler( file )
    .filename(
      static_cast< std::string >(
        Files::ProtocolFilename{ targetId(), Files::ProtocolFileType::OperatorDefinedDownloadAnswer } ) )
    .remote( boost::asio::ip::udp::endpoint{ targetAddress(), configuration().tftpConfiguration.tftpServerPort } )
    .local( configuration().localInterfaceAddress );

  answerOperationV->request();

  // Update Statistic
  Files::ProtocolFileStatistic::globalTransmit().protocolFile( Files::ProtocolFileType::OperatorDefinedDownloadAnswer );
}

void OperatorDefinedDownloadOperationImpl::tftpRequest(
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
      switch ( Files::ProtocolFilename{ filename }.fileType() )
      {
        using enum Files::ProtocolFileType;

        case DownloadStatus:
          // Handle the status file
          statusFileRequest( std::move( remote ), std::move( clientTftpOptions ), std::move( clientArinc615aOptions ) );

          // Update Statistic
          Files::ProtocolFileStatistic::globalReceive().protocolFile( DownloadStatus );
          break;

        case OperatorDefinedDownloadList:
          // Handle the list file
          listFileRequest( std::move( remote ), std::move( clientTftpOptions ), std::move( clientArinc615aOptions ) );

          // Update Statistic
          Files::ProtocolFileStatistic::globalReceive().protocolFile( OperatorDefinedDownloadList );
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

void OperatorDefinedDownloadOperationImpl::listFileRequest(
  boost::asio::ip::udp::endpoint remote,
  ::Tftp::Packets::TftpOptions clientTftpOptions,
  Tftp::Arinc615aOptions clientArinc615aOptions )
{
  SPDLOG_INFO( "List file request" );

  try
  {
    if ( listFileOperationV )
    {
      SPDLOG_INFO( "List File request, when list file reception is active" );

      listFileOperationV->gracefulAbort( ::Tftp::Packets::ErrorCode::NotDefined, "New list file received" );
    }

    // no ARINC 615A Options (Part Number, Checksum or port) expected - discard
    if ( clientArinc615aOptions )
    {
      SPDLOG_INFO( "Received unexpected ARINC 615A options: {}", Arinc615aOptions_toString( clientArinc615aOptions ) );
    }

    // Create TFTP Server Read Operation, to receive the List File.
    auto memFile{ std::make_shared< ::Tftp::Files::MemoryFile>() };

    listFileOperationV = tftpServer().writeOperation();
    assert( listFileOperationV );

    listFileOperationV
      ->tftpTimeout( configuration().tftpConfiguration.tftpTimeout )
      .tftpRetries( configuration().tftpConfiguration.tftpRetries )
      .dally( configuration().tftpConfiguration.dally )
      .optionsConfiguration( configuration().tftpOptionsConfiguration )
      .completionHandler( std::bind_front( &OperatorDefinedDownloadOperationImpl::listFileCompleted, this, memFile ) )
      .dataHandler( memFile )
      .remote( std::move( remote ) )
      .local( configuration().localInterfaceAddress )
      .clientOptions( std::move( clientTftpOptions ) );

    listFileOperationV->start();
  }
  catch ( const Arinc615aException &e )
  {
    SPDLOG_ERROR( "Error receiving list file: {}", boost::diagnostic_information( e ) );
  }
}

void OperatorDefinedDownloadOperationImpl::listFileCompleted(
  ::Tftp::Files::MemoryFilePtr rawListFile,
  const ::Tftp::TransferStatus status )
{
  listFileOperationV.reset();

  if ( ::Tftp::TransferStatus::Successful != status )
  {
    SPDLOG_ERROR( "List file could not be received" );

    abort( AbortReason::Protocol );
    return;
  }

  try
  {
    protocolFileLogger().receivedProtocolFile(
      protocolFilename( Files::ProtocolFileType::OperatorDefinedDownloadList ),
      rawListFile->data() );

    // decode list file
    Files::DownloadOperationListFile listFile{ rawListFile->data() };

    // Validate protocol version (only check and warn but no abort
    if ( listFile.protocolVersion() != protocolVersion() )
    {
      SPDLOG_WARN( "List file protocol version differs from expected" );
    }

    // call handler
    handlerV.downloadingList( listFile.files() );
  }
  catch ( const Arinc615aException &e )
  {
    SPDLOG_ERROR( "decoding/ handling list file: {}", boost::diagnostic_information( e ) );

    abort( AbortReason::Protocol );
  }
}

bool OperatorDefinedDownloadOperationImpl::answerOptionsNegotiation( const Tftp::Arinc615aOptions &serverOptions )
{
  if ( serverOptions )
  {
    // no ARINC 615A Options (Checksum or port) expected - reject
    SPDLOG_ERROR( "Received unexpected ARINC 615A options: {}", Arinc615aOptions_toString( serverOptions ) );
    return false;
  }

  return true;
}

void OperatorDefinedDownloadOperationImpl::answerCompleted( const Tftp::TransferStatus status )
{
  answerOperationV.reset();

  SPDLOG_INFO( "Answer file transmission completed" );

  if ( Tftp::TransferStatus::Successful != status )
  {
    SPDLOG_ERROR( "Download answer file could not be transmitted" );
    abort( AbortReason::Protocol );
    return;
  }

  answerSentV = true;
}

}
