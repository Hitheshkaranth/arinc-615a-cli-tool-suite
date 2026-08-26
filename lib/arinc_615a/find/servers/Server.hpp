// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Find::Servers::Server.
 **/

#ifndef ARINC_615A_FIND_SERVERS_SERVER_HPP
#define ARINC_615A_FIND_SERVERS_SERVER_HPP

#include <arinc_615a/find/servers/Servers.hpp>

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/udp.hpp>

#include <functional>

namespace Arinc615a::Find::Servers {

/**
 * @brief ARINC 615A FIND %Server.
 *
 * The FIND server is capable of responding to FIND queries, which are not originated from the well-known FIND port.
 * The FIND server will always send its response from the well-known FIND port.
 **/
class ARINC_615A_EXPORT Server
{
  public:
    /**
     * @brief Creates a FIND Server Instance.
     *
     * @param[in] ioContext
     *   I/O context used for Communication.
     *
     * @return FIND Server Instance.
     **/
    static ServerPtr instance( boost::asio::io_context &ioContext );

    /**
     * @brief Destructor
     *
     * Performs clean-up of the FIND server.
     **/
    virtual ~Server() noexcept = default;

    /**
     * @brief Sets the FIND Request Handler.
     *
     * @param[in] requestHandler
     *   FIND Request Handler
     *
     * @return *this for chaining.
     **/
    virtual Server& requestHandler( FindRequestHandler requestHandler ) = 0;

    /**
     * @brief Sets Local endpoint to connect.
     *
     * By default, the FIND server listens on the @ref DefaultPort on any IPV4 address.
     *
     * @param[in] localEndpoint
     *   Local endpoint to wait for requests.
     *
     * @return *this for chaining.
     **/
    virtual Server& localEndpoint( boost::asio::ip::udp::endpoint localEndpoint ) = 0;

    /**
     * @brief Starts the FIND Server.
     **/
    virtual void start() = 0;

    /**
     * @brief Stops the FIND Server.
     **/
    virtual void stop() = 0;

    /**
     * @brief Sends the FIND information @p targetsInformation to @p remote.
     *
     * This operation should be used to send the FIND response after reception of the FIND request via
     * @ref FindRequestHandler.
     *
     * @param[in] remote
     *   Address of the response receiver.
     * @param[in] findInformation
     *   FIND Information to send.
     **/
    virtual void response(
      const boost::asio::ip::udp::endpoint &remote,
      const TargetInformation &findInformation ) = 0;
};

}

#endif
