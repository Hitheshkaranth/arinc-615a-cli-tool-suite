// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615a::Target::ErrorOperationImpl.
 **/

#include "ErrorOperationImpl.hpp"

#include <arinc_615a/files/InitializationFile.hpp>

#include <arinc_615a/tftp/servers/ReadOperation.hpp>
#include <arinc_615a/tftp/servers/Server.hpp>

#include <arinc_615a/tftp/Arinc615aOptions.hpp>

#include <tftp/files/MemoryFile.hpp>

#include <tftp/packets/TftpOptions.hpp>

#include <spdlog/spdlog.h>

namespace Arinc615a::Target {

ErrorOperationImpl::ErrorOperationImpl(
  Arinc615aConfiguration dlConfiguration,
  Arinc615aVersion protocolVersion,
  Tftp::Servers::ServerPtr tftpServer,
  ErrorOperationConfiguration configuration ):
  dlConfigurationV{ dlConfiguration },
  protocolVersionV{ protocolVersion },
  tftpServerV{ std::move( tftpServer ) },
  configurationV{ std::move( configuration ) }
{
  protocolFileLoggerV
    .operation( configurationV.operation )
    .loggingEnabled( dlConfiguration.protocolFileLogging );
}

void ErrorOperationImpl::start(
  boost::asio::ip::udp::endpoint hostAddress,
  ::Tftp::Packets::TftpOptions clientTftpOptions,
  const std::optional< uint16_t > port )
{
  const Files::InitializationFile initialisationFile(
    protocolVersionV,
    Information::InitializationResponse( configurationV.status, configurationV.description ) );

  initialisationFileOperationV = tftpServerV->readOperation();
  assert( initialisationFileOperationV );

  const auto file{
    std::make_shared< ::Tftp::Files::MemoryFile >( static_cast< Helper::RawData >( initialisationFile ) ) };
  assert( file );

  protocolFileLoggerV.transmitProtocolFile( std::format( "{}.INIT", configurationV.targetId ), file->data() );

  initialisationFileOperationV
    ->tftpTimeout( dlConfigurationV.tftpConfiguration.tftpTimeout )
    .tftpRetries( dlConfigurationV.tftpConfiguration.tftpRetries )
    .completionHandler( std::bind_front( &ErrorOperationImpl::initialisationFileCompleted, this ) )
    .dataHandler( file )
    .remote( std::move( hostAddress ) )
    .local( dlConfigurationV.localInterfaceAddress )
    .clientOptions( std::move( clientTftpOptions ) )
    .negotiatedArinc615aOptions(
      Tftp::Arinc615aOptions{
        .port = port,
        .partNumber = {},
        .checksum = Arinc649::CheckValue::NoCheckValue } );

  initialisationFileOperationV->start();
}

void ErrorOperationImpl::initialisationFileCompleted( const ::Tftp::TransferStatus status )
{
  initialisationFileOperationV.reset();

  if ( ::Tftp::TransferStatus::Successful != status )
  {
    SPDLOG_ERROR( "Initialisation file could not be transmitted" );
    return;
  }

  SPDLOG_INFO( "Error operation completed" );

  // call completion handler
  if ( configurationV.completionHandler )
  {
    configurationV.completionHandler();
  }
}

}
