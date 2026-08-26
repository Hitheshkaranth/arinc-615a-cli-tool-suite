// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Target::InformationOperationImpl.
 **/

#ifndef ARINC_615A_TARGET_INFORMATIONOPERATIONIMPL_HPP
#define ARINC_615A_TARGET_INFORMATIONOPERATIONIMPL_HPP

#include <arinc_615a/target/Target.hpp>
#include <arinc_615a/target/InformationOperation.hpp>
#include <arinc_615a/target/InformationOperationHandler.hpp>

#include <arinc_615a/target/implementation/OperationImpl.hpp>

#include <arinc_615a/information/Status.hpp>

#include <mutex>

namespace Arinc615a::Target {

/**
 * @brief ARINC 615A %Target %Information %Operation.
 *
 * Server side implementation of ARINC 615A information operation.
 **/
class InformationOperationImpl final : public InformationOperation, public OperationImpl
{
  public:
    /**
     * @brief Initialises the ARINC 615A target information operation instance.
     *
     * @param[in] ioContext
     *   I/O context used for asynchronous operation
     * @param[in] dlConfiguration
     *   Data Loader Configuration.
     * @param[in] protocolVersion
     *   ARINC 615A Version used for protocol files.
     * @param[in] tftpServer
     *   TFTP Server
     * @param[in] operationConfiguration
     *   Operation Configuration.
     **/
    InformationOperationImpl(
      boost::asio::io_context &ioContext,
      Arinc615aConfiguration dlConfiguration,
      Arinc615aVersion protocolVersion,
      Tftp::Servers::ServerPtr tftpServer,
      InformationOperationConfiguration operationConfiguration );

    //! Destructor
    ~InformationOperationImpl() override = default;

    //! @copydoc InformationOperation::start()
    void start(
      boost::asio::ip::udp::endpoint hostAddress,
      ::Tftp::Packets::TftpOptions clientTftpOptions,
      std::optional< uint16_t > port ) override;

    //! @copydoc InformationOperation::accepted
    void accepted() override;

    //! @copydoc InformationOperation::inProgress
    void inProgress(
      bool immediateTransmission = false,
      int16_t estimatedTime = -1,
      std::string description = {} ) override;

    //! @copydoc InformationOperation::targetInformation
    void targetInformation(
      Information::TargetsHardware targetsHardware,
      Arinc649::CheckValueType checkValueType = Arinc649::CheckValueType::NotUsed ) override;

    //! @copydoc InformationOperation::finished()
    void finished( FinalStatus finalStatus, std::string description = {} ) override;

  private:
    /**
     * @copydoc OperationImpl::tftpRequest()
     *
     * The Information Operation doesn't expect any transfers.
     * All requests are denied.
     **/
    void tftpRequest(
      boost::asio::ip::udp::endpoint remote,
      Tftp::RequestType requestType,
      std::string filename,
      ::Tftp::Packets::TftpOptions clientTftpOptions,
      Tftp::Arinc615aOptions clientArinc615aOptions ) override;

    //! @copydoc OperationImpl::status(StatusCode,std::string,int16_t)
    void status( StatusCode code, std::string description = {}, int16_t estimatedTime = 0 ) override;

    //! @copydoc OperationImpl::statusFile
    void statusFile() override;

    /**
     * @brief Completion handler of status transmission operation.
     *
     * @param[in] sentStatus
     *   Sent Status.
     * @param[in] transferStatus
     *   Transfer status.
     **/
    void statusFileCompleted( Information::InformationStatus sentStatus, Tftp::TransferStatus transferStatus );

    /**
     * @brief Handler which is called after completion of target information transmission.
     *
     * Prepares finalisation of operation.
     * If status has been sent previously, sent final status and complete.
     *
     * @param[in] transferStatus
     *   TFTP transfer status.
     **/
    void targetInformationCompleted( Tftp::TransferStatus transferStatus );

    //! Information Operation Handler.
    InformationOperationHandler &handler;
    //! Status Information, which is periodically transmitted.
    Information::InformationStatus statusV;
    //! Status Mutex
    std::mutex statusMutex;
    //! Target Information TFTP Client Operation
    Tftp::Clients::WriteOperationPtr targetInformationOperation;
    //! Status TFTP Client Operation.
    Tftp::Clients::WriteOperationPtr statusOperation;
};

}

#endif
