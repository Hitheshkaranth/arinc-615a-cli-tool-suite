// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Target::MediaDefinedDownloadOperationHandler.
 **/

#ifndef ARINC_615A_TARGET_TARGETMEDIADEFINEDDOWNLOADOPERATIONHANDLER_HPP
#define ARINC_615A_TARGET_TARGETMEDIADEFINEDDOWNLOADOPERATIONHANDLER_HPP

#include <arinc_615a/target/Target.hpp>
#include <arinc_615a/target/DownloadOperationHandler.hpp>

#include <arinc_615a/information/Information.hpp>

#include <helper/RawData.hpp>

namespace Arinc615a::Target {

/**
 * @brief ARINC 615A %Target Media Defined Download %Operation Handler.
 *
 * @sa @ref MediaDefinedDownloadOperation
 **/
class MediaDefinedDownloadOperationHandler : public DownloadOperationHandler
{
  public:
    /**
     * @brief Downloading Request Handler.
     *
     * Called when the downloading request has been received from the DLA and the data has been decoded.
     * The THA implementation uses this information to initiate the file transfers.
     *
     * @param[in] files
     *   Download Files
     * @param[in] userDefinedData
     *   Additional user defined data.
     **/
    virtual void downloadingRequest(
      const Information::DownloadFiles &files,
      Helper::ConstRawDataSpan userDefinedData ) = 0;
};

}

#endif
