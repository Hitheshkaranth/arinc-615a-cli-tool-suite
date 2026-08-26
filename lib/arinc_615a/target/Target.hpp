// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Namespace Arinc615a::Target.
 **/

#ifndef ARINC_615A_TARGET_TARGET_HPP
#define ARINC_615A_TARGET_TARGET_HPP

#include <arinc_615a/Arinc615a.hpp>

#include <arinc_649/Arinc649.hpp>

#include <tftp/packets/Packets.hpp>

#include <boost/asio/ip/udp.hpp>

#include <functional>
#include <memory>
#include <optional>

/**
 * @dir
 * @brief Declaration/ Definition of Namespace Arinc615a::Target.
 **/

/**
 * @brief ARINC 615A %Target %Protocol.
 *
 * This namespace provides a base implementation, which can be used by a ARINC 615A THA to implement the ARINC 615A
 * operations.
 *
 * The entry for an ARINC 615A target data loader is the interface class @ref Protocol.
 *
 * @sa @ref Protocol
 * @sa @ref InformationOperation
 * @sa @ref UploadOperation
 * @sa @ref MediaDefinedDownloadOperation
 * @sa @ref OperatorDefinedDownloadOperation
 **/
namespace Arinc615a::Target {

/**
 * @name Protocol
 *
 * @{
 **/

class Protocol;
//! %Target Data Loader Instance Pointer.
using ProtocolPtr = std::shared_ptr< Protocol >;

struct ProtocolConfiguration;

/** @} **/

/**
 * @name Error Operations
 *
 * @{
 **/

class ErrorOperation;
//! Error %Operation Instance Pointer
using ErrorOperationPtr = std::shared_ptr< ErrorOperation >;

/**
 * @brief Error %Operation Completion Handler
 **/
using ErrorOperationCompletionHandler = std::function< void() >;

struct ErrorOperationConfiguration;

/** @} **/

/**
 * @name Base Operations
 *
 * @{
 **/
class Operation;
class OperationHandler;
template< typename OperationHandlerT >
struct OperationConfiguration;
//! %Target Data Loader %Operation Pointer
using OperationPtr = std::shared_ptr< Operation >;

/** @} **/

/**
 * @name Information Operations
 *
 * @{
 **/

class InformationOperation;
class InformationOperationHandler;
//! %Information %Operation Instance Pointer
using InformationOperationPtr = std::shared_ptr< InformationOperation >;
//! Configuration of %Information %Operation
using InformationOperationConfiguration = OperationConfiguration< InformationOperationHandler >;

/** @} **/

/**
 * @name Upload Operations
 *
 * @{
 **/

class UploadOperation;
class UploadOperationHandler;
//! Upload %Operation Instance Pointer
using UploadOperationPtr = std::shared_ptr< UploadOperation >;
//! Configuration of Upload %Operation
using UploadOperationConfiguration = OperationConfiguration< UploadOperationHandler >;

/** @} **/

/**
 * @name Download Operations
 *
 * @{
 **/

class DownloadOperation;
class DownloadOperationHandler;

class MediaDefinedDownloadOperation;
class MediaDefinedDownloadOperationHandler;
//! Media Defined Download %Operation Instance Pointer
using MediaDefinedDownloadOperationPtr = std::shared_ptr< MediaDefinedDownloadOperation >;
//! Configuration of Media Defined Download %Operation
using MediaDefinedDownloadOperationConfiguration = OperationConfiguration< MediaDefinedDownloadOperationHandler >;

class OperatorDefinedDownloadOperation;
class OperatorDefinedDownloadOperationHandler;
//! Operator Defined Download %Operation Instance Pointer
using OperatorDefinedDownloadOperationPtr = std::shared_ptr< OperatorDefinedDownloadOperation >;
//! Configuration of Operator Defined Download %Operation
using OperatorDefinedDownloadOperationConfiguration = OperationConfiguration< OperatorDefinedDownloadOperationHandler >;

/** @} **/

/**
 * @brief New %Operation Request Handler.
 *
 * @param[in] operation
 *   ARINC 615A Operation Type.
 * @param[in] hostAddress
 *   Remote Endpoint (Host).
 * @param[in] clientTftpOptions
 *   Received TFTP Client Options (host).
 *   Should be directed to Target Operation.
 * @param[in] port
 *   ARINC 615A Port Option from Client.
 *   Should be directed to Target Operation.
 * @param[in] targetId
 *   Target ID.
 *
 * @sa @ref Arinc615a::Target::Protocol::errorOperation
 * @sa @ref Arinc615a::Target::Protocol::informationOperation
 * @sa @ref Arinc615a::Target::Protocol::uploadOperation
 * @sa @ref Arinc615a::Target::Protocol::mediaDefinedDownloadOperation
 * @sa @ref Arinc615a::Target::Protocol::operatorDefinedDownloadOperation
 **/
using NewOperationRequestHandler = std::function< void(
  OperationType operation,
  const boost::asio::ip::udp::endpoint &hostAddress,
  const ::Tftp::Packets::TftpOptions &clientTftpOptions,
  std::optional< uint16_t > port,
  const TargetId &targetId ) >;

/**
 * @brief ARINC 615A Checksum Option Negotiation Handler.
 *
 * For file transfers the *checksum* and *part number* option can be transmitted.
 * This callback is used to validate the returned checksum and part number from the TFTP Server (Host Data Loader).
 * The Part Number must be empty or the same as sent.
 * The Checksum must be empty (@p Arinc649::CheckValue::NoCheckValue) or the same as sent.
 *
 * @param[in] partNumber
 *   Received Part Number Option
 * @param[in] checksum
 *   Received Checksum Option TFTP Server
 *
 * @return If the Checksum Option Negotiation was successful.
 **/
using Arinc615aOptionNegotiationHandler =
  std::function< bool( std::string_view partNumber, const Arinc649::CheckValue &checksum ) >;

}

#endif
