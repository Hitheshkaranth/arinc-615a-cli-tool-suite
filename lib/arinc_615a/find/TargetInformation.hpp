// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Find::TargetInformation.
 **/

#ifndef ARINC_615A_FIND_TARGETINFORMATION_HPP
#define ARINC_615A_FIND_TARGETINFORMATION_HPP

#include <arinc_615a/find/Find.hpp>

#include <arinc_615a/TargetId.hpp>

#include <boost/property_tree/ptree_fwd.hpp>

#include <string>

namespace Arinc615a::Find {

/**
 * @brief ARINC 615A FIND Target Information.
 *
 * This class holds the target information, which are provided by the *ARINC 615A FIND Operation*.
 *
 * Helpers are provided to serialize and deserialize the information to/from _Boost Property Trees_.
 * Also, additional address information can be provided.
 **/
class ARINC_615A_EXPORT TargetInformation
{
  public:
    /**
     * @name Targets Address Information
     *
     * @{
     **/

    /**
     * @brief Load Targets Address Information from the given Configuration.
     *
     * @param[in] properties
     *   Configuration
     *
     * @return Load Target Information.
     **/
    [[nodiscard]] static TargetsAddressInformation targetsAddressInformation(
      const boost::property_tree::ptree &properties );

    /**
     * @brief Converts the given Targets Address Information to a boost property tree.
     *
     * @param[in] information
     *   Targets Address Information.
     *
     * @return Boost Property Tree
     **/
    [[nodiscard]] static boost::property_tree::ptree targetsAddressInformation(
      const TargetsAddressInformation &information );

    /**
     * @brief Converts the given Targets Address Information to a boost property tree.
     *
     * @param[in] information
     *   Targets Address Information.
     *
     * @return Boost Property Tree
     **/
    [[nodiscard]] static boost::property_tree::ptree targetsAddressInformation(
      const TargetsAddressInformationMap &information );

    /** @} **/

    /**
     * @name Target Address Information
     *
     * @{
     **/

    /**
     * @brief Load Target Address Information from the given Configuration.
     *
     * @param[in] properties
     *   Configuration as Property Tree
     *
     * @return Target Address Information.
     **/
    [[nodiscard]] static TargetAddressInformation targetAddressInformation(
      const boost::property_tree::ptree &properties );

    /**
     * @brief Converts the Target Address Information to a boost property tree.
     *
     * @param[in] information
     *   Target Address Information.
     *
     * @return Target Information as Boost Property Tree.
     **/
    [[nodiscard]] static boost::property_tree::ptree targetAddressInformation(
      const TargetAddressInformation &information );

    /** } **/

    /**
     * @name Targets Information
     *
     * @{
     **/

    /**
     * @brief Returns a list of Target Information.
     *
     * Targets information is loaded from a property tree.
     *
     * @param[in] properties
     *   Property Tree.
     *
     * @return Targets Information
     **/
    [[nodiscard]] static TargetsInformation targetsInformation( const boost::property_tree::ptree &properties );

    /**
     * @brief Converts the given Targets Information to a boost property tree.
     *
     * @param[in] targets
     *   Targets Information.
     *
     * @return Boost Property Tree
     **/
    [[nodiscard]] static boost::property_tree::ptree targetsInformation( const TargetsInformation &targets );

    /** } **/

    /**
     * @brief Constructor.
     *
     * All Members are initialised to contain no data.
     **/
    TargetInformation() = default;

    /**
     * @brief Constructor to initialise all data members.
     *
     * @param[in] thwId
     *   Target Hardware ID
     * @param[in] thwTypeName
     *   Type of the Target Hardware
     * @param[in] thwPosition
     *   Position of the Target Hardware
     * @param[in] literalName
     *   Literal Name
     * @param[in] manufacturerCode
     *   Manufacturer Code
     **/
    TargetInformation(
      std::string thwId,
      std::string thwTypeName,
      std::string thwPosition,
      std::string literalName,
      std::string manufacturerCode );

    /**
     * @brief Parses the Target Information from the given property tree.
     *
     * @param[in] properties
     *   Boost Property Tree.
     **/
    explicit TargetInformation( const boost::property_tree::ptree &properties );

    /**
     * @brief Parses the Target Information from the given property tree.
     *
     * @param[in] properties
     *   Boost Property Tree.
     **/
    void fromProperties( const boost::property_tree::ptree &properties );

    /**
     * @brief Converts the Target Information to Boost Property Tree.
     *
     * @return Target Information as Boost Property Tree.
     **/
    [[nodiscard]] boost::property_tree::ptree toProperties() const;

    /**
     * @brief Compare operator against a Target ID.
     *
     * @param[in] rhs
     *  Target ID to compare.
     *
     * @return Comparison esult of Target IDs.
     **/
    inline auto operator <=>( const TargetId &rhs ) const;

    /**
     * @brief Create a Target ID from the given Target Information
     *
     * @return Target ID based on the Target information.
     **/
    [[nodiscard]] TargetId targetId() const;

    //! Target Hardware ID
    std::string thwId;
    //! Target Hardware Type Name
    std::string thwTypeName;
    //! Target Hardware Position
    std::string thwPosition;
    //! Literal Name
    std::string literalName;
    //! Manufacturer Code
    std::string manufacturerCode;
};

inline auto TargetInformation::operator<=>( const TargetId &rhs ) const
{
  return ( targetId() <=> rhs );
}

/**
 * @brief Converts targets address information into a map structure.
 *
 * This function processes the provided targets address information and converts it into a map structure for further
 * use.
 *
 * @param[in] targetsAddressInformation
 *   The input targets address information to be transformed.
 *
 * @return A map containing the transformed targets address information.
 **/
[[nodiscard]] ARINC_615A_EXPORT TargetsAddressInformationMap TargetsAddressInformationMap_fromTargetsAddressInformation(
  const TargetsAddressInformation &targetsAddressInformation );

}

#endif
