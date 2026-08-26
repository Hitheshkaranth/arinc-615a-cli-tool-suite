// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Namespace Arinc615a::Tftp::Clients.
 **/

/**
 * @dir
 * @brief Declaration/ Definition of Namespace Arinc615a::Tftp::Clients.
 **/

#ifndef ARINC_615A_TFTP_CLIENTS_CLIENTS_HPP
#define ARINC_615A_TFTP_CLIENTS_CLIENTS_HPP

#include <arinc_615a/tftp/Tftp.hpp>

#include <chrono>
#include <functional>
#include <memory>

/**
 * @brief ARINC 615A Protocol TFTP %Client.
 **/
namespace Arinc615a::Tftp::Clients {

// Forward Declarations
class Client;
class Operation;
class ReadOperation;
class WriteOperation;

//! TFTP %Client Instance Pointer
using ClientPtr = std::shared_ptr< Client >;

//! TFTP %Client %Operation Instance.
using OperationPtr = std::shared_ptr< Operation >;

//! TFTP %Client Read %Operation Instance.
using ReadOperationPtr = std::shared_ptr< ReadOperation >;

//! TFTP %Client Write %Operation Instance.
using WriteOperationPtr = std::shared_ptr< WriteOperation >;

/**
 * @brief Operation Deferred Handler.
 *
 * This handler is called when a TFTP operation is aborted by a _WAIT_ error.
 * The operation is restarted after the indicated wait time.
 *
 * @param[in] waitTime
 *   Wait Time Deferred.
 **/
using OperationDeferredHandler = std::function< void( std::chrono::seconds waitTime ) >;

/**
 * @brief ARINC 615A TFTP Client Option Negotiation Handler.
 *
 * Is called to negotiate the specific ARINC 615A Options for validity.
 *
 * @param[in] serverOptions
 *   Received Options from TFTP Server
 *
 * @return If the TFTP Option Negotiation was successful.
 **/
using OptionNegotiationHandler = std::function< bool( const Arinc615aOptions &serverOptions ) >;

/**
 * @brief Operation Completed handler, which indicates if the transfer is completed.
 *
 * @param[in] transferStatus
 *   Status of operation.
 **/
using OperationCompletedHandler = std::function< void( TransferStatus transferStatus ) >;

}

#endif
