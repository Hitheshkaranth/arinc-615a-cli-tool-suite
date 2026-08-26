// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615a::Information::InitializationResponse.
 **/

#include "InitializationResponse.hpp"

#include <arinc_615a/Arinc615aException.hpp>

#include <helper/Exception.hpp>

#include <boost/exception/all.hpp>

namespace Arinc615a::Information {

InitializationResponse::InitializationResponse( const OperationAcceptanceStatusCode code, std::string description ) :
  codeV{ code },
  descriptionV{ std::move( description ) }
{
}

OperationAcceptanceStatusCode InitializationResponse::code() const  noexcept
{
  return codeV;
}

void InitializationResponse::code( const OperationAcceptanceStatusCode code )  noexcept
{
  codeV = code;
}

std::string_view InitializationResponse::description() const  noexcept
{
  return descriptionV;
}

void InitializationResponse::description( std::string description )
{
  descriptionV = std::move( description );
}

void InitializationResponse::validate() const
{
  if ( ( OperationAcceptanceStatusCode::OperationAccepted == codeV ) && !descriptionV.empty() )
  {
    BOOST_THROW_EXCEPTION( Arinc615aException{} << Helper::AdditionalInfo{ "Accepted and description not empty" } );
  }
}

bool InitializationResponse::operator ==( const InitializationResponse &other ) const
{
  return ( codeV == other.codeV ) && ( descriptionV == other.descriptionV );
}

}
