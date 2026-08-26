// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Target::OperatorDefinedDownloadOperationHandler.
 **/

#ifndef ARINC_615A_TARGET_OPERATORDEFINEDDOWNLOADOPERATIONHANDLER_HPP
#define ARINC_615A_TARGET_OPERATORDEFINEDDOWNLOADOPERATIONHANDLER_HPP

#include <arinc_615a/target/Target.hpp>
#include <arinc_615a/target/DownloadOperationHandler.hpp>

namespace Arinc615a::Target {

/**
 * @brief ARINC 615A %Target Operator Defined Download %Operation Handler.
 *
 * @sa @ref OperatorDefinedDownloadOperation
 **/
class OperatorDefinedDownloadOperationHandler : public DownloadOperationHandler
{
  public:
    /**
     * @brief Downloading Answer Handler.
     *
     * This handler is called when the download answer has been received from the host DLA.
     * The THA implementation uses this information to initiate the file transfers.
     *
     * @param[in] files
     *   Download Files
     **/
    virtual void downloadingAnswer( const Information::DownloadFiles &files ) = 0;
};

}

#endif
