// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615a::Find::TargetInformation.
 **/

#include "TargetInformation.hpp"

#include <boost/property_tree/ptree.hpp>

#include <ranges>

namespace Arinc615a::Find {

TargetsAddressInformation TargetInformation::targetsAddressInformation( const boost::property_tree::ptree &properties )
{
  TargetsAddressInformation information;

  for ( const auto &entry : properties | std::views::values )
  {
    // key is ignored
    information.emplace_back( targetAddressInformation( entry ) );
  }

  return information;
}

boost::property_tree::ptree TargetInformation::targetsAddressInformation( const TargetsAddressInformation &information )
{
  boost::property_tree::ptree properties;

  for ( const auto &targetInformation : information )
  {
    properties.add_child( "target", targetAddressInformation( targetInformation ) );
  }

  return properties;
}

boost::property_tree::ptree TargetInformation::targetsAddressInformation(
  const TargetsAddressInformationMap &information )
{
  //return targetsAddressInformation( TargetsAddressInformation{ std::from_range, information | std::views::values } );
  boost::property_tree::ptree properties;

  // TODO change to from_range construction when available in GCC 15 and Clang > 19
  for ( const auto &targetInformation : information | std::views::values  )
  {
    properties.add_child( "target", targetAddressInformation( targetInformation ) );
  }

  return properties;
}

TargetAddressInformation TargetInformation::targetAddressInformation( const boost::property_tree::ptree &properties )
{
  auto address{ properties.get_optional< std::string >( "address" )
    .map( []( const auto &address ) { return boost::asio::ip::make_address( address ); } )
    .value_or( boost::asio::ip::address_v4::any() ) };
  auto information{ TargetInformation{ properties } };

  return { std::move( address ), std::move( information ) };
}

boost::property_tree::ptree TargetInformation::targetAddressInformation( const TargetAddressInformation &information )
{
  boost::property_tree::ptree properties;

  properties.add( "address", std::get<0 >( information ).to_string() );

  const auto target{ std::get< 1>( information ).toProperties() };

  properties.insert( properties.end(), target.begin(), target.end() );

  return properties;
}

TargetsInformation TargetInformation::targetsInformation( const boost::property_tree::ptree &properties )
{
  TargetsInformation targets{};

  for ( const auto &entry : properties | std::views::values )
  {
    // key is ignored (could be also an array list)
    targets.emplace_back( entry );
  }

  return targets;
}

boost::property_tree::ptree TargetInformation::targetsInformation( const TargetsInformation &targets )
{
  boost::property_tree::ptree properties{};

  for ( const auto &target : targets )
  {
    properties.add_child( "target", target.toProperties() );
  }

  return properties;
}

TargetInformation::TargetInformation(
  std::string thwId,
  std::string thwTypeName,
  std::string thwPosition,
  std::string literalName,
  std::string manufacturerCode ) :
  thwId{ std::move( thwId ) },
  thwTypeName{ std::move( thwTypeName ) },
  thwPosition{ std::move( thwPosition ) },
  literalName{ std::move( literalName ) },
  manufacturerCode{ std::move( manufacturerCode ) }
{
}

TargetInformation::TargetInformation( const boost::property_tree::ptree &properties )
{
  fromProperties( properties );
}

void TargetInformation::fromProperties( const boost::property_tree::ptree &properties )
{
  thwId = properties.get( "thwId", thwId );
  thwTypeName = properties.get( "thwTypeName", thwTypeName );
  thwPosition = properties.get( "thwPosition", thwPosition );
  literalName = properties.get( "literalName", literalName );
  manufacturerCode = properties.get( "manufacturerCode", manufacturerCode );
}

boost::property_tree::ptree TargetInformation::toProperties() const
{
  boost::property_tree::ptree properties;

  properties.add( "thwId", thwId );
  properties.add( "thwTypeName", thwTypeName );
  properties.add( "thwPosition", thwPosition );
  properties.add( "literalName", literalName );
  properties.add( "manufacturerCode", manufacturerCode );

  return properties;
}

TargetId TargetInformation::targetId() const
{
  return { thwId, thwPosition };
}

TargetsAddressInformationMap TargetsAddressInformationMap_fromTargetsAddressInformation(
  const TargetsAddressInformation &targetsAddressInformation )
{
  TargetsAddressInformationMap information;

  for ( const auto &targetAddressInformation : targetsAddressInformation )
  {
    information.emplace( std::get< 1 >( targetAddressInformation ).targetId(), targetAddressInformation );
  }

  return information;
}

}
