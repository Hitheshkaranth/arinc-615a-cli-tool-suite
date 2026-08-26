// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Host::InformationOperationHandler.
 **/

#ifndef ARINC_615A_HOST_CLIENTINFORMATIONOPERATIONHANDLER_HPP
#define ARINC_615A_HOST_CLIENTINFORMATIONOPERATIONHANDLER_HPP

#include <arinc_615a/host/Host.hpp>
#include <arinc_615a/host/OperationHandler.hpp>

#include <arinc_615a/information/Information.hpp>

namespace Arinc615a::Host {

/**
 * @brief ARINC 615A %Host %Information %Operation Handler.
 *
 * @sa @ref InformationOperation
 **/
class InformationOperationHandler : public OperationHandler
{
  public:
    /**
     * @brief Status Message has been received by the Host Data Loader.
     *
     * This handler is called when the Host Data Loader has received a *Load Configuration Status File*.
     * The file is decoded and the status information is provided to the registered handler.
     *
     * @param[in] status
     *   Information Operation Status information.
     **/
    virtual void status( const Information::InformationStatus &status ) = 0;

    /**
     * @brief Provides the Target Hardware Information has been received.
     *
     * This handler is called when the Host Data Loader has received the *Load Configuration List File*.
     * The file is decoded and provided to the registered handler.
     *
     * @param[in] targetsHardware
     *   Targets Hardware Information.
     * @param[in] integrity
     *   Provides integrity information of the target hardware information.
     *   This information is calculated when the target hardware loader provides a checksum option.
     *   If not set, this parameter is always set to @p true.
     **/
    virtual void targetInformation( const Information::TargetsHardware &targetsHardware, bool integrity ) = 0;
};

}

#endif
