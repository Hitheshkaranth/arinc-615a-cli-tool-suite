// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Tftp::Clients::ReadOperationImpl.
 **/

#ifndef ARINC_615A_TFTP_CLIENTS_READOPERATIONIMPL_HPP
#define ARINC_615A_TFTP_CLIENTS_READOPERATIONIMPL_HPP

#include <arinc_615a/tftp/clients/ReadOperation.hpp>

#include <arinc_615a/tftp/clients/implementation/OperationImpl.hpp>

#include <arinc_649/CheckValue.hpp>

#include <tftp/TftpOptionsConfiguration.hpp>

#include <boost/asio/ip/udp.hpp>

namespace Arinc615a::Tftp::Clients {

/**
 * @brief Encapsulation of TFTP Client Read Operation.
 **/
class ReadOperationImpl final : public ReadOperation, private OperationImpl
{
  public:
    /**
     * @brief Initialises the ARINC 615A TFTP client read operation.
     *
     * @param[in] tftpClient
     *   TFTP client.
     * @param[in] ioContext
     *   I/O context used for communication.
     **/
    ReadOperationImpl( ::Tftp::Clients::ClientPtr tftpClient, boost::asio::io_context &ioContext );

    //! Destructor.
    ~ReadOperationImpl() override = default;

    //! @copydoc ReadOperation::tftpTimeout()
    ReadOperation& tftpTimeout( std::chrono::seconds timeout ) override;

    //! @copydoc ReadOperation::tftpRetries()
    ReadOperation& tftpRetries( uint16_t retries ) override;

    //! @copydoc ReadOperation::dally()
    ReadOperation& dally( bool dally ) override;

    //! @copydoc ReadOperation::optionsConfiguration()
    ReadOperation& optionsConfiguration( ::Tftp::TftpOptionsConfiguration optionsConfiguration ) override;

    //! @copydoc ReadOperation::dlpRetries()
    ReadOperation& dlpRetries( uint16_t retries ) override;

    //! @copydoc ReadOperation::operationDeferredHandler()
    ReadOperation& operationDeferredHandler( OperationDeferredHandler handler ) override;

    //! @copydoc ReadOperation::optionNegotiationHandler()
    ReadOperation& optionNegotiationHandler( OptionNegotiationHandler handler ) override;

    //! @copydoc ReadOperation::completionHandler()
    ReadOperation& completionHandler( OperationCompletedHandler handler ) override;

    //! @copydoc ReadOperation::dataHandler()
    ReadOperation& dataHandler( ReceiveDataHandlerPtr handler ) override;

    //! @copydoc ReadOperation::filename
    ReadOperation& filename( std::string filename ) override;

    //! @copydoc ReadOperation::portOption
    ReadOperation& portOption( std::optional< uint16_t > portOption ) override;

    //! @copydoc ReadOperation::partNumberOption()
    ReadOperation& partNumberOption( std::string partNumberOption ) override;

    //! @copydoc ReadOperation::checksumOption()
    ReadOperation& checksumOption(
      Arinc649::CheckValue checksumOption ) override;

    //! @copydoc ReadOperation::remote
    ReadOperation& remote( boost::asio::ip::udp::endpoint remote ) override;

    //! @copydoc ReadOperation::local
    ReadOperation& local( boost::asio::ip::address local ) override;

    //! @copydoc ReadOperation::request()
    void request() override;

    //! @copydoc ReadOperation::gracefulAbort()
    void gracefulAbort( ::Tftp::Packets::ErrorCode errorCode, std::string errorMessage = {} ) override;

    //! @copydoc ReadOperation::abort()
    void abort() override;

    //! @copydoc ReadOperation::errorInformation() const
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
    //! TFTP Dally Option
    bool dallyV{ false };
    //! TFTP Options Configuration.
    ::Tftp::TftpOptionsConfiguration tftpOptionsConfigurationV;
    //! Data Handler
    ReceiveDataHandlerPtr dataHandlerV;
    //! Which file shall be requested
    std::string filenameV;
    //! Port Option.
    std::optional< uint16_t > portOptionV;
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
