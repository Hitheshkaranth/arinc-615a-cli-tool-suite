// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Tftp::Servers::WriteOperationImpl.
 **/

#ifndef ARINC_615A_TFTP_SERVERS_IMPLEMENTATION_WRITEOPERATIONIMPL_HPP
#define ARINC_615A_TFTP_SERVERS_IMPLEMENTATION_WRITEOPERATIONIMPL_HPP

#include <arinc_615a/tftp/servers/Servers.hpp>
#include <arinc_615a/tftp/servers/WriteOperation.hpp>

namespace Arinc615a::Tftp::Servers {

//! ARINC 615A TFTP %Server Write %Operation (TFTP WRQ).
class ARINC_615A_EXPORT WriteOperationImpl final : public WriteOperation
{
  public:
    /**
     * @brief Creates ARINC 615A TFTP Server Write Operation.
     *
     * @param[in] operation
     *   Base TFTP Server Write Operation.
     **/
    explicit WriteOperationImpl( ::Tftp::Servers::WriteOperationPtr operation );

    //! Destructor.
    ~WriteOperationImpl() override = default;

    //! @copydoc WriteOperation::tftpTimeout()
    WriteOperation& tftpTimeout( std::chrono::seconds timeout ) override;

    //! @copydoc WriteOperation::tftpRetries()
    WriteOperation& tftpRetries( uint16_t retries ) override;

    //! @copydoc WriteOperation::dally()
    WriteOperation& dally( bool dally ) override;

    //! @copydoc WriteOperation::optionsConfiguration()
    WriteOperation &optionsConfiguration( ::Tftp::TftpOptionsConfiguration optionsConfiguration ) override;

    //! @copydoc WriteOperation::completionHandler()
    WriteOperation &completionHandler( OperationCompletedHandler handler ) override;

    //! @copydoc WriteOperation::dataHandler()
    WriteOperation& dataHandler( ReceiveDataHandlerPtr handler ) override;

    //! @copydoc WriteOperation::remote()
    WriteOperation& remote( boost::asio::ip::udp::endpoint remote ) override;

    //! @copydoc WriteOperation::local()
    WriteOperation& local( boost::asio::ip::address local ) override;

    //! @copydoc WriteOperation::clientOptions()
    WriteOperation &clientOptions( ::Tftp::Packets::TftpOptions clientOptions ) override;

    //! @copydoc WriteOperation::negotiatedArinc615aOptions()
    WriteOperation &negotiatedArinc615aOptions( Arinc615aOptions options ) override;

    //! @copydoc WriteOperation::start()
    void start() override;

    //! @copydoc WriteOperation::gracefulAbort()
    void gracefulAbort( ::Tftp::Packets::ErrorCode errorCode, std::string errorMessage = {} ) override;

    //! @copydoc WriteOperation::abort()
    void abort() override;

    //! @copydoc WriteOperation::errorInformation() const
    [[nodiscard]] const ::Tftp::Packets::ErrorInformation& errorInformation() const override;

  private:
    //! Base TFTP Server Write Operation.
    ::Tftp::Servers::WriteOperationPtr operationV;
};

}

#endif
