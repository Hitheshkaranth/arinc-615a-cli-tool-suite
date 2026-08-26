// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Module Arinc615a::Tftp ErrorMessage.
 **/

#include "ErrorMessage.hpp"

#include <arinc_615a/Arinc615aException.hpp>

#include <helper/Exception.hpp>

#include <boost/exception/all.hpp>

#include <charconv>
#include <format>
#include <string>

namespace Arinc615a::Tftp {

//! Wait Error String
static constexpr std::string_view WaitErrorString{ "WAIT:" };

//! Abort Error String
static constexpr std::string_view AbortErrorString{ "ABORT:" };

ErrorMessageType ErrorMessage_type( const ::Tftp::Packets::ErrorInformation &errorInformation ) noexcept
{
  if ( !errorInformation || ( ::Tftp::Packets::ErrorCode::NotDefined != std::get< 0 >( *errorInformation ) ) )
  {
    return ErrorMessageType::Invalid;
  }

  const auto &errorMessage{ std::get< 1 >( *errorInformation ) };

  if ( errorMessage.starts_with( WaitErrorString ) )
  {
    if ( errorMessage.size() <= WaitErrorString.size() )
    {
      return ErrorMessageType::Invalid;
    }

    return ErrorMessageType::Wait;
  }

  if ( errorMessage.starts_with( AbortErrorString ) )
  {
    if ( errorMessage.size() <= AbortErrorString.size() )
    {
      return ErrorMessageType::Invalid;
    }

    return ErrorMessageType::Abort;
  }

  return ErrorMessageType::Invalid;
}

std::string ErrorMessage_abort( const StatusCode statusCode )
{
  return std::format( "ABORT:{:04X}", static_cast< uint16_t >( statusCode ) );
}

StatusCode ErrorMessage_abort( const std::string_view errorMessage ) noexcept
{
  // check abort-string
  if ( !errorMessage.starts_with( AbortErrorString ) )
  {
    return StatusCode::Invalid;
  }

  // decode abort status code
  uint16_t statusCode{0U};
  auto result{ std::from_chars(
    errorMessage.data() + 6U,
    errorMessage.data() + errorMessage.size(),
    statusCode,
    16 ) };

  if ( result.ec != std::errc{} )
  {
    return StatusCode::Invalid;
  }

  // check decoded status code value.
  switch ( StatusCode{ statusCode } )
  {
    case StatusCode::OperationAccepted:
    case StatusCode::OperationNotAccepted:
    case StatusCode::OperationNotSupported:
    case StatusCode::OperationInProgress:
    case StatusCode::OperationCompleted:
    case StatusCode::OperationAbortedByTargetHw:
    case StatusCode::OperationAbortedByDlp:
    case StatusCode::OperationAbortedByOperator:
    case StatusCode::LoadPartNumberOrDownloadFileFailed:
      return static_cast< StatusCode >( statusCode );

    default:
      return StatusCode::Invalid;
  }
}

std::string ErrorMessage_wait( const std::chrono::seconds waitTime )
{
  if ( !std::in_range< uint16_t >( waitTime.count() ) )
  {
    BOOST_THROW_EXCEPTION( Arinc615aException()
      << Helper::AdditionalInfo( "Value out of range" ) );
  }

  return std::format( "WAIT:{}", waitTime.count() );
}

std::optional< std::chrono::seconds > ErrorMessage_wait( const std::string_view errorMessage ) noexcept
{
  if ( !errorMessage.starts_with( WaitErrorString ) )
  {
    return {};
  }

  // decode wait time
  uint16_t waitTime{ 0U };
  auto result{ std::from_chars(
    errorMessage.data() + 5U,
    errorMessage.data() + errorMessage.size(),
    waitTime,
    10 ) };

  if ( result.ec != std::errc{} )
  {
    return {};
  }

  // Assign wait time
  return std::chrono::seconds{ waitTime };
}

}
