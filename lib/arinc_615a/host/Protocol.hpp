// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Host::Protocol.
 **/

#ifndef ARINC_615A_HOST_PROTOCOL_HPP
#define ARINC_615A_HOST_PROTOCOL_HPP

#include <arinc_615a/host/Host.hpp>

#include <boost/asio/ip/udp.hpp>

namespace Arinc615a::Host {

/**
 * @brief ARINC 615A %Host Data Loader %Protocol.
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
     * @brief Creates the %Host Data Loader Protocol Instance.
     *
     * @param[in] ioContext
     *   I/O context used for Communication.
     *
     * @return Host data loader instance
     **/
    [[nodiscard]] static ProtocolPtr instance( boost::asio::io_context &ioContext );

    //! Destructor.
    virtual ~Protocol() noexcept = default;

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
