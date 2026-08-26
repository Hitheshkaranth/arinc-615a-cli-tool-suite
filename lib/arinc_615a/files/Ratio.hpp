// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Module Arinc615a::Files Ratio.
 **/

#ifndef ARINC_615A_FILES_UTILS_HPP
#define ARINC_615A_FILES_UTILS_HPP

#include <arinc_615a/files/Files.hpp>

#include <arinc_615a/information/Information.hpp>

#include <helper/RawData.hpp>

namespace Arinc615a::Files {


/**
 * @name Encoding/ decoding of Ratio
 *
 * @{
 **/

/**
 * @brief Decode the given Raw Data as Ratio.
 *
 * @param[in] rawData
 *   Raw data
 *
 * @return Tuple of remaining un-decoded data and decoded Ratio.
 *
 * @sa @ref Ratio_encode()
 **/
[[nodiscard]] ARINC_615A_EXPORT std::tuple< Helper::ConstRawDataSpan, Information::Ratio > Ratio_decode(
  Helper::ConstRawDataSpan rawData );

/**
 * @brief Encodes Ratio as Raw Data.
 *
 * @param[in] ratio
 *   Ratio
 *
 * @return Raw data representing ratio
 *
 * @sa @ref Ratio_decode()
 **/
[[nodiscard]] ARINC_615A_EXPORT Helper::RawData Ratio_encode( const Information::Ratio &ratio );

/** @} **/

}

#endif
