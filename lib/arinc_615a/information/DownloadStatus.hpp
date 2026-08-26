// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Information::DownloadStatus.
 **/

#ifndef ARINC_615A_INFORMATION_DOWNLOADSTATUS_HPP
#define ARINC_615A_INFORMATION_DOWNLOADSTATUS_HPP

#include <arinc_615a/information/Information.hpp>

#include <arinc_615a/information/DownloadFileStatus.hpp>
#include <arinc_615a/information/Status.hpp>
#include <arinc_615a/information/Ratio.hpp>

namespace Arinc615a::Information {

/**
 * @brief Download Operation %Status %Information.
 *
 * The information is the same for the _Media Defined_ and _Operator Defined Download Operation_.
 **/
class ARINC_615A_EXPORT DownloadStatus final : public Status
{
  public:
    //! Default Initialisation of Download Status.
    DownloadStatus() = default;

    /**
     * @brief Creates the Download Status Information.
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
     *   Completion ratio for the downloading operation.
     *   Valid range is [0..100] -> will be set to range boundary if exceed limits.
     * @param[in] filesStatus
     *   Download files status list.
     *   Creates per default an empty list.
     **/
    DownloadStatus(
      uint16_t counter,
      uint16_t exceptionTimer,
      int16_t estimatedTime,
      StatusCode code,
      std::string description,
      Ratio listRatio,
      DownloadFilesStatus filesStatus = {} );

    /**
     * @name Download List Ratio
     *
     * Percentage of the Download completed.
     *
     * @{
     **/

    /**
     * @brief Returns the download list ration.
     *
     * @return The download list ratio [0..100] in per cent.
     **/
    [[nodiscard]] const Ratio& listRatio() const noexcept;

    //! @copydoc listRatio() const
    [[nodiscard]] Ratio& listRatio() noexcept;

    /**
     * @brief Updates the Download List Ratio.
     *
     * @param[in] listRatio
     *   The completion ratio for the downloading operation.
     *   Valid range is [0..100] -> will be set to range boundary if exceed limits.
     **/
    void listRatio( Ratio listRatio );

    /** @} **/

    /**
     * @name File Status
     *
     * @sa @ref DownloadFileStatus
     *
     * @{
     **/

    /**
     * @brief Returns the Files Status Information.
     *
     * @return Files Status information.
     **/
    [[nodiscard]] const DownloadFilesStatus& files() const noexcept;

    //! @copydoc files() const
    [[nodiscard]] DownloadFilesStatus& files() noexcept;

    /**
     * @brief Updates the Files Status Information.
     *
     * @param[in] filesStatus
     *   Files Status Information.
     **/
    void files( DownloadFilesStatus filesStatus );

    /**
     * @brief Adds Download File Status.
     *
     * @param[in] filename
     *   Download Filename.
     *
     * @return Created Download File Status.
     **/
    DownloadFileStatus& file( std::string filename );

    /**
     * @brief Returns the Download File Status.
     *
     * @param[in] filename
     *   Download Filename.
     *
     * @return Download File Status.
     **/
    DownloadFileStatus& file( std::string_view filename );

    /** @} **/

  private:
    //! Download List ratio between [0,100]
    Ratio listRatioV;
    //! Files Status Information.
    DownloadFilesStatus filesStatusV;
};

}

#endif
