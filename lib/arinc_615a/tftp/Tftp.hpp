// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Namespace Arinc615a::Tftp.
 **/

#ifndef ARINC_615A_TFTP_TFTP_HPP
#define ARINC_615A_TFTP_TFTP_HPP

#include <arinc_615a/Arinc615a.hpp>

#include <tftp/Tftp.hpp>

#include <utility>

/**
 * @dir
 * @brief Declaration/ Definition of Namespace Arinc615a::Tftp.
 **/

/**
 * @brief ARINC 615A TFTP Protocol Specialisation.
 *
 * @sa @ref Arinc615a::Tftp::Clients
 * @sa @ref Arinc615a::Tftp::Servers
 **/
namespace Arinc615a::Tftp {

//! Specialisation of TFTP Transfer Status (@ref ::Tftp::TransferStatus)
enum class TransferStatus
{
  //! Transfer completed successfully
  Successful = std::to_underlying( ::Tftp::TransferStatus::Successful ),
  //! Communication error (i.e. timeout, read error) occurred
  CommunicationError = std::to_underlying( ::Tftp::TransferStatus::CommunicationError ),
  //! Error received/ transmitted - RRQ/ WRQ rejected
  RequestError = std::to_underlying( ::Tftp::TransferStatus::RequestError ),
  //! Error received/ transmitted - Option Negotiation Failed
  OptionNegotiationError = std::to_underlying( ::Tftp::TransferStatus::OptionNegotiationError ),
  //! Error received/ transmitted - Invalid data or packets
  TransferError = std::to_underlying( ::Tftp::TransferStatus::TransferError ),
  //! User (own side) requested abort (gracefully or not gracefully)
  Aborted = std::to_underlying( ::Tftp::TransferStatus::Aborted ),

  //! Operation aborted by DLP
  OperationAbortedByDlp,
  //! Operation aborted by operator
  OperationAbortedByOperator,

  //! Invalid value
  Invalid
};

/**
 * @brief Special TFTP Error Message type for ARINC 615A.
 *
 * ARINC 615A uses special TFTP Error packets for Abort and Wait.
 **/
enum class ErrorMessageType
{
  Abort,  //!< ARINC 615A Abort message packet
  Wait,   //!< ARINC 615A Wait message packet

  Invalid //!< Invalid value
};

struct Arinc615aOptions;

//! TFTP Request Type
using RequestType = ::Tftp::RequestType;

//! Receive Data Handler Pointer
using ReceiveDataHandlerPtr = std::shared_ptr< ::Tftp::ReceiveDataHandler >;
//! Transmit Data Handler Pointer
using TransmitDataHandlerPtr = std::shared_ptr< ::Tftp::TransmitDataHandler >;

//! TFTP Options Configuration
using TftpOptionsConfiguration = ::Tftp::TftpOptionsConfiguration;

}

#endif
