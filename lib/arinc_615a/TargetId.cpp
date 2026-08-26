// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615a::TargetId.
 **/

#include "TargetId.hpp"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <functional>
#include <istream>
#include <ostream>

namespace Arinc615a {

std::tuple< std::string_view, std::string_view> TargetId::splitTargetId( const std::string_view targetId )
{
  // find the first occurrence of '_'
  const auto underscorePos{ targetId.find( '_' ) };

  if ( std::string_view::npos == underscorePos )
  {
    // '_' not found
    return {};
  }

  return std::make_tuple( targetId.substr( 0U, underscorePos ), targetId.substr( underscorePos + 1U ) );
}

bool TargetId::isTargetId( const std::string_view targetId ) noexcept
{
  const auto [ thwId, position ] = splitTargetId( targetId );

  return isThwId( thwId ) && isPosition( position );
}

bool TargetId::isThwId( const std::string_view thwId ) noexcept
{
  return isCompliant( thwId, ThwIdSizeMin, ThwIdSizeMax );
}

bool TargetId::isPosition( const std::string_view position ) noexcept
{
  return isCompliant( position, PositionSizeMin, PositionSizeMax );
}

TargetId::TargetId( const std::string_view targetId )
{
  std::tie( thwIdV, positionV) = splitTargetId( targetId );
}

TargetId::TargetId( std::string thwId, std::string position ) :
  thwIdV{ std::move( thwId ) },
  positionV{ std::move( position ) }
{
}

std::string_view TargetId::thwId() const
{
  return thwIdV;
}

void TargetId::thwId( std::string thwId )
{
  thwIdV = std::move( thwId );
}

std::string_view TargetId::position() const
{
  return positionV;
}

void TargetId::position( std::string position )
{
  positionV = std::move( position );
}

std::string TargetId::toString() const
{
  return std::format( "{}_{}", thwIdV, positionV );
}

TargetId::operator std::string() const
{
  return toString();
}

TargetId::operator bool() const
{
  return isCompliant( thwIdV, ThwIdSizeMin, ThwIdSizeMax )
      && isCompliant( positionV, PositionSizeMin, PositionSizeMax );
}

TargetId& TargetId::operator=( const std::string_view other )
{
  std::tie( thwIdV, positionV ) = splitTargetId( other );

  return *this;
}

bool TargetId::operator==( const std::string_view other ) const
{
  return toString() == other;
}

bool TargetId::isCompliant( const std::string_view stringToCheck, const size_t minLength, const size_t maxLength )
{
  // assertion due to private function (is not used public)
  assert( minLength <= maxLength );

  // Check length
  if ( ( stringToCheck.size() < minLength ) || ( stringToCheck.size() > maxLength ) )
  {
    return false;
  }

  // Check string for alphanumeric content

  const std::function< bool( std::string::value_type ) > compareFunction{
    std::bind( &std::isalnum< std::string::value_type >, std::placeholders::_1, std::locale::classic() ) };

  return std::ranges::all_of( stringToCheck, compareFunction );
}

std::ostream& operator<<( std::ostream &stream, const TargetId &targetId )
{
  return stream << static_cast< std::string >( targetId );
}

std::istream& operator>>( std::istream &stream, TargetId &targetId )
{
  std::string targetIdStr;
  stream >> targetIdStr;
  targetId = targetIdStr;
  return stream;
}

}
