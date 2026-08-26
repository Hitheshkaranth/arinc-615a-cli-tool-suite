// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Namespace Arinc615a::Find.
 **/

/**
 * @dir
 * @brief Declaration/ Definition of Namespace Arinc615a::Find.
 **/

#ifndef ARINC_615A_FIND_FIND_HPP
#define ARINC_615A_FIND_FIND_HPP

#include <arinc_615a/Arinc615a.hpp>

#include <boost/asio/ip/address.hpp>

#include <chrono>
#include <cstdint>
#include <list>
#include <map>
#include <tuple>

/**
 * @brief ARINC 615A FIND Protocol.
 *
 * The FIND protocol is used to discover ARINC 615A loadable targets over the data network.
 *
 * A ARINC 615A target hardware implements the FIND protocol by using the @ref Arinc615a::Find::Servers::Server class.
 * A ARINC 615A data loader implements the FIND protocol by using the @ref Arinc615a::Find::Clients::Client class.
 *
 * Sub-namespaces:
 * - @ref Arinc615a::Find::Clients (ARINC 615A FIND %Client implementation)
 * - @ref Arinc615a::Find::Servers (ARINC 615A FIND %Server implementation)
 * - @ref Arinc615a::Find::Packets (ARINC 615A FIND %Packets implementation)
 **/
namespace Arinc615a::Find {

//! Default ARINC 615A FIND Port.
constexpr uint16_t DefaultPort{ 1001U };

//! Default Receive Timeout for FIND Receive as Defined in ARINC 615A.
constexpr std::chrono::seconds DefaultReceiveTimeout{ 3U };

// forward declaration
class FindConfiguration;

class TargetInformation;

/**
 * @brief %Target Address %Information.
 *
 * The FIND %Target %Information associated with the %Target IP-Address.
 *
 * @sa @ref Clients::Query
 **/
using TargetAddressInformation = std::tuple< boost::asio::ip::address, TargetInformation >;

/**
 * @brief %Targets %Information.
 *
 * List of %Target %Information.
 *
 * @sa @ref TargetInformation
 **/
using TargetsInformation = std::list< TargetInformation >;

/**
 * @brief %Targets Address %Information.
 *
 * List of %Target Address %Information.
 *
 * @sa @ref TargetAddressInformation
 **/
using TargetsAddressInformation = std::list< TargetAddressInformation >;

/**
 * @brief %Target Address %Information mapped to %Target ID.
 *
 * The mapped value is const to assure, that the key (Target ID) always aligned wth the mapped value.
 *
 * @sa @ref TargetAddressInformation
 * @sa @ref TargetId
 **/
using TargetsAddressInformationMap = std::map< TargetId, const TargetAddressInformation, std::less<> >;

}

#endif
