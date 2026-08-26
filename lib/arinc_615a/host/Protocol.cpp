// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615a::Host::Protocol.
 **/

#include "Protocol.hpp"

#include <arinc_615a/host/implementation/ProtocolImpl.hpp>

namespace Arinc615a::Host {

ProtocolPtr Protocol::instance( boost::asio::io_context &ioContext )
{
  // create the concrete instance
  return std::make_shared< ProtocolImpl>( ioContext );
}

}
