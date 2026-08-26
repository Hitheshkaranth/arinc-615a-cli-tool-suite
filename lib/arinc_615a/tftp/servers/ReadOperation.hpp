// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Tftp::Servers::ReadOperation.
 **/

#ifndef ARINC_615A_TFTP_SERVERS_READOPERATION_HPP
#define ARINC_615A_TFTP_SERVERS_READOPERATION_HPP

#include <arinc_615a/tftp/servers/Servers.hpp>
#include <arinc_615a/tftp/servers/Operation.hpp>

namespace Arinc615a::Tftp::Servers {

/**
 * @brief ARINC 615A TFTP %Server Read %Operation (TFTP RRQ).
 **/
class ARINC_615A_EXPORT ReadOperation : public Operation
{
  public:
    //! Destructor.
    ~ReadOperation() override = default;

    /**
     * @name Configuration Operations
     * @{
     **/

    //! @copydoc Operation::tftpTimeout()
    ReadOperation& tftpTimeout( std::chrono::seconds timeout ) override = 0;

    //! @copydoc Operation::tftpRetries()
    ReadOperation& tftpRetries( uint16_t retries ) override = 0;

    //! @copydoc Operation::optionsConfiguration()
    ReadOperation& optionsConfiguration( ::Tftp::TftpOptionsConfiguration optionsConfiguration ) override = 0;

    //! @copydoc Operation::completionHandler()
    ReadOperation& completionHandler( OperationCompletedHandler handler ) override = 0;

    /**
     * @brief Updates the Transmit Data Handler.
     *
     * This handler is required.
     * If not provided, the operation will fail.
     *
     * @param[in] handler
     *   Handler for Transmit Data.
     *
     * @return @p *this for chaining.
     **/
    virtual ReadOperation& dataHandler( TransmitDataHandlerPtr handler ) = 0;

    //! @copydoc Operation::remote()
    ReadOperation& remote( boost::asio::ip::udp::endpoint remote ) override = 0;

    //! @copydoc Operation::local()
    ReadOperation& local( boost::asio::ip::address local ) override = 0;

    //! @copydoc Operation::clientOptions()
    ReadOperation& clientOptions( ::Tftp::Packets::TftpOptions clientOptions ) override = 0;

    //! @copydoc Operation::negotiatedArinc615aOptions()
    ReadOperation& negotiatedArinc615aOptions( Arinc615aOptions options ) override = 0;

    /** @} **/

    //! @copydoc Operation::start()
    void start() override = 0;

    //! @copydoc Operation::gracefulAbort()
    void gracefulAbort( ::Tftp::Packets::ErrorCode errorCode, std::string errorMessage = {} ) override = 0;

    //! @copydoc Operation::abort()
    void abort() override = 0;

    //! @copydoc Operation::errorInformation()
    [[nodiscard]] const ::Tftp::Packets::ErrorInformation& errorInformation() const override = 0;
};

}

#endif
