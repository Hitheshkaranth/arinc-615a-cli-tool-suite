// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615a::Tftp::Clients::OperationImpl.
 **/

#include "OperationImpl.hpp"

#include <arinc_615a/tftp/Arinc615aOptions.hpp>
#include <arinc_615a/tftp/ErrorMessage.hpp>

#include <arinc_615a/Arinc615aException.hpp>

#include <tftp/clients/Client.hpp>

#include <tftp/packets/Options.hpp>

#include <spdlog/spdlog.h>

#include <cassert>

namespace Arinc615a::Tftp::Clients {

void OperationImpl::dlpRetries( const uint16_t retries )
{
  dlpRetriesV = retries;
}

void OperationImpl::handleAbort( const bool handleAbort )
{
  handleAbortV = handleAbort;
}

void OperationImpl::request()
{
  SPDLOG_INFO( "Start ARINC 615A TFTP client operation" );

  retriesV = 0;

  // get TFTP client operation
  operationV = tftpOperation();
  assert( operationV );

  // initiate operation
  operationV->request();
}

void OperationImpl::gracefulAbort( const ::Tftp::Packets::ErrorCode errorCode, std::string errorMessage )
{
  waitTimerV.cancel();

  if ( operationV )
  {
    operationV->gracefulAbort( errorCode, std::move( errorMessage ) );
  }
}

void OperationImpl::abort()
{
  waitTimerV.cancel();

  if ( operationV )
  {
    operationV->abort();
  }
}

const ::Tftp::Packets::ErrorInformation& OperationImpl::errorInformation() const
{
  return errorInformationV;
}

void OperationImpl::operationDeferredHandler( OperationDeferredHandler handler )
{
  operationDeferredHandlerV = std::move( handler );
}

void OperationImpl::optionNegotiationHandler( OptionNegotiationHandler handler )
{
  optionNegotiationHandlerV = std::move( handler );
}

void OperationImpl::completionHandler( OperationCompletedHandler handler )
{
  operationCompletionHandlerV = std::move( handler );
}

OperationImpl::OperationImpl( boost::asio::io_context &ioContext ) :
  waitTimerV{ ioContext }
{
}

OperationImpl::~OperationImpl()
try
{
  if ( operationV )
  {
    operationV->abort();
  }

  // cancel potential wait timer
  waitTimerV.cancel();
}
catch ( const boost::system::system_error &err )
{
  SPDLOG_CRITICAL( "timer chancel error: {}", err.what() );
}

bool OperationImpl::handleOptionNegotiation( ::Tftp::Packets::Options &serverOptions )
{
  Arinc615aOptions options;

  // Port Option
  auto [ portValid, portValue ]{
    ::Tftp::Packets::Options_getOption< uint16_t >( serverOptions, Arinc615aOptions_name( KnownOptions::Port ) ) };

  if ( !portValid )
  {
    // invalid port option returned
    return false;
  }

  if ( portValue )
  {
    options.port = portValue;
  }

  // Part Number
  if (
    // TODO remove std::string generation if P2077R3 is implemented within stdlibc++ (GCC)
    const auto partNumber{
      serverOptions.extract( std::string{ Arinc615aOptions_name( KnownOptions::PartNumber ) } ) };
    partNumber )
  {
    options.partNumber = std::move( partNumber.mapped() );
  }

  // Checksum Option
  auto [ checksumValid, checksumValue ] = Arinc615aOptions_checksum( serverOptions );

  options.checksum = std::move( checksumValue );

  // potential remaining server options are handled by TFTP Client operation
  // option negotiation and will lead to an abort of the operation.

  return optionNegotiationHandlerV( options );
}

void OperationImpl::handleCompletion( const ::Tftp::TransferStatus status )
{
  if ( ::Tftp::TransferStatus::Successful == status )
  {
    SPDLOG_INFO( "TFTP client operation completed" );

    // cleanup TFTP operation
    operationV.reset();
    // reset error information
    errorInformationV.reset();

    if ( operationCompletionHandlerV )
    {
      operationCompletionHandlerV( TransferStatus::Successful );
    }

    return;
  }

  // copy error information
  errorInformationV = operationV->errorInformation();
  // cleanup TFTP operation
  operationV.reset();

  if ( ::Tftp::TransferStatus::Aborted == status )
  {
    // explicit user abort

    SPDLOG_INFO( "TFTP client operation aborted by local instance" );

    if ( operationCompletionHandlerV )
    {
      operationCompletionHandlerV( TransferStatus::Aborted );
    }

    return;
  }

  if ( ::Tftp::TransferStatus::RequestError == status )
  {
    // Request rejected -> check error code for ABORT/WAIT

    // Check if the received Error packet is an ARINC 615A message
    switch ( ErrorMessage_type( errorInformationV ) )
    {
      case ErrorMessageType::Abort:
        // Handle ABORT

        if ( handleAbortV )
        {
          assert( errorInformationV );
          const auto &errorMessage{ std::get< 1 >( *errorInformationV ) };
          const auto abortCode{ ErrorMessage_abort( errorMessage ) };

          SPDLOG_INFO( "ABORT received" );

          switch ( abortCode )
          {
            case StatusCode::OperationAbortedByDlp:
              if ( operationCompletionHandlerV )
              {
                operationCompletionHandlerV( TransferStatus::OperationAbortedByDlp );
              }
              return;

            case StatusCode::OperationAbortedByOperator:
              if ( operationCompletionHandlerV )
              {
                operationCompletionHandlerV( TransferStatus::OperationAbortedByOperator );
              }
              return;

            default:
              SPDLOG_WARN( "Invalid ABORT status code: {}", errorMessage );
              break;
          }
        }
        break;

      case ErrorMessageType::Wait:
      {
        // Handle WAIT
        SPDLOG_INFO( "WAIT received" );

        assert( errorInformationV );
        const auto &errorMessage{ std::get< 1 >( *errorInformationV ) };
        const auto waitTime{ ErrorMessage_wait( errorMessage ) };

        if ( !waitTime )
        {
          SPDLOG_ERROR( "Error decoding Wait Message: {}", errorMessage );
          break;
        }

        // Wait the requested time and Retry without decrementing retry counter
        waitTimerV.expires_after( waitTime.value() );
        waitTimerV.async_wait( std::bind_front( &OperationImpl::handleWaitTimeout, this ) );

        if ( operationDeferredHandlerV )
        {
          operationDeferredHandlerV( *waitTime );
        }
        return;
      }

      default:
        // No ARINC 615A message.
        SPDLOG_ERROR( "Received TFTP error" );
        break;
    }
  }

  SPDLOG_WARN( "Retry TFTP client operation" );

  // increment retry counter and retry.
  ++retriesV;

  // Check exceeding retry counter
  if ( retriesV > dlpRetriesV )
  {
    SPDLOG_ERROR( "DLP Retry counter reached" );

    if ( operationCompletionHandlerV )
    {
      operationCompletionHandlerV( TransferStatus::CommunicationError );
    }
    return;
  }

  // get TFTP client operation
  operationV = tftpOperation();
  assert( operationV );

  // initiate operation
  operationV->request();
}

void OperationImpl::handleWaitTimeout( const boost::system::error_code errorCode )
{
  // wait operation aborted
  if ( boost::asio::error::operation_aborted == errorCode )
  {
    if ( operationCompletionHandlerV )
    {
      operationCompletionHandlerV( TransferStatus::OperationAbortedByDlp );
    }
    return;
  }

  // internal (timer) error occurred
  if ( errorCode )
  {
    SPDLOG_ERROR( "timer error: {}", errorCode.message() );

    if ( operationCompletionHandlerV )
    {
      operationCompletionHandlerV( TransferStatus::OperationAbortedByDlp );
    }
    return;
  }

  // get TFTP client operation
  operationV = tftpOperation();
  assert( operationV );

  // initiate operation
  operationV->request();
}

}
