// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Tftp::Clients::ReadOperation.
 **/

#ifndef ARINC_615A_TFTP_CLIENTS_READOPERATION_HPP
#define ARINC_615A_TFTP_CLIENTS_READOPERATION_HPP

#include <arinc_615a/tftp/clients/Clients.hpp>
#include <arinc_615a/tftp/clients/Operation.hpp>

namespace Arinc615a::Tftp::Clients {

/**
 * @brief ARINC 615A TFTP %Client Read %Operation (TFTP RRQ).
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

    /**
     * @brief Updates the Dally Parameter.
     *
     * If the _dally_ option is set, the operation waits after transmission of the last _Acknowledgement_ packet for
     * potential retry operations.
     *
     * @param[in] dally
     *   If set to @p true, the _dally_ handling is activated.
     *
     * @return @p *this for chaining.
     **/
    virtual ReadOperation& dally( bool dally ) = 0;

    //! @copydoc Operation::optionsConfiguration()
    ReadOperation& optionsConfiguration( ::Tftp::TftpOptionsConfiguration optionsConfiguration ) override = 0;

    //! @copydoc Operation::dlpRetries()
    ReadOperation& dlpRetries( uint16_t retries ) override = 0;

    //! @copydoc Operation::operationDeferredHandler()
    ReadOperation& operationDeferredHandler( OperationDeferredHandler handler ) override = 0;

    //! @copydoc Operation::optionNegotiationHandler
    ReadOperation& optionNegotiationHandler( OptionNegotiationHandler handler ) override = 0;

    //! @copydoc Operation::completionHandler
    ReadOperation& completionHandler( OperationCompletedHandler handler ) override = 0;

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
    virtual ReadOperation& dataHandler( ReceiveDataHandlerPtr handler ) = 0;

    //! @copydoc Operation::filename
    ReadOperation& filename( std::string filename ) override = 0;

    /**
     * @brief Updates the ARINC 615A Port Option.
     *
     * This option could be set for the reception of the initialisation file from the host data loader to advertise a
     * specific UDP-port as _Well-Known-Port_ for further TFTP operations.
     *
     * This option shall only be set on the host data loader application for the reception of the operation
     * initialisation file.
     * This option shall never be set on the target data loader.
     *
     * @param[in] portOption
     *   The UDP-Port used for target-to-host communication.
     *
     * @return @p *this for chaining.
     **/
    virtual ReadOperation& portOption( std::optional< uint16_t > portOption ) = 0;

    //! @copydoc Operation::partNumberOption()
    ReadOperation& partNumberOption( std::string partNumberOption ) override = 0;

    //! @copydoc Operation::checksumOption()
    ReadOperation& checksumOption( Arinc649::CheckValue checksumOption ) override = 0;

    //! @copydoc Operation::remote()
    ReadOperation& remote( boost::asio::ip::udp::endpoint remote ) override = 0;

    //! @copydoc Operation::local()
    ReadOperation& local( boost::asio::ip::address local ) override = 0;

    /** @} **/
};

}

#endif
