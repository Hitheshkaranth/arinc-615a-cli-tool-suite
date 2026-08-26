// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Host::UploadOperation.
 **/

#ifndef ARINC_615A_HOST_UPLOADOPERATION_HPP
#define ARINC_615A_HOST_UPLOADOPERATION_HPP

#include <arinc_615a/host/Host.hpp>
#include <arinc_615a/host/Operation.hpp>

#include <arinc_615a/information/Information.hpp>

#include <arinc_615a/tftp/servers/Servers.hpp>

#include <arinc_649/Arinc649.hpp>

#include <boost/asio/ip/udp.hpp>

#include <chrono>

namespace Arinc615a::Host {

/**
 * @brief ARINC 615A %Host Upload %Operation.
 *
 * Operation Steps:
 * - Initialisation Phase:
 *   - Request Initialisation File (TFTP RRQ from Host DL)
 * - List Transfer Phase
 *   - Send Load List (TFTP WRQ from Host DL)
 * - Transfer Phase:
 *   - Transmit (Load Header, Data, and Support) Files (TFTP RRQ from Target DL)
 **/
class ARINC_615A_EXPORT UploadOperation : public Operation
{
  public:
    /**
     * @brief Sends the given Upload Operation Request File to the target.
     *
     * This operation should be called exactly once, after the operation has been accepted from the target data loader.
     *
     * @param[in] loads
     *   Upload Load List.
     **/
    virtual void loadList( Information::UploadLoads loads ) = 0;

    /**
     * @brief Initiates the file transfer, which was requested by the Target Hardware Loader.
     *
     * The ARINC 615A *Part Number Option* could be provided by the target hardware loader to identify the part
     * containing the file being requested.
     *
     * The ARINC 615A *Checksum Option* could be provided by the target hardware loader to identify the file, which is
     * requested.
     *
     * @param[in] dataHandler
     *   Handler, which supplies the data.
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
     * @return TFTP Server Operation for File Transfer
     **/
    [[nodiscard]] virtual Tftp::Servers::ReadOperationPtr fileTransfer(
      Tftp::TransmitDataHandlerPtr dataHandler,
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
     * This operation is used to cancel a file transfer request from the THL when the host is not able to servicing
     * them.
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
