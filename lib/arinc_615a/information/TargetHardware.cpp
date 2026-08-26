// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615a::Information::TargetHardware.
 **/

#include "TargetHardware.hpp"

#include <boost/property_tree/ptree.hpp>

#include <format>
#include <ranges>

namespace Arinc615a::Information {

TargetsHardware TargetHardware::targetsHardware( const boost::property_tree::ptree &properties )
{
  TargetsHardware list;

  for ( const auto &value : properties | std::views::values )
  {
    list.emplace_back( value );
  }

  return list;
}

boost::property_tree::ptree TargetHardware::targetsHardware( const TargetsHardware &targetsHardware )
{
  boost::property_tree::ptree properties;

  for ( const auto &targetHardware : targetsHardware )
  {
    properties.add_child( "target_hardware", targetHardware.toProperties() );
  }

  return properties;
}

TargetHardware::TargetHardware( std::string literalName, std::string serialNumber, PartNumbers partNumbers ) :
  literalName{ std::move( literalName ) },
  serialNumber{ std::move( serialNumber ) },
  partNumbers{ std::move( partNumbers ) }
{
}

TargetHardware::TargetHardware( const boost::property_tree::ptree &properties )
{
  fromProperties( properties );
}

void TargetHardware::fromProperties( const boost::property_tree::ptree &properties )
{
  literalName = properties.get( "literal_name", std::string() );
  serialNumber = properties.get( "serial_number", std::string() );
  partNumbers.clear();
  if ( const auto partNumbersProperties{ properties.get_child_optional( "part_numbers" ) }; partNumbersProperties )
  {
    partNumbers = PartNumber::partNumbers( *partNumbersProperties );
  }
}

boost::property_tree::ptree TargetHardware::toProperties() const
{
  boost::property_tree::ptree properties{};

  properties.add( "literal_name", literalName );
  properties.add( "serial_number", serialNumber );
  properties.add_child( "part_numbers", PartNumber::partNumbers( partNumbers ) );

  return properties;
}

std::string TargetHardware::toString( std::string_view firstIndention, std::string_view secondIndention ) const
{
  std::string output{ "Target Hardware Information:\n" };

  std::format_to(
    std::back_inserter( output ),
    "{}Literal Name:  '{}'\n",
    firstIndention,
    literalName );
  std::format_to(
    std::back_inserter( output ),
    "{}Serial Number: '{}'\n",
    firstIndention,
    serialNumber );

  for ( const auto &partNumber : partNumbers )
  {
    std::format_to(
      std::back_inserter( output ),
      "{}Part Number Information:\n", firstIndention );

    std::format_to(
      std::back_inserter( output ),
      "{}{}Part Number:      '{}'\n",
      firstIndention,
      secondIndention,
      partNumber.partNumber );

    std::format_to(
      std::back_inserter( output ),
      "{}{}Amendment:        '{}'\n",
      firstIndention,
      secondIndention,
      partNumber.amendment );

    std::format_to(
      std::back_inserter( output ),
      "{}{}Part Designation: '{}'\n",
      firstIndention,
      secondIndention,
      partNumber.partDesignation );
  }

  return output;
}

}
