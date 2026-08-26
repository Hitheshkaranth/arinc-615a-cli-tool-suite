// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Version.
 **/

#ifndef ARINC_615A_VERSION_HPP
#define ARINC_615A_VERSION_HPP

#include <arinc_615a/Arinc615a.hpp>

namespace Arinc615a {

//! ARINC 615A C++ Library %Version %Information.
// Note don't export class, as the data is available complete in declaration
class Version final
{
  public:
    // Deleted Constructor
    Version() = delete;

    //! Product Key.
    static constexpr auto Key{ "${PROJECT_NAME}" };

    //! Product Name.
    static constexpr auto Name{ "${PROJECT_DESCRIPTION}" };

    //! Product License.
    static constexpr auto License{ "MPL-2.0" };

    //! Product Url.
    static constexpr auto Url{ "${PROJECT_HOMEPAGE_URL}" };

    //! Version Number as String.
    static constexpr auto Number{ "${PROJECT_VERSION}" };

    //! Source Code Management Information.
    static constexpr auto Scm{ "${PROJECT_GIT_HASH}" };

    //! Full Version Information as String.
    static constexpr auto VersionInformation{ "${PROJECT_VERSION_FULL}" };
};

}

#endif
