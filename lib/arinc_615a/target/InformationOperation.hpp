// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Target::InformationOperation.
 **/

#ifndef ARINC_615A_TARGET_INFORMATIONOPERATION_HPP
#define ARINC_615A_TARGET_INFORMATIONOPERATION_HPP

#include <arinc_615a/target/Target.hpp>
#include <arinc_615a/target/Operation.hpp>

#include <arinc_615a/information/Information.hpp>

#include <arinc_649/Arinc649.hpp>

namespace Arinc615a::Target {

/**
 * @brief ARINC 615A %Target %Information %Operation.
 **/
class ARINC_615A_EXPORT InformationOperation : public Operation
{
  public:
    /**
     * @brief Send the given Targets Hardware Information to the Requester.
     *
     * When the target information has been transferred, the operation is completed automatically.
     *
     * @param[in] targetsHardware
     *   Targets Hardware Information.
     * @param[in] checkValueType
     *   Check Value Type used for integrity protection of the target configuration list.
     **/
    virtual void targetInformation(
      Information::TargetsHardware targetsHardware,
      Arinc649::CheckValueType checkValueType = Arinc649::CheckValueType::NotUsed ) = 0;
};

}

#endif
