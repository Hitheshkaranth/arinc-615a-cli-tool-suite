// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Namespace Arinc615a::Find::Packets.
 **/

#include "Packets.hpp"

namespace Arinc615a::Find::Packets {

const ParameterMaxLengthMap& parameterMaxLengths()
{
  static const ParameterMaxLengthMap ParameterMaxLengths{
    { ParameterList::ThwId,            15U },
    { ParameterList::ThwTypeName,       8U },
    { ParameterList::ThwPosition,       8U },
    { ParameterList::LiteralName,      20U },
    { ParameterList::ManufacturerCode,  3U }
  };

  return ParameterMaxLengths;
}

}
