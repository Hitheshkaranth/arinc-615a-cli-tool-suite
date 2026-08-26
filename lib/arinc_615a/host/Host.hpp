// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Namespace Arinc615a::Host.
 **/

#ifndef ARINC_615A_HOST_HOST_HPP
#define ARINC_615A_HOST_HOST_HPP

#include <arinc_615a/Arinc615a.hpp>

#include <memory>

/**
 * @dir
 * @brief Declaration/ Definition of Namespace Arinc615a::Host.
 **/

/**
 * @brief ARINC 615A %Host %Protocol.
 *
 * This namespace contains the classes to implement an ARINC 615A host data loader.
 * Starting point is the class @ref Protocol.
 *
 * @sa @ref Protocol
 * @sa @ref InformationOperation
 * @sa @ref UploadOperation
 * @sa @ref MediaDefinedDownloadOperation
 * @sa @ref OperatorDefinedDownloadOperation
 **/
namespace Arinc615a::Host {

class Protocol;
//! %Host Data Loader Instance
using ProtocolPtr = std::shared_ptr< Protocol>;

class Operation;
class OperationHandler;
template< typename OperationHandlerT >
struct OperationConfiguration;

class InformationOperation;
class InformationOperationHandler;
//! %Information %Operation Instance Pointer
using InformationOperationPtr = std::shared_ptr< InformationOperation >;
//! Configuration of %Information %Operation
using InformationOperationConfiguration = OperationConfiguration< InformationOperationHandler >;

class UploadOperation;
class UploadOperationHandler;
//! Upload %Operation Instance Pointer
using UploadOperationPtr = std::shared_ptr< UploadOperation >;
//! Configuration of Upload %Operation
using UploadOperationConfiguration = OperationConfiguration< UploadOperationHandler >;

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

class DownloadInformation;

class BatchUploadOperationProxy;
class BatchUploadOperationHandler;

}

#endif
