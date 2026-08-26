// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Target::ErrorOperation.
 **/

#ifndef ARINC_615A_TARGET_ERROROPERATION_HPP
#define ARINC_615A_TARGET_ERROROPERATION_HPP

#include <arinc_615a/target/Target.hpp>

namespace Arinc615a::Target {

/**
 * @brief ARINC 615A %Target Error %Operation.
 *
 * Send the Initialisation file with the Error-Status Code.
 *
 * @note
 * Because the initialisation file is sent asynchronously, the error operation is implemented as object to hold the
 * instance until completion.
 * Otherwise, the operation would need to be implemented synchronously.
 */
class ARINC_615A_EXPORT ErrorOperation
{
  public:
    //! Destructor.
    virtual ~ErrorOperation() noexcept = default;

    /**
     * @brief Executes Error Operation
     *
     * @note
     * Even if the *Port Option* is not actively used, i.e. no further file requests are performed, the received option
     * value is transmitted back by the TFTP Option negotiation (when not discarded by the new operation handler).
     *
     * @param[in] hostAddress
     *   Host Address.
     * @param[in] clientTftpOptions
     *   Received TFTP Options.
     * @param[in] port
     *   Received ARINC 615A Port Option.
     *   See Note.
     **/
    virtual void start(
      boost::asio::ip::udp::endpoint hostAddress,
      ::Tftp::Packets::TftpOptions clientTftpOptions,
      std::optional< uint16_t > port ) = 0;
};

}

#endif
