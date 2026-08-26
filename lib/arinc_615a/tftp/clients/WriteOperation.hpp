// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Tftp::Clients::WriteOperation.
 **/

#ifndef ARINC_615A_TFTP_CLIENTS_WRITEOPERATION_HPP
#define ARINC_615A_TFTP_CLIENTS_WRITEOPERATION_HPP

#include <arinc_615a/tftp/clients/Clients.hpp>
#include <arinc_615a/tftp/clients/Operation.hpp>

namespace Arinc615a::Tftp::Clients {

/**
 * @brief ARINC 615A TFTP %Client Write %Operation (TFTP WRQ).
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

    //! @copydoc Operation::optionsConfiguration()
    WriteOperation& optionsConfiguration( ::Tftp::TftpOptionsConfiguration optionsConfiguration ) override = 0;

    //! @copydoc Operation::dlpRetries()
    WriteOperation& dlpRetries( uint16_t retries ) override = 0;

    /**
     * @brief Updates the ABORT Handling Setting.
     *
     * If this setting is enabled, the protocol handler checks for _ABORT_ error messages and handles them.
     * This setting shall be activated on the target data loader for transmission of operation status files only.
     * If not set, abort handling is deactivated.
     * On the host data loader, this option shall never be activated for file transfers.
     *
     * @param[in] handleAbort
     *   If set to `true`, Abort requests shall be handled.
     *
     * @return @p *this for chaining.
     **/
    virtual WriteOperation& handleAbort( bool handleAbort ) = 0;

    //! @copydoc Operation::operationDeferredHandler()
    WriteOperation& operationDeferredHandler( OperationDeferredHandler handler ) override = 0;

    //! @copydoc Operation::optionNegotiationHandler
    WriteOperation& optionNegotiationHandler( OptionNegotiationHandler handler ) override = 0;

    //! @copydoc Operation::completionHandler
    WriteOperation& completionHandler( OperationCompletedHandler handler ) override = 0;

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
    virtual WriteOperation& dataHandler( TransmitDataHandlerPtr handler ) = 0;

    //! @copydoc Operation::filename
    WriteOperation& filename( std::string filename ) override = 0;

    //! @copydoc Operation::partNumberOption()
    WriteOperation& partNumberOption( std::string partNumberOption ) override = 0;

    //! @copydoc Operation::checksumOption()
    WriteOperation& checksumOption( Arinc649::CheckValue checksumOption ) override = 0;

    //! @copydoc Operation::remote
    WriteOperation& remote( boost::asio::ip::udp::endpoint remote ) override = 0;

    //! @copydoc Operation::local
    WriteOperation& local( boost::asio::ip::address local ) override = 0;

    /** @} **/
};

}

#endif
