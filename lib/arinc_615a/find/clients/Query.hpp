// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Find::Clients::Query.
 **/

#ifndef ARINC_615A_FIND_CLIENTS_QUERY_HPP
#define ARINC_615A_FIND_CLIENTS_QUERY_HPP

#include "Clients.hpp"

#include <boost/asio/ip/address.hpp>

namespace Arinc615a::Find::Clients {

/**
 * @brief ARINC 615A FIND Query.
 *
 * The FIND Query is used to send the FIND Request to the configured target address and waits for responses.
 *
 * For each response the callback @ref ResponseHandler is called.
 * When the configured timeout has reached, the callback @ref CompletionHandler is called.
 *
 * @sa @ref Client
 **/
class ARINC_615A_EXPORT Query
{
  public:
    //! Destructor
    virtual ~Query() noexcept = default;

    /**
     * @name FIND %Query Configuration
     * @{
     **/

    /**
     * @brief Sets Handler, which is called during operation.
     *
     * @param[in] responseHandler
     *   FIND Response Handler
     *
     * @return *this for chaining.
     **/
    virtual Query& responseHandler( ResponseHandler responseHandler ) = 0;

    /**
     * @brief Sets Handler, which is called on completion of the FIND operation.
     *
     * @param[in] completionHandler
     *   FIND Completion Handler.
     *
     * @return *this for chaining.
     **/
    virtual Query& completionHandler( CompletionHandler completionHandler ) = 0;

    /**
     * @brief Sets Local address used for transmitting request and wait for response.
     *
     * @param[in] localAddress
     *   Local FIND IP Address.
     *
     * @return *this for chaining.
     **/
    virtual Query& localAddress( boost::asio::ip::address localAddress ) = 0;

    /**
     * @brief Sets Remote Address to send request to.
     *
     * By default, the global IPV4 broadcast address is used.
     *
     * @param[in] remoteAddress
     *   Remote FIND IP Address
     *
     * @return *this for chaining.
     **/
    virtual Query& remoteAddress( boost::asio::ip::address remoteAddress ) = 0;

    /**
     * @brief Sets the FIND port to use.
     *
     * If not modified, this is set to @ref DefaultPort.
     *
     * @param[in] port
     *   FIND Port
     *
     * @return *this for chaining.
     **/
    virtual Query& port( uint16_t port ) = 0;

    /**
     * @brief Modify the _Dynamic Local Port_ Option.
     *
     * If @p dynamicLocalPort is set to @p true, the local port is dynamically chosen.
     *
     * @param[in] dynamicLocalPort
     *   Use dynamic local port.
     *
     * @return *this for chaining.
     **/
    virtual Query& dynamicLocalPort( bool dynamicLocalPort ) = 0;

    /**
     * @brief Sets the FIND Operation timeout.
     *
     * @param[in] timeout
     *   FIND Query Timeout.
     *   Defaults to the Default FIND Receive Timeout.
     *
     * @return *this for chaining.
     **/
    virtual Query& timeout( std::chrono::seconds timeout ) = 0;

    /** @} **/

    /**
     * @brief Initiates the FIND Query.
     *
     * Returns immediately.
     **/
    virtual void start() = 0;

    /**
     * @brief Aborts the FIND Query.
     **/
    virtual void abort() = 0;

  protected:
    //! Constructor.
    Query() = default;
};

}

#endif
