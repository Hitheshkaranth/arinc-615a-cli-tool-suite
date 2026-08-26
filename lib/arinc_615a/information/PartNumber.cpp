// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615a::Information::PartNumber.
 **/

#include "PartNumber.hpp"

#include <boost/property_tree/ptree.hpp>

#include <ranges>

namespace Arinc615a::Information {

PartNumbers PartNumber::partNumbers( const boost::property_tree::ptree &properties )
{
  PartNumbers partNumbers;

  for ( const auto &partNumber : properties | std::views::values )
  {
    partNumbers.emplace_back( partNumber );
  }

  return partNumbers;
}

boost::property_tree::ptree PartNumber::partNumbers( const PartNumbers &partNumbers )
{
  boost::property_tree::ptree properties;

  for ( const auto &partNumber : partNumbers )
  {
    properties.add_child( "part_number", partNumber.toProperties() );
  }

  return properties;
}

PartNumber::PartNumber( std::string partNumber, std::string amendment, std::string partDesignation ) :
  partNumber{ std::move( partNumber ) },
  amendment{ std::move( amendment ) },
  partDesignation{ std::move( partDesignation ) }
{
}

PartNumber::PartNumber( const boost::property_tree::ptree &properties ) :
  partNumber{ properties.get( "part_number", std::string() ) },
  amendment{ properties.get( "amendment", std::string() ) },
  partDesignation{ properties.get( "part_designation", std::string() ) }
{
}

boost::property_tree::ptree PartNumber::toProperties() const
{
  boost::property_tree::ptree ptree{};

  ptree.add( "part_number", partNumber );
  ptree.add( "amendment", amendment );
  ptree.add( "part_designation", partDesignation );

  return ptree;
}

}
