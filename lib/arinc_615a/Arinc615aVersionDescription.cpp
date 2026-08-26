// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615a::Arinc615aVersionDescription.
 **/

#include "Arinc615aVersionDescription.hpp"

#include <boost/exception/exception.hpp>

#include <boost/program_options.hpp>

namespace Arinc615a {

Arinc615aVersionDescription::Arinc615aVersionDescription() :
  Description{
    { "Arinc615a1",  Arinc615aVersion::Arinc615a1 },
    { "Arinc615a2",  Arinc615aVersion::Arinc615a2 },
    { "Arinc615a34", Arinc615aVersion::Arinc615a34 }
  }
{
}

std::ostream& operator<<( std::ostream &stream, Arinc615aVersion version )
{
  return ( stream << Arinc615aVersionDescription::instance().name( version ) );
}

std::istream& operator>>( std::istream& stream, Arinc615aVersion &version )
{
  std::string versionStr;

  // extract string from stream
  stream >> versionStr;

  // Decode
  const auto optionalVersion{ Arinc615aVersionDescription::instance().enumeration( versionStr ) };

  if ( !optionalVersion )
  {
    BOOST_THROW_EXCEPTION( boost::program_options::invalid_option_value{ versionStr } );
  }

  version = *optionalVersion;

  return stream;
}

}
