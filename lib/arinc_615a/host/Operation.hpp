// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Host::Operation.
 **/

#ifndef ARINC_615A_HOST_OPERATION_HPP
#define ARINC_615A_HOST_OPERATION_HPP

#include <arinc_615a/host/Host.hpp>

namespace Arinc615a::Host {

/**
 * @brief ARINC 615A %Host %Operation.
 *
 * An ARINC 615A operation on the host side is initialised by the host but mainly controlled by the target.
 * The host initiates the operation by requesting an initialisation file.
 * If this operation is permitted, the host waits for further requests from the target.
 **/
class ARINC_615A_EXPORT Operation
{
  public:
    //! Abort Reason
    enum class AbortReason
    {
      //! Initial value for state handling.
      NoAbort,

      //! Abort initiated through the operator.
      Operator,
      //! Abort initiated through the internal protocol handler.
      Protocol
    };

    //! Destructor.
    virtual ~Operation() noexcept = default;

    /**
     * @brief Initiates the host operation.
     *
     * Returns immediately.
     **/
    virtual void start() = 0;

    /**
     * @brief Request Abort of the Current Operation.
     *
     * A flag is stored within the operation.
     * When the next status file request is received from the Target Data Loader, the abort request is transmitted.
     *
     * @param[in] reason
     *   Abort Reason.
     **/
    virtual void abort( AbortReason reason ) = 0;

    /**
     * @brief Terminates the Current Operation.
     *
     * Immediately cancel all transfers and close connections.
     *
     * @param[in] reason
     *   Terminate Reason.
     **/
    virtual void terminate( AbortReason reason ) = 0;
};

}

#endif
