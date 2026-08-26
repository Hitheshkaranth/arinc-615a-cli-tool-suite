// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Namespace Arinc615a::Files.
 **/

/**
 * @dir
 * @brief Declaration/ Definition of Namespace Arinc615a::Files.
 **/

#ifndef ARINC_615A_FILES_FILES_HPP
#define ARINC_615A_FILES_FILES_HPP

#include <arinc_615a/Arinc615a.hpp>

/**
 * @brief ARINC 615A Protocol %Files.
 *
 * This namespace provides the definitions of all ARINC 615A Protocol files used during operations.
 * Additionally, helpers for *Protocol Filename* handling (@ref ProtocolFilename) and a *Protocol File Statistic*
 * (@ref ProtocolFileStatistic) are provided.
 *
 * Name of %File      | Operation                              | Implementation
 * -------------------|----------------------------------------|-------------------------------------
 * `[THW_ID_POS].LCI` | %Information Operation                 | @ref InitializationFile
 * `[THW_ID_POS].LCL` | %Information Operation                 | @ref LoadConfigurationListFile
 * `[THW_ID_POS].LCS` | %Information Operation                 | @ref InformationOperationStatusFile
 * `[THW_ID_POS].LUI` | Uploading Operation                    | @ref InitializationFile
 * `[THW_ID_POS].LUR` | Uploading Operation                    | @ref UploadOperationRequestFile
 * `[THW_ID_POS].LUS` | Uploading Operation                    | @ref UploadOperationStatusFile
 * `[THW_ID_POS].LND` | Media Defined Downloading Operation    | @ref InitializationFile
 * `[THW_ID_POS].LNR` | Media Defined Downloading Operation    | @ref DownloadOperationRequestFile
 * `[THW_ID_POS].LNS` | Downloading Operation                  | @ref DownloadOperationStatusFile
 * `[THW_ID_POS].LNO` | Operator Defined Downloading Operation | @ref InitializationFile
 * `[THW_ID_POS].LNL` | Operator Defined Downloading Operation | @ref DownloadOperationListFile
 * `[THW_ID_POS].LNA` | Operator Defined Downloading Operation | @ref DownloadOperationAnswerFile
 *
 * @sa @sa ARINC 615A-4 Section 6.4
 * @sa @ref ProtocolFilename
 * @sa @ref ProtocolFileStatistic
 **/
namespace Arinc615a::Files {

//! Protocol File Enumeration
enum class ProtocolFileType
{
  //! ARINC 615A Load Configuration Initialisation file
  LoadConfigurationInitialization,
  //! ARINC 615A Load Configuration List file
  LoadConfigurationList,
  //! ARINC 615A Load Configuration status file
  LoadConfigurationStatus,

  //! ARINC 615A Load Uploading Initialisation file
  UploadInitialization,
  //! ARINC 615A Load Uploading Request file
  UploadRequest,
  //! ARINC 615A Load Uploading Status file
  UploadStatus,

  //! ARINC 615A Load Downloading Media Defined Initialisation file
  MediaDefinedDownloadInitialization,
  //! ARINC 615A Load Downloading Media Defined Request file
  MediaDefinedDownloadRequest,

  //! ARINC 615A Load Downloading Operator Defined Initialisation file
  OperatorDefinedDownloadInitialization,
  //! ARINC 615A Load Downloading Operator Defined List file
  OperatorDefinedDownloadList,
  //! ARINC 615A Load Downloading Operator Defined Answer file
  OperatorDefinedDownloadAnswer,
  //! ARINC 615A Load Downloading Status file
  DownloadStatus,

  //! Invalid value marker
  Invalid
};

class ProtocolFilename;

class ProtocolFile;

class InitializationFile;

/**
 * @brief  @name Information Operation
 *
 * @{
 **/
class InformationOperationStatusFile;
class LoadConfigurationListFile;
/** @} **/

/**
 * @brief  @name Upload Operation
 *
 * @{
 **/
class UploadOperationStatusFile;
class UploadOperationRequestFile;
/** @} **/

/**
 * @brief  @name Download Operations
 *
 * @{
 **/
class DownloadOperationStatusFile;
class DownloadOperationRequestFile;
class DownloadOperationAnswerFile;
class DownloadOperationListFile;
/** @} **/

class ProtocolFileStatistic;

class ProtocolFileLogger;

}

#endif
