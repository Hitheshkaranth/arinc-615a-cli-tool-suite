// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Template Arinc615a::Host::OperationConfiguration.
 **/

#ifndef ARINC_615A_HOST_OPERATIONCONFIGURATION_HPP
#define ARINC_615A_HOST_OPERATIONCONFIGURATION_HPP

#include <arinc_615a/host/Host.hpp>

#include <arinc_615a/Arinc615aConfiguration.hpp>
#include <arinc_615a/TargetId.hpp>

#include <boost/asio/ip/udp.hpp>

namespace Arinc615a::Host {

/**
 * @brief %Operation Configuration.
 *
 * Configuration used for creation of Operations.
 *
 * @par Part Number and Checksum Options
 * There are no configuration options for the *Part Number* and *Checksum* option.
 * The options are handled directly by the user of this library, when receiving a file transfer request.
 * For the ARINC 615A Host Operation the options are transparent and directed from/ the %Target HW to/ from the host
 * implementation.
 *
 * @tparam OperationHandlerT
 *   Operation Handler
 **/
template< typename OperationHandlerT >
struct OperationConfiguration
{
  //! Data Loader Configuration.
  Arinc615aConfiguration dataLoaderConfiguration;
  //! %Operation Handler.
  OperationHandlerT &handler;
  //! %Target Address.
  boost::asio::ip::address targetAddress;
  //! %Target ID
  TargetId targetId;
  //! Timeout for DL-Transfers (e.g. Status Information).
  std::chrono::seconds dlpTimeout{ Arinc615a::DefaultArinc615aDlpTimeout };
  //! If set to true, the port option is set with a dynamic port.
  //! If the %Target HW rejects this option, the operation is cancelled.
  bool portOption{ false };
};

}

#endif
