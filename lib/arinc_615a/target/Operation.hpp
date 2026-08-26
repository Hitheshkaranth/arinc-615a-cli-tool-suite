// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Target::Operation.
 **/

#ifndef ARINC_615A_TARGET_OPERATION_HPP
#define ARINC_615A_TARGET_OPERATION_HPP

#include <arinc_615a/target/Target.hpp>

#include <tftp/packets/Packets.hpp>

#include <boost/asio/ip/udp.hpp>

#include <string>

namespace Arinc615a::Target {

/**
 * @brief ARINC 615A %Target %Operation.
 **/
class ARINC_615A_EXPORT Operation
{
  public:
    //! Destructor.
    virtual ~Operation() noexcept = default;

    /**
     * @brief Executes the ARINC 615A Operation.
     *
     * @param[in] hostAddress
     *   Host address.
     * @param[in] clientTftpOptions
     *   Received TFTP Options.
     * @param[in] port
     *   Received ARINC 615A Port Option.
     **/
    virtual void start(
      boost::asio::ip::udp::endpoint hostAddress,
      ::Tftp::Packets::TftpOptions clientTftpOptions,
      std::optional< uint16_t > port ) = 0;

    /**
     * @brief Updates the Operation Status.
     *
     * The status message (with the status code set to *Operation Accepted*) is sent immediately.
     *
     * @throw Arinc615aException
     *   If already sent status was modified by calling inProgress.
     **/
    virtual void accepted() = 0;

    /**
     * @brief Updates the Operation Status.
     *
     * The status message is sent after a configured timeout to the host.
     * This operation sets the status which will be transmitted on timeout.
     *
     * If @p immediateTransmission is set to true, the status is sent immediately.
     *
     * @param[in] immediateTransmission
     *   If set to @p true, the status is sent immediately.
     * @param[in] estimatedTime
     *   Estimated time until completion.
     *   If set to @p -1, no information is available.
     * @param[in] description
     *   Status description sent to the host.
     **/
    virtual void inProgress(
      bool immediateTransmission = false,
      int16_t estimatedTime = -1,
      std::string description = {} ) = 0;

    /**
     * @brief Completes the Upload Operation.
     *
     * When this operation is called, the final status is set according to @p finalStatus and @p description and sent.
     * When the status file has been transmitted, the operation completes.
     * No further file operations shall be initiated after this operation.
     *
     * @param[in] finalStatus
     *   Final Status.
     * @param description
     *   Final Status Description
     **/
    virtual void finished( FinalStatus finalStatus, std::string description = {} ) = 0;
};

}

#endif
