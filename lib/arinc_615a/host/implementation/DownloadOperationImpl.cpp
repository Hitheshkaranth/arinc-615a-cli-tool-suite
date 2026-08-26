// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615a::Host::DownloadOperationImpl.
 **/

#include "DownloadOperationImpl.hpp"
#include "ProtocolImpl.hpp"

#include <arinc_615a/host/DownloadOperationHandler.hpp>

#include <arinc_615a/files/download/DownloadOperationStatusFile.hpp>

#include <arinc_615a/tftp/servers/Server.hpp>
#include <arinc_615a/tftp/servers/WriteOperation.hpp>

#include <arinc_615a/tftp/Arinc615aOptions.hpp>

#include <arinc_615a/Arinc615aException.hpp>

#include <tftp/files/MemoryFile.hpp>

#include <tftp/packets/TftpOptions.hpp>

#include <spdlog/spdlog.h>

#include <boost/exception/all.hpp>

namespace Arinc615a::Host {

DownloadOperationImpl::DownloadOperationImpl(
  boost::asio::io_context &ioContext,
  Arinc615aConfiguration configuration,
  DownloadOperationHandler &handler,
  boost::asio::ip::address targetAddress,
  TargetId targetId,
  const std::chrono::seconds dlpTimeout,
  const bool portOption ) :
  OperationImpl{
    ioContext,
    std::move( configuration ),
    handler,
    std::move( targetAddress ),
    std::move( targetId ),
    dlpTimeout,
    portOption },
  handler{ handler }
{
}

Tftp::Servers::WriteOperationPtr DownloadOperationImpl::doFileTransfer(
  Tftp::ReceiveDataHandlerPtr dataHandler,
  boost::asio::ip::udp::endpoint remote,
  ::Tftp::Packets::TftpOptions clientTftpOptions,
  std::string partNumber,
  Arinc649::CheckValue checkValue )
{
  Tftp::Arinc615aOptions options{};

  // Port Option is not used here

  // Add checksum option
  if ( !partNumber.empty() )
  {
    options.partNumber = std::move( partNumber );
  }

  // Add checksum option
  if ( Arinc649::CheckValueType::NotUsed != checkValue.type() )
  {
    options.checksum = std::move( checkValue );
  }

  auto operation{ tftpServer().writeOperation() };
  assert( operation );

  operation
    ->tftpTimeout( configuration().tftpConfiguration.tftpTimeout )
    .tftpRetries( configuration().tftpConfiguration.tftpRetries )
    .dally( configuration().tftpConfiguration.dally )
    .optionsConfiguration( configuration().tftpOptionsConfiguration )
    .dataHandler( std::move( dataHandler ) )
    .remote( std::move( remote ) )
    .local( configuration().localInterfaceAddress )
    .clientOptions( std::move( clientTftpOptions ) )
    .negotiatedArinc615aOptions( std::move( options ) );

  return operation;
}

void DownloadOperationImpl::doWaitOperation(
  const boost::asio::ip::udp::endpoint &remote,
  const std::chrono::seconds waitTime )
{
  tftpServer().waitOperation( remote, waitTime );
}

void DownloadOperationImpl::doErrorOperation(
  const boost::asio::ip::udp::endpoint &remote,
  const ::Tftp::Packets::ErrorCode errorCode,
  std::string errorMessage )
{
  tftpServer().errorOperation( remote, errorCode, std::move( errorMessage ) );
}

void DownloadOperationImpl::statusFileRequest(
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
      .completionHandler( std::bind_front( &DownloadOperationImpl::statusFileCompleted, this, memFile, operation ) )
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

void DownloadOperationImpl::statusFileCompleted(
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
      protocolFilename( Files::ProtocolFileType::DownloadStatus ),
      rawStatusFile->data() );

    // decode received data as status file
    Files::DownloadOperationStatusFile statusFile{ rawStatusFile->data() };

    // Validate protocol version (only check and warn but no abort
    if ( statusFile.protocolVersion() != protocolVersion() )
    {
      SPDLOG_WARN( "Status file protocol version differs from expected" );
    }

    // call handler
    handler.status( statusFile.status() );

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
        triggerDlpTimeout();
        break;

      case OperationInProgress:
      case OperationInProgressAdditionalInfo:
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

}
