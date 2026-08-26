// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Tftp::Clients::ClientImpl.
 **/

#ifndef ARINC_615A_TFTP_CLIENTS_CLIENTIMPL_HPP
#define ARINC_615A_TFTP_CLIENTS_CLIENTIMPL_HPP

#include <arinc_615a/tftp/clients/Clients.hpp>
#include <arinc_615a/tftp/clients/Client.hpp>

#include <tftp/clients/Clients.hpp>

#include <boost/asio/io_context.hpp>

namespace Arinc615a::Tftp::Clients {

//! Implementation of the ARINC 615A TFTP Client
class ClientImpl final : public Client
{
  public:
    /**
     * @brief Initialises the TFTP Client Instance.
     *
     * @param[in] ioContext
     *   I/O context used for communication.
     **/
    explicit ClientImpl( boost::asio::io_context &ioContext );

    //! @copydoc Client::readOperation
    Tftp::Clients::ReadOperationPtr readOperation() override;

    //! @copydoc Client::writeOperation
    Tftp::Clients::WriteOperationPtr writeOperation() override;

  private:
    //! I/O Context
    boost::asio::io_context &ioContextV;
    //! Underlying TFTP client.
    ::Tftp::Clients::ClientPtr baseTftpClientV;
};

}

#endif
