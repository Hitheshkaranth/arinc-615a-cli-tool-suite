// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Find::FindConfiguration.
 **/

#ifndef ARINC_615A_FIND_FINDCONFIGURATION_HPP
#define ARINC_615A_FIND_FINDCONFIGURATION_HPP

#include <arinc_615a/find/Find.hpp>

#include <boost/asio/ip/address.hpp>

#include <boost/program_options/options_description.hpp>

#include <boost/property_tree/ptree_fwd.hpp>

#include <cstdint>

namespace Arinc615a::Find {

/**
 * @brief ARINC 615A FIND Configuration.
 *
 * This configuration is used for _FIND Servers_ and _FIND Clients_.
 * The configuration can be loaded via a property tree.
 * For CLI applications a command line parser interface is provided.
 **/
class ARINC_615A_EXPORT FindConfiguration
{
  public:
    /**
     * @brief Creates a Data Loader configuration with default values.
     **/
    FindConfiguration() noexcept = default;

    /**
     * @brief Creates a Data Loader configuration with the values given by the property tree.
     *
     * Values not set by the property tree are filled with default values.
     *
     * @param[in] properties
     *   Configuration as a property tree.
     **/
    explicit FindConfiguration( const boost::property_tree::ptree &properties );

    /**
     * @brief Load Configuration from a given Property Tree.
     *
     * @param[in] properties
     *   Configuration as Property Tree
     **/
    void fromProperties( const boost::property_tree::ptree &properties );

    /**
     * @brief Returns the configuration as a property tree.
     *
     * @param[in] full
     *   If set to true, all options are added to the property tree, even if defaulted.
     *
     * @return Configuration as a property tree.
     **/
    [[nodiscard]] boost::property_tree::ptree toProperties( bool full = false ) const;

    /**
     * @brief Returns an option description, which can be used to parse a command line.
     *
     * @return FIND configuration options description.
     **/
    [[nodiscard]] boost::program_options::options_description options();

    //! Local Interface Address
    boost::asio::ip::address localInterfaceAddress{ boost::asio::ip::address_v4::any() };
    //! UDP Port for FIND Operation.
    uint16_t findPort{ DefaultPort };
};

}

#endif
