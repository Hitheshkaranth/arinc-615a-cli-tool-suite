// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Arinc615aVersionDescription.
 **/

#ifndef ARINC_615A_ARINC615AVERSIONDESCRIPTION_HPP
#define ARINC_615A_ARINC615AVERSIONDESCRIPTION_HPP

#include <arinc_615a/Arinc615a.hpp>

#include <helper/Description.hpp>

#include <iosfwd>

namespace Arinc615a {

/**
 * @name ARINC 615A Version Description
 * @{
 **/

/**
 * @brief ARINC 615A %Version Description.
 *
 * @sa @ref Arinc615aVersion
 **/
class ARINC_615A_EXPORT Arinc615aVersionDescription final :
  public Helper::Description< Arinc615aVersionDescription, Arinc615aVersion >
{
  public:
    //! Initialises Instance
    Arinc615aVersionDescription();
};

/**
 * @brief Streaming Operator to Output ARINC 615A %Version as String.
 *
 * @param[in,out] stream
 *   Output stream
 * @param[in] version
 *   ARINC 615A version
 *
 * @return @p stream for chaining.
 *
 * @sa @ref Arinc615aVersion
 * @sa @ref Arinc615aVersionDescription
 **/
ARINC_615A_EXPORT std::ostream& operator<<( std::ostream &stream, Arinc615aVersion version );

/**
 * @brief Streaming Operator for Parsing an ARINC 615A %Version String as ARINC 615A %Version Type.
 *
 * @param[in,out] stream
 *   Input stream
 * @param[out] version
 *   Decoded operation
 *
 * @return @p stream for chaining.
 *
 * @sa @ref Arinc615aVersion
 * @sa @ref Arinc615aVersionDescription
 **/
ARINC_615A_EXPORT std::istream& operator>>( std::istream &stream, Arinc615aVersion &version );

/** @} **/

}

#endif
