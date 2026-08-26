/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Host::BatchUploadOperationHandler.
 **/

#ifndef ARINC_615A_HOST_BATCHUPLOADOPERATIONHANDLER_HPP
#define ARINC_615A_HOST_BATCHUPLOADOPERATIONHANDLER_HPP

#include <arinc_615a/host/Host.hpp>

#include <arinc_615a/information/Information.hpp>

#include <arinc_649/Arinc649.hpp>

#include <tftp/packets/Packets.hpp>

#include <boost/asio/ip/udp.hpp>

#include <string_view>

namespace Arinc615a::Host {

/**
 * @brief Handler for Batch Upload Operations.
 *
 * This class is used by the proxy @ref BatchUploadOperationProxy to add Target ID foreach handler called.
 *
 * This handler is used to implement ARINC 615A Batch Upload operations to get the additional *Target ID* information.
 **/
class BatchUploadOperationHandler
{
  public:
    //! Destructor.
    virtual ~BatchUploadOperationHandler() noexcept = default;

    /**
     * @copydoc Arinc615a::Host::UploadOperationHandler::initialisationDeferred
     *
     * @param[in] targetId
     *   Target ID of the Target Hardware.
     **/
    virtual void initialisationDeferred( const TargetId &targetId, std::chrono::seconds waitTime ) = 0;

    /**
     * @copydoc Arinc615a::Host::UploadOperationHandler::initialisationResponse
     *
     * @param[in] targetId
     *   Target ID of the Target Hardware.
     **/
    virtual void
    initialisationResponse( const TargetId &targetId, const Information::InitializationResponse &response ) = 0;

    /**
     * @copydoc Arinc615a::Host::UploadOperationHandler::finished
     *
     * @param[in] targetId
     *   Target ID of the Target Hardware.
     **/
    virtual void finished( const TargetId &targetId, StatusCode code, std::string_view description ) = 0;

    /**
     * @copydoc Arinc615a::Host::UploadOperationHandler::status
     *
     * @param[in] targetId
     *   Target ID of the Target Hardware.
     **/
    virtual void status( const TargetId &targetId, const Information::UploadStatus &status ) = 0;

    /**
     * @copydoc Arinc615a::Host::UploadOperationHandler::fileRequest
     *
     * @param[in] targetId
     *   Target ID of the Target Hardware.
     **/
    virtual void fileRequest(
      const TargetId &targetId,
      const boost::asio::ip::udp::endpoint &remote,
      std::string_view filename,
      const ::Tftp::Packets::TftpOptions &clientTftpOptions,
      std::string_view loadPartNumber,
      const Arinc649::CheckValue &checkValue ) = 0;
};

}

#endif
