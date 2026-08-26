// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Host::MediaDefinedDownloadOperation.
 **/

#ifndef ARINC_615A_HOST_MEDIADEFINEDDOWNLOADOPERATION_HPP
#define ARINC_615A_HOST_MEDIADEFINEDDOWNLOADOPERATION_HPP

#include <arinc_615a/host/Host.hpp>
#include <arinc_615a/host/DownloadOperation.hpp>

#include <arinc_615a/information/Information.hpp>

#include <helper/RawData.hpp>

namespace Arinc615a::Host {

/**
 * @brief ARINC 615A %Host Media Defined Download %Operation.
 *
 * This class interface provides additional operation, which are necessary to perform the _Media Defined Download_
 * operation.
 *
 * Operation Steps:
 * - Initialisation Phase:
 *   - Request Initialisation File (TFTP RRQ from Host DL)
 * - List Transfer Phase
 *   - Send Downloading Request (TFTP WRQ from Host DL)
 * - Transfer Phase:
 *   - Receive Files (TFTP WRQ from Target DL)
 **/
class ARINC_615A_EXPORT MediaDefinedDownloadOperation : public DownloadOperation
{
  public:
    /**
     * @brief This operation sends the given download request to the target.
     *
     * This operation should be called exactly once, after the operation has been accepted from the target data loader.
     *
     * @param[in] files
     *   Download Files.
     * @param[in] userDefinedData
     *   User Defined Data.
     **/
    virtual void request( Information::DownloadFiles files, Helper::RawData userDefinedData = {} ) = 0;
};

}

#endif
