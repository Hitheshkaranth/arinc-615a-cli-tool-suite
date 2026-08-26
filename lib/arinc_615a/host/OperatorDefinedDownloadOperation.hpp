// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Host::OperatorDefinedDownloadOperation.
 **/

#ifndef ARINC_615A_HOST_OPERATORDEFINEDDOWNLOADOPERATION_HPP
#define ARINC_615A_HOST_OPERATORDEFINEDDOWNLOADOPERATION_HPP

#include <arinc_615a/host/Host.hpp>
#include <arinc_615a/host/DownloadOperation.hpp>

#include <arinc_615a/information/Information.hpp>

namespace Arinc615a::Host {

/**
 * @brief ARINC 615A %Host Operator Defined Download %Operation.
 *
 * Operation Steps:
 * - Initialisation Phase:
 *   - Request Initialisation File (TFTP RRQ from Host DL)
 * - List Transfer Phase
 *   - Receive Downloading List (TFTP WRQ from Target DL)
 *   - Send Downloading Answer (TFTP WRQ from Host DL)
 * - Transfer Phase:
 *   - Receive Files (TFTP WRQ from Target DL)
 **/
class ARINC_615A_EXPORT OperatorDefinedDownloadOperation : public DownloadOperation
{
  public:
    //! Destructor
    ~OperatorDefinedDownloadOperation() override = default;

    /**
     * @brief Sends the downloading answer back to the target data loader.
     *
     * The target data loader will start transferring the requested files to the host data loader.
     *
     * @param[in] files
     *   Download Files.
     **/
    virtual void answer( Information::DownloadFiles files ) = 0;
};

}

#endif
