// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Struct Arinc615a::Target::ProtocolConfiguration.
 **/

#ifndef ARINC_615A_TARGET_PROTOCOLCONFIGURATION_HPP
#define ARINC_615A_TARGET_PROTOCOLCONFIGURATION_HPP

#include <arinc_615a/target/Target.hpp>

#include <arinc_615a/Arinc615aConfiguration.hpp>

namespace Arinc615a::Target {

/**
 * @brief %Protocol Configuration.
 *
 * @sa @ref Protocol
 **/
struct ProtocolConfiguration final
{
  //! Handler for new ARINC 615A operations.
  NewOperationRequestHandler newOperationRequestHandler;
  //! Data Loader Configuration.
  Arinc615aConfiguration configuration;
  //! ARINC 615A Version used for protocol files.
  Arinc615aVersion protocolVersion;
};

}

#endif
