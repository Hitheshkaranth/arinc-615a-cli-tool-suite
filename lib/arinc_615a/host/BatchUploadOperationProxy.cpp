/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615a::Host::BatchUploadOperationProxy.
 **/

#include "BatchUploadOperationProxy.hpp"

#include <arinc_615a/host/BatchUploadOperationHandler.hpp>

namespace Arinc615a::Host {

BatchUploadOperationProxy::BatchUploadOperationProxy( TargetId targetId, BatchUploadOperationHandler &handler ) :
  targetIdV{ std::move( targetId ) },
  handlerV{ handler }
{
}

void BatchUploadOperationProxy::initialisationDeferred( const std::chrono::seconds waitTime )
{
  handlerV.initialisationDeferred( targetIdV, waitTime );
}

void BatchUploadOperationProxy::initialisationResponse( const Information::InitializationResponse &response )
{
  handlerV.initialisationResponse( targetIdV, response );
}

void BatchUploadOperationProxy::finished( const StatusCode code, const std::string_view description )
{
  handlerV.finished( targetIdV, code, description );
}

void BatchUploadOperationProxy::status( const Information::UploadStatus &status )
{
  handlerV.status( targetIdV, status );
}

void BatchUploadOperationProxy::fileRequest(
  const boost::asio::ip::udp::endpoint &remote,
  const std::string_view filename,
  const ::Tftp::Packets::TftpOptions &clientTftpOptions,
  const std::string_view loadPartNumber,
  const Arinc649::CheckValue &checkValue )
{
  handlerV.fileRequest( targetIdV, remote, filename, clientTftpOptions, loadPartNumber, checkValue );
}

}
