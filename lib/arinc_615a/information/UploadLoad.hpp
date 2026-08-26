// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Information::UploadLoad.
 **/

#ifndef ARINC_615A_INFORMATION_UPLOADLOAD_HPP
#define ARINC_615A_INFORMATION_UPLOADLOAD_HPP

#include <arinc_615a/information/Information.hpp>

#include <string>

namespace Arinc615a::Information {

/**
 * @brief This class contains the upload information, transferred from the host to the target, which contains the
 *   available loads.
 *
 * For the Upload Operation a list of this information is transferred to the target.
 **/
struct UploadLoad final
{
  //! Load Upload Header Filename
  std::string headerFilename;
  //! Load Part Number
  std::string partNumber;
};

}

#endif
