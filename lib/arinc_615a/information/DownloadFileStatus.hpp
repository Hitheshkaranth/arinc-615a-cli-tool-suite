// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Information::DownloadFileStatus.
 **/

#ifndef ARINC_615A_INFORMATION_DOWNLOADFILESTATUS_HPP
#define ARINC_615A_INFORMATION_DOWNLOADFILESTATUS_HPP

#include <arinc_615a/information/Information.hpp>

#include <string>
#include <string_view>

namespace Arinc615a::Information {

/**
 * @brief Download %File %Status %Information.
 *
 * This class provides information per *Download %File* during *Operator* or *Media Defined Download Operation* within
 * the status transmission.
 * This information is repeated for every downloaded file.
 *
 * @ref Arinc615a::Information::DownloadStatus
 **/
class ARINC_615A_EXPORT DownloadFileStatus final
{
  public:
    //! Default Initialisation of Download File Status.
    DownloadFileStatus() = default;

    /**
     * @brief Initialises the Download File Status only with Filename.
     *
     * All other members are default initialised.
     *
     * @param[in] filename
     *   Download Filename.
     **/
    explicit DownloadFileStatus( std::string filename );

    /**
     * @brief Initialises the Download File Status.
     *
     * @param[in] filename
     *   Download Filename.
     * @param[in] code
     *   Download File Status Code.
     * @param[in] description
     *   Download File Status Description.
     **/
    DownloadFileStatus( std::string filename, StatusCode code, std::string description );

    /**
     * @name Filename
     *
     * @{
     **/

    /**
     * @brief Returns the Download Filename.
     *
     * @return Download Filename.
     **/
    [[nodiscard]] std::string_view filename() const noexcept;

    /**
     * @brief Updates the Download Filename.
     *
     * @param[in] filename
     *   Download Filename.
     **/
    void filename( std::string filename );

    /** @} **/

    /**
     * @name File Status for each File
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
     * Automatically resets the code description to empty string.
     *
     * @param[in] code
     *   Status Code.
     **/
    void code( StatusCode code );

    /** @} **/

    /**
     * @name Download File Status Description.
     *
     * Description of the reason for the Status Code.
     * Used only for the Status Codes _Operation In Progress with Additional Info_, _Operation Aborted by Target HW_ and
     * _Load Part Number or Download File Failed_.
     * For other File status codes, the content of this field is ignored but may contain data (i.e. leftover data).
     *
     * @{
     **/

    /**
     * @brief Returns the Status Description.
     *
     * @return Status Description.
     **/
    [[nodiscard]] std::string_view description() const noexcept;

    /**
     * @brief Updates the Status Description.
     *
     * @param[in] description
     *   Status Description.
     **/
    void description( std::string description );

    /** @} **/

    /**
     * @brief Combined Version of @ref code() and @ref description().
     *
     * @param[in] code
     *   Status Code
     * @param[in] description
     *   Status Description
     **/
    void set( StatusCode code, std::string description = {} );

  private:
    //! Download Filename.
    std::string filenameV;
    //! Download File Status Code.
    StatusCode codeV{ StatusCode::Invalid };
    //! Download File Status Description.
    std::string descriptionV;
};

}

#endif
