// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Tftp::Servers::Operation.
 **/

#ifndef ARINC_615A_TFTP_SERVERS_OPERATION_HPP
#define ARINC_615A_TFTP_SERVERS_OPERATION_HPP

#include <arinc_615a/tftp/servers/Servers.hpp>

#include <tftp/servers/Servers.hpp>

#include <tftp/packets/Packets.hpp>

namespace Arinc615a::Tftp::Servers {

//! ARINC 615A TFTP Server Operation.
class ARINC_615A_EXPORT Operation
{
  public:
    //! Destructor.
    virtual ~Operation() = default;

    /**
     * @name Configuration Operations
     * @{
     **/

    /**
     * @brief Updates TFTP Timeout.
     *
     * TFTP Timeout, when no timeout option is negotiated in seconds.
     * If the _TFTP Timeout_ parameter is not set, the TFTP defaults are used.
     *
     * @param[in] timeout
     *   TFTP timeout.
     *
     * @return @p *this for chaining.
     **/
    virtual Operation& tftpTimeout( std::chrono::seconds timeout ) = 0;

    /**
     * @brief Updates the Number of TFTP Packet Retries.
     *
     * If the _TFTP Packet Retries_ parameter is not set, the TFTP defaults are used.
     *
     * @param[in] retries
     *   Number of TFTP Packet Retries.
     *
     * @return @p *this for chaining.
     **/
    virtual Operation& tftpRetries( uint16_t retries ) = 0;

    /**
     * @brief Updates TFTP Options Configuration.
     *
     * If no TFTP Option configuration is provided, the defaults are used.
     *
     * @param[in] optionsConfiguration
     *   TFTP Options Configuration.
     *
     * @return @p *this for chaining.
     **/
    virtual Operation &optionsConfiguration( ::Tftp::TftpOptionsConfiguration optionsConfiguration ) = 0;

    /**
     * @brief Updates the Operation Completed Handler.
     *
     * @param[in] handler
     *   Handler which is called on completion of the operation.
     *
     * @return @p *this for chaining.
     **/
    virtual Operation &completionHandler( OperationCompletedHandler handler ) = 0;

    /**
     * @brief Updates the remote (client address).
     *
     * This parameter is required.
     *
     * @param[in] remote
     *   Where the connection should be established to.
     *
     * @return @p *this for chaining.
     **/
    virtual Operation& remote( boost::asio::ip::udp::endpoint remote ) = 0;

    /**
     * @brief Updates the local address to use as connection source.
     *
     * To set a fixed IP-address and leave the UDP port up to the IP-Stack, set the port to `0`.
     *
     * @param[in] local
     *   Parameter to define the communication source
     *
     * @return @p *this for chaining.
     **/
    virtual Operation& local( boost::asio::ip::address local ) = 0;

    /**
     * @brief Updates the Client Options
     *
     * These options will be negotiated within TFTP Server Request Operation.
     *
     * @param[in] clientOptions
     *   Received TFTP Client Options
     *
     * @return @return @p *this for chaining.
     **/
    virtual Operation &clientOptions( ::Tftp::Packets::TftpOptions clientOptions ) = 0;

    /**
     * @brief Updates additional negotiated ARINC 615A TFTP Options.
     *
     * By default, no additional Options are sent to the client.
     *
     * @param[in] options
     *   Additional negotiated TFTP options sent to the client.
     *
     * @return @p *this for chaining.
     **/
    virtual Operation &negotiatedArinc615aOptions( Arinc615aOptions options ) = 0;

    /** @} **/

    /**
     * @brief Executes the TFTP %Server %Operation.
     *
     * Sends a response to the requestor and waits for answers.
     **/
    virtual void start() = 0;

    /**
     * @brief Aborts the %Operation Gracefully.
     *
     * With a graceful abort, the transfer is cancelled by sending an error packet to the client, signalling the abort
     * of the transfer.
     *
     * @param[in] errorCode
     *   Abort error code.
     * @param errorMessage
     *   Abort error message.
     **/
    virtual void gracefulAbort( ::Tftp::Packets::ErrorCode errorCode, std::string errorMessage = {} ) = 0;

    /**
     * @brief Aborts the %Operation Immediately.
     *
     * No error message is sent.
     **/
    virtual void abort() = 0;

    /**
     * @brief Returns the Error Information of this %Operation.
     *
     * @return The error info of this operation.
     * @retval Tftp::Packets::ErrorInformation{}
     *   If no error occurred.
     **/
    [[nodiscard]] virtual const ::Tftp::Packets::ErrorInformation& errorInformation() const = 0;
};

}

#endif
