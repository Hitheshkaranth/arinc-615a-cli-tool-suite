// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Find::Clients::FindClientException.
 **/

#ifndef ARINC_615A_FIND_CLIENTS_FINDCLIENTEXCEPTION_HPP
#define ARINC_615A_FIND_CLIENTS_FINDCLIENTEXCEPTION_HPP

#include "Clients.hpp"

#include <arinc_615a/Arinc615aException.hpp>

namespace Arinc615a::Find::Clients {

//! ARINC 615A FIND %Client Exception.
class ARINC_615A_EXPORT FindClientException : public Arinc615aException
{
  public:
    /**
     * @brief Return an exception description.
     *
     * @return An exception description.
     **/
    const char * what() const noexcept override
    {
      return "ARINC 615A FIND Client Exception";
    }
};

}

#endif
