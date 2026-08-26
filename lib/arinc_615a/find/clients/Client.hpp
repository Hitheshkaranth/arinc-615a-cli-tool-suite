// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Find::Clients::Client.
 **/

#ifndef ARINC_615A_FIND_CLIENTS_CLIENT_HPP
#define ARINC_615A_FIND_CLIENTS_CLIENT_HPP

#include "Clients.hpp"

#include <boost/asio/io_context.hpp>

namespace Arinc615a::Find::Clients {

/**
 * @brief ARINC 615A FIND %Client.
 *
 * The FIND client is used to generate FIND queries by calling @ref Client::query().
 * The FIND client is able to use a dynamic allocated source port.
 **/
class ARINC_615A_EXPORT Client
{
  public:
    /**
     * @brief Generates a FIND %Client.
     *
     * @param[in] ioContext
     *   I/O context used for Communication.
     *
     * @return Generated FIND Client Instance.
     **/
    static ClientPtr instance( boost::asio::io_context &ioContext );

    //! Destructor
    virtual ~Client() noexcept = default;

    /**
     * @brief Creates a FIND %Query Operation.
     *
     * @return Generated FIND %Query Instance.
     **/
    [[nodiscard]] virtual QueryPtr query() = 0;

  protected:
    //! Constructor.
    Client() = default;
};

}

#endif
