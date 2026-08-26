// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615a::Target::Protocol.
 **/

#include "Protocol.hpp"

#include <arinc_615a/target/implementation/ProtocolImpl.hpp>

#include <arinc_615a/target/ProtocolConfiguration.hpp>

namespace Arinc615a::Target {

ProtocolPtr Protocol::instance( boost::asio::io_context &ioContext, ProtocolConfiguration configuration )
{
  return std::make_shared< ProtocolImpl >( ioContext, std::move( configuration ) );
}

}
