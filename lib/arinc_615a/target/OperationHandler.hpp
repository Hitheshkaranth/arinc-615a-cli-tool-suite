// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Target::OperationHandler.
 **/

#ifndef ARINC_615A_TARGET_OPERATIONHANDLER_HPP
#define ARINC_615A_TARGET_OPERATIONHANDLER_HPP

#include <arinc_615a/target/Target.hpp>

#include <string>

namespace Arinc615a::Target {

/**
 * @brief Base class of all target operation handlers.
 **/
class OperationHandler
{
  public:
    //! Destructor.
    virtual ~OperationHandler() noexcept = default;

    /**
     * @brief Operation Initialised Handler.
     *
     * Informs the application that the operation has been initialised.
     **/
    virtual void initialised() = 0;

    /**
     * @brief Operation Finished Handler.
     *
     * Informs the application that the operation completes.
     *
     * @param[in] finalStatus
     *   Final status.
     * @param[in] description
     *   Final description.
     **/
    virtual void finished( FinalStatus finalStatus, std::string_view description ) = 0;

    /**
     * @brief Abort from Host Request Handler.
     *
     * Informs the application that an abort by the host data loader (protocol is requested) has been received.
     *
     * This is used by the handler to abort pending (file) transfers.
     * Finally, the handler must call @ref Arinc615a::Target::Operation::finished() to finally abort the operation by
     * transmitting the final status file with the appropriate status code.
     *
     * @param[in] abortRequest
     *   The requested abort condition.
     *
     * @sa Arinc615a::Target::Operation::abortByHost
     **/
    virtual void abortRequest( AbortRequest abortRequest ) = 0;
};

}

#endif
