// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Namespace Arinc615a::Find::Clients.
 **/

/**
 * @dir
 * @brief Declaration/ Definition of Namespace Arinc615a::Find::Clients.
 **/

#ifndef ARINC_615A_FIND_CLIENTS_CLIENTS_HPP
#define ARINC_615A_FIND_CLIENTS_CLIENTS_HPP

#include <arinc_615a/find/Find.hpp>

#include <boost/asio/ip/address.hpp>

#include <functional>
#include <memory>

/**
 * @brief ARINC 615A FIND %Client.
 *
 * Use the FindClient for instantiating a FIND client.
 **/
namespace Arinc615a::Find::Clients {

// forward declarations
class FindClientException;
class Client;
class Query;

//! Pointer to FIND %Client Instance.
using ClientPtr = std::shared_ptr< Client >;

//! FIND %Query %Operation Instance.
using QueryPtr = std::shared_ptr< Query >;

/**
 * @brief FIND Response Handler.
 *
 * The FIND response handler is called for each received FIND response.
 *
 * @sa @ref Arinc615a::Find::Clients::Query
 *
 * @param[in] address
 *   Source of response.
 * @param[in] targetInformation
 *   Received FIND target information.
 **/
using ResponseHandler =
  std::function< void( const boost::asio::ip::address &address, const TargetInformation &targetInformation ) >;

/**
 * @brief FIND Completion Handler.
 *
 * The FIND response handler is called when the FIND query completes.
 *
 * @sa @ref Arinc615a::Find::Clients::Query
 **/
using CompletionHandler = std::function< void() >;

}

#endif
