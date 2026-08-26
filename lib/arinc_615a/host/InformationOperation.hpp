// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Host::InformationOperation.
 **/

#ifndef ARINC_615A_HOST_INFORMATIONOPERATION_HPP
#define ARINC_615A_HOST_INFORMATIONOPERATION_HPP

#include <arinc_615a/host/Operation.hpp>

namespace Arinc615a::Host {

/**
 * @brief ARINC 615A %Host %Information %Operation.
 *
 * No additional interface is necessary, the concrete interface is the base Host Operation interface.
 **/
class ARINC_615A_EXPORT InformationOperation : public Operation
{
};

}

#endif
