// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Information::Ratio.
 **/

#include "Ratio.hpp"

#include <arinc_615a/Arinc615aException.hpp>

#include <helper/Exception.hpp>

#include <boost/exception/all.hpp>

#include <iostream>

namespace Arinc615a::Information {

Ratio::Ratio( const unsigned int ratio ) :
  ratioV{ ratio }
{
  if ( ratioV > 100U )
  {
    BOOST_THROW_EXCEPTION( Arinc615aException{} << Helper::AdditionalInfo{ "Ratio  out of range" } );
  }
}

unsigned int Ratio::value() const  noexcept
{
  return ratioV;
}

Ratio::operator unsigned int() const noexcept
{
  return ratioV;
}

Ratio& Ratio::operator =( const unsigned int ratio )
{
  if ( ratio > 100U )
  {
    BOOST_THROW_EXCEPTION( Arinc615aException{} << Helper::AdditionalInfo{ "Ratio out of range" } );
  }

  ratioV = ratio;

  return *this;
}

std::ostream& operator<<( std::ostream &stream, const Ratio &ratio )
{
  return stream << ratio.value();
}

}
