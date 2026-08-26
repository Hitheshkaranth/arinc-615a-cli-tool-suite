// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Module Arinc615a::Tftp ErrorMessage.
 **/

#ifndef ARINC_615A_TFTP_ERRORMESSAGE_HPP
#define ARINC_615A_TFTP_ERRORMESSAGE_HPP

#include <arinc_615a/tftp/Tftp.hpp>

#include <tftp/packets/Packets.hpp>

#include <chrono>
#include <optional>
#include <string>
#include <string_view>

namespace Arinc615a::Tftp {

/**
 * @brief Determines the Message Type of the given TFTP Error Information.
 *
 * @param[in] errorInformation
 *   Error Information.
 *
 * @return The packet type.
 * @retval Arinc615aTftpPacketType::Invalid
 *   If the TFTP packet is not a valid ARINC 615A error packet.
 **/
[[nodiscard]] ARINC_615A_EXPORT ErrorMessageType ErrorMessage_type(
  const ::Tftp::Packets::ErrorInformation &errorInformation ) noexcept;

/**
 * @name ARINC 615A Abort Message
 *
 * This message is used to cancel the communication.
 *
 * To abort the process in progress, the Data Loader Protocol sends an abort message in response to a TFTP request from
 * the %Target Hardware.
 * The Abort message is only allowed after an WRQ of a status file.
 *
 * An abort message is a TFTP error packet with the error code set to `ERROR_CODE_NOT_DEFINED` and the error message set
 * to `"ABORT:[status code]"`
 *
 * @{
 **/

/**
 * @brief Return the Abort Message Error Message.
 *
 * The error message contains the abort status code.
 * The format is: `"ABORT:[status code]"`.
 *
 * @param[in] statusCode
 *   Abort Status Code.
 *
 * @return Error String
 **/
[[nodiscard]] ARINC_615A_EXPORT std::string ErrorMessage_abort( StatusCode statusCode );

/**
 * @brief Decodes the Abort Message Status Code from the Error Message.
 *
 * Also checks if the Error Message is a valid Abort Message.
 *
 * @param[in] errorMessage
 *   Error Message.
 *
 * @return The ARINC 615A abort status code.
 * @retval StatusCode::Invalid
 *   If the error message is not an Abort Message.
 **/
[[nodiscard]] ARINC_615A_EXPORT StatusCode ErrorMessage_abort( std::string_view errorMessage ) noexcept;

/** @} **/

/**
 * @name ARINC 615A Wait Message
 *
 * This message is used to pause the communication for a defined time.
 *
 * A wait message is a TFTP error packet with the error code set to `ERROR_CODE_NOT_DEFINED` and the error message set
 * to `"WAIT:[wait time]"`.
 *
 * @{
 **/

/**
 * @brief Return the Wait Message Error Message.
 *
 * The error message contains the wait time.
 * The format is: `"WAIT:[wait time]"`.
 *
 * @param[in] waitTime
 *   Wait time in seconds.
 *
 * @return Corresponding error message.
 **/
[[nodiscard]] ARINC_615A_EXPORT std::string ErrorMessage_wait( std::chrono::seconds waitTime );

/**
 * @brief Try to decode Error Message as Wait Message and Returns Wait Time.
 *
 * @param[in] errorMessage
 *   Error Message.
 *
 * @return Decoded Wait Time.
 * @retval {}
 *   If Wait Time cannot be decoded
 **/
[[nodiscard]] ARINC_615A_EXPORT std::optional< std::chrono::seconds > ErrorMessage_wait(
  std::string_view errorMessage ) noexcept;

/** @} **/

}

#endif
