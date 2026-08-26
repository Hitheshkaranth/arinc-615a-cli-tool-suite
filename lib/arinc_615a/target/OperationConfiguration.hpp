// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Template Arinc615a::Target::OperationConfiguration.
 **/

#ifndef ARINC_615A_TARGET_OPERATIONCONFIGURATION_HPP
#define ARINC_615A_TARGET_OPERATIONCONFIGURATION_HPP

#include <arinc_615a/target/Target.hpp>

#include <arinc_615a/TargetId.hpp>

namespace Arinc615a::Target {

/**
 * @brief %Operation Configuration.
 *
 * Configuration used for creation of Operations.
 *
 * @tparam OperationHandlerT
 *   Operation Handler
 **/
template< typename OperationHandlerT >
struct OperationConfiguration
{
  //! %Operation Handler.
  OperationHandlerT &handler;
  //! %Target ID.
  TargetId targetId;
  //! Transmission Rate of Status Information.
  std::chrono::seconds statusTransmissionRate;
};

}

#endif
