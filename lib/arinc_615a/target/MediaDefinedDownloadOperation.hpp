// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Target::MediaDefinedDownloadOperation.
 **/

#ifndef ARINC_615A_TARGET_MEDIADEFINEDDOWNLOADOPERATION_HPP
#define ARINC_615A_TARGET_MEDIADEFINEDDOWNLOADOPERATION_HPP

#include <arinc_615a/target/Target.hpp>
#include <arinc_615a/target/DownloadOperation.hpp>

namespace Arinc615a::Target {

/**
 * @brief ARINC 615A %Target Media Defined Download %Operation.
 **/
class ARINC_615A_EXPORT MediaDefinedDownloadOperation : public DownloadOperation
{
  public:
    // no further interfaces necessary
};

}

#endif
