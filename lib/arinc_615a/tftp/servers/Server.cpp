// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615a::Tftp::Servers::Server.
 **/

#include "Server.hpp"

#include <arinc_615a/tftp/servers/implementation/ServerImpl.hpp>

namespace Arinc615a::Tftp::Servers {

ServerPtr Server::instance( boost::asio::io_context &ioContext )
{
  // create and return the real TFTP server
  return std::make_shared< ServerImpl >( ioContext );
}

}
