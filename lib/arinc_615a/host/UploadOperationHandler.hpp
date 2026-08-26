// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Host::UploadOperationHandler.
 **/

#ifndef ARINC_615A_HOST_UPLOADOPERATIONHANDLER_HPP
#define ARINC_615A_HOST_UPLOADOPERATIONHANDLER_HPP

#include <arinc_615a/host/Host.hpp>
#include <arinc_615a/host/OperationHandler.hpp>

#include <arinc_615a/information/Information.hpp>

#include <arinc_649/Arinc649.hpp>

#include <tftp/packets/Packets.hpp>

#include <boost/asio/ip/udp.hpp>

#include <string_view>

namespace Arinc615a::Host {

/**
 * @brief ARINC 615A %Host Upload %Operation Handler.
 *
 * @sa @ref Arinc615a::Host::UploadOperation
 **/
class UploadOperationHandler : public OperationHandler
{
  public:
    /**
     * @brief Status Message has been received.
     *
     * This handler is called when the Host Data Loader has received a *Upload Status File*.
     * The file is decoded and the status information is provided to the registered handler.
     *
     * @param[in] status
     *   Status of the Load Upload Operation.
     **/
    virtual void status( const Information::UploadStatus &status ) = 0;

    /**
     * @brief File Transfer Request from the Target Data Loader.
     *
     * The ARINC 615A *Part Number Option* could be provided by the target hardware loader to identify the file, which
     * is requested.
     *
     * The ARINC 615A *Checksum Option* could be provided by the target hardware loader to identify the file, which is
     * requested.
     *
     * The implementor should perform one of the following actions:
     * - create and execute a file transfer with the appropriate file.
     * - reject the transfer with an error operation
     * - suspend the transfer with a wait operation.
     *
     * @sa @ref Arinc615a::Host::UploadOperation::fileTransfer()
     *
     * @param[in] remote
     *   Source Address Information.
     * @param[in] filename
     *   Requested filename.
     * @param[in] clientTftpOptions
     *   Received TFTP options (from THL).
     *   Should be provided unmodified to the file transfer operation.
     * @param[in] loadPartNumber
     *   Load Part Number, containing the requested file, provided as `part number` option, if any.
     *   If used, it must be forwarded to the file transfer operation
     *   (@ref Arinc615a::Host::UploadOperation::fileTransfer() ).
     * @param[in] checkValue
     *   Check Value provided as `checksum_x` option, if any.
     *   If used, it must be provided to file transfer operation
     *   (@ref Arinc615a::Host::UploadOperation::fileTransfer() ).
     **/
    virtual void fileRequest(
      const boost::asio::ip::udp::endpoint &remote,
      std::string_view filename,
      const ::Tftp::Packets::TftpOptions &clientTftpOptions,
      std::string_view loadPartNumber,
      const Arinc649::CheckValue &checkValue ) = 0;
};

}

#endif
