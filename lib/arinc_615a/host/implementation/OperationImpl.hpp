// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Host::OperationImpl.
 **/

#ifndef ARINC_615A_HOST_OPERATIONIMPL_HPP
#define ARINC_615A_HOST_OPERATIONIMPL_HPP

#include <arinc_615a/host/Host.hpp>
#include <arinc_615a/host/Operation.hpp>

#include <arinc_615a/files/Files.hpp>
#include <arinc_615a/files/ProtocolFileLogger.hpp>

#include <arinc_615a/tftp/clients/Clients.hpp>

#include <arinc_615a/tftp/servers/Servers.hpp>

#include <arinc_615a/Arinc615aConfiguration.hpp>
#include <arinc_615a/TargetId.hpp>

#include <tftp/packets/Packets.hpp>

#include <tftp/files/Files.hpp>

#include <boost/asio/ip/udp.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/system_timer.hpp>

#include <cstdint>
#include <chrono>
#include <string>

namespace Arinc615a::Host {

/**
 * @brief Base implementation of host operation.
 **/
class OperationImpl
{
  protected:
    /**
     * @brief Constructor of HostOperation which must be called by derived operations.
     *
     * @param[in] ioContext
     *   I/O context used for asynchronous operations.
     * @param[in] configuration
     *   Data Loader Configuration.
     * @param[in] handler
     *   Operation handler.
     * @param[in] targetAddress
     *   Address information of target.
     * @param[in] targetId
     *   Target ID of target queried.
     * @param[in] dlpTimeout
     *   Timeout for DL-Transfers (e.g. Status Information).
     * @param[in] portOption
     *   Use dynamic port and port option.
     **/
    OperationImpl(
      boost::asio::io_context &ioContext,
      Arinc615aConfiguration configuration,
      OperationHandler &handler,
      boost::asio::ip::address targetAddress,
      TargetId targetId,
      std::chrono::seconds dlpTimeout,
      bool portOption );

    //! Destructor
    virtual ~OperationImpl() noexcept;

    /**
     * @copydoc Operation::abort
     *
     * Own transfers must be aborted by the caller.
     **/
    void doAbort( Operation::AbortReason reason );

    /**
     * @copydoc Operation::terminate
     *
     * Own transfers must be terminated by the caller.
     **/
    void doTerminate( Operation::AbortReason reason );

    /**
     * @brief Returns the Data Loader Configuration.
     *
     * @return Data Loader Configuration.
     **/
    [[nodiscard]] const Arinc615aConfiguration& configuration() const;

    /**
     * @brief Returns the TFTP Client for Communication.
     *
     * @return TFTP Client Instance.
     **/
    [[nodiscard]] Tftp::Clients::Client& tftpClient();

    /**
     * @brief Returns the TFTP Server for Communication.
     *
     * @return TFTP Server Instance.
     **/
    [[nodiscard]] Tftp::Servers::Server& tftpServer();

    /**
     * @brief Returns the Protocol File Logger.
     *
     * @return Protocol File Logger
     **/
    [[nodiscard]] Files::ProtocolFileLogger& protocolFileLogger();

    /**
     * @brief Handle a TFTP request from the target.
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

    /**
     * @brief Re-triggers the DLP timeout with a received exception time value.
     *
     * @param[in] exceptionTime
     *   The time in seconds, where no request is expected.
     *   If @p 0, the default configuration timeout is used
     **/
    void triggerDlpTimeout( std::chrono::seconds exceptionTime = std::chrono::seconds{ 0 } );

    /**
     * @brief Sets the finished flag and aborts the timeout wait operation.
     *
     * @param[in] status
     *   Final Status Code
     * @param description
     *   Final Status Description.
     **/
    void finished( StatusCode status, std::string_view description = {} );

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
     * @brief Returns the target ID of the target of this operation.
     *
     * @return The target ID of the target of this operation.
     **/
    [[nodiscard]] TargetId targetId() const;

    /**
     * @brief Returns the address of the target.
     *
     * @return Address of the target.
     **/
    [[nodiscard]] boost::asio::ip::address targetAddress() const;

    /**
     * @brief Checks, if operation is aborted and performs appropriate operations.
     *
     * This operation should be called, when a Write Request to a status file is received.
     *
     * @param[in] remote
     *   Source of the request (used for response in case of aborting).
     *
     * @return If the operation is aborted
     * @retval true
     *   Operation is aborted
     * @retval false
     *   Operation is not aborted.
     **/
    bool isAborted( const boost::asio::ip::udp::endpoint &remote );

    /**
     * @brief Checks some request information.
     *
     * Checks following:
     * - when file is a protocol file, the correct target ID is checked.
     *
     * If the check fails, a corresponding error packet is sent.
     *
     * @param[in] filename
     *   Request filename
     * @param[in] remote
     *   Source of request.
     *
     * @return If the check was successful.
     * @retval true
     *   Check was successful, transfer could be accepted.
     * @retval false
     *   Check failed - transfer has been cancelled.
     **/
    bool checkRequest( std::string_view filename, const boost::asio::ip::udp::endpoint &remote );

    /**
     * @brief Performs the initialisation operation.
     *
     * Queries the proper initialisation file from the target.
     *
     * This operation issues a TFTP read request to the target.
     *
     * @param[in] fileType
     *   Protocol File Type.
     *   Should be one of the 4 initialisation protocol file types
     **/
    void initialise( Files::ProtocolFileType fileType );

    /**
     * @brief Returns the Version to use for Protocol Files.
     *
     * The correct version to use is determined on reception of the initialisation file.
     *
     * @return Protocol Version used for Operation.
     **/
    [[nodiscard]] Arinc615aVersion protocolVersion() const noexcept;

  private:
    /**
     * @brief Handles TFTP requests from the TFTP server.
     *
     * @param[in] remote
     *   Remote Endpoint
     * @param[in] requestType
     *   TFTP Request Type.
     * @param[in] filename
     *   Requested filename.
     * @param[in] clientTftpOptions
     *   Received TFTP Options.
     * @param[in] clientArinc615aOptions
     *   Received ARINC 615A Options.
     **/
    void receivedTftpRequest(
      boost::asio::ip::udp::endpoint remote,
      Tftp::RequestType requestType,
      std::string_view filename,
      const ::Tftp::Packets::TftpOptions &clientTftpOptions,
      const Tftp::Arinc615aOptions &clientArinc615aOptions );

    /**
     * @brief Handler, which is called on DLP timeout
     *
     * @param[in] errorCode
     *   Error code given by the timer service
     **/
    void timerHandler( const boost::system::error_code& errorCode );

    /**
     * @brief Handler Called When the reception of the initialisation file is deferred.
     *
     * @param[in] waitTime
     *   Wait Time
     **/
    void initialisationFileDeferred( std::chrono::seconds waitTime );

    /**
     * @brief Additional Option Negotiation for Initialisation File.
     *
     * @param[in] options
     *   Received additional Options.
     *
     * @return If Option Negotiation was successful.
     **/
    bool initialisationFileOptionsNegotiation( const Tftp::Arinc615aOptions &options );

    /**
     * @brief Handler, which is called when the initialisation file receive operation has been completed.
     *
     * @param[in] fileType
     *   Protocol File Type.
     * @param[in] rawInitialisationFile
     *   Raw file data.
     * @param[in] status
     *   TFTP transfer status.
     **/
    void initialisationFileCompleted(
      Files::ProtocolFileType fileType,
      ::Tftp::Files::MemoryFilePtr rawInitialisationFile,
      Tftp::TransferStatus status );

    //! Data Loader Configuration
    const Arinc615aConfiguration configurationV;
    //! Operation Handler
    OperationHandler &handlerV;
    //! Target address (IP) of this Operation
    const boost::asio::ip::address targetAddressV;
    //! Target ID of this Operation
    const TargetId targetIdV;
    //! DLP Timeout
    const std::chrono::seconds dlpTimeoutV;
    //! Port Option
    const bool portOptionV;

    //! TFTP Client for Data Loading
    Tftp::Clients::ClientPtr tftpClientV;
    //! TFTP Server for Data Loading
    Tftp::Servers::ServerPtr tftpServerV;
    //! Protocol File Version (Determined on initialisation
    Arinc615aVersion protocolVersionV{};
    //! If aborted stores the abort reason
    Operation::AbortReason abortReasonV{ Operation::AbortReason::NoAbort };
    //! Protocol File Logger
    Files::ProtocolFileLogger protocolFileLoggerV;

    //! Time, who handles the reception timeout
    boost::asio::system_timer timerV;

    //! Initialisation File Transfer Operation
    Tftp::Clients::ReadOperationPtr initialisationOperationV;
};

}

#endif
