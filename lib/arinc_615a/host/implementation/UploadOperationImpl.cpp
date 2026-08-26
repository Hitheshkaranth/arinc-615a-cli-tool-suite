// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615a::Host::UploadOperationImpl.
 **/

#include "UploadOperationImpl.hpp"
#include "ProtocolImpl.hpp"

#include <arinc_615a/host/OperationConfiguration.hpp>
#include <arinc_615a/host/UploadOperationHandler.hpp>

#include <arinc_615a/files/upload/UploadOperationRequestFile.hpp>
#include <arinc_615a/files/upload/UploadOperationStatusFile.hpp>

#include <arinc_615a/files/ProtocolFilename.hpp>
#include <arinc_615a/files/ProtocolFileTypeDescription.hpp>
#include <arinc_615a/files/ProtocolFileStatistic.hpp>

#include <arinc_615a/tftp/clients/Client.hpp>
#include <arinc_615a/tftp/clients/WriteOperation.hpp>

#include <arinc_615a/tftp/servers/ReadOperation.hpp>
#include <arinc_615a/tftp/servers/Server.hpp>
#include <arinc_615a/tftp/servers/WriteOperation.hpp>

#include <arinc_615a/tftp/Arinc615aOptions.hpp>

#include <arinc_615a/Arinc615aException.hpp>

#include <arinc_649/CheckValue.hpp>

#include <tftp/files/MemoryFile.hpp>

#include <tftp/packets/TftpOptions.hpp>

#include <spdlog/spdlog.h>

#include <boost/exception/all.hpp>

#include <utility>

namespace Arinc615a::Host {

UploadOperationImpl::UploadOperationImpl(
  boost::asio::io_context &ioContext,
  UploadOperationConfiguration operationConfiguration ) :
  OperationImpl{
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
    .operation( OperationType::Upload );
}

UploadOperationImpl::~UploadOperationImpl() = default;

void UploadOperationImpl::start()
{
  loadListSentV = false;
  // send initialisation
  initialise( Files::ProtocolFileType::UploadInitialization );
}

void UploadOperationImpl::abort( const AbortReason reason )
{
  doAbort( reason );
}

void UploadOperationImpl::terminate( const AbortReason reason )
{
  doTerminate( reason );
}

void UploadOperationImpl::loadList( Information::UploadLoads loads )
{
  if ( loadListSentV )
  {
    SPDLOG_WARN( "Load list can be only transmitted once" );
    return;
  }

  // Assemble load list information
  Files::UploadOperationRequestFile uploadOperationRequestFile{ protocolVersion(), std::move( loads ) };

  const auto file{
    std::make_shared< ::Tftp::Files::MemoryFile >( static_cast< Helper::RawData >( uploadOperationRequestFile ) ) };
  assert( file );

  protocolFileLogger().transmitProtocolFile( protocolFilename( Files::ProtocolFileType::UploadRequest ), file->data() );

  // Perform write operation of the load list file
  loadListOperationV = tftpClient().writeOperation();
  assert( loadListOperationV );

  loadListOperationV
    ->tftpTimeout( configuration().tftpConfiguration.tftpTimeout )
    .tftpRetries( configuration().tftpConfiguration.tftpRetries )
    .optionsConfiguration( configuration().tftpOptionsConfiguration )
    .dlpRetries( configuration().dlpRetries )
    .optionNegotiationHandler( std::bind_front( &UploadOperationImpl::loadListOptionNegotiation, this ) )
    .completionHandler( std::bind_front( &UploadOperationImpl::loadListCompleted, this ) )
    .dataHandler( file )
    .filename(
      static_cast< std::string >( Files::ProtocolFilename{ targetId(), Files::ProtocolFileType::UploadRequest } ) )
    .remote( boost::asio::ip::udp::endpoint{ targetAddress(), configuration().tftpConfiguration.tftpServerPort } )
    .local( configuration().localInterfaceAddress );

  loadListOperationV->request();

  // Update Statistic
  Files::ProtocolFileStatistic::globalTransmit().protocolFile( Files::ProtocolFileType::UploadRequest );
}

Tftp::Servers::ReadOperationPtr UploadOperationImpl::fileTransfer(
  Tftp::TransmitDataHandlerPtr dataHandler,
  boost::asio::ip::udp::endpoint remote,
  ::Tftp::Packets::TftpOptions clientTftpOptions,
  std::string partNumber,
  Arinc649::CheckValue checkValue )
{
  Tftp::Arinc615aOptions options{};

  // Port Option is not used here

  // Add ARINC 615A Part Number option
  if ( !partNumber.empty() )
  {
    options.partNumber = std::move( partNumber );
  }

  // Add ARINC 615A checksum option
  if ( Arinc649::CheckValueType::NotUsed != checkValue.type() )
  {
    options.checksum = std::move( checkValue );
  }

  auto operation{ tftpServer().readOperation() };
  assert( operation );
  operation
    ->tftpTimeout( configuration().tftpConfiguration.tftpTimeout )
    .tftpRetries( configuration().tftpConfiguration.tftpRetries )
    .dataHandler( std::move( dataHandler ) )
    .remote( std::move( remote ) )
    .local( configuration().localInterfaceAddress )
    .clientOptions( std::move( clientTftpOptions ) )
    .negotiatedArinc615aOptions( std::move( options ) );

  return operation;
}

void UploadOperationImpl::waitOperation(
  const boost::asio::ip::udp::endpoint &remote,
  const std::chrono::seconds waitTime )
{
  tftpServer().waitOperation( remote, waitTime );
}

void UploadOperationImpl::errorOperation(
  const boost::asio::ip::udp::endpoint &remote,
  const ::Tftp::Packets::ErrorCode errorCode,
  std::string errorMessage )
{
  tftpServer().errorOperation( remote, errorCode, std::move( errorMessage ) );
}

void UploadOperationImpl::tftpRequest(
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
      SPDLOG_INFO( "Received file request: {}", filename );

      // handlerV directly handles file request
      // (Must create TFTP operation and execute them)
      handlerV.fileRequest(
        std::move( remote ),
        std::move( filename ),
        std::move( clientTftpOptions ),
        std::move( clientArinc615aOptions.partNumber ),
        std::move( clientArinc615aOptions.checksum ) );
      break;

    case Write:
    {
      const Files::ProtocolFilename file{ filename };

      SPDLOG_INFO(
        "Received protocol file {}",
        Files::ProtocolFileTypeDescription::instance().name( file.fileType() ) );

      // Check file type
      // NOLINTNEXTLINE( readability-trivial-switch ): Keep for better protocol file readability
      switch ( file.fileType() )
      {
        case Files::ProtocolFileType::UploadStatus:
          statusFileRequest( std::move( remote ), std::move( clientTftpOptions ), std::move( clientArinc615aOptions ) );

          // Update Statistic
          Files::ProtocolFileStatistic::globalReceive().protocolFile( Files::ProtocolFileType::UploadStatus );
          break;

        default:
          SPDLOG_ERROR( "Unexpected WRQ received" );

          tftpServer().errorOperation( remote, ::Tftp::Packets::ErrorCode::FileNotFound, "Wrong filename" );

          break;
      }
      break;
    }

    default:
      SPDLOG_ERROR( "Invalid request" );
      break;
  }
}

bool UploadOperationImpl::loadListOptionNegotiation( const Tftp::Arinc615aOptions &serverOptions )
{
  if ( serverOptions )
  {
    // no ARINC 615A Options (Checksum or port) expected - reject
    SPDLOG_ERROR( "Received unexpected ARINC 615A options: {}", Arinc615aOptions_toString( serverOptions ) );
    return false;
  }

  return true;
}

void UploadOperationImpl::loadListCompleted( const Tftp::TransferStatus status )
{
  loadListOperationV.reset();

  SPDLOG_INFO( "Load list file transmission completed" );

  if ( Tftp::TransferStatus::Successful != status )
  {
    SPDLOG_ERROR( "Load list file could not be transmitted" );
    abort( AbortReason::Protocol );
    return;
  }

  loadListSentV = true;
}

void UploadOperationImpl::statusFileRequest(
  boost::asio::ip::udp::endpoint remote,
  ::Tftp::Packets::TftpOptions clientTftpOptions,
  Tftp::Arinc615aOptions clientArinc615aOptions )
{
  // check if aborted
  if ( isAborted( remote ) )
  {
    // Abort Error code has been sent - return here.
    return;
  }

  try
  {
    // no ARINC 615A Options (Part Number, Checksum or port) expected - discard
    if ( clientArinc615aOptions )
    {
      SPDLOG_INFO(
        "Received unexpected ARINC 615A options: {}",
        Tftp::Arinc615aOptions_toString( clientArinc615aOptions ) );
    }

    // Create TFTP Server Read Operation, to receive the Status File.
    auto memFile{ std::make_shared< ::Tftp::Files::MemoryFile >() };

    auto operation{ statusFileOperationsV.emplace_front( tftpServer().writeOperation() ) };
    assert( operation );

    ( *operation )
      .tftpTimeout( configuration().tftpConfiguration.tftpTimeout )
      .tftpRetries( configuration().tftpConfiguration.tftpRetries )
      .dally( configuration().tftpConfiguration.dally )
      .optionsConfiguration( configuration().tftpOptionsConfiguration )
      .completionHandler( std::bind_front( &UploadOperationImpl::statusFileCompleted, this, memFile, operation ) )
      .dataHandler( memFile )
      .remote( std::move( remote ) )
      .local( configuration().localInterfaceAddress )
      .clientOptions( std::move( clientTftpOptions ) );

    operation->start();
  }
  catch ( const Arinc615aException &e )
  {
    SPDLOG_ERROR( "Error receiving status file: {}", boost::diagnostic_information( e ) );
  }
}

void UploadOperationImpl::statusFileCompleted(
  ::Tftp::Files::MemoryFilePtr rawStatusFile,
  const Tftp::Servers::WriteOperationPtr &operation,
  const ::Tftp::TransferStatus status )
{
  if ( 1U != statusFileOperationsV.remove( operation ) )
  {
    SPDLOG_ERROR( "Status file operation completed, which was not initiated" );
    finished( StatusCode::OperationAbortedByDlp, "Status file operation completed, which was not initiated" );
  }

  if ( ::Tftp::TransferStatus::Successful != status )
  {
    SPDLOG_ERROR( "Status file could not be received" );

    finished( StatusCode::OperationAbortedByDlp, "Status file could not be received" );
    return;
  }

  try
  {
    protocolFileLogger().receivedProtocolFile(
      protocolFilename( Files::ProtocolFileType::UploadStatus ),
      rawStatusFile->data() );

    // decode received data as a status file
    Files::UploadOperationStatusFile statusFile{ rawStatusFile->data() };
    const auto &uploadStatus{ statusFile.status() };

    // validate protocol version (only check and warn but no abort
    if ( statusFile.protocolVersion() != protocolVersion() )
    {
      SPDLOG_WARN( "Status file protocol version differs from expected" );
    }

    // call handler
    handlerV.status( uploadStatus );

    switch ( uploadStatus.code() )
    {
      using enum Arinc615a::StatusCode;

      case OperationNotAccepted:
      case OperationNotSupported:
      case OperationCompleted:
      case OperationAbortedByTargetHw:
      case OperationAbortedByDlp:
      case OperationAbortedByOperator:
        // Operation finished by Data Loader
        finished( uploadStatus.code(), uploadStatus.description() );
        break;

      case OperationAccepted:
        triggerDlpTimeout();
        break;

      case OperationInProgress:
      case OperationInProgressAdditionalInfo:
        // Exception Time only valid here
        triggerDlpTimeout( std::chrono::seconds{ uploadStatus.exceptionTimer() } );
        break;

      default:
        SPDLOG_WARN( "Invalid status - operation finished" );
        finished( OperationAbortedByDlp );
        break;
    }
  }
  catch ( const Arinc615aException &e )
  {
    SPDLOG_ERROR( "Decoding/Handling status file: {}", boost::diagnostic_information( e ) );
  }
}

}
