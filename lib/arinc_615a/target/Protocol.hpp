// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Target::Protocol.
 **/

#ifndef ARINC_615A_TARGET_PROTOCOL_HPP
#define ARINC_615A_TARGET_PROTOCOL_HPP

#include <arinc_615a/target/Target.hpp>

#include <boost/asio/io_context.hpp>

namespace Arinc615a::Target {

/**
 * @brief ARINC 615A %Target Data Loader %Protocol.
 *
 * The _Target Protocol_ is responsible for handling ARINC 615A Operation requests.
 * It startes a TFTP Server listening on the ARINC 615A Well-known Port.
 * TFTP Requests are analysed, and if an Initialisation file request has been received, this request is forwarded to the
 * THA.
 * Additionally, file requests are forwarded to an active ARINC 615A Operation, if any.
 *
 * The concrete instance is created by calling the static operation @ref Protocol::instance().
 *
 * @sa @ref InformationOperation
 * @sa @ref UploadOperation
 * @sa @ref MediaDefinedDownloadOperation
 * @sa @ref OperatorDefinedDownloadOperation
 **/
class ARINC_615A_EXPORT Protocol
{
  public:
    /**
     * @brief Creates the %Target Data Loader Protocol Instance.
     *
     * @param[in] ioContext
     *   I/O context used for Communication.
     * @param[in] configuration
     *   Target Protocol Configuration.
     *
     * @return Instance of the Target Data Loader.
     **/
    [[nodiscard]] static ProtocolPtr instance(
      boost::asio::io_context &ioContext,
      ProtocolConfiguration configuration );

    //! Destructor.
    virtual ~Protocol() noexcept = default;

    /**
     * @brief Starts the Target Data Loader Protocol Handler.
     *
     * After this call, the Target Data Loader listens for ARINC 615A requests and handles them.
     *
     * @note
     * Requests are handled asynchronously in the I/O Context provided during instantiation of the Protocol Handler.
     **/
    virtual void start() = 0;

    /**
     * @brief Stops the Target Data Loader Protocol Handler.
     *
     * After this call, the Target Data Loader cannot handle any new ARINC 615A request.
     *
     * @important
     * Already running operations are *not* cancelled and must be aborted by the caller.
     **/
    virtual void stop() = 0;

    /**
     * @brief Creates an Error Operation to deny a ARINC 615A Operation Request.
     *
     * Sends an initialisation response file with the appropriate content.
     * Valid error codes are _NOT SUPPORTED_ or _DENIED_.
     *
     * @param[in] configuration
     *   Error Operation Configuration.
     *
     * @return Error operation, which can be executed.
     **/
    [[nodiscard]] virtual ErrorOperationPtr errorOperation( ErrorOperationConfiguration configuration ) = 0;

    /**
     * @brief Creates an ARINC 615A Information Operation.
     *
     * @param[in] configuration
     *   Operation Configuration.
     *
     * @return ARINC 615A Information Operation Instance.
     **/
    [[nodiscard]] virtual InformationOperationPtr informationOperation(
      InformationOperationConfiguration configuration ) = 0;

    /**
     * @brief Creates an ARINC 615A Upload Operation.
     *
     * @param[in] configuration
     *   Operation Configuration.
     *
     * @return ARINC 615A Upload Operation Instance.
     **/
    [[nodiscard]] virtual UploadOperationPtr uploadOperation( UploadOperationConfiguration configuration ) = 0;

    /**
     * @brief Creates an ARINC 615A Media Defined Download Operation.
     *
     * @param[in] configuration
     *   Operation Configuration.
     *
     * @return ARINC 615A Media Defined Download Operation Instance.
     **/
    [[nodiscard]] virtual MediaDefinedDownloadOperationPtr mediaDefinedDownloadOperation(
      MediaDefinedDownloadOperationConfiguration configuration ) = 0;

    /**
     * @brief Creates an ARINC 615A Operator Defined Download Operation.
     *
     * @param[in] configuration
     *   Operation Configuration.
     *
     * @return ARINC 615A Media Defined Download Operation Instance.
     **/
    [[nodiscard]] virtual OperatorDefinedDownloadOperationPtr operatorDefinedDownloadOperation(
      OperatorDefinedDownloadOperationConfiguration configuration ) = 0;
};

}

#endif
