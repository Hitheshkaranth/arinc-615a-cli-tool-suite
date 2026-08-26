// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::StatusCodeDescription.
 **/

#ifndef ARINC_615A_STATUSCODEDESCRIPTION_HPP
#define ARINC_615A_STATUSCODEDESCRIPTION_HPP

#include <arinc_615a/Arinc615a.hpp>

#include <helper/Description.hpp>

#include <format>
#include <iosfwd>

namespace Arinc615a {

/**
 * @name ARINC 615A Status Code
 * @{
 **/

/**
 * @brief ARINC 615A Status Code Description.
 *
 * @sa @ref StatusCode
 **/
class ARINC_615A_EXPORT StatusCodeDescription final : public Helper::Description< StatusCodeDescription, StatusCode >
{
  public:
    //! Initialises Instance
    StatusCodeDescription();
};

/**
 * @brief Streaming Operator to Output Status Code as String.
 *
 * The Output format is:
 * `{String Representation} ({Hex Code})`
 *
 * @param[in,out] stream
 *   Output Stream.
 * @param[in] status
 *   Status Code.
 *
 * @return Stream for Chaining.
 *
 * @sa @ref StatusCodeDescription
 * @sa @ref StatusCode
 **/
ARINC_615A_EXPORT std::ostream& operator<<( std::ostream &stream, StatusCode status );

/**
 * @brief Streaming Operator to Output Operation Acceptance Status Code as String.
 *
 * The Output format is:
 * `{String Representation} ({Hex Code})`
 *
 * @param[in,out] stream
 *   Output Stream.
 * @param[in] status
 *   Status Code.
 *
 * @return Stream for Chaining.
 *
 * @sa @ref StatusCodeDescription
 * @sa @ref OperationAcceptanceStatusCode
 **/
ARINC_615A_EXPORT std::ostream& operator<<( std::ostream &stream, OperationAcceptanceStatusCode status );

/** @} **/

}

namespace std {

/**
 * @brief Specialisation of @p std::formatter for @ref Arinc615a::StatusCode.
 *
 * @sa @ref Arinc615a::StatusCodeDescription
 * @sa @ref Arinc615a::StatusCode
 **/
template <>
struct formatter< Arinc615a::StatusCode > : std::formatter< std::string_view >
{
  /**
   * @brief Arinc615a::StatusCode format routine.
   *
   * @tparam FmtContext
   *   Formatting Context
   * @param[in] code
   *   Status Code
   * @param[in,out] ctx
   *   Formatting Context
   *
   * @return Iterator to the end of output.
   **/
  template< class FmtContext >
  FmtContext::iterator format( const Arinc615a::StatusCode code, FmtContext &ctx ) const
  {
    return std::formatter< string_view >::format(
      std::format(
        "{} ({:04X})",
        Arinc615a::StatusCodeDescription::instance().name( code ),
        static_cast< uint16_t >( code ) ),
      ctx );
  }
};

/**
 * @brief Specialisation of @p std::formatter for @ref Arinc615a::OperationAcceptanceStatusCode
 *
 * @sa @ref Arinc615a::StatusCodeDescription
 * @sa @ref Arinc615a::OperationAcceptanceStatusCode
 **/
template <>
struct formatter< Arinc615a::OperationAcceptanceStatusCode > : std::formatter< std::string_view >
{
  /**
   * @brief Arinc615a::OperationAcceptanceStatusCode format routine.
   *
   * @tparam FmtContext
   *   Formatting Context
   * @param[in] code
   *   Operation Acceptance Status Code
   * @param[in,out] ctx
   *   Formatting Context
   *
   * @return Iterator to the end of output.
   **/
  template< class FmtContext >
  FmtContext::iterator format( const Arinc615a::OperationAcceptanceStatusCode code, FmtContext &ctx ) const
  {
    return std::formatter< string_view >::format(
      std::format(
        "{} ({:04X})",
        Arinc615a::StatusCodeDescription::instance().name( static_cast< Arinc615a::StatusCode >( code ) ),
        static_cast< uint16_t >( code ) ),
      ctx );
  }
};

}

#endif
