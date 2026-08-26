// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Arinc615aConfiguration.
 **/

#ifndef ARINC_615A_ARINC615ACONFIGURATION_HPP
#define ARINC_615A_ARINC615ACONFIGURATION_HPP

#include <arinc_615a/Arinc615a.hpp>

#include <tftp/TftpConfiguration.hpp>
#include <tftp/TftpOptionsConfiguration.hpp>

#include <boost/asio/ip/address.hpp>

#include <boost/program_options/options_description.hpp>

#include <boost/property_tree/ptree_fwd.hpp>

#include <cstdint>

namespace Arinc615a {

/**
 * @brief ARINC 615A Data Loader Configuration.
 *
 * This configuration is used by the *Host* and the *Target* implementation.
 *
 * The configuration can be loaded via a property tree or @p boost::program_options.
 **/
class ARINC_615A_EXPORT Arinc615aConfiguration final
{
  public:
    /**
     * @brief Creates a Data Loader configuration with default values.
     **/
    Arinc615aConfiguration() noexcept = default;

    /**
     * @brief Creates a Data Loader configuration with the values given by the property tree.
     *
     * Values not set by the property tree are filled with default values.
     *
     * @param[in] properties
     *   Configuration as a property tree.
     **/
    explicit Arinc615aConfiguration( const boost::property_tree::ptree &properties );

    //! Destructor
    ~Arinc615aConfiguration() = default;

    /**
     * @brief Load Configuration from a given Property Tree.
     *
     * @param[in] properties
     *   Configuration as Property Tree
     **/
    void fromProperties( const boost::property_tree::ptree &properties );

    /**
     * @brief Exports the configuration to a property tree.
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
     * @return Option description for filling this configuration.
     **/
    [[nodiscard]] boost::program_options::options_description options();

    //! Local Interface Address
    boost::asio::ip::address localInterfaceAddress{ boost::asio::ip::address_v4::any() };
    //! Number of DLP-Operation Retries.
    uint16_t dlpRetries{ DefaultArinc615aDlpRetries };
    //! TFTP configuration, which is part of the ARINC 615A Configuration.
    ::Tftp::TftpConfiguration tftpConfiguration{ DefaultArinc615aTftpPort };
    //! TFTP options configuration, which is part of the ARINC 615A Configuration.
    ::Tftp::TftpOptionsConfiguration tftpOptionsConfiguration;
    //! Enable Protocol File Logging
    bool protocolFileLogging{ false };
};

}

#endif
