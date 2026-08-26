// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615a::Tftp::Clients::ClientImpl.
 **/

#include "ClientImpl.hpp"

#include <arinc_615a/tftp/clients/implementation/ReadOperationImpl.hpp>
#include <arinc_615a/tftp/clients/implementation/WriteOperationImpl.hpp>

#include <tftp/clients/Client.hpp>

namespace Arinc615a::Tftp::Clients {

ClientImpl::ClientImpl( boost::asio::io_context &ioContext ) :
  ioContextV{ ioContext },
  baseTftpClientV{ ::Tftp::Clients::Client::instance( ioContext ) }
{
}

ReadOperationPtr ClientImpl::readOperation()
{
  return std::make_shared< ReadOperationImpl >( baseTftpClientV, ioContextV );
}

WriteOperationPtr ClientImpl::writeOperation()
{
  return std::make_shared< WriteOperationImpl >( baseTftpClientV, ioContextV );
}

}
