// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Target::UploadOperationHandler.
 **/

#ifndef ARINC_615A_TARGET_UPLOADOPERATIONHANDLER_HPP
#define ARINC_615A_TARGET_UPLOADOPERATIONHANDLER_HPP

#include <arinc_615a/target/Target.hpp>
#include <arinc_615a/target/OperationHandler.hpp>

#include <arinc_615a/information/Information.hpp>

namespace Arinc615a::Target {

/**
 * @brief ARINC 615A %Target Upload %Operation Handler.
 *
 * @sa @ref UploadOperation
 **/
class UploadOperationHandler : public OperationHandler
{
  public:
    /**
     * @brief Status Transmitted Handler.
     *
     * Called when status information has been sent to the host.
     *
     * @param[in] status
     *   Status Information transferred to Host.
     **/
    virtual void status( const Information::UploadStatus &status ) = 0;

    /**
     * @brief Loads List Received Handler.
     *
     * Called when a load list has been received from the host.
     *
     * @param[in] loads
     *   Upload load list.
     **/
    virtual void loadList( const Information::UploadLoads &loads ) = 0;
};

}

#endif
