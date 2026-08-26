// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Tftp::Clients::WriteOperationImpl.
 **/

#ifndef ARINC_615A_TFTP_CLIENTS_WRITEOPERATIONIMPL_HPP
#define ARINC_615A_TFTP_CLIENTS_WRITEOPERATIONIMPL_HPP

#include <arinc_615a/tftp/clients/WriteOperation.hpp>

#include <arinc_615a/tftp/clients/implementation/OperationImpl.hpp>

#include <arinc_649/CheckValue.hpp>

#include <tftp/TftpOptionsConfiguration.hpp>

#include <boost/asio/ip/udp.hpp>

namespace Arinc615a::Tftp::Clients {

/**
 * @brief Encapsulation of TFTP Client Write Operation.
 **/
class WriteOperationImpl final : public WriteOperation, private OperationImpl
{
  public:
    /**
     * @brief Initialises the ARINC 615A TFTP client write operation.
     *
     * @param[in] tftpClient
     *   TFTP client.
     * @param[in] ioContext
     *   I/O context used for communication.
     **/
    WriteOperationImpl( ::Tftp::Clients::ClientPtr tftpClient, boost::asio::io_context &ioContext );

    //! Destructor.
    ~WriteOperationImpl() override = default;

    //! @copydoc WriteOperation::tftpTimeout()
    WriteOperation& tftpTimeout( std::chrono::seconds timeout ) override;

    //! @copydoc WriteOperation::tftpRetries()
    WriteOperation& tftpRetries( uint16_t retries ) override;

    //! @copydoc WriteOperation::optionsConfiguration()
    WriteOperation& optionsConfiguration( ::Tftp::TftpOptionsConfiguration optionsConfiguration ) override;

    //! @copydoc WriteOperation::dlpRetries()
    WriteOperation& dlpRetries( uint16_t retries ) override;

    //! @copydoc WriteOperation::handleAbort()
    WriteOperation& handleAbort( bool handleAbort ) override;

    //! @copydoc WriteOperation::operationDeferredHandler()
    WriteOperation& operationDeferredHandler( OperationDeferredHandler handler ) override;

    //! @copydoc WriteOperation::optionNegotiationHandler()
    WriteOperation& optionNegotiationHandler( OptionNegotiationHandler handler ) override;

    //! @copydoc WriteOperation::completionHandler()
    WriteOperation& completionHandler( OperationCompletedHandler handler ) override;

    //! @copydoc WriteOperation::dataHandler()
    WriteOperation& dataHandler( TransmitDataHandlerPtr handler ) override;

    //! @copydoc WriteOperation::filename
    WriteOperation& filename( std::string filename ) override;

    //! @copydoc WriteOperation::partNumberOption()
    WriteOperation& partNumberOption( std::string partNumberOption ) override;

    //! @copydoc WriteOperation::checksumOption()
    WriteOperation& checksumOption( Arinc649::CheckValue checksumOption ) override;

    //! @copydoc WriteOperation::remote
    WriteOperation& remote( boost::asio::ip::udp::endpoint remote ) override;

    //! @copydoc WriteOperation::local
    WriteOperation& local( boost::asio::ip::address local ) override;

    //! @copydoc WriteOperation::request()
    void request() override;

    //! @copydoc WriteOperation::gracefulAbort()
    void gracefulAbort( ::Tftp::Packets::ErrorCode errorCode, std::string errorMessage = {} ) override;

    //! @copydoc WriteOperation::abort()
    void abort() override;

    //! @copydoc WriteOperation::errorInformation() const
    [[nodiscard]] const ::Tftp::Packets::ErrorInformation& errorInformation() const override;

  protected:
    //! @copydoc OperationImpl::tftpOperation
    ::Tftp::Clients::OperationPtr tftpOperation() override;

  private:
    //! TFTP client
    ::Tftp::Clients::ClientPtr tftpClient;

    //! TFTP Packet Timeout
    std::chrono::seconds tftpTimeoutV{ ::Tftp::DefaultTftpReceiveTimeout };
    //! TFTP Packet Retries
    uint16_t tftpRetriesV{ ::Tftp::DefaultTftpRetries };
    //! TFTP Options Configuration.
    ::Tftp::TftpOptionsConfiguration tftpOptionsConfigurationV;
    //! Data Handler
    TransmitDataHandlerPtr dataHandlerV;
    //! Which file shall be requested
    std::string filenameV;
    //! Part Number Option
    std::string partNumberOptionV;
    //! Checksum Option (If set to NoCheckValue, not set)
    Arinc649::CheckValue checksumOptionV{ Arinc649::CheckValue::NoCheckValue };
    //! Where the connection should be established to.
    boost::asio::ip::udp::endpoint remoteV;
    //! Parameter to define the communication source
    boost::asio::ip::address localV;
};

}

#endif
