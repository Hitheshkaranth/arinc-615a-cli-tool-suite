// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Information::InitializationResponse.
 **/

#ifndef ARINC_615A_INFORMATION_INITIALIZATIONINFORMATION_HPP
#define ARINC_615A_INFORMATION_INITIALIZATIONINFORMATION_HPP

#include <arinc_615a/information/Information.hpp>

#include <string>
#include <string_view>

namespace Arinc615a::Information {

/**
 * @brief Initialisation information, which is transferred from the target back to the host.
 *
 * The information is the same for Information, Media/ Operator Defined Download, and Upload Operation.
 **/
class ARINC_615A_EXPORT InitializationResponse final
{
  public:
    //! Default initialisation
    InitializationResponse() = default;

    /**
     * @brief Constructs initialisation information from the given data.
     *
     * Per specification, the parameter @p description should only be set when the parameter @p code is set to
     * _OperationDenied_.
     *
     * @param[in] code
     *   Operation Acceptance Status Code.
     * @param[in] description
     *   Status Description.
     *   The description is empty by default.
     **/
    explicit InitializationResponse( OperationAcceptanceStatusCode code, std::string description = {} );

    /**
     * @name Operation Acceptance Status Code
     *
     * @{
     **/

    /**
     * @brief Returns the operation acceptance status code.
     *
     * @return The operation acceptance status code.
     **/
    [[nodiscard]] OperationAcceptanceStatusCode code() const noexcept;

    /**
     * @brief Sets the operation acceptance code to the given value.
     *
     * @param[in] code
     *   Operation Acceptance Code.
     **/
    void code( OperationAcceptanceStatusCode code ) noexcept;

    /** @} **/

    /**
     * @name Status Description.
     *
     * Provides a description of the reason or additional information for this status code, it is used only for status
     * code _Operation not Supported_.
     * For Status Codes _Operation Accepted_ and _Operation Denied_ the content of this field is ignored.
     *
     * @{
     **/

    /**
     * @brief Returns the Status Description.
     *
     * @return Status Description.
     **/
    [[nodiscard]] std::string_view description() const  noexcept;

    /**
     * @brief Set the Status Description to the give value.
     *
     * @todo check status and description together.
     *
     * @param[in] description
     *   New Status Description.
     **/
    void description( std::string description );

    /** @} **/

    /**
     * @brief Validates the information.
     *
     * @throw Arinc615aException
     *   When @p descriptionV is not empty when @p statusV is set to accept.
     **/
    void validate() const;

    /**
     * @brief Compares @p this with @p other for equality.
     *
     * @param[in] other
     *   Other Initialisation Response.
     *
     * @return If @p this is equal to @p other
     **/
    bool operator ==( const InitializationResponse &other ) const;

  private:
    //! Status Code
    OperationAcceptanceStatusCode codeV{ OperationAcceptanceStatusCode::OperationAccepted };
    //! Status Message
    std::string descriptionV;
};

}

#endif
