// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615a::Arinc615aConfiguration.
 **/

#include "Arinc615aConfiguration.hpp"

#include <helper/BoostAsioProgramOptions.hpp>

#include <boost/property_tree/ptree.hpp>

namespace Arinc615a {

Arinc615aConfiguration::Arinc615aConfiguration( const boost::property_tree::ptree &properties )
{
  fromProperties( properties );
}

void Arinc615aConfiguration::fromProperties( const boost::property_tree::ptree &properties )
{
  localInterfaceAddress =
    properties.get_optional< std::string >( "local_tftp_address" )
      .map( []( const auto &address ) {
        return boost::asio::ip::make_address( address );
      } )
      .value_or( localInterfaceAddress );
  dlpRetries = properties.get( "dlp_retries", dlpRetries );
  tftpConfiguration = ::Tftp::TftpConfiguration{};
  if (
    const auto tftpProperties{ properties.get_child_optional( "tftp" ) };
    tftpProperties )
  {
    tftpConfiguration.fromProperties( *tftpProperties );
  }
  tftpOptionsConfiguration = ::Tftp::TftpOptionsConfiguration{};
  if (
    const auto tftpOptionsProperties{ properties.get_child_optional( "tftp_options" ) };
    tftpOptionsProperties )
  {
    tftpOptionsConfiguration.fromProperties( *tftpOptionsProperties );
  }

  protocolFileLogging = properties.get( "protocol_file_logging", protocolFileLogging );
}

boost::property_tree::ptree Arinc615aConfiguration::toProperties( const bool full ) const
{
  boost::property_tree::ptree properties;

  if ( full || ( localInterfaceAddress != boost::asio::ip::address_v4::any() ) )
  {
    properties.add( "local_tftp_address", localInterfaceAddress.to_string() );
  }

  if ( full || ( dlpRetries != DefaultArinc615aDlpRetries ) )
  {
    properties.add( "dlp_retries", dlpRetries );
  }

  if (
    const auto tftpProperties{ tftpConfiguration.toProperties( full ) };
    full || !tftpProperties.empty() )
  {
    properties.add_child( "tftp", tftpProperties );
  }

  if (
    const auto tftpOptionsProperties{ tftpOptionsConfiguration.toProperties( full ) };
    full || !tftpOptionsProperties.empty() )
  {
    properties.add_child( "tftp_options", tftpOptionsProperties );
  }

  if ( full || protocolFileLogging )
  {
    properties.add( "protocol_file_logging", protocolFileLogging );
  }

  return properties;
}

boost::program_options::options_description Arinc615aConfiguration::options()
{
  boost::program_options::options_description options{ "ARINC 615A Options" };

  options.add_options()
  (
    "local-tftp-address",
    boost::program_options::value( &localInterfaceAddress )
      ->value_name( "ip" ),
    "Local interface IP address, where the TFTP server binds to."
  )
  (
    "dlp-retries",
    boost::program_options::value( &dlpRetries )
      ->value_name( "retries" ),
    "DLP retries."
  )
  (
    "log-protocol-files",
    boost::program_options::value( &protocolFileLogging )
      ->implicit_value( true, "true" )
      ->value_name( "true|false" ),
    "Enables Logging of received and transmitted Protocol Files."
  );

  options.add( tftpConfiguration.options() );
  options.add( tftpOptionsConfiguration.options() );

  return options;
}

}
