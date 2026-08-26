// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Information::PartNumber.
 **/

#ifndef ARINC_615A_INFORMATION_PARTNUMBER_HPP
#define ARINC_615A_INFORMATION_PARTNUMBER_HPP

#include <arinc_615a/information/Information.hpp>

#include <boost/property_tree/ptree_fwd.hpp>

#include <string>

namespace Arinc615a::Information {

/**
 * @brief Part Number information, which is part of the load configuration list.
 *
 * This information is provided during the _ARINC 615A Information Operation_.
 *
 * @ref Arinc615a::Information::TargetHardware
 **/
struct ARINC_615A_EXPORT PartNumber final
{
  public:
    /**
     * @brief Load part numbers from a property tree.
     *
     * @param[in] properties
     *   Property Tree.
     *
     * @return Part Number List.
     **/
    [[nodiscard]] static PartNumbers partNumbers( const boost::property_tree::ptree &properties );

    /**
     * @brief Returns the Part Numbers' %Information as boost property tree.
     *
     * @param[in] partNumbers
     *   Part numbers information to export.
     *
     * @return Part numbers information as boost property tree.
     **/
    [[nodiscard]] static boost::property_tree::ptree partNumbers( const PartNumbers &partNumbers );

    //! Default Constructs Part Number
    PartNumber() = default;

    /**
     * @brief Constructs Part Number with given Parameters.
     *
     * @param[in] partNumber
     *   Part Number
     * @param[in] amendment
     *   Amendment
     * @param[in] partDesignation
     *   Part Designation.
     **/
    PartNumber( std::string partNumber, std::string amendment, std::string partDesignation );

    /**
     * @brief Initialise part number from given property tree.
     *
     * @param[in] properties
     *   Property tree.
     **/
    explicit PartNumber( const boost::property_tree::ptree &properties );

    /**
     * @brief Exports the part number information to a boost property tree.
     *
     * @return Part number information as boost property tree.
     **/
    [[nodiscard]] boost::property_tree::ptree toProperties() const;

    //! Part Number.
    std::string partNumber;
    /**
     * @brief Amendment.
     *
     * Amendment is usually related to hardware identification, permitting the tracking of modification for form, fit,
     * and function that does not impact interchangeability and interoperability.
     **/
    std::string amendment;
    /**
     * @brief Part Designation.
     *
     * *%Target Hardware* name (acronyms or code) for the hardware parts (e.g., `FMGEC 1`), and function name or
     * designation for the software parts (e.g., `FMS 1 OPS`), identifying the specific part and occurrence in the
     * aircraft to be displayed.
     **/
    std::string partDesignation;
};

}

#endif
