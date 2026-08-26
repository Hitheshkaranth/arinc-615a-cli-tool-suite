// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Information::Status.
 **/

#ifndef ARINC_615A_INFORMATION_STATUS_HPP
#define ARINC_615A_INFORMATION_STATUS_HPP

#include <arinc_615a/information/Information.hpp>

#include <string>

namespace Arinc615a::Information {

/**
 * @brief Base class of all ARINC 615A status information.
 **/
class ARINC_615A_EXPORT Status
{
  public:
    /**
     * @brief Creates Default Status Information
     **/
    Status() = default;

    /**
     * @brief Creates the status information.
     *
     * @param[in] counter
     *   Counter Value.
     * @param[in] exceptionTimer
     *   Exception Timer.
     * @param[in] estimatedTime
     *   Estimated Time.
     * @param[in] code
     *   Status Code.
     * @param[in] description
     *   Status Description.
     **/
    Status(
      uint16_t counter,
      uint16_t exceptionTimer,
      int16_t estimatedTime,
      StatusCode code,
      std::string description = {} );

    /**
     * @name Status Counter
     *
     * @{
     **/

    /**
     * @brief Returns the Status Counter Value.
     *
     * @return The Status Counter Value.
     **/
    [[nodiscard]] uint16_t counter() const noexcept;

    /**
     * @brief Updates the Status Counter Value.
     *
     * @param[in] counter
     *   The new status counter.
     **/
    void counter( uint16_t counter ) noexcept;

    /**
     * @brief Increments the Status Counter Value.
     **/
    void incrementCounter() noexcept;

    /** @} **/

    /**
     * @name Exception Timer
     *
     * Number of seconds during which the target is expected to be silent.
     * During this delay, the code file is not sent by the target, and it does not respond to any request from the data
     * loader (0 to 2^16 –1 seconds).
     *
     * This field is used only if the Status Code field is _Operation in Progress_ or _Operation in Progress with
     * Additional Info_, for all other code codes, this field must be set to `0x0000`.
     * @{
     **/

    /**
     * @brief Returns the Exception Timer Value.
     *
     * @return The exception timer value.
     **/
    [[nodiscard]] uint16_t exceptionTimer() const noexcept;

    /**
     * @brief Updates the Exception Timer Value.
     *
     * @param[in] exceptionTimer
     *   Exception Timer Value.
     **/
    void exceptionTimer( uint16_t exceptionTimer );

    /** @} **/

    /**
     * @name Estimated Time
     *
     * Binary, estimated number of seconds to complete the operation (-1 to 2^15 -1 seconds).
     * The value “–1” means the Estimated Time is not given.
     * Estimated Time is the time remaining to complete the operation as computed by the target hardware.
     * As soon as possible during the operation, the Target HW should provide the Estimated Time.
     *
     * This field is used only if the Status Code field is  _Operation in Progress_ or _Operation in Progress with
     * Additional Info_.
     * For all other Status Codes, this field is set equal to `0x0000`.
     *
     * If the target hardware does not give this value, the Estimated Time field must be set to `-1`.
     *
     * @{
     **/

    /**
     * @brief Returns the Estimated Time.
     *
     * @return Estimated Time Value.
     **/
    [[nodiscard]] int16_t estimatedTime() const noexcept;

    /**
     * @brief Updates the Estimated Time.
     *
     * @param[in] estimatedTime
     *   Estimated Time Value.
     **/
    void estimatedTime( int16_t estimatedTime );

    /** @} **/

    /**
     * @name Status Code
     *
     * @{
     **/

    /**
     * @brief Returns the Status Code.
     *
     * @return Status Code.
     **/
    [[nodiscard]] StatusCode code() const noexcept;

    /**
     * @brief Updates the Status Code.
     *
     * Automatically resets the status description to empty string.
     *
     * @param[in] code
     *   Status Code Value.
     **/
    void code( StatusCode code );

    /** @} **/

    /**
     * @name Status Description.
     *
     * Provides a description of the reason or additional information for the Status Code.
     * Used for Status Code _Operation Aborted by Target HW_ or _Operation in Progress with Additional Info_.
     * For other Status Codes, the content of this field is ignored but may contain data (e.g., left over data).
     *
     * @{
     **/

    /**
     * @brief Returns the Status Description Value.
     *
     * @return Status Description Value.
     **/
    [[nodiscard]] std::string_view description() const noexcept;

    /**
     * @brief Updates the Status Description Value.
     *
     * @param[in] description
     *   Status Description Value.
     **/
    void description( std::string description );

    /** @} **/

    /**
     * @brief Combined version of code() and description()
     *
     * @param[in] code
     *   Status code to set
     * @param[in] description
     *   Description of status to set
     **/
    void set( StatusCode code, std::string description = {} );

  private:
    //! Status Transmission Counter
    uint16_t counterV{ 0U };
    //! Exception Timer
    uint16_t exceptionTimerV{ 0U };
    //! Estimated Time
    int16_t estimatedTimeV{ 0 };
    //! Status Code
    StatusCode codeV{ StatusCode::Invalid };
    //! Status Description
    std::string descriptionV;
};

}

#endif
