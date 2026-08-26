// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615a::Find::FindConfiguration.
 **/

#include "FindConfiguration.hpp"

#include <helper/BoostAsioProgramOptions.hpp>

#include <boost/property_tree/ptree.hpp>

namespace Arinc615a::Find {

FindConfiguration::FindConfiguration( const boost::property_tree::ptree &properties )
{
  fromProperties( properties );
}

void FindConfiguration::fromProperties( const boost::property_tree::ptree &properties )
{
  localInterfaceAddress = properties.get_optional< std::string >( "local_find_address" )
    .map( []( const auto &address ) { return boost::asio::ip::make_address( address ); } )
    .value_or( localInterfaceAddress );
  findPort = properties.get( "find_port", findPort );
}

boost::property_tree::ptree FindConfiguration::toProperties( const bool full ) const
{
  boost::property_tree::ptree properties;

  if ( full || ( localInterfaceAddress != boost::asio::ip::address_v4::any() ) )
  {
    properties.add( "local_find_address", localInterfaceAddress );
  }

  if ( full || ( findPort != DefaultPort ) )
  {
    properties.add( "find_port", findPort );
  }

  return properties;
}

boost::program_options::options_description FindConfiguration::options()
{
  boost::program_options::options_description options{ "ARINC 615A FIND Options" };

  options.add_options()
  (
    "local-find-address",
    boost::program_options::value( &localInterfaceAddress )
      ->value_name( "ip" ),
    "Local interface address of FIND interface"
  )
  (
    "find-port",
    boost::program_options::value( &findPort )
      ->value_name( "port" ),
    "FIND port"
  );

  return options;
}

}
