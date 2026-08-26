// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Tftp::Servers::ReadOperationImpl.
 **/

#ifndef ARINC_615A_TFTP_SERVERS_IMPLEMENTATION_READOPERATIONIMPL_HPP
#define ARINC_615A_TFTP_SERVERS_IMPLEMENTATION_READOPERATIONIMPL_HPP

#include <arinc_615a/tftp/servers/Servers.hpp>
#include <arinc_615a/tftp/servers/ReadOperation.hpp>

namespace Arinc615a::Tftp::Servers {

//! ARINC 615A TFTP %Server Read %Operation.
class ARINC_615A_EXPORT ReadOperationImpl final : public ReadOperation
{
  public:
    /**
     * @brief Creates ARINC 615A TFTP Server Read Operation.
     *
     * @param[in] operation
     *   Base TFTP Server Read Operation.
     **/
    explicit ReadOperationImpl( ::Tftp::Servers::ReadOperationPtr operation );

    //! Destructor.
    ~ReadOperationImpl() override = default;

    //! @copydoc ReadOperation::tftpTimeout()
    ReadOperation& tftpTimeout( std::chrono::seconds timeout ) override;

    //! @copydoc ReadOperation::tftpRetries()
    ReadOperation& tftpRetries( uint16_t retries ) override;

    //! @copydoc ReadOperation::optionsConfiguration()
    ReadOperation &optionsConfiguration( ::Tftp::TftpOptionsConfiguration optionsConfiguration ) override;

    //! @copydoc ReadOperation::completionHandler()
    ReadOperation &completionHandler( OperationCompletedHandler handler ) override;

    //! @copydoc ReadOperation::dataHandler()
    ReadOperation& dataHandler( TransmitDataHandlerPtr handler ) override;

    //! @copydoc ReadOperation::remote()
    ReadOperation& remote( boost::asio::ip::udp::endpoint remote ) override;

    //! @copydoc ReadOperation::local()
    ReadOperation& local( boost::asio::ip::address local ) override;

    //! @copydoc ReadOperation::clientOptions()
    ReadOperation &clientOptions( ::Tftp::Packets::TftpOptions clientOptions ) override;

    //! @copydoc ReadOperation::negotiatedArinc615aOptions()
    ReadOperation &negotiatedArinc615aOptions( Arinc615aOptions options ) override;

    //! @copydoc ReadOperation::start()
    void start() override;

    //! @copydoc ReadOperation::gracefulAbort()
    void gracefulAbort( ::Tftp::Packets::ErrorCode errorCode, std::string errorMessage = {} ) override;

    //! @copydoc ReadOperation::abort()
    void abort() override;

    //! @copydoc ReadOperation::errorInformation() const
    [[nodiscard]] const ::Tftp::Packets::ErrorInformation& errorInformation() const override;

  private:
    //! Base TFTP Server Read Operation.
    ::Tftp::Servers::ReadOperationPtr operationV;
};

}

#endif
