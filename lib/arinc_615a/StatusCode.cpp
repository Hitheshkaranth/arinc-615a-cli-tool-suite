// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Module Arinc615a::StatusCode.
 **/

#include "StatusCode.hpp"

#include <arinc_615a/Arinc615aException.hpp>

#include <helper/Exception.hpp>

#include <spdlog/spdlog.h>

#include <boost/exception/all.hpp>

#include <format>

namespace Arinc615a {

StatusCode statusCode( const FinalStatus status )
{
  switch ( status )
  {
    using enum FinalStatus;

    case Completed:
      return StatusCode::OperationCompleted;

    case AbortedByDlp:
      return StatusCode::OperationAbortedByDlp;

    case AbortedByOperator:
      return StatusCode::OperationAbortedByOperator;

    case AbortedByTargetHardware:
      return StatusCode::OperationAbortedByTargetHw;

    default:
      SPDLOG_WARN( "Invalid final status" );
      return StatusCode::OperationAbortedByTargetHw;
  }
}

FinalStatus finalStatus( const StatusCode code )
{
  switch ( code )
  {
    using enum StatusCode;

    case OperationCompleted:
      return FinalStatus::Completed;

    case OperationAbortedByDlp:
      return FinalStatus::AbortedByDlp;

    case OperationAbortedByOperator:
      return FinalStatus::AbortedByOperator;

    case OperationAbortedByTargetHw:
      return FinalStatus::AbortedByTargetHardware;

    case LoadPartNumberOrDownloadFileFailed:
      return FinalStatus::LoadPartNumberOrDownloadFileFailed;

    default:
      SPDLOG_WARN( "Invalid status code" );
      return FinalStatus::AbortedByTargetHardware;
  }
}

FinalStatus finalStatus( const AbortRequest abortRequest )
{
  switch ( abortRequest )
  {
    using enum AbortRequest;

    case AbortByDlp:
      return FinalStatus::AbortedByDlp;

    case AbortByOperator:
      return FinalStatus::AbortedByOperator;

    default:
      SPDLOG_WARN( "Invalid abort request" );
      return FinalStatus::AbortedByTargetHardware;
  }
}

StatusCode statusCode( uint16_t const code )
{
  // NOLINTNEXTLINE( clang-analyzer-optin.core.EnumCastOutOfRange ): Check for validity
  switch ( StatusCode{ code } )
  {
    using enum Arinc615a::StatusCode;

    case OperationAccepted:
    case OperationNotAccepted:
    case OperationNotSupported:
    case OperationInProgress:
    case OperationCompleted:
    case OperationInProgressAdditionalInfo:
    case OperationAbortedByTargetHw:
    case OperationAbortedByDlp:
    case OperationAbortedByOperator:
    case LoadPartNumberOrDownloadFileFailed:
      break;

    default:
      BOOST_THROW_EXCEPTION( Arinc615aException{} << Helper::AdditionalInfo{ "Invalid status code" } );
  }

  return StatusCode{ code };
}

OperationAcceptanceStatusCode operationAcceptanceStatusCode( uint16_t const code )
{
  // NOLINTNEXTLINE( clang-analyzer-optin.core.EnumCastOutOfRange ): Check for validity
  switch ( OperationAcceptanceStatusCode{ code } )
  {
    using enum Arinc615a::OperationAcceptanceStatusCode;

    case OperationAccepted:
    case OperationDenied:
    case OperationNotSupported:
      break;

    default:
      BOOST_THROW_EXCEPTION( Arinc615aException{} << Helper::AdditionalInfo{ "Invalid status code" } );
  }

  return OperationAcceptanceStatusCode{ code };
}

std::string status(
  OperationClass const operationClass,
  StatusCode const code,
  std::string_view const description,
  std::string_view const loadPartNumberOrFilename )
{
  std::string_view operation;

  switch ( operationClass )
  {
    case OperationClass::Information:
      operation = "Information";
      break;

    case OperationClass::Upload:
      operation = "Uploading";
      break;

    case OperationClass::Download:
      operation = "Downloading";
      break;

    default:
      return "**INVALID**";
  }

  switch ( code )
  {
    case StatusCode::OperationAccepted:
      return {};

    case StatusCode::OperationNotAccepted:
      return std::format( "{} Operation Denied. {}", operation, description );

    case StatusCode::OperationNotSupported:
      return std::format( "{} Operation not supported by the target. {}", operation, description );

    case StatusCode::OperationInProgress:
      return {};

    case StatusCode::OperationCompleted:
      return std::format( "{} Operation Completed.", operation );

    case StatusCode::OperationInProgressAdditionalInfo:
      return std::string{ description };

    case StatusCode::OperationAbortedByTargetHw:
      return std::format( "{} Operation aborted by the Target Hardware. {}", operation, description );

    case StatusCode::OperationAbortedByDlp:
      return std::format( "{} Operation aborted by the Data Loader. {}", operation, description );

    case StatusCode::OperationAbortedByOperator:
      return std::format( "{} Operation cancelled by the operator.", operation );

    case StatusCode::LoadPartNumberOrDownloadFileFailed:
      return std::format( "{} failed. {}", loadPartNumberOrFilename, description );

    case StatusCode::OperationDeferred:
      return std::format( "{} Operation deferred. {}", operation, description );

    default:
      return "**INVALID**";
  }
}

std::string status(
  OperationClass const operationClass,
  OperationAcceptanceStatusCode const code,
  std::string_view const description )
{
  return status( operationClass, static_cast< StatusCode >( code ), description );
}

}
