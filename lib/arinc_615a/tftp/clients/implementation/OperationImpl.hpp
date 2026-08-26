// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Tftp::Clients::OperationImpl.
 **/

#ifndef ARINC_615A_TFTP_CLIENTS_OPERATIONIMPL_HPP
#define ARINC_615A_TFTP_CLIENTS_OPERATIONIMPL_HPP

#include <arinc_615a/tftp/clients/Clients.hpp>

#include <tftp/Tftp.hpp>

#include <tftp/clients/Operation.hpp>

#include <tftp/packets/Packets.hpp>

#include <boost/asio/system_timer.hpp>
#include <boost/asio/io_context.hpp>

#include <string>

namespace Arinc615a::Tftp::Clients {

/**
 * @brief Encapsulates TFTP Client Operation.
 *
 * This encapsulation is used to implement the common ARINC 615A retry mechanism.
 * When a TFTP operation fails, this operation is retried.
 **/
class OperationImpl
{
  protected:
    /**
     * @brief Updates the ARINC 615A DLP Retries Parameter
     *
     * @param[in] retries
     *   ARINC 615A DLP Retries Parameter
     **/
    void dlpRetries( uint16_t retries );

    /**
     * @brief Updates the ABORT Handling.
     *
     * If not set, abort handling is deactivated.
     *
     * @param[in] handleAbort
     *   If set to @p true, abort requests shall be handled.
     *   Set to true if the client shall handle _Abort Messages_ (sent by the host, and handled by the target).
     **/
    void handleAbort( bool handleAbort );

    /**
     * @brief Executes the operation.
     *
     * Gets the concrete client operation by calling @ref tftpOperation() and tries to execute them.
     * On error the error is analysed and reactions are performed:
     * - on valid _WAIT_ error, the operation is restarted after the given wait time
     * - on valid _ABORT_ error (only for a ARINC 615A target), the ABORT is propagated.
     * - For all other errors a retry is performed.
     **/
    void request();

    //! @copydoc Operation::gracefulAbort()
    void gracefulAbort( ::Tftp::Packets::ErrorCode errorCode, std::string errorMessage = {} );

    //! @copydoc Operation::abort()
    void abort();

    //! @copydoc Operation::errorInformation() const
    [[nodiscard]] const ::Tftp::Packets::ErrorInformation& errorInformation() const;

    /**
     * @brief Updates the Operation Deferred Handler.
     *
     * @param[in] handler
     *   Operation deferred handler.
     **/
    void operationDeferredHandler( OperationDeferredHandler handler );

    /**
     * @brief Updates the Option Negotiation Handler.
     *
     * @param[in] handler
     *   Option negotiation handler.
     **/
    void optionNegotiationHandler( OptionNegotiationHandler handler );

    /**
     * @brief Updates the Operation Completed Handler.
     *
     * @param[in] handler
     *   Handler which is called on completion of the operation.
     **/
    void completionHandler( OperationCompletedHandler handler );

    /**
     * @brief Constructs the Client Operation
     *
     * @param[in] ioContext
     *   I/O context used for communication.
     **/
    explicit OperationImpl( boost::asio::io_context &ioContext );

    //! Destructor
    virtual ~OperationImpl();

    /**
     * @brief TFTP Option Negotiation handler.
     *
     * Converts the TFTP Options to ARINC 615A Options and calls the option negotiation handler.
     *
     * @param[in] serverOptions
     *   Received TFTP Server Options
     *
     * @return If the option negotiation was successful.
     **/
    bool handleOptionNegotiation( ::Tftp::Packets::Options &serverOptions );

    /**
     * @brief Completion handler of TFTP Transfer.
     *
     * @param[in] status
     *   TFTP transfer status.
     **/
    void handleCompletion( ::Tftp::TransferStatus status );

    /**
     * @brief Wait Timeout Handler.
     *
     * When a `WAIT` Error Code is received und the handling activated, a timer is set up, which will retry the
     * operation.
     *
     * @param[in] errorCode
     *   ASIO Error Code
     **/
    void handleWaitTimeout( boost::system::error_code errorCode );

    /**
     * @brief Returns the TFTP client operation to use for the transfer.
     *
     * This operation must be implemented by the child class.
     *
     * @return TFTP Client Operation.
     **/
    virtual ::Tftp::Clients::OperationPtr tftpOperation() = 0;

  private:
    //! Number of DL-Operation Retries.
    uint16_t dlpRetriesV{ DefaultArinc615aDlpRetries };
    //! Handle abort
    bool handleAbortV{ false };
    //! Operation Deferred Handler
    OperationDeferredHandler operationDeferredHandlerV;
    //! Option Negotiation Handler
    OptionNegotiationHandler optionNegotiationHandlerV;
    //! Completion handler
    OperationCompletedHandler operationCompletionHandlerV;

    //! Number of Retries
    uint16_t retriesV{ 0U };
    //! TFTP Client Operation
    ::Tftp::Clients::OperationPtr operationV;
    //! Error Information
    ::Tftp::Packets::ErrorInformation errorInformationV;
    //! Timer for Wait Operation
    boost::asio::system_timer waitTimerV;
};

}

#endif
