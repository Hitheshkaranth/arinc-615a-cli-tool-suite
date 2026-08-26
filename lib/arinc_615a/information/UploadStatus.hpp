// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Information::UploadStatus.
 **/

#ifndef ARINC_615A_INFORMATION_UPLOADSTATUSINFORMATION_HPP
#define ARINC_615A_INFORMATION_UPLOADSTATUSINFORMATION_HPP

#include <arinc_615a/information/Information.hpp>
#include <arinc_615a/information/Status.hpp>
#include <arinc_615a/information/UploadLoadStatus.hpp>
#include <arinc_615a/information/Ratio.hpp>

#include <list>
#include <string>

namespace Arinc615a::Information {

/**
 * @brief Upload Operation %Status %Information.
 **/
class ARINC_615A_EXPORT UploadStatus final : public Status
{
  public:
    //! Default Initialisation of Upload Status.
    UploadStatus() = default;

    /**
     * @brief Creates the Upload Status Information.
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
     * @param[in] listRatio
     *   Completion ratio for the uploading operation.
     * @param[in] loadsStatus
     *   Upload Loads Status list.
     **/
    UploadStatus(
      uint16_t counter,
      uint16_t exceptionTimer,
      int16_t estimatedTime,
      StatusCode code,
      std::string description,
      Ratio listRatio,
      UploadLoadsStatus loadsStatus = {} );

    /**
     * @name Load List Ratio
     *
     * Percentage of the Load List completed.
     *
     * @{
     **/

    /**
     * @brief Returns the Load List Ratio.
     *
     * @return Load List Ratio.
     **/
    [[nodiscard]] const Ratio& listRatio() const noexcept;

    //! @copydoc listRatio() const
    [[nodiscard]] Ratio& listRatio() noexcept;

    /**
     * @brief Updates the Load List ratio.
     *
     * @param[in] listRatio
     *   The completion ratio for the uploading operation.
     *   Valid range is [0..100] -> will be set to range boundary if exceed limits.
     **/
    void listRatio( Ratio listRatio );

    /** @} **/

    /**
     * @name Loads Status
     *
     * @sa @ref UploadLoadsStatus
     *
     * @{
     **/

    /**
     * @brief Returns the loads (list) status.
     *
     * @return Loads Status information.
     **/
    [[nodiscard]] const UploadLoadsStatus& loads() const noexcept;

    //! @copydoc loads() const
    [[nodiscard]] UploadLoadsStatus& loads() noexcept;

    /**
     * @brief Updates the loads (list) Status Information.
     *
     * @param[in] loads
     *   Loads Status Information.
     **/
    void loads( UploadLoadsStatus loads );

    /**
     * @brief Adds Load Status.
     *
     * @param[in] headerFilename
     *   Load Header Filename.
     * @param[in] partNumber
     *   Load Part Number.
     *
     * @return Created Load Status.
     **/
    [[nodiscard]] UploadLoadStatus& load( std::string headerFilename, std::string partNumber );

    /**
     * @brief Returns the Load Status.
     *
     * @param[in] headerFilename
     *   Load Header Filename.
     *
     * @return Load Status.
     **/
    [[nodiscard]] UploadLoadStatus& load( std::string_view headerFilename );

    /** @} **/

  private:
    //! Load List Ratio
    Ratio listRatioV;
    //! Upload load Status Information.
    UploadLoadsStatus loadsStatusV;
};

}

#endif
