// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Target::OperatorDefinedDownloadOperation.
 **/

#ifndef ARINC_615A_TARGET_OPERATORDEFINEDDOWNLOADOPERATION_HPP
#define ARINC_615A_TARGET_OPERATORDEFINEDDOWNLOADOPERATION_HPP

#include <arinc_615a/target/Target.hpp>
#include <arinc_615a/target/DownloadOperation.hpp>

#include <arinc_615a/information/Information.hpp>

namespace Arinc615a::Target {

/**
 * @brief ARINC 615A %Target Media Operator Download %Operation.
 **/
class ARINC_615A_EXPORT OperatorDefinedDownloadOperation : public DownloadOperation
{
  public:
    /**
     * @brief Sends the downloading list to the DLA.
     *
     * This list is used to provide information, which files are available.
     *
     * @param[in] files
     *   Download Files Information.
     **/
    virtual void downloadingList( Information::DownloadFilesInformation files ) = 0;
};

}

#endif
