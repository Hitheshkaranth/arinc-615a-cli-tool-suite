// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615a::Target::ProtocolImpl.
 **/

#include "ProtocolImpl.hpp"

#include <arinc_615a/target/implementation/ErrorOperationImpl.hpp>
#include <arinc_615a/target/implementation/InformationOperationImpl.hpp>
#include <arinc_615a/target/implementation/UploadOperationImpl.hpp>
#include <arinc_615a/target/implementation/MediaDefinedDownloadOperationImpl.hpp>
#include <arinc_615a/target/implementation/OperatorDefinedDownloadOperationImpl.hpp>

#include <arinc_615a/target/ErrorOperationConfiguration.hpp>
#include <arinc_615a/target/OperationConfiguration.hpp>

#include <arinc_615a/files/ProtocolFilename.hpp>
#include <arinc_615a/files/ProtocolFileStatistic.hpp>

#include <arinc_615a/tftp/servers/Server.hpp>

#include <arinc_615a/tftp/Arinc615aOptions.hpp>

#include <arinc_615a/Arinc615aException.hpp>

#include <tftp/packets/TftpOptions.hpp>

#include <tftp/TftpException.hpp>

#include <spdlog/spdlog.h>

#include <boost/exception/all.hpp>

namespace Arinc615a::Target {

ProtocolImpl::ProtocolImpl( boost::asio::io_context &ioContext, ProtocolConfiguration configuration ) :
  ioContext{ ioContext },
  configurationV{ std::move( configuration ) },
  tftpServerV{ Tftp::Servers::Server::instance( ioContext ) }
{
  tftpServerV
    ->serverAddress(
      { configurationV.configuration.localInterfaceAddress,
        configurationV.configuration.tftpConfiguration.tftpServerPort } )
    .requestHandler( std::bind_front( &ProtocolImpl::receivedTftpRequest, this ) );
}

ProtocolImpl::~ProtocolImpl() noexcept
{
}

void ProtocolImpl::start()
{
  assert( tftpServerV );

  SPDLOG_INFO( "Start ARINC 615A Data Loader" );

  try
  {
    // Start the TFTP server - wait for incoming messages
    tftpServerV->start();
  }
  catch ( const Arinc615aException &e )
  {
    SPDLOG_ERROR( "Unhandled ARINC 615A exception: {}", boost::diagnostic_information( e ) );
    throw;
  }
  catch ( const ::Tftp::TftpException &e )
  {
    SPDLOG_ERROR( "Unhandled TFTP exception: {}", boost::diagnostic_information( e ) );
    throw;
  }
  catch ( const boost::exception &e )
  {
    SPDLOG_ERROR( "Unhandled exception: {}", boost::diagnostic_information( e ) );
    throw;
  }
  catch ( ... )
  {
    SPDLOG_ERROR( "Unhandled unknown exception" );
    throw;
  }
}

void ProtocolImpl::stop()
{
  assert( tftpServerV );

  SPDLOG_INFO( "Stop ARINC 615A Data Loader" );

  tftpServerV->stop();
}

ErrorOperationPtr ProtocolImpl::errorOperation( ErrorOperationConfiguration configuration )
{
  auto operation{ std::make_shared< ErrorOperationImpl >(
    configurationV.configuration,
    configurationV.protocolVersion,
    tftpServerV,
    std::move( configuration ) ) };
  assert( operation );

  return operation;
}

InformationOperationPtr ProtocolImpl::informationOperation( InformationOperationConfiguration configuration )
{
  auto operation{ std::make_shared< InformationOperationImpl>(
    ioContext,
    configurationV.configuration,
    configurationV.protocolVersion,
    tftpServerV,
    std::move( configuration ) ) };
  assert( operation );

  operationV = operation;

  return operation;
}

UploadOperationPtr ProtocolImpl::uploadOperation( UploadOperationConfiguration configuration )
{
  auto operation{ std::make_shared< UploadOperationImpl >(
    ioContext,
    configurationV.configuration,
    configurationV.protocolVersion,
    tftpServerV,
    std::move( configuration ) ) };
  assert( operation );

  operationV = operation;

  return operation;
}

MediaDefinedDownloadOperationPtr ProtocolImpl::mediaDefinedDownloadOperation(
  MediaDefinedDownloadOperationConfiguration configuration )
{
    auto operation{ std::make_shared< MediaDefinedDownloadOperationImpl>(
    ioContext,
    configurationV.configuration,
    configurationV.protocolVersion,
    tftpServerV,
    std::move( configuration ) ) };
    assert( operation );

    operationV = operation;

    return operation;
}

OperatorDefinedDownloadOperationPtr ProtocolImpl::operatorDefinedDownloadOperation(
  OperatorDefinedDownloadOperationConfiguration configuration )
{
  auto operation{ std::make_shared< OperatorDefinedDownloadOperationImpl >(
    ioContext,
    configurationV.configuration,
    configurationV.protocolVersion,
    tftpServerV,
    std::move( configuration ) ) };
  assert( operation );

  operationV = operation;

  return operation;
}

void ProtocolImpl::receivedTftpRequest(
  const boost::asio::ip::udp::endpoint &remote,
  const Tftp::RequestType requestType,
  std::string_view filename,
  const ::Tftp::Packets::TftpOptions &clientTftpOptions,
  const Tftp::Arinc615aOptions &clientArinc615aOptions )
{
  SPDLOG_INFO( "Received TFTP request" );

  // check for protocol filename
  if ( !Files::ProtocolFilename::isProtocolFilename( filename ) )
  {
    SPDLOG_ERROR( "No protocol file requested" );

    tftpServerV->errorOperation( remote, ::Tftp::Packets::ErrorCode::FileNotFound, "File not found" );

    return;
  }

  // If RRQ and Initialisation file
  if ( requestType == Tftp::RequestType::Read )
  {
    Files::ProtocolFilename protocolFilename{ filename };

    switch ( protocolFilename.fileType() )
    {
      using enum Files::ProtocolFileType;

      case LoadConfigurationInitialization:
      case UploadInitialization:
      case MediaDefinedDownloadInitialization:
      case OperatorDefinedDownloadInitialization:
        handleInitialisationFile( remote, protocolFilename, clientTftpOptions, clientArinc615aOptions );
        return;

      default:
        break;
    }
  }

  // Check for registered target operation
  auto operation{ operationV.lock() };
  if ( !operation )
  {
    SPDLOG_ERROR( "No operation registered" );

    tftpServerV->errorOperation( remote, ::Tftp::Packets::ErrorCode::FileNotFound, "No registered operation" );

    return;
  }

  // Redirect request to operation
  operation->tftpRequest( remote, requestType, std::string{ filename }, clientTftpOptions, clientArinc615aOptions );
}

void ProtocolImpl::handleInitialisationFile(
  const boost::asio::ip::udp::endpoint &remote,
  const Files::ProtocolFilename &filename,
  const ::Tftp::Packets::TftpOptions &clientTftpOptions,
  const Tftp::Arinc615aOptions &clientArinc615aOptions )
{
  if ( clientArinc615aOptions.checksum.type() != Arinc649::CheckValueType::NotUsed )
  {
    SPDLOG_INFO( "Ignore unexpected checksum option" );
  }

  OperationType operationType{};

  switch ( filename.fileType() )
  {
    using enum Files::ProtocolFileType;

    case LoadConfigurationInitialization:
      SPDLOG_INFO( "LOAD CONFIGURATION INITIALIZATION" );
      operationType = OperationType::Information;
      break;

    case UploadInitialization:
      SPDLOG_INFO( "UPLOAD INITIALIZATION" );
      operationType = OperationType::Upload;
      break;

    case MediaDefinedDownloadInitialization:
      SPDLOG_INFO( "MEDIA DEFINED DOWNLOAD INITIALIZATION" );
      operationType = OperationType::MediaDefinedDownload;
      break;

    case OperatorDefinedDownloadInitialization:
      SPDLOG_INFO( "OPERATOR DEFINED DOWNLOAD INITIALIZATION" );
      operationType = OperationType::OperatorDefinedDownload;
      break;

    default:
      // invalid value should not occur
      std::unreachable();
      break;
  }

  // call new operation handler - Add port option, when in ARINC 615A-3
  configurationV.newOperationRequestHandler(
    operationType,
    remote,
    clientTftpOptions,
    ( configurationV.protocolVersion == Arinc615aVersion::Arinc615a34 ) ?
      clientArinc615aOptions.port :
      std::nullopt,
    filename.targetId() );

  // TODO maybe move statistic update to Error-Operation and OperationImpl
  Files::ProtocolFileStatistic::globalTransmit().protocolFile( filename.fileType() );
}

}
