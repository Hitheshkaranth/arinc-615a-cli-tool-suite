// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Module Arinc615a::StatusCode.
 **/

#ifndef ARINC_615A_STATUSCODE_HPP
#define ARINC_615A_STATUSCODE_HPP

#include <arinc_615a/Arinc615a.hpp>

#include <string>
#include <string_view>

namespace Arinc615a {

/**
 * @name ARINC 615A Status Conversion
 *
 * @{
 **/

/**
 * @brief Converts final status to the corresponding status code.
 *
 * @param[in] status
 *   Final status
 *
 * @return Corresponding status code.
 **/
[[nodiscard]] ARINC_615A_EXPORT StatusCode statusCode( FinalStatus status );

/**
 * @brief Converts status code to the corresponding final status.
 *
 * @param[in] code
 *   Status Code.
 *
 * @return Corresponding final status.
 **/
[[nodiscard]] ARINC_615A_EXPORT FinalStatus finalStatus( StatusCode code );

/**
 * @brief Converts abort request to corresponding final status.
 *
 * @param[in] abortRequest
 *   Abort Request.
 *
 * @return Corresponding final status.
 **/
[[nodiscard]] ARINC_615A_EXPORT FinalStatus finalStatus( AbortRequest abortRequest );

/**
 * @brief Convert Raw uint16_t Status Code to ARINC 615A Status Code.
 *
 * Checks validity of values.
 *
 * @param[in] code
 *   Integer representation of the ARINC 615A Status Code.
 *
 * @return ARINC 615A Status Code.
 **/
[[nodiscard]] ARINC_615A_EXPORT StatusCode statusCode( uint16_t code );

/**
 * @brief Convert Raw uint16_t Status Code to ARINC 615A Operation Acceptance Status Code.
 *
 * Checks validity of values.
 *
 * @param[in] code
 *   Integer representation of the ARINC 615A Status Code.
 *
 * @return ARINC 615A Operation Acceptance Status Code.
 **/
[[nodiscard]] ARINC_615A_EXPORT OperationAcceptanceStatusCode operationAcceptanceStatusCode( uint16_t code );

/** @} **/

/**
 * @name ARINC 615A Operation Status.
 * @{
 **/

/**
 * @brief Provides the Operation Status according to the ARINC 615A Specification.
 *
 * Status Code                                         | Status
 * :---------------------------------------------------|:-----------------
 * @ref StatusCode::OperationAccepted                  | _empty string_
 * @ref StatusCode::OperationNotAccepted               | _XXX_ Operation Denied. *%Target Hardware Text*
 * @ref StatusCode::OperationNotSupported              | _XXX_ Operation not supported by the target. *%Target Hardware Text*
 * @ref StatusCode::OperationInProgress                | _empty string_
 * @ref StatusCode::OperationCompleted                 | _XXX_ Operation Completed.
 * @ref StatusCode::OperationInProgressAdditionalInfo  | *%Target Hardware Text*
 * @ref StatusCode::OperationAbortedByTargetHw         | _XXX_ Operation aborted by the %Target Hardware. *%Target Hardware Text*
 * @ref StatusCode::OperationAbortedByDlp              | _XXX_ Operation aborted by the Data Loader. *%Target Hardware Text*
 * @ref StatusCode::OperationAbortedByOperator         | _XXX_ Operation cancelled by the operator.
 * @ref StatusCode::LoadPartNumberOrDownloadFileFailed | *Load Part Number* failed. *%Target Hardware Text*
 * @ref StatusCode::LoadPartNumberOrDownloadFileFailed | *File name* failed. *%Target Hardware Text*
 * _XXX_ = _%Information_ \| _Uploading_ \| _Downloading_ ||
 *
 * @sa ARINC 615A - section 6.4.10.
 *
 * @param[in] operationClass
 *   Operation Class
 * @param[in] code
 *   Status Code
 * @param[in] description
 *   Status Code Description
 * @param[in] loadPartNumberOrFilename
 *   Load Part Number of Filename (for status code @ref StatusCode::LoadPartNumberOrDownloadFileFailed).
 *
 * @return Operation status text.
 **/
[[nodiscard]] ARINC_615A_EXPORT std::string status(
  OperationClass operationClass,
  StatusCode code,
  std::string_view description,
  std::string_view loadPartNumberOrFilename = {} );

/**
 * @brief Provides the Operation Status according to the ARINC 615A Specification.
 *
 * @sa ARINC 615A - section 6.4.10.
 *
 * @param[in] operationClass
 *   Operation Class
 * @param[in] code
 *   Acceptance Status Code
 * @param[in] description
 *   Status Code Description
 *
 * @return Operation status text.
 **/
[[nodiscard]] ARINC_615A_EXPORT std::string status(
  OperationClass operationClass,
  OperationAcceptanceStatusCode code,
  std::string_view description );

/** @} **/

}

#endif
