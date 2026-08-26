// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Tftp::Clients::Client.
 **/

#ifndef ARINC_615A_TFTP_CLIENTS_CLIENT_HPP
#define ARINC_615A_TFTP_CLIENTS_CLIENT_HPP

#include <arinc_615a/tftp/clients/Clients.hpp>

#include <boost/asio/ip/udp.hpp>

namespace Arinc615a::Tftp::Clients {

/**
 * @brief This class implements an additional TFTP client on top of the standard TFTP client.
 *
 * The additional functionality of this client is:
 * - Handling of WAIT-error operations
 * - Handling of ABORT-error operations (if activated specifically)
 * - Handling of Retries.
 *
 * @sa @ref ReadOperation
 * @sa @ref WriteOperation
 **/
class ARINC_615A_EXPORT Client
{
  public:
    /**
     * @brief Creates a TFTP Client Instance.
     *
     * @param[in] ioContext
     *   I/O context used for communication.
     *
     * @return TFTP Client Instance.
     **/
    static ClientPtr instance( boost::asio::io_context &ioContext );

    //! Destructor
    virtual ~Client() noexcept = default;

    /**
     * @brief Creates a Client Read Operation.
     *
     * @return Created Client Read Operation.
     **/
    [[nodiscard]] virtual ReadOperationPtr readOperation() = 0;

    /**
     * @brief Creates a Client Write Operation.
     *
     * @return Created Client Write Operation.
     **/
    [[nodiscard]] virtual WriteOperationPtr writeOperation() = 0;
};

}

#endif
