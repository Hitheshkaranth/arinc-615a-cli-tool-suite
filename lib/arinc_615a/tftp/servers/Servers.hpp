// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Namespace Arinc615a::Tftp::Server.
 **/

/**
 * @dir
 * @brief Declaration/ Definition of Namespace Arinc615a::Tftp::Server.
 **/

#ifndef ARINC_615A_TFTP_SERVERS_SERVERS_HPP
#define ARINC_615A_TFTP_SERVERS_SERVERS_HPP

#include <arinc_615a/tftp/Tftp.hpp>

#include <tftp/servers/Servers.hpp>

#include <functional>
#include <memory>

/**
 * @brief ARINC 615A Protocol TFTP %Server.
 **/
namespace Arinc615a::Tftp::Servers {

// Forward declarations
class Server;
class Operation;
class ReadOperation;
class WriteOperation;

//! TFTP %Server Instance Pointer.
using ServerPtr = std::shared_ptr< Server >;

//! TFTP %Server %Operation Instance Pointer.
using OperationPtr = std::shared_ptr< Operation >;

//! TFTP %Server Read %Operation Instance Pointer.
using ReadOperationPtr = std::shared_ptr< ReadOperation >;

//! TFTP %Server Write %Operation Instance Pointer.
using WriteOperationPtr = std::shared_ptr< WriteOperation >;

/**
 * @brief Received TFTP Request Handler.
 *
 * The registered handler is called when a TFTP Server instance receives a TFTP request.
 * The handler can analyse it and responds with:
 * - TFTP %Server Error %Operation
 * - TFTP %Server Read Request
 * - TFTP %Server Write Request
 *
 * @param[in] remote
 *   Remote Endpoint
 * @param[in] requestType
 *   TFTP Request Type
 * @param[in] filename
 *   Filename
 * @param[in] clientTftpOptions
 *   Received TFTP %Client %Options (TFTP specific).
 *   Should be passed to server operation unmodified.
 * @param[in] clientArinc615aOptions
 *   Received ARINC 615A %Client %Options.
 **/
using ReceivedTftpRequestHandler =
  std::function< void(
    const boost::asio::ip::udp::endpoint &remote,
    RequestType requestType,
    std::string_view filename,
    const ::Tftp::Packets::TftpOptions &clientTftpOptions,
    const Arinc615aOptions &clientArinc615aOptions ) >;

//! Operation Completed handler, which indicates if the transfer was successful
using OperationCompletedHandler = ::Tftp::Servers::OperationCompletedHandler;

}

#endif
