// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Target::OperationImpl.
 **/

#ifndef ARINC_615A_TARGET_OPERATIONIMPL_HPP
#define ARINC_615A_TARGET_OPERATIONIMPL_HPP

#include <arinc_615a/target/Operation.hpp>
#include <arinc_615a/target/OperationHandler.hpp>
#include <arinc_615a/target/Target.hpp>

#include <arinc_615a/information/Information.hpp>

#include <arinc_615a/files/Files.hpp>
#include <arinc_615a/files/ProtocolFileLogger.hpp>

#include <arinc_615a/tftp/clients/Clients.hpp>

#include <arinc_615a/tftp/servers/Servers.hpp>

#include <arinc_615a/Arinc615aConfiguration.hpp>
#include <arinc_615a/TargetId.hpp>

#include <arinc_649/CheckValue.hpp>

#include <boost/asio/ip/udp.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/system_timer.hpp>

#include <string>

namespace Arinc615a::Target {

/**
 * @brief Implementation of ARINC 615A Target Operation.
 **/
class OperationImpl
{
  public:
    /**
     * @brief TFTP request handler called from Target Protocol.
     *
     * Is called by Protocol TFTP Server handler, when a new TFTP request has been received.
     *
     * @param[in] remote
     *   Source of request.
     * @param[in] requestType
     *   TFTP request type.
     * @param[in] filename
     *   Request filename.
     * @param[in] clientTftpOptions
     *   Received TFTP Client Options.
     * @param[in] clientArinc615aOptions
     *   Received ARINC 615A Options.
     **/
    virtual void tftpRequest(
      boost::asio::ip::udp::endpoint remote,
      Tftp::RequestType requestType,
      std::string filename,
      ::Tftp::Packets::TftpOptions clientTftpOptions,
      Tftp::Arinc615aOptions clientArinc615aOptions ) = 0;

  protected:
    /**
     * @brief Initialises the operation instance.
     *
     * @param[in] ioContext
     *   I/O context used for asynchronous operations.
     * @param[in] configuration
     *   Data Loader Configuration.
     * @param[in] protocolVersion
     *   ARINC 615A Version used for protocol files.
     * @param[in] tftpServer
     *   TFTP Server
     * @param[in] handler
     *   Operation handler.
     * @param[in] targetId
     *   Target ID.
     * @param[in] statusTransmissionRate
     *   Transmission Rate of Status Information.
     **/
    OperationImpl(
      boost::asio::io_context &ioContext,
      Arinc615aConfiguration configuration,
      Arinc615aVersion protocolVersion,
      Tftp::Servers::ServerPtr tftpServer,
      OperationHandler &handler,
      TargetId targetId,
      std::chrono::seconds statusTransmissionRate );

    //! Destructor
    virtual ~OperationImpl();

    /**
     * @brief Returns the Data Loader configuration.
     *
     * @return Data Loader Configuration
     **/
    [[nodiscard]] const Arinc615aConfiguration& configuration() const noexcept;

    /**
     * @brief Returns the Version to use for Protocol Files.
     *
     * @return Protocol Version used for Operation.
     **/
    [[nodiscard]] Arinc615aVersion protocolVersion() const noexcept;

    /**
     * @brief Returns the Protocol File Logger.
     *
     * @return Protocol File Logger
     **/
    [[nodiscard]] Files::ProtocolFileLogger& protocolFileLogger();

    /**
     * @brief Returns the protocol filename for the given file type.
     *
     * @param[in] fileType
     *   File type.
     *
     * @return Protocol filename for the given file type.
     **/
    [[nodiscard]] std::string protocolFilename( Files::ProtocolFileType fileType ) const;

    /**
     * @brief Executes a TFTP Server Error Operation.
     *
     * Sends a TFTP Error Packet.
     *
     * @param[in] remote
     *   Where the error packet shall be transmitted to.
     * @param[in] errorCode
     *   Error code of the error packet.
     * @param[in] errorMessage
     *   Error message of the packet.
     **/
    void tftpServerErrorOperation(
      const boost::asio::ip::udp::endpoint &remote,
      ::Tftp::Packets::ErrorCode errorCode,
      std::string errorMessage );

    /**
     * @brief Creates a TFTP Server Write Operation (TFTP WRQ)
     *
     * This operation will read data from the client to server.
     *
     * @return TFTP Server Operation.
     **/
    [[nodiscard]] Tftp::Servers::WriteOperationPtr tftpServerWriteOperation();

    /**
     * @brief Creates a TFTP Client Read Operation.
     *
     * @param[in] operationDeferredHandler
     *   Operation Deferred Handler
     * @param[in] optionNegotiationHandler
     *   TFTP Options Negotiation Handler
     * @param[in] completionHandler
     *   Operation Completion Handler
     * @param[in] dataHandler
     *   Receive Data Handler
     * @param[in] filename
     *   Request Filename
     * @param[in] partNumberOption
     *   Part Number Option
     * @param[in] checksumOption
     *   Checksum Option (If set to NoCheckValue, not set)
     *
     * @return Created Client Read Operation.
     **/
    [[nodiscard]] Tftp::Clients::ReadOperationPtr tftpClientReadOperation(
      Tftp::Clients::OperationDeferredHandler operationDeferredHandler,
      Tftp::Clients::OptionNegotiationHandler optionNegotiationHandler,
      Tftp::Clients::OperationCompletedHandler completionHandler,
      Tftp::ReceiveDataHandlerPtr dataHandler,
      std::string filename,
      std::string partNumberOption,
      Arinc649::CheckValue checksumOption );

    /**
     * @brief Creates a TFTP Client Write Operation.
     *
     * @param[in] operationDeferredHandler
     *   Operation Deferred Handler
     * @param[in] optionNegotiationHandler
     *   TFTP Options Negotiation Handler
     * @param[in] completionHandler
     *   Operation Completion Handler
     * @param[in] dataHandler
     *   Receive Data Handler
     * @param[in] filename
     *   Request Filename
     * @param[in] partNumberOption
     *   Part Number Option
     * @param[in] checksumOption
     *   Checksum Option (If set to NoCheckValue, not set)
     *
     * @return Created Client Read Operation.
     **/
    [[nodiscard]] Tftp::Clients::WriteOperationPtr tftpClientWriteOperation(
      Tftp::Clients::OperationDeferredHandler operationDeferredHandler,
      Tftp::Clients::OptionNegotiationHandler optionNegotiationHandler,
      Tftp::Clients::OperationCompletedHandler completionHandler,
      Tftp::TransmitDataHandlerPtr dataHandler,
      std::string filename,
      std::string partNumberOption,
      Arinc649::CheckValue checksumOption );

    /**
     * @copydoc Operation::start()
     *
     * # Sends the initialisation file
     * # starts the timer for triggering status responses.
     **/
    void doStart(
      boost::asio::ip::udp::endpoint hostAddress,
      ::Tftp::Packets::TftpOptions clientTftpOptions,
      std::optional< uint16_t > port );

    /**
     * @brief Sends the initialisation file to the host DLA.
     *
     * The initialisation file is always sent with status code
     * Operation accepted and no additional description.
     *
     * @param[in] hostAddress
     *   Host address, where to send the file.
     * @param[in] clientTftpOptions
     *   TFTP Options received from host.
     * @param[in] port
     *   Additional Options received from host.
     **/
    void sendInitFile(
      boost::asio::ip::udp::endpoint hostAddress,
      ::Tftp::Packets::TftpOptions clientTftpOptions,
      std::optional< uint16_t > port );

    /**
     * @brief Completion handler of Initialisation file transmission.
     *
     * @param[in] transferStatus
     *   Transfer Status.
     **/
    void sendInitFileComplete( ::Tftp::TransferStatus transferStatus );

    /**
     * @brief Sends a protocol file to the DLA.
     *
     * Creates a TFTP write request operation and sends the given sata.
     *
     * @param[in] fileType
     *   File type for filename determination.
     * @param[in] operationDeferredHandler
     *   Operation Deferred Handler
     * @param[in] completionHandler
     *   Transfer completion handler.
     * @param[in] dataHandler
     *   Protocol File Data.
     * @param[in] checkValue
     *   Check Value to use as *Checksum Option*.
     *
     * @return TFTP Client Operation.
     **/
    Tftp::Clients::WriteOperationPtr protocolFileOperation(
      Files::ProtocolFileType fileType,
      Tftp::Clients::OperationDeferredHandler operationDeferredHandler,
      Tftp::Clients::OperationCompletedHandler completionHandler,
      Tftp::TransmitDataHandlerPtr dataHandler,
      Arinc649::CheckValue checkValue = Arinc649::CheckValue::NoCheckValue );

    /**
     * @brief Triggers immediate Status Transmission.
     *
     * The status transmission timer is set to `0`, therefore the status is sent as soon as possible.
     **/
    void triggerStatusTransmission();

    /**
     * @brief Re-Triggers the Status Transmission Timer.
     *
     * When the timer expires, a status file is sent
     **/
    void triggerStatusTransmissionTimer();

    /**
     * @brief Finish Operation.
     *
     * Inform handler and chancel status transmission timer.
     **/
    void finalise( FinalStatus finalStatus = FinalStatus::Completed, std::string_view description = {} );

    /**
     * @brief Returns the host address.
     *
     * @return Host address
     **/
    [[nodiscard]] const boost::asio::ip::address& hostAddress() const;

    /**
     * @brief Sets the status to the given states.
     *
     * @param[in] code
     *   Status code
     * @param description
     *   Status description
     * @param estimatedTime
     *   Estimated time.
     **/
    virtual void status( StatusCode code, std::string description = {}, int16_t estimatedTime = 0U ) = 0;

    /**
     * @brief Instruct the class to send the currently set status to the host.
     *
     * As the status file is operation dependant, this must be implemented by child classes.
     **/
    virtual void statusFile() = 0;

  private:
    /**
     * @brief Deadline timer handler.
     *
     * This handler is called when the DLP timeout occurs.
     *
     * Sends the status file.
     *
     * @param[in] errorCode
     *   Error code.
     **/
    void statusTransmissionTimerHandler( const boost::system::error_code &errorCode );

    /**
     * @brief Option Negotiation Operation for Protocol File Transmission.
     *
     * Negotiates the additional options received from the Host TFTP Server.
     *
     * @param[in] providedCheckValue
     *   Check Value Option as sent to the DLA.
     * @param[in] serverOptions
     *   Received TFTP Options.
     *
     * @return If Option Negotiation was successful.
     **/
    bool protocolFileOptionsNegotiation(
      const Arinc649::CheckValue &providedCheckValue,
      const Tftp::Arinc615aOptions &serverOptions );

    //! Data Loader Configuration
    Arinc615aConfiguration configurationV;
    //! ARINC 615A Version used for protocol files.
    Arinc615aVersion protocolVersionV;
    //! TFTP Server
    Tftp::Servers::ServerPtr tftpServerV;
    //! TFTP Client
    Tftp::Clients::ClientPtr tftpClientV;
    //! Operation Handler
    OperationHandler &handlerV;
    //! Target ID
    const TargetId targetIdV;
    //! Transmission Rate of Status Information.
    std::chrono::seconds statusTransmissionRateV;
    //! Protocol File Logger
    Files::ProtocolFileLogger protocolFileLoggerV;

    //! Host IP address (stored after sending init file)
    boost::asio::ip::address hostAddressV;
    //! Host TFTP Server Port - set to config default and can be overridden by port option of initialisation file
    uint16_t hostPortV;

    //! Timer who handles Status Transmission Timeout
    boost::asio::system_timer statusTransmissionTimerV;

    //! Initialisation Operation
    Tftp::Servers::ReadOperationPtr initialisationOperationV;
};

}

#endif
