// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Target::ErrorOperationImpl.
 **/

#ifndef ARINC_615A_TARGET_ERROROPERATIONIMPL_HPP
#define ARINC_615A_TARGET_ERROROPERATIONIMPL_HPP

#include <arinc_615a/target/Target.hpp>
#include <arinc_615a/target/ErrorOperation.hpp>
#include <arinc_615a/target/ErrorOperationConfiguration.hpp>

#include <arinc_615a/files/ProtocolFileLogger.hpp>

#include <arinc_615a/tftp/clients/Clients.hpp>

#include <arinc_615a/tftp/servers/Servers.hpp>

#include <arinc_615a/Arinc615aConfiguration.hpp>

#include <boost/asio/ip/udp.hpp>

#include <string>
#include <memory>

namespace Arinc615a::Target {

/**
 * @brief ARINC 615A Target Error Operation.
 *
 * This operation creates an initialisation file, with the supplied error information provided.
 **/
class ErrorOperationImpl final : public ErrorOperation
{
  public:
    /**
     * @brief Creates the error operation.
     *
     * @param[in] dlConfiguration
     *   Data Loader Configuration.
     * @param[in] protocolVersion
     *   ARINC 615A Version used for protocol files.
     * @param[in] tftpServer
     *   TFTP Server
     * @param[in] configuration
     *   Error Operation Configuration.
     **/
    ErrorOperationImpl(
      Arinc615aConfiguration dlConfiguration,
      Arinc615aVersion protocolVersion,
      Tftp::Servers::ServerPtr tftpServer,
      ErrorOperationConfiguration configuration );

    //! Destructor
    ~ErrorOperationImpl() noexcept = default;

    //!@ @copydoc ErrorOperation::start
    void start(
      boost::asio::ip::udp::endpoint hostAddress,
      ::Tftp::Packets::TftpOptions clientTftpOptions,
      std::optional< uint16_t > port ) override;

  private:
    /**
     * @brief Handles the transmitted status file.
     *
     * @param[in] status
     *   TFTP Transfer Status.
     **/
    void initialisationFileCompleted( ::Tftp::TransferStatus status );

    //! Data Loader Configuration
    Arinc615aConfiguration dlConfigurationV;
    //! ARINC 615A Version used for protocol files.
    Arinc615aVersion protocolVersionV;
    //! TFTP Server
    Tftp::Servers::ServerPtr tftpServerV;
    //! Error Operation Configuration
    ErrorOperationConfiguration configurationV;
    //! Protocol File Logger
    Files::ProtocolFileLogger protocolFileLoggerV;

    //! Initialisation File Operation
    Tftp::Servers::ReadOperationPtr initialisationFileOperationV;
};

}

#endif
