// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Host::DownloadOperation.
 **/

#ifndef ARINC_615A_HOST_DOWNLOADOPERATION_HPP
#define ARINC_615A_HOST_DOWNLOADOPERATION_HPP

#include <arinc_615a/host/Host.hpp>
#include <arinc_615a/host/Operation.hpp>

#include <arinc_615a/tftp/servers/Servers.hpp>

#include <arinc_649/Arinc649.hpp>

#include <boost/asio/ip/udp.hpp>

#include <memory>

namespace Arinc615a::Host {

/**
 * @brief ARINC 615A %Host Download %Operation.
 *
 * This is the base class for the:
 * - Media Defined Download Operation (@ref MediaDefinedDownloadOperation), and
 * - Operator Defined Download Operation (@ref OperatorDefinedDownloadOperation).
 **/
class ARINC_615A_EXPORT DownloadOperation : public Operation
{
  public:
    /**
     * @brief Initiates a new %File Transfer.
     *
     * This operation should be used to receive the file, which is transferred by the target data loader.
     *
     * The ARINC 615A *Part Number Option* could be provided by the target hardware loader to identify the file, which
     * will be transmitted.
     * @note
     * The *Part Number Option* is officially not part of the file transfers of Download Operations.
     * This is an extension to the ARINC 615A %Protocol.
     *
     * The ARINC 615A *Checksum Option* could be provided by the target hardware loader to check the integrity of the
     * file, which will be transmitted.
     *
     * @param[in] dataHandler
     *   Data handler (retrieves the data)
     * @param[in] remote
     *   Source of the request (from THL)
     * @param[in] clientTftpOptions
     *   TFTP options (from THL).
     *   Will be negotiated by this operation.
     * @param[in] partNumber
     *   Part Number provided as the `part number` option, if any.
     * @param[in] checkValue
     *   Check Value provided as the `checksum_x` option, if any.
     *
     * @return TFTP Server %Operation for %File Transfer
     **/
    [[nodiscard]] virtual Tftp::Servers::WriteOperationPtr fileTransfer(
      Tftp::ReceiveDataHandlerPtr dataHandler,
      boost::asio::ip::udp::endpoint remote,
      ::Tftp::Packets::TftpOptions clientTftpOptions,
      std::string partNumber,
      Arinc649::CheckValue checkValue ) = 0;

    /**
     * @brief Performs a Wait Operation.
     *
     * This operation is used to suspend a file transfer request from the THL when the host is not ready for servicing
     * them, e.g. Media not inserted.
     *
     * @param[in] remote
     *   Source of request
     * @param[in] waitTime
     *   Desired wait time in seconds.
     **/
    virtual void waitOperation( const boost::asio::ip::udp::endpoint &remote, std::chrono::seconds waitTime ) = 0;

    /**
     * @brief Performs a TFTP Error Operation.
     *
     * This operation is used to cancel a file transfer request from the THL when the host is not able to service them.
     *
     * @param[in] remote
     *   Where the error packet shall be transmitted to.
     * @param[in] errorCode
     *   Error code of the error packet.
     * @param[in] errorMessage
     *   Error message of the packet.
     **/
    virtual void errorOperation(
      const boost::asio::ip::udp::endpoint &remote,
      ::Tftp::Packets::ErrorCode errorCode,
      std::string errorMessage ) = 0;
};

}

#endif
