// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615a::Find::Servers::Server.
 **/

#include "Server.hpp"

#include <arinc_615a/find/servers/implementation/ServerImpl.hpp>

namespace Arinc615a::Find::Servers {

ServerPtr Server::instance( boost::asio::io_context &ioContext )
{
  return ServerPtr{ std::make_shared< ServerImpl >( ioContext ) };
}

}
