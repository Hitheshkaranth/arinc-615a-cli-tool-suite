// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Find::Clients::ClientImpl.
 **/

#ifndef ARINC_615A_FIND_CLIENTS_CLIENTIMPL_HPP
#define ARINC_615A_FIND_CLIENTS_CLIENTIMPL_HPP

#include <arinc_615a/find/clients/Clients.hpp>
#include <arinc_615a/find/clients/Client.hpp>

#include <boost/asio/io_context.hpp>

namespace Arinc615a::Find::Clients {

/**
 * @brief FIND Client Implementation.
 **/
class ClientImpl final : public Client
{
  public:
    /**
     * @brief Constructs FIND %Client.
     *
     * @param[in] ioContext
     *   I/O Context Used for Communication.
     **/
    explicit ClientImpl( boost::asio::io_context &ioContext );

    //! @copydoc Client::query()
    [[nodiscard]] QueryPtr query() override;

  private:
    //! I/O Context
    boost::asio::io_context &ioContext;
};

}

#endif
