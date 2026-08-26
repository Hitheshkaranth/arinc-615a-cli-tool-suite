// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615a::Host::InformationOperationImpl.
 **/

#include "InformationOperationImpl.hpp"
#include "ProtocolImpl.hpp"

#include <arinc_615a/information/TargetHardware.hpp>
#include <arinc_615a/information/PartNumber.hpp>

#include <arinc_615a/host/OperationConfiguration.hpp>
#include <arinc_615a/host/InformationOperationHandler.hpp>

#include <arinc_615a/files/information/InformationOperationStatusFile.hpp>
#include <arinc_615a/files/information/LoadConfigurationListFile.hpp>

#include <arinc_615a/files/ProtocolFilename.hpp>
#include <arinc_615a/files/ProtocolFileTypeDescription.hpp>
#include <arinc_615a/files/ProtocolFileStatistic.hpp>

#include <arinc_615a/tftp/servers/Server.hpp>
#include <arinc_615a/tftp/servers/WriteOperation.hpp>

#include <arinc_615a/tftp/Arinc615aOptions.hpp>

#include <arinc_615a/Arinc615aException.hpp>

#include <arinc_649/CheckValueGenerator.hpp>

#include <tftp/files/MemoryFile.hpp>

#include <tftp/packets/TftpOptions.hpp>

#include <spdlog/spdlog.h>

#include <boost/exception/all.hpp>

#include <utility>

namespace Arinc615a::Host {

InformationOperationImpl::InformationOperationImpl(
  boost::asio::io_context &ioContext,
  InformationOperationConfiguration operationConfiguration ) :
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
    .operation( OperationType::Information );
}

InformationOperationImpl::~InformationOperationImpl() = default;

void InformationOperationImpl::start()
{
  waitForFinalStatusV = false;
  // send initialisation
  initialise( Files::ProtocolFileType::LoadConfigurationInitialization );
}

void InformationOperationImpl::abort( const AbortReason reason )
{
  doAbort( reason );
}

void InformationOperationImpl::terminate( const AbortReason reason )
{
  doTerminate( reason );
}

void InformationOperationImpl::tftpRequest(
  boost::asio::ip::udp::endpoint remote,
  const Tftp::RequestType requestType,
  std::string filename,
  ::Tftp::Packets::TftpOptions clientTftpOptions,
  Tftp::Arinc615aOptions clientArinc615aOptions )
{
  Files::ProtocolFilename file{ filename };

  // check request
  if ( !checkRequest( filename, remote ) )
  {
    // Check of request failed - already reported - exit now
    return;
  }

  // only protocol files are written to host TFTP server
  if ( Tftp::RequestType::Write != requestType )
  {
    tftpServer().errorOperation( remote, ::Tftp::Packets::ErrorCode::IllegalTftpOperation, "Only WRQ allowed" );
  }

  SPDLOG_INFO( "Protocol File Request: {}", Files::ProtocolFileTypeDescription::instance().name( file.fileType() ) );

  // check file type
  switch ( file.fileType() )
  {
    using enum Files::ProtocolFileType;

    case LoadConfigurationList:
      listFileRequest( std::move( remote ), std::move( clientTftpOptions ), std::move( clientArinc615aOptions ) );
      // Update Statistic
      Files::ProtocolFileStatistic::globalReceive().protocolFile( LoadConfigurationList );
      break;

    case LoadConfigurationStatus:
      statusFileRequest( std::move( remote ), std::move( clientTftpOptions ), std::move( clientArinc615aOptions ) );
      // Update Statistic
      Files::ProtocolFileStatistic::globalReceive().protocolFile( LoadConfigurationStatus );
      break;

    default:
      SPDLOG_ERROR( "Unexpected file received" );
      tftpServer().errorOperation( remote, ::Tftp::Packets::ErrorCode::FileNotFound, "Wrong filename" );
      break;
  }
}

void InformationOperationImpl::statusFileRequest(
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
      SPDLOG_INFO( "Received unexpected ARINC 615A options: {}", Arinc615aOptions_toString( clientArinc615aOptions ) );
    }

    // Create TFTP Server Read Operation, to receive the Status File.
    auto memFile{ std::make_shared< ::Tftp::Files::MemoryFile >() };

    auto operation{ statusFileOperationsV.emplace_front( tftpServer().writeOperation() ) };
    assert( operation );

    operation
      ->tftpTimeout( configuration().tftpConfiguration.tftpTimeout )
      .tftpRetries( configuration().tftpConfiguration.tftpRetries )
      .dally( configuration().tftpConfiguration.dally )
      .optionsConfiguration( configuration().tftpOptionsConfiguration )
      .completionHandler( std::bind_front( &InformationOperationImpl::statusFileCompleted, this, memFile, operation ) )
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

void InformationOperationImpl::statusFileCompleted(
  ::Tftp::Files::MemoryFilePtr rawStatusFile,
  const Tftp::Servers::WriteOperationPtr &operation,
  const ::Tftp::TransferStatus status )
{
  if ( 1U != statusFileOperationsV.remove( operation ) )
  {
    SPDLOG_ERROR( "Status file operation completed, which was not initiated" );
    finished( StatusCode::OperationAbortedByDlp, "Status file operation completed, which was not initiated" );
    return;
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
      protocolFilename( Files::ProtocolFileType::LoadConfigurationStatus ),
      rawStatusFile->data() );

    // decode received data as status file
    Files::InformationOperationStatusFile statusFile{ rawStatusFile->data() };

    // Validate protocol version (only check and warn but no abort
    if ( statusFile.protocolVersion() != protocolVersion() )
    {
      SPDLOG_WARN( "Status file protocol version differs from expected" );
    }

    // call handler
    handlerV.status( statusFile.status() );

    switch ( statusFile.status().code() )
    {
      using enum Arinc615a::StatusCode;

      case OperationNotAccepted:
      case OperationNotSupported:
      case OperationCompleted:
      case OperationAbortedByTargetHw:
      case OperationAbortedByDlp:
      case OperationAbortedByOperator:
        // Operation finished by Data Loader
        finished( statusFile.status().code(), statusFile.status().description() );
        break;

      case OperationAccepted:
        // If we received the first Status Message, we wait also for final status.
        waitForFinalStatusV = true;

        triggerDlpTimeout();
        break;

      case OperationInProgress:
      case OperationInProgressAdditionalInfo:
        // If we received the first Status Message, we wait also for final status.
        waitForFinalStatusV = true;

        // Exception Time only valid here
        triggerDlpTimeout( std::chrono::seconds{ statusFile.status().exceptionTimer() } );
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

void InformationOperationImpl::listFileRequest(
  boost::asio::ip::udp::endpoint remote,
  ::Tftp::Packets::TftpOptions clientTftpOptions,
  Tftp::Arinc615aOptions clientArinc615aOptions )
{
  try
  {
    if ( listFileOperationV )
    {
      SPDLOG_INFO( "Received List File request, when list file reception is active" );

      listFileOperationV->gracefulAbort( ::Tftp::Packets::ErrorCode::NotDefined, "New list file received" );
    }

    // ARINC 615A Checksum Option is allowed, all others are discarded
    if ( clientArinc615aOptions.checksum != Arinc649::CheckValue::NoCheckValue )
    {
      SPDLOG_INFO( "Received ARINC 615A Checksum Option '{}'", clientArinc615aOptions.checksum.format() );
    }

    // no ARINC 615A Part Number or port expected - discard
    if ( !clientArinc615aOptions.partNumber.empty() || clientArinc615aOptions.port )
    {
      SPDLOG_INFO( "Received unexpected ARINC 615A options: {}", Arinc615aOptions_toString( clientArinc615aOptions ) );
    }

    // Create TFTP Server Read Operation, to receive the List File.
    auto memFile{ std::make_shared< ::Tftp::Files::MemoryFile>() };

    listFileOperationV = tftpServer().writeOperation();
    assert( listFileOperationV );

    ( *listFileOperationV )
      .tftpTimeout( configuration().tftpConfiguration.tftpTimeout )
      .tftpRetries( configuration().tftpConfiguration.tftpRetries )
      .dally( configuration().tftpConfiguration.dally )
      .optionsConfiguration( configuration().tftpOptionsConfiguration )
      .completionHandler(
        std::bind_front(
          &InformationOperationImpl::listFileCompleted,
          this,
          memFile,
          clientArinc615aOptions.checksum ) )
      .dataHandler( memFile )
      .remote( std::move( remote ) )
      .local( configuration().localInterfaceAddress )
      .clientOptions( std::move( clientTftpOptions ) )
      .negotiatedArinc615aOptions( { .port{}, .partNumber{}, .checksum{ clientArinc615aOptions.checksum } } );

    listFileOperationV->start();
  }
  catch ( const Arinc615aException &e )
  {
    SPDLOG_ERROR( "Error receiving list file: {}", boost::diagnostic_information( e ) );

    abort( AbortReason::Protocol );
  }
}

void InformationOperationImpl::listFileCompleted(
  ::Tftp::Files::MemoryFilePtr rawListFile,
  Arinc649::CheckValue checkValue,
  const ::Tftp::TransferStatus status )
{
  listFileOperationV.reset();

  if ( ::Tftp::TransferStatus::Successful != status )
  {
    SPDLOG_ERROR( "List file could not be received" );

    if ( !waitForFinalStatusV )
    {
      finished( StatusCode::OperationAbortedByDlp, "Invalid Protocol file" );
    }
    else
    {
      abort( AbortReason::Protocol );
    }
    return;
  }

  try
  {
    // Check checksum
    const auto checkValueGenerator{ Arinc649::CheckValueGenerator::create( checkValue.type() ) };
    assert( checkValueGenerator );

    bool integrityInformation{ true };
    checkValueGenerator->process( std::as_bytes( rawListFile->data() ) );
    if ( checkValueGenerator->checkValue() != checkValue )
    {
      SPDLOG_WARN(
        "Supplied Check Values differs: '{}' '{}'",
        checkValue.format(),
        checkValueGenerator->checkValue().format() );
      integrityInformation = false;
    }

    protocolFileLogger().receivedProtocolFile(
      protocolFilename( Files::ProtocolFileType::LoadConfigurationList ),
      rawListFile->data() );

    // decode list file
    Files::LoadConfigurationListFile listFile{ rawListFile->data() };

    // Validate protocol version (only check and warn but no abort
    if ( listFile.protocolVersion() != protocolVersion() )
    {
      SPDLOG_WARN( "List file protocol version differs from expected" );
    }

    // call handler
    handlerV.targetInformation( listFile.targetsHardware(), integrityInformation );

    // if flag is not set (no previous status file received) the whole operation is completed
    if ( !waitForFinalStatusV )
    {
      finished( StatusCode::OperationCompleted );
    }
    else
    {
      triggerDlpTimeout();
    }
  }
  catch ( const Arinc615aException &e )
  {
    SPDLOG_ERROR( "Decoding / handling list configuration: {}", boost::diagnostic_information( e ) );

    // if the "wait for final status" flag is not set (no previous status file was received), the whole operation is
    // completed
    if ( !waitForFinalStatusV )
    {
      finished( StatusCode::OperationAbortedByDlp, "Invalid Protocol file" );
    }
    else
    {
      abort( AbortReason::Protocol );
    }
  }
}

}
