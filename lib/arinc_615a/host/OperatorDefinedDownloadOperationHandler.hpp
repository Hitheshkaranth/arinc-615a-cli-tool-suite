// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Host::OperatorDefinedDownloadOperationHandler
 **/

#ifndef ARINC_615A_HOST_OPERATORDEFINEDDOWNLOADOPERATIONHANDLER_HPP
#define ARINC_615A_HOST_OPERATORDEFINEDDOWNLOADOPERATIONHANDLER_HPP

#include <arinc_615a/host/Host.hpp>
#include <arinc_615a/host/DownloadOperationHandler.hpp>

#include <arinc_615a/information/Information.hpp>

namespace Arinc615a::Host {

/**
 * @brief ARINC 615A %Host Operator Defined Download %Operation Handler.
 *
 * @sa @ref OperatorDefinedDownloadOperation
 **/
class OperatorDefinedDownloadOperationHandler : public DownloadOperationHandler
{
  public:
    //! Destructor
    ~OperatorDefinedDownloadOperationHandler() override = default;

    /**
     * @brief Downloading Information Message has been received by the Host Data Loader.
     *
     * This handler is called when the Host Data Loader has received a *Download Operation List File*.
     * The file is decoded and the list information is provided to the registered handler.
     *
     * With this information, the host data loader must send the answer file back to the target data loader.
     *
     * @param[in] files
     *   The Downloading Information List.
     *   Contains the available files from the Target HW.
     *
     * @sa OperatorDefinedDownloadOperation::answer()
     **/
    virtual void downloadingList( const Information::DownloadFilesInformation &files ) = 0;
};

}

#endif
