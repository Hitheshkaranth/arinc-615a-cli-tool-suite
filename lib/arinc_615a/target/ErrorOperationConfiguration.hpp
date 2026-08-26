// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Template Arinc615a::Target::ErrorOperationConfiguration.
 **/

#ifndef ARINC_615A_TARGET_ERROROPERATIONCONFIGURATION_HPP
#define ARINC_615A_TARGET_ERROROPERATIONCONFIGURATION_HPP

#include <arinc_615a/target/Target.hpp>

#include <arinc_615a/TargetId.hpp>

#include <string>

namespace Arinc615a::Target {

/**
 * @brief ARINC 615A %Target Error %Operation Configuration.
 *
 * @sa @ref ErrorOperation
 **/
struct ErrorOperationConfiguration final
{
  //! Error Operation Completion Handler
  ErrorOperationCompletionHandler completionHandler;
  //! Operation Type
  OperationType operation{ OperationType::Invalid };
  //! %Target ID.
  TargetId targetId;
  //! Error Code
  OperationAcceptanceStatusCode status;
  //! Error Description.
  std::string description;
};

}

#endif
