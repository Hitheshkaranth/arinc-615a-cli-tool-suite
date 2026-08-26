// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615a::Host::ProtocolImpl.
 **/

#include "ProtocolImpl.hpp"

#include <arinc_615a/host/OperationConfiguration.hpp>
#include <arinc_615a/host/implementation/InformationOperationImpl.hpp>
#include <arinc_615a/host/implementation/UploadOperationImpl.hpp>
#include <arinc_615a/host/implementation/MediaDefinedDownloadOperationImpl.hpp>
#include <arinc_615a/host/implementation/OperatorDefinedDownloadOperationImpl.hpp>

namespace Arinc615a::Host {

ProtocolImpl::ProtocolImpl( boost::asio::io_context &ioContext ) :
  ioContextV{ ioContext }
{
}

ProtocolImpl::~ProtocolImpl() noexcept = default;

InformationOperationPtr ProtocolImpl::informationOperation( InformationOperationConfiguration configuration )
{
  return std::make_shared< InformationOperationImpl >( ioContextV, std::move( configuration ) );
}

UploadOperationPtr ProtocolImpl::uploadOperation( UploadOperationConfiguration configuration )
{
  return std::make_shared< UploadOperationImpl >( ioContextV, std::move( configuration ) );
}

MediaDefinedDownloadOperationPtr ProtocolImpl::mediaDefinedDownloadOperation(
  MediaDefinedDownloadOperationConfiguration configuration )
{
  return std::make_shared< MediaDefinedDownloadOperationImpl >( ioContextV, std::move( configuration ) );
}

OperatorDefinedDownloadOperationPtr ProtocolImpl::operatorDefinedDownloadOperation(
  OperatorDefinedDownloadOperationConfiguration configuration )
{
  return std::make_shared< OperatorDefinedDownloadOperationImpl >( ioContextV, std::move( configuration ) );
}

}
