// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Information::UploadLoadStatus.
 **/

#ifndef ARINC_615A_INFORMATION_UPLOADLOADSTATUS_HPP
#define ARINC_615A_INFORMATION_UPLOADLOADSTATUS_HPP

#include <arinc_615a/information/Information.hpp>
#include <arinc_615a/information/Ratio.hpp>

#include <string>
#include <string_view>

namespace Arinc615a::Information {

/**
 * @brief Upload Load %Status %Information.
 *
 * @ref Arinc615a::Information::UploadStatus
 **/
class ARINC_615A_EXPORT UploadLoadStatus final
{
  public:
    //! Default Initialisation of Upload Load Status.
    UploadLoadStatus() = default;

    /**
     * @brief Initialises the Upload Load Status.
     *
     * @param[in] headerFilename
     *   Filename of the load header.
     * @param[in] partNumber
     *   Load part number.
     **/
    UploadLoadStatus( std::string headerFilename, std::string partNumber );

    /**
     * @brief Initialises the Upload Load Status.
     *
     * @param[in] headerFilename
     *   Filename of the load header.
     * @param[in] partNumber
     *   Load part number.
     * @param[in] ratio
     *   Load ratio (0..100).
     * @param[in] code
     *   Load Status Code.
     * @param[in] description
     *   Load Status Description.
     **/
    UploadLoadStatus(
      std::string headerFilename,
      std::string partNumber,
      Ratio ratio,
      StatusCode code,
      std::string description = {} );

    /**
     * @name Header File Name
     *
     * @{
     **/

    /**
     * @brief Returns the Load Upload Header filename.
     *
     * @return Load Upload Header filename.
     **/
    [[nodiscard]] std::string_view headerFilename() const noexcept;

    /**
     * @brief Sets the Load Upload Header filename.
     *
     * @param[in] headerFilename
     *   New Load Upload Header filename.
     **/
    void headerFilename( std::string headerFilename );

    /** @} **/

    /**
     * @name Load Part Number
     *
     * @{
     **/

    /**
     * @brief Returns the Load Part Number.
     *
     * @return Load Part Number.
     **/
    [[nodiscard]] std::string_view partNumber() const noexcept;

    /**
     * @brief Updates the Load Part Number.
     *
     * @param[in] partNumber
     *   Load Part Number.
     **/
    void partNumber( std::string partNumber );

    /** @} **/

    /**
     * @name Load Ratio
     *
     * Percentage of the specific load completed.
     *
     * @{
     **/

    /**
     * @brief Returns the Load Ratio.
     *
     * @return Load Ratio.
     **/
    [[nodiscard]] const Ratio& ratio() const noexcept;

    //! @copydoc ratio() const
    [[nodiscard]] Ratio& ratio() noexcept;

    /**
     * @brief Updates the Ratio.
     *
     * @param[in] ratio
     *   Upload load ratio
     **/
    void ratio( Ratio ratio );

    /** @} **/

    /**
     * @name Load Status Code
     *
     * Status for each Header File, i.e. each software load being uploaded.
     * It gives the status of the load defined by the associated Header File (not only the status of the upload of the
     * Header File.)
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
     * @name Load Status Description.
     *
     * Provides a description of the reason for the Status Code.
     * Used only for the Status Codes _Operation In Progress with Additional Info_, _Operation Aborted by Target HW_ and
     * _Load Part Number or Download File Failed_.
     * For other status codes, the content of this field is ignored but may contain data (i.e. leftover data).
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
     * @brief Combined Version of @ref ratio(), @ref code() and @ref description()
     *
     * @param[in] ratio
     *   Upload load ratio
     * @param[in] code
     *   Status Code
     * @param[in] description
     *   Status Description
     **/
    void set( Ratio ratio, StatusCode code, std::string description = {} );

  private:
    //! Filename of the Load Header.
    std::string headerFilenameV;
    //! Load Part Number.
    std::string partNumberV;
    //! Load Ratio.
    Ratio ratioV;
    //! Load Status Code.
    StatusCode codeV{ StatusCode::Invalid };
    //! Load Status Description.
    std::string descriptionV;
};

}

#endif
