// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615a::Find::Clients::ClientImpl.
 **/

#include "ClientImpl.hpp"

#include <arinc_615a/find/clients/implementation/QueryImpl.hpp>

namespace Arinc615a::Find::Clients {

ClientImpl::ClientImpl( boost::asio::io_context &ioContext ) :
  ioContext{ ioContext }
{
}

QueryPtr ClientImpl::query()
{
  return std::make_shared< QueryImpl >( ioContext );
}

}
