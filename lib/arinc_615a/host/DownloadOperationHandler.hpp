// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Host::DownloadOperationHandler.
 **/

#ifndef ARINC_615A_HOST_DOWNLOADOPERATIONHANDLER_HPP
#define ARINC_615A_HOST_DOWNLOADOPERATIONHANDLER_HPP

#include <arinc_615a/host/Host.hpp>
#include <arinc_615a/host/OperationHandler.hpp>

#include <arinc_615a/information/Information.hpp>

#include <arinc_649/Arinc649.hpp>

#include <tftp/packets/Packets.hpp>

#include <boost/asio/ip/udp.hpp>

#include <string>

namespace Arinc615a::Host {

/**
 * @brief ARINC 615A %Host Download %Operation Handler.
 **/
class DownloadOperationHandler : public OperationHandler
{
  public:
    /**
     * @brief Status Message has been received by the Host Data Loader.
     *
     * This handler is called when the Host Data Loader has received a *Download Status File*.
     * The file is decoded and the status information is provided to the registered handler.
     *
     * @param[in] status
     *   Status of the download operation.
     **/
    virtual void status( const Information::DownloadStatus &status ) = 0;

    /**
     * @brief File request (WRQ) has been received by the Host Data Loader.
     *
     * The ARINC 615A *Part Number Option* could be provided by the target hardware loader to identify the file, which
     * will be transmitted.
     * @note
     * The *Part Number Option* is officially not part of the file transfers of Download Operations.
     * This is an extension to the ARINC 615A Protocol.
     *
     * The ARINC 615A *Checksum Option* could be provided by the target hardware loader to check the integrity of the
     * file, which will be transmitted.
     *
     * The implementor of this callback should perform one of the following actions:
     * - Create and execute a file transfer with the corresponding file.
     *   @ref Arinc615a::Host::DownloadOperation::fileTransfer()
     * - Reject the transfer with an error operation
     *   @ref Arinc615a::Host::DownloadOperation::errorOperation()
     * - Suspend the transfer with a wait operation.
     *   @ref Arinc615a::Host::DownloadOperation::waitOperation()
     *
     * @param[in] remote
     *   Source address information of the request.
     * @param[in] filename
     *   Filename of request
     * @param[in] clientTftpOptions
     *   Received TFTP options (from THL).
     *   Should be provided unmodified to the file transfer operation.
     * @param[in] partNumber
     *   Part Number provided as the `part number` option, if any.
     *   If used, must be provided unmodified to the file transfer operation.
     * @param[in] checkValue
     *   Check Value provided as the `checksum_x` option, if any.
     *   If used, must be provided unmodified to the file transfer operation.
     *
     * @sa Arinc615a::Host::DownloadOperation::fileTransfer()
     * @sa Arinc615a::Host::DownloadOperation::errorOperation()
     * @sa Arinc615a::Host::DownloadOperation::waitOperation()
     **/
    virtual void fileRequest(
      const boost::asio::ip::udp::endpoint &remote,
      std::string_view filename,
      const ::Tftp::Packets::TftpOptions &clientTftpOptions,
      std::string_view partNumber,
      const Arinc649::CheckValue &checkValue ) = 0;
};

}

#endif
