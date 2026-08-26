// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Find::Servers::FindServerException.
 **/

#ifndef ARINC_615A_FIND_SERVERS_FINDSERVEREXCEPTION_HPP
#define ARINC_615A_FIND_SERVERS_FINDSERVEREXCEPTION_HPP

#include <arinc_615a/find/servers/Servers.hpp>

#include <arinc_615a/Arinc615aException.hpp>

namespace Arinc615a::Find::Servers {

//! FIND %Server %Exception.
class ARINC_615A_EXPORT FindServerException : public virtual Arinc615aException
{
  public:
    /**
     * @brief Returns the exception description.
     *
     * @return The exception description.
     **/
    const char * what() const noexcept override
    {
      return "ARINC 615A FIND Server Exception";
    }
};

}

#endif
