/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Host::BatchUploadOperationProxy.
 **/

#ifndef ARINC_615A_HOST_BATCHUPLOADOPERATIONPROXY_HPP
#define ARINC_615A_HOST_BATCHUPLOADOPERATIONPROXY_HPP

#include <arinc_615a/host/Host.hpp>
#include <arinc_615a/host/UploadOperationHandler.hpp>

#include <arinc_615a/TargetId.hpp>

namespace Arinc615a::Host {

/**
 * @brief Batch Upload %Operation Proxy.
 *
 * This proxy is used to forward callbacks to the Batch Upload Operation Handler.
 * It adds the Target ID to the callbacks.
 *
 * @sa @ref Arinc615a::Host::UploadOperation
 * @sa @ref Arinc615a::Host::BatchUploadOperationHandler
 **/
class ARINC_615A_EXPORT BatchUploadOperationProxy final : public UploadOperationHandler
{
  public:
    /**
     * @brief Constructs Batch Upload Operation Proxy
     *
     * @param[in] targetId
     *   Target ID used for callbacks
     * @param[in] handler
     *   Batch Upload Operation Handler.
     **/
    BatchUploadOperationProxy( TargetId targetId, BatchUploadOperationHandler &handler );

    /**
     * @copydoc UploadOperationHandler::initialisationDeferred
     **/
    void initialisationDeferred( std::chrono::seconds waitTime ) override;

    /**
     * @copydoc UploadOperationHandler::initialisationResponse
     **/
    void initialisationResponse( const Information::InitializationResponse &response ) override;

    /**
     * @copydoc UploadOperationHandler::finished
     **/
    void finished( StatusCode code, std::string_view description ) override;

    /**
     * @copydoc UploadOperationHandler::status
     **/
    void status( const Information::UploadStatus &status ) override;

    /**
     * @copydoc UploadOperationHandler::fileRequest
     **/
    void fileRequest(
      const boost::asio::ip::udp::endpoint &remote,
      std::string_view filename,
      const ::Tftp::Packets::TftpOptions &clientTftpOptions,
      std::string_view loadPartNumber,
      const Arinc649::CheckValue &checkValue ) override;

  private:
    //! Target ID to be used for callbacks.
    TargetId targetIdV;
    //! Batch Upload Callback handler.
    BatchUploadOperationHandler &handlerV;
};

}

#endif
