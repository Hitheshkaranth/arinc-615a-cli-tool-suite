// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Module Arinc615a::Files String.
 **/

#ifndef ARINC_615A_FILES_STRING_HPP
#define ARINC_615A_FILES_STRING_HPP

#include <arinc_615a/files/Files.hpp>

#include <helper/RawData.hpp>

#include <string_view>

namespace Arinc615a::Files {

/**
 * @name String Operations
 * @{
 **/

/**
 * @brief Operation to decode a String from the given Raw Data.
 *
 * Decodes the string according to the ARINC 615A protocol file specification.
 * The string is trimmed to the actual length of the string data.
 *
 * @param[in] rawData
 *   Raw data containing the string
 *
 * @return Tuple of remaining un-decoded data and decoded string.
 *
 * @throw Arinc615aException
 *   If the string length is invalid (length, null characters, ...)
 *
 * @sa @ref String_encode()
 **/
[[nodiscard]] ARINC_615A_EXPORT std::tuple< Helper::ConstRawDataSpan, std::string_view > String_decode(
  Helper::ConstRawDataSpan rawData );

/**
 * @brief Encodes the given String as Raw Data.
 *
 * Some implementations want to use fixed length raw string representation.
 * By setting @p fixedLength to `0 < fixedLength <= 255`, the raw data buffer is extended to @p fixedLength.
 * The @p str length must be smaller than @p fixedLength, as the string must be terminated with a @p NULL-character.
 *
 * @param[in] stringToEncode
 *   String, which shall be encoded (string length must be <255).
 * @param[in] fixedLength
 *   If set to `0 < fixedLength <= 255`, generate a raw string representation with a fixed length.
 *
 * @return Encoded string.
 *
 * @throw Arinc615aException
 *   If the string length is invalid
 *
 * @sa @ref String_decode()
 **/
[[nodiscard]] ARINC_615A_EXPORT Helper::RawData String_encode( std::string_view stringToEncode, uint8_t fixedLength = 0 );

/** @} **/

}

#endif
