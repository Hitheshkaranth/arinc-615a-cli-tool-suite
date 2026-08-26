// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::OperationTypeDescription.
 **/

#ifndef ARINC_615A_OPERATIONTYPEDESCRITPTION_HPP
#define ARINC_615A_OPERATIONTYPEDESCRITPTION_HPP

#include <arinc_615a/Arinc615a.hpp>

#include <helper/Description.hpp>

#include <iosfwd>

namespace Arinc615a {

/**
 * @name ARINC 615A Operation Type
 * @{
 **/

/**
 * @brief ARINC 615A Operation Type Description.
 *
 * @sa @ref OperationType
 **/
class ARINC_615A_EXPORT OperationTypeDescription final :
  public Helper::Description< OperationTypeDescription, OperationType >
{
  public:
    //! Initialises Instance
    OperationTypeDescription();
};

/**
 * @brief Streaming Operator to Output Operation Code as String
 *
 * @param[in,out] stream
 *   Output stream
 * @param[in] operation
 *   Operation
 *
 * @return @p stream for chaining.
 *
 * @sa @ref OperationType
 * @sa @ref OperationTypeDescription
 **/
ARINC_615A_EXPORT std::ostream& operator<<( std::ostream &stream, OperationType operation );

/**
 * @brief Streaming Operator for Parsing an Operation String as Operation Type.
 *
 * @param[in,out] stream
 *   Input stream
 * @param[out] operation
 *   Decoded operation
 *
 * @return @p stream for chaining.
 *
 * @sa @ref OperationType
 * @sa @ref OperationTypeDescription
 **/
ARINC_615A_EXPORT std::istream& operator>>( std::istream& stream, OperationType &operation );

/** @} **/

}

#endif
