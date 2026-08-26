// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Host::MediaDefinedDownloadOperationHandler.
 **/

#ifndef ARINC_615A_HOST_MEDIADEFINEDDOWNLOADOPERATIONHANDLER_HPP
#define ARINC_615A_HOST_MEDIADEFINEDDOWNLOADOPERATIONHANDLER_HPP

#include <arinc_615a/host/Host.hpp>
#include <arinc_615a/host/DownloadOperationHandler.hpp>

namespace Arinc615a::Host {

/**
 * @brief ARINC 615A %Host Media Defined Download %Operation Handler.
 *
 * No specific enhancement to DownloadOperationHandler.
 *
 * @sa @ref MediaDefinedDownloadOperation
 **/
class MediaDefinedDownloadOperationHandler : public DownloadOperationHandler
{
};

}

#endif
