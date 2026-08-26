// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Host::OperationHandler.
 **/

#ifndef ARINC_615A_HOST_OPERATIONHANDLER_HPP
#define ARINC_615A_HOST_OPERATIONHANDLER_HPP

#include <arinc_615a/host/Host.hpp>

#include <arinc_615a/information/Information.hpp>

#include <string>

namespace Arinc615a::Host {

/**
 * @brief Base Handler for Client Operations.
 **/
class OperationHandler
{
  public:
    //! Destructor.
    virtual ~OperationHandler() noexcept = default;

    /**
     * @brief Initialisation is Deferred.
     *
     * The Target responds with a _WAIT_ message for deferring the communication.
     * The communication is retried automatically after elapsing @p waitTime seconds.
     *
     * @param[in] waitTime
     *   Number of seconds the initialisation is deferred.
     **/
    virtual void initialisationDeferred( std::chrono::seconds waitTime ) = 0;

    /**
     * @brief Initialisation Response has been received.
     *
     * This handler is called when the Host Data Loader has received the *Initialisation File*.
     * The file is decoded and the initialisation response provided to the registered handler.
     *
     * @param[in] response
     *   Initialisation Response.
     **/
    virtual void initialisationResponse( const Information::InitializationResponse &response ) = 0;

    /**
     * @brief Operation has been completed.
     *
     * This handler is called when the Host Data Loader has received the final *Status File* or the *Load Configuration
     * List File* during the Information Operation, when no _Status File_ has been transmitted.
     *
     * The final status is decoded and provided to the registered handler.
     *
     * @param[in] code
     *   Final Status Code.
     * @param[in] description
     *   Final Status Description.
     **/
    virtual void finished( StatusCode code, std::string_view description ) = 0;
};

}

#endif
