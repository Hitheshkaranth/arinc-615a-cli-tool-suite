// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Host::ProtocolImpl.
 **/

#ifndef ARINC_615A_HOST_PROTOCOLIMPL_HPP
#define ARINC_615A_HOST_PROTOCOLIMPL_HPP

#include <arinc_615a/host/Protocol.hpp>

#include <tftp/packets/Packets.hpp>

#include <boost/asio/io_context.hpp>

namespace Arinc615a::Host {

/**
 * @brief Implementation of the Host Protocol with internal helpers.
 **/
class ProtocolImpl final : public Protocol
{
  public:
    /**
     * @brief Initialises the internal host protocol.
     *
     * @param[in] ioContext
     *   I/O Context used for Communication.
     **/
    explicit ProtocolImpl( boost::asio::io_context &ioContext );

    //! Destructor
    ~ProtocolImpl() noexcept override;

    //! @copydoc Protocol::informationOperation
    InformationOperationPtr informationOperation( InformationOperationConfiguration configuration ) override;

    //! @copydoc Protocol::uploadOperation
    UploadOperationPtr uploadOperation( UploadOperationConfiguration configuration ) override;

    //! @copydoc Protocol::mediaDefinedDownloadOperation
    MediaDefinedDownloadOperationPtr mediaDefinedDownloadOperation(
      MediaDefinedDownloadOperationConfiguration configuration ) override;

    //! @copydoc Protocol::operatorDefinedDownloadOperation
    OperatorDefinedDownloadOperationPtr operatorDefinedDownloadOperation(
      OperatorDefinedDownloadOperationConfiguration configuration ) override;

  private:
    //! I/O Context used for the Operations.
    boost::asio::io_context &ioContextV;
};

}

#endif
