// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration/ Definition of Classes Arinc615a::Arinc615aException, and Arinc615a::Arinc615aAbortException.
 **/

#ifndef ARINC_615A_ARINC615AEXCEPTION_HPP
#define ARINC_615A_ARINC615AEXCEPTION_HPP

#include <arinc_615a/Arinc615a.hpp>

#include <boost/exception/exception.hpp>
#include <boost/exception/error_info.hpp>

#include <exception>

namespace Arinc615a {

/**
 * @brief ARINC 615A Exception.
 **/
class ARINC_615A_EXPORT Arinc615aException : public std::exception, public boost::exception
{
  public:
    /**
     * @brief Returns an Exception Description.
     *
     * @return Exception Description.
     **/
    [[nodiscard]] char const * what() const noexcept override
    {
      return "ARINC 615A Exception.";
    }
};

/**
 * @brief ARINC 615A Abort Occurred Exception.
 *
 * This exception shall always contain the additional StatusCodeInfo.
 */
class ARINC_615A_EXPORT Arinc615aAbortException : public Arinc615aException
{
  public:
    //! @copydoc Arinc615aException::what() const
    char const * what() const noexcept override
    {
      return "ARINC 615A Abort Exception.";
    }
};

//! ARINC 615A Status Code Error %Information.
using StatusCodeInfo = boost::error_info< struct StatusCodeInfoTag, StatusCode >;

//! ARINC 615A Operation Type Error %Information.
using OperationTypeInfo = boost::error_info< struct OperationInfoTag, OperationType >;

}

#endif
