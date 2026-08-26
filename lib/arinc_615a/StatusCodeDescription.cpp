// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615a::StatusCodeDescription.
 **/

#include "StatusCodeDescription.hpp"

namespace Arinc615a {

StatusCodeDescription::StatusCodeDescription():
  Description{
    {
      "Operation Accepted",
      StatusCode::OperationAccepted
    },
    {
      "Operation not accepted",
      StatusCode::OperationNotAccepted
    },
    {
      "Operation not supported",
      StatusCode::OperationNotSupported
    },
    {
      "Operation in progress",
      StatusCode::OperationInProgress
    },
    {
      "Operation completed",
      StatusCode::OperationCompleted
    },
    {
      "Operation in progress with additional information",
      StatusCode::OperationInProgressAdditionalInfo
    },
    {
      "Operation aborted by target hardware",
      StatusCode::OperationAbortedByTargetHw
    },
    {
      "Operation aborted by data loader",
      StatusCode::OperationAbortedByDlp
    },
    {
      "Operation aborted by operator",
      StatusCode::OperationAbortedByOperator
    },
    {
      "Load Part Number or Download File failed",
      StatusCode::LoadPartNumberOrDownloadFileFailed
    },
    {
      "Operation deferred",
      StatusCode::OperationDeferred
    }
  }
{
}

std::ostream& operator<<( std::ostream &stream, StatusCode status )
{
  return stream << std::format( "{}", status );
}

std::ostream& operator<<( std::ostream &stream, const OperationAcceptanceStatusCode status )
{
  return stream << std::format( "{}", status );
}

}
