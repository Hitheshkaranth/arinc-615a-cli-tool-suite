// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Namespace Arinc615a.
 **/

/**
 * @dir
 * @brief Declaration/ Definition of Namespace Arinc615a.
 **/

#ifndef ARINC_615A_ARINC615A_HPP
#define ARINC_615A_ARINC615A_HPP

#include <arinc_615a/arinc_615a_export.h>

#include <helper/Helper.hpp>

#include <spdlog/spdlog.h>

#include <chrono>
#include <cstdint>

/**
 * @brief ARINC 615A Protocol.
 *
 * This library provides a basic implementation of the ARINC 615A protocol via IP based networks.
 * The ARINC 615A FIND protocol is implemented within the namespace @ref Arinc615a::Find.
 *
 * The library is divided into sub-namespaces:
 * - @ref Arinc615a::Find for ARINC 615A FIND Protocol Implementation,
 * - @ref Arinc615a::Files for _ARINC 615A Protocol File_ Implementation,
 * - @ref Arinc615a::Information for _ARINC 615A Protocol %Information_ Implementation,
 * - @ref Arinc615a::Tftp for _ARINC 615A Protocol_ TFTP Implementation,
 * - @ref Arinc615a::Host for _ARINC 615A Protocol %Host_ Implementation, and
 * - @ref Arinc615a::Target for _ARINC 615A Protocol %Target_ Implementation.
 *
 * @par Referenced Documents
 * - ARINC Report 615A-1: Software Data Loader Using Ethernet Interfaces;
 *   2001-01-12
 * - ARINC Report 615A-2: Software Data Loader Using Ethernet Interfaces;
 *   2002-05-10
 * - ARINC Report 615A-3: Software Data Loader Using Ethernet Interfaces;
 *   2007-06-30
 * - ARINC Report 615A-4: Software Data Loader Using Ethernet Interfaces;
 *   2023-07-24
 **/
namespace Arinc615a {

/**
 * @brief ARINC 615A Versions.
 *
 * @note
 * ARINC 615A-4 does not introduce changes within the protocol files.
 * Therefore, the Protocol File Version of ARINC 615A-3 is used.
 **/
enum class Arinc615aVersion : uint16_t
{
  //! ARINC 615A-1 `A2`
  Arinc615a1 = 0x4132,
  //! ARINC 615A-2 `A3`
  Arinc615a2 = 0x4133,
  //! ARINC 615A-3 and ARINC 615A-4 `A4`
  Arinc615a34 = 0x4134
};

//! Enumeration of all ARINC 615A Operations.
enum class OperationType
{
  //! Invalid
  Invalid,

  //! %Information Operation
  Information,
  //! Uploading Operation
  Upload,
  //! Media Defined Download Operation
  MediaDefinedDownload,
  //! Operator Defined Download Operation
  OperatorDefinedDownload
};

//! Enumeration of ARINC 615A Operation Classes.
enum class OperationClass
{
  //! %Information Operation
  Information,
  //! Uploading Operation
  Upload,
  //! Download Operation (Media Defined or Operator Defined)
  Download
};

//! Default Port for ARINC 615A TFTP Server.
constexpr uint16_t DefaultArinc615aTftpPort{ 59U };

//! Default TFTP Timeout
constexpr std::chrono::seconds DefaultArinc615aTftpTimeout{ 2U };

//! Default TFTP Retries
constexpr uint16_t DefaultArinc615aTftpRetries{ 1U };

//! Default DLP Timeout
constexpr std::chrono::seconds DefaultArinc615aDlpTimeout{ 13U };

//! Default DLP Retries
constexpr uint16_t DefaultArinc615aDlpRetries{ 1U };

/**
 * @brief Additional TFTP Options (since ARINC 615A-3).
 *
 * These options are used during TFTP Options negotiation for ARINC 615A parameter exchange.
 *
 * @note
 * The description for the "Part Number Option" was invalid in ARINC 615A-3, as this contains a duplicated description
 * from the "Timeout Interval Option".
 * This description has been corrected in ARINC 615A-4.
 **/
enum class KnownOptions
{
  //! Part Number Option
  PartNumber,
  //! CRC8 Checksum Option
  ChecksumCrc8,
  //! CRC16 Checksum Option
  ChecksumCrc16,
  //! CRC32 Checksum Option
  ChecksumCrc32,
  //! MD5 Checksum Option
  ChecksumMd5,
  //! SHA1 Checksum Option
  ChecksumSha1,
  //! SHA256 Checksum Option
  ChecksumSha256,
  //! SHA256 Checksum Option
  ChecksumSha512,
  //! CRC64 Checksum Option
  ChecksumCrc64,
  //! Port Option
  Port
};

/**
 * @brief ARINC 615A Status Codes.
 *
 * Used by status messages and abort error message.
 *
 * The status values are the ones used in ARIN 615A-2/3/4.
 * Older Supplements are not supported.
 **/
enum class StatusCode : uint16_t
{
  //! Operation Accepted. (not yet started)
  OperationAccepted                  = 0x0001U,
  //! Operation not Accepted. (Only Initialisation Response)
  OperationNotAccepted               = 0x1000U,
  //! Operation not Supported. (Only Initialisation Response)
  OperationNotSupported              = 0x1002U,
  //! Operation in Progress.
  OperationInProgress                = 0x0002U,
  //! Operation Completed without Error.
  OperationCompleted                 = 0x0003U,
  //! Operation in Progress with additional information. (since ARINC 615A-3)
  OperationInProgressAdditionalInfo  = 0x0004U,
  //! Operation Aborted by %Target Hardware.
  OperationAbortedByTargetHw         = 0x1003U,
  //! Operation Aborted by DLP.
  OperationAbortedByDlp              = 0x1004U,
  //! Operation Aborted by Operator.
  OperationAbortedByOperator         = 0x1005U,
  //! Load Part Number wrong or Download of file failed.
  //! (Only file status within Upload/ Download Operation)
  LoadPartNumberOrDownloadFileFailed = 0x1007U,

  //! Special unofficial value used internally for signalling deferred initialisation (WAIT response).
  OperationDeferred                  = 0xFFFEU,

  //! Invalid Value
  Invalid                            = 0xFFFFU,
};

//! Status code, which is used to indicate the acceptance of the operation.
enum class OperationAcceptanceStatusCode : uint16_t
{
  //! Operation Accepted
  OperationAccepted = static_cast< uint16_t>( StatusCode::OperationAccepted ),
  //! Operation Denied
  OperationDenied = static_cast< uint16_t>( StatusCode::OperationNotAccepted ),
  //! Operation is not Supported
  OperationNotSupported = static_cast< uint16_t>( StatusCode::OperationNotSupported ),

  //! Operation Deferred (Internal used for WAIT response)
  OperationDeferred = static_cast< uint16_t>( StatusCode::OperationDeferred ),
};

//! Operation Status
enum class OperationState
{
  NotStarted,
  InitialisationStep,
  ListTransferStep,
  TransferStep,
  Finished
};

//! Abort Request.
enum class AbortRequest
{
  //! Abort initiated by DLP
  AbortByDlp,
  //! Abort initiated by Operator
  AbortByOperator
};

//! ARINC 615A Final Status by Operation
enum class FinalStatus
{
  //! Completed
  Completed,
  //! Aborted initiated by DLP
  AbortedByDlp,
  //! Abort initiated by Operator
  AbortedByOperator,
  //! Abort by %Target Hardware
  AbortedByTargetHardware,
  //! Load Part Number wrong or Download of file failed.
  //! (Only file status within Upload/ Download Operation)
  LoadPartNumberOrDownloadFileFailed
};

class Arinc615aConfiguration;
class TargetId;

/**
 * @brief Provide ARINC 615A Library %Version %Information.
 *
 * @return %Version information.
 **/
[[nodiscard]] ARINC_615A_EXPORT Helper::VersionsInformation::value_type arinc615aVersion();

/**
 * @brief Set the logging level for the ARINC 615A Library.
 *
 * Adjusts the verbosity of log messages generated by the ARINC 615A Library.
 *
 * @param level Logging level to be set.
 *   The level defines the granularity of log messages.
 **/
ARINC_615A_EXPORT void setLogLevel( spdlog::level::level_enum level );

}

#endif
