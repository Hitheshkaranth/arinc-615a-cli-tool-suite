/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Tftp::Clients::Operation.
 **/

#ifndef ARINC_615A_TFTP_CLIENTS_OPERATION_HPP
#define ARINC_615A_TFTP_CLIENTS_OPERATION_HPP

#include <arinc_615a/tftp/clients/Clients.hpp>

#include <arinc_649/Arinc649.hpp>

#include <tftp/clients/Clients.hpp>

#include <tftp/packets/Packets.hpp>

#include <boost/asio/ip/udp.hpp>

#include <string>

namespace Arinc615a::Tftp::Clients {

/**
 * @brief ARINC 615A TFTP %Client %Operation.
 *
 * The _Transfer Mode_ is fixed to `OCTET`.
 * This operation adds _ARINC 615A_ specific options.
 *
 * @sa @ref ::Tftp::Clients::Operation
 **/
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
     * @brief Updates the TFTP Timeout Setting.
     *
     * This setting defines the _TFTP Timeout_ in seconds when no timeout option is negotiated.
     * If the _TFTP Timeout_ setting is not set, the TFTP defaults are used.
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
     * If no TFTP Options configuration is provided, the defaults are used.
     *
     * @param[in] optionsConfiguration
     *   TFTP Options Configuration.
     *
     * @return @p *this for chaining.
     **/
    virtual Operation& optionsConfiguration( ::Tftp::TftpOptionsConfiguration optionsConfiguration ) = 0;

    /**
     * @brief Updates the ARINC 615A DLP Retries Settings.
     *
     * If the _ARINC 615A DLP Retries_ parameter is not set, the ARINC 615A defaults are used.
     *
     * @param[in] retries
     *   ARINC 615A DLP Retries.
     *
     * @return @p *this for chaining.
     */
    virtual Operation& dlpRetries( uint16_t retries ) = 0;

    /**
     * @brief Updates the Operation Deferred Handler.
     *
     * The handler is called when the `WAIT` error message has been received.
     * If no handler is registered, the wait-operation is performed but no one is informed.
     *
     * @param[in] handler
     *   Operation deferred handler.
     *
     * @return @p *this for chaining.
     **/
    virtual Operation& operationDeferredHandler( OperationDeferredHandler handler ) = 0;

    /**
     * @brief Updates the Option Negotiation Handler.
     *
     * This handler is called for the ARINC 615A options received by TFTP option acknowledgement to verify if the answer
     * of the TFTP server is accepted.
     * When no Option Negotiation Handler is registered, the TFTP Options acknowledged by the TFTP server are accepted.
     *
     * @param[in] handler
     *   Option negotiation handler.
     *
     * @return @p *this for chaining.
     **/
    virtual Operation& optionNegotiationHandler( OptionNegotiationHandler handler ) = 0;

    /**
     * @brief Updates the %Operation Completed Handler.
     *
     * This handler is called when the TFTP operation completes.
     *
     * @param[in] handler
     *   Handler which is called on completion of the operation.
     *
     * @return @p *this for chaining.
     **/
    virtual Operation& completionHandler( OperationCompletedHandler handler ) = 0;

    /**
     * @brief Updates the Request Filename.
     *
     * Defines the name of the file, which is requested.
     * This parameter is required.
     *
     * @param[in] filename
     *   The name of the file to be requested.
     *
     * @return @p *this for chaining.
     **/
    virtual Operation& filename( std::string filename ) = 0;

    /**
     * @brief Updates the ARINC 615A Part Number Option.
     *
     * Provides a Part Number to the TFTP Server during TFTP option negotiation.
     * The meaning of this option depends on the ARINC 615A operation.
     *
     * If this option is not set, no Part Number Option is transmitted.
     *
     * @param[in] partNumberOption
     *   ARINC 615A Part Number Option
     *
     * @return @p *this for chaining.
     **/
    virtual Operation& partNumberOption( std::string partNumberOption ) = 0;

    /**
     * @brief Updates the ARINC 615A Checksum Option.
     *
     * Provides a Check Value to the TFTP Server during TFTP option negotiation.
     * The meaning of this option depends on the ARINC 615A operation.
     * By default, no checksum option is transmitted.
     * If this option is set to @p Arinc649::NoCheckValue, no checksum option is transmitted.
     *
     * @param[in] checksumOption
     *   ARINC 615A Checksum Option.
     *   Set to @p Arinc649::NoCheckValue for deactivation.
     *
     * @return @p *this for chaining.
     **/
    virtual Operation& checksumOption( Arinc649::CheckValue checksumOption ) = 0;

    /**
     * @brief Updates the remote (server address).
     *
     * This is the remote address of the TFTP Server.
     * This parameter is required.
     *
     * @param[in] remote
     *   Where the connection should be established to.
     *
     * @return @p *this for chaining.
     **/
    virtual Operation& remote( boost::asio::ip::udp::endpoint remote ) = 0;

    /**
     * @brief Updates the local address to use as the source for communication.
     *
     * If not set, the Operating System selects the source address automatically.
     *
     * @param[in] local
     *   Parameter to define the communication source
     *
     * @return @p *this for chaining.
     **/
    virtual Operation& local( boost::asio::ip::address local ) = 0;

    /** @} **/

    /**
     * @brief Executes the TFTP %Client %Operation.
     *
     * It prepares the TFTP request packet, sends it to the remote endpoint and starts the reception loop.
     *
     * It returns immediately after sending the request.
     **/
    virtual void request() = 0;

    /**
     * @brief Aborts the %Operation Gracefully.
     *
     * Sends an error packet at the next possible time point.
     *
     * @param[in] errorCode
     *   TFTP error code.
     * @param[in] errorMessage
     *   An additional error message.
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
     * @return The error information of this operation.
     * @retval ::Tftp::Packets::ErrorInformation()
     *   If no error occurred.
     **/
    [[nodiscard]] virtual const ::Tftp::Packets::ErrorInformation& errorInformation() const = 0;
};

}

#endif
