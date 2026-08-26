// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615a::Information::Status.
 **/

#include "Status.hpp"

namespace Arinc615a::Information {

Status::Status(
  const uint16_t counter,
  const uint16_t exceptionTimer,
  const int16_t estimatedTime,
  const StatusCode code,
  std::string description ) :
  counterV{ counter },
  exceptionTimerV{ exceptionTimer },
  estimatedTimeV{ estimatedTime },
  codeV{ code },
  descriptionV{ std::move( description ) }
{
}

uint16_t Status::counter() const noexcept
{
  return counterV;
}

void Status::counter( const uint16_t counter ) noexcept
{
  counterV = counter;
}

void Status::incrementCounter() noexcept
{
  ++counterV;
}

uint16_t Status::exceptionTimer() const noexcept
{
  return exceptionTimerV;
}

void Status::exceptionTimer( const uint16_t exceptionTimer )
{
  exceptionTimerV = exceptionTimer;
}

int16_t Status::estimatedTime() const noexcept
{
  return estimatedTimeV;
}

void Status::estimatedTime( const int16_t estimatedTime )
{
  estimatedTimeV = estimatedTime;
}

StatusCode Status::code() const noexcept
{
  return codeV;
}

void Status::code( StatusCode const code )
{
  codeV = code;
  descriptionV.clear();
}

std::string_view Status::description() const noexcept
{
  return descriptionV;
}

void Status::description( std::string description )
{
  descriptionV = std::move( description );
}

void Status::set( const StatusCode code, std::string description )
{
  codeV = code;
  descriptionV = std::move( description );
}

}
