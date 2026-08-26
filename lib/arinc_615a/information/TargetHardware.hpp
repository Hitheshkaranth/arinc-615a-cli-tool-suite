// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Information::TargetHardware.
 **/

#ifndef ARINC_615A_INFORMATION_TARGETHARDWAREINFORMATION_HPP
#define ARINC_615A_INFORMATION_TARGETHARDWAREINFORMATION_HPP

#include <arinc_615a/information/Information.hpp>
#include <arinc_615a/information/PartNumber.hpp>

#include <boost/property_tree/ptree_fwd.hpp>

#include <string>
#include <string_view>

namespace Arinc615a::Information {

/**
 * @brief The target hardware information, which is part of the load configuration list.
 *
 * This information is provided during the _ARINC 615A Information Operation_.
 *
 * @note
 * In ARINC 615A-2 The field Literal Name was named %Target Hardware Code.
 *
 * @sa @ref Arinc615a::Information::PartNumber
 **/
struct ARINC_615A_EXPORT TargetHardware final
{
  public:
    /**
     * @brief Load Targets Hardware Information from a property tree.
     *
     * @param[in] properties
     *   properties.
     *
     * @return Targets Hardware Information.
     **/
    static TargetsHardware targetsHardware( const boost::property_tree::ptree &properties );

    /**
     * @brief Returns the Targets Hardware Information as a boost property tree.
     *
     * @param[in] targetsHardware
     *   Targets Hardware Information to export.
     *
     * @return Targets Hardware Information as a Boost property tree.
     **/
    static boost::property_tree::ptree targetsHardware( const TargetsHardware &targetsHardware );

    /** Default Initialises Target Hardware **/
    TargetHardware() = default;

    /**
     * @brief Initialise the target hardware.
     *
     * @param[in] literalName
     *   Literal name.
     * @param[in] serialNumber
     *   Serial number.
     * @param[in] partNumbers
     *   Part numbers
     **/
    TargetHardware( std::string literalName, std::string serialNumber, PartNumbers partNumbers );

    /**
     * @brief Initialise the target hardware from the properties.
     *
     * @param[in] properties
     *   Properties.
     **/
    explicit TargetHardware( const boost::property_tree::ptree &properties );

    /**
     * @brief Load target hardware information from the given property tree.
     *
     * @param[in] properties
     *   Configuration as Property Tree
     **/
    void fromProperties( const boost::property_tree::ptree &properties );

    /**
     * @brief Exports the target hardware information to a boost property tree.
     *
     * @return Target hardware information as a Boost property tree.
     **/
    [[nodiscard]] boost::property_tree::ptree toProperties() const;

    /**
     * @brief Outputs the Target Hardware Information to @p std::string.
     *
     * @param[in] firstIndention
     *   Indention string prepended.
     * @param[in] secondIndention
     *  Indention additionally added for part number information.
     *
     * @return Target Hardware Information as @p std::string.
     **/
    [[nodiscard]] std::string toString(
      std::string_view firstIndention = "\t",
      std::string_view secondIndention = "\t" ) const;

    /**
     * @brief Literal Name.
     *
     * In ARINC 615A-2 this was named Target Hardware Code.
     * This field corresponds to the "Literal Name" field of the FIND answer.
     **/
    std::string literalName;
    /**
     * @brief Serial Number.
     *
     * %Target Hardware Serial Number of the %Target Hardware.
     **/
    std::string serialNumber;
    /**
     * @brief List of Part Numbers.
     *
     * Hardware or any load Part Number.
     **/
    PartNumbers partNumbers;
};

}

#endif
