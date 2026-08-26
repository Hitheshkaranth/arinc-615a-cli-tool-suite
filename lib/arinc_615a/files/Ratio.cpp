// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Module Arinc615a::Files Ratio.
 **/

#include "Ratio.hpp"

#include <arinc_615a/information/Ratio.hpp>

#include <arinc_615a/Arinc615aException.hpp>

#include <helper/Exception.hpp>

#include <boost/throw_exception.hpp>

namespace Arinc615a::Files {

std::tuple< Helper::ConstRawDataSpan, Information::Ratio > Ratio_decode( const Helper::ConstRawDataSpan rawData )
{
  if ( rawData.size() < 3U )
  {
    BOOST_THROW_EXCEPTION( Arinc615aException{} << Helper::AdditionalInfo{ "Ratio string size invalid" } );
  }

  unsigned int ratio{};
  auto [ remaining, ratioStr ]{ Helper::RawData_getString( rawData, 3U ) };
  try
  {
    ratio = static_cast< unsigned int >( std::stoul( std::string{ ratioStr } ) );
  }
  catch ( const std::exception &e )
  {
    BOOST_THROW_EXCEPTION( Arinc615aException{} << Helper::AdditionalInfo{ e.what() } );
  }

  if ( ratio > 100U )
  {
    BOOST_THROW_EXCEPTION( Arinc615aException{} << Helper::AdditionalInfo{ "Ratio value out of range" } );
  }

  return { remaining, Information::Ratio{ ratio } };
}

[[nodiscard]] Helper::RawData Ratio_encode( const Information::Ratio &ratio )
{
  assert( ratio.value() <= 100U );

  const auto ratioString{ std::format( "{:3}", ratio.value() ) };
  assert( ratioString.size() == 3U );

  Helper::RawData rawRatio( 3 );
  Helper::RawData_setString( rawRatio, ratioString );

  return rawRatio;
}

}
