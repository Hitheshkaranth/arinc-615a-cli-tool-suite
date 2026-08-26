// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Namespace Arinc615a.
 **/

#include "Arinc615a.hpp"

#include <arinc_615a/Version.hpp>

#include <helper/VersionInformation.hpp>

namespace Arinc615a {

Helper::VersionsInformation::value_type arinc615aVersion()
{
  return {
    Version::Key,
    {
      Version::Name,
      Version::VersionInformation,
      Version::License,
      Version::Url
    }
  };
}

void setLogLevel( const spdlog::level::level_enum level )
{
  spdlog::set_level( level );
}

}
