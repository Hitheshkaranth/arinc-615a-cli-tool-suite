// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Information::DownloadFileInformation.
 **/

#ifndef ARINC_615A_INFORMATION_DOWNLOADFILEINFORMATION_HPP
#define ARINC_615A_INFORMATION_DOWNLOADFILEINFORMATION_HPP

#include <arinc_615a/information/Information.hpp>

#include <string>

namespace Arinc615a::Information {

/**
 * @brief Download %File %Information
 *
 * The Download information is sent from the target to the host during the Operator Defined Download Operation.
 *
 * This information is used to inform about the files, which can be downloaded.
 **/
struct DownloadFileInformation final
{
  //! Filename
  std::string filename;
  //! Description
  std::string description;
};

}

#endif
