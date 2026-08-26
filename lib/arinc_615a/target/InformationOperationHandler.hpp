// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Target::InformationOperationHandler.
 **/

#ifndef ARINC_615A_TARGET_TARGETINFORMATIONOPERATIONHANDLER_HPP
#define ARINC_615A_TARGET_TARGETINFORMATIONOPERATIONHANDLER_HPP

#include <arinc_615a/target/Target.hpp>
#include <arinc_615a/target/OperationHandler.hpp>

#include <arinc_615a/information/Information.hpp>

namespace Arinc615a::Target {

/**
 * @brief ARINC 615A %Target %Information %Operation Handler.
 *
 * @sa @ref InformationOperation
 **/
class InformationOperationHandler : public OperationHandler
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
    virtual void status( const Information::InformationStatus &status ) = 0;
};

}

#endif
