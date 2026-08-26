// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Namespace Arinc615a::Find::Servers.
 **/

/**
 * @dir
 * @brief Declaration/ Definition of Namespace Arinc615a::Find::Servers.
 **/

#ifndef ARINC_615A_FIND_SERVERS_SERVERS_HPP
#define ARINC_615A_FIND_SERVERS_SERVERS_HPP

#include <arinc_615a/find/Find.hpp>

#include <boost/asio/ip/udp.hpp>

#include <functional>
#include <memory>

/**
 * @brief ARINC 615A FIND %Server.
 *
 * Use the @ref Server class for instantiating a FIND server.
 **/
namespace Arinc615a::Find::Servers {

// Forward declaration
class FindServerException;
class Server;

//! FIND Server Instance.
using ServerPtr = std::shared_ptr< Server>;

/**
 * @brief FIND Request Handler
 *
 * @param[in] remote
 *   Source of Request
 **/
using FindRequestHandler = std::function< void( const boost::asio::ip::udp::endpoint &remote ) >;

}

#endif
