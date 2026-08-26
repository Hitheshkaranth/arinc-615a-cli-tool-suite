// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Tftp::Servers::WriteOperation.
 **/

#ifndef ARINC_615A_TFTP_SERVERS_WRITEOPERATION_HPP
#define ARINC_615A_TFTP_SERVERS_WRITEOPERATION_HPP

#include <arinc_615a/tftp/servers/Servers.hpp>
#include <arinc_615a/tftp/servers/Operation.hpp>

namespace Arinc615a::Tftp::Servers {

/**
 * @brief ARINC 615A TFTP %Server Write %Operation (TFTP WRQ).
 **/
class ARINC_615A_EXPORT WriteOperation : public Operation
{
  public:
    //! Destructor.
    ~WriteOperation() override = default;

    /**
     * @name Configuration Operations
     * @{
     **/

    //! @copydoc Operation::tftpTimeout()
    WriteOperation& tftpTimeout( std::chrono::seconds timeout ) override = 0;

    //! @copydoc Operation::tftpRetries()
    WriteOperation& tftpRetries( uint16_t retries ) override = 0;

    /**
     * @brief Updates the Dally Parameter.
     *
     * If the _dally_ option is set, the operation waits after transmission of
     * the last _Acknowledgement_ packet for potential retry operations.
     *
     * @param[in] dally
     *   If set to @p true, the @p dally handling is activated.
     *
     * @return @p *this for chaining.
     **/
    virtual WriteOperation& dally( bool dally ) = 0;

    //! @copydoc Operation::optionsConfiguration()
    WriteOperation &optionsConfiguration( ::Tftp::TftpOptionsConfiguration optionsConfiguration ) override = 0;

    //! @copydoc Operation::completionHandler()
    WriteOperation &completionHandler( OperationCompletedHandler handler ) override = 0;

    /**
     * @brief Updates the Receive Data Handler.
     *
     * This handler is required.
     * If not provided, the operation will fail.
     *
     * @param[in] handler
     *   Handler for Received Data.
     *
     * @return @p *this for chaining.
     **/
    virtual WriteOperation& dataHandler( ReceiveDataHandlerPtr handler ) = 0;

    //! @copydoc Operation::remote()
    WriteOperation &remote( boost::asio::ip::udp::endpoint remote ) override = 0;

    //! @copydoc Operation::local()
    WriteOperation &local( boost::asio::ip::address local ) override = 0;

    //! @copydoc Operation::clientOptions()
    WriteOperation &clientOptions( ::Tftp::Packets::TftpOptions clientOptions ) override = 0;

    //! @copydoc Operation::negotiatedArinc615aOptions()
    WriteOperation &negotiatedArinc615aOptions( Arinc615aOptions options ) override = 0;

    //! @copydoc Operation::start()
    void start() override = 0;

    //! @copydoc Operation::gracefulAbort()
    void gracefulAbort( ::Tftp::Packets::ErrorCode errorCode, std::string errorMessage = {} ) override = 0;

    //! @copydoc Operation::abort()
    void abort() override = 0;

    //! @copydoc Operation::errorInformation()
    [[nodiscard]] const ::Tftp::Packets::ErrorInformation &errorInformation() const override = 0;

    /** @} **/
};

}

#endif
