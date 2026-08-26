# SPDX-License-Identifier: MPL-2.0

# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required( VERSION 4.3 )

target_sources(
  arinc_615a

  PUBLIC
    FILE_SET HEADERS
      FILES
        Arinc615aOptions.hpp
        ErrorMessage.hpp
        Tftp.hpp

  PRIVATE
    Arinc615aOptions.cpp
    ErrorMessage.cpp )

target_sources(
  arinc_615a_test
  PRIVATE
    test/Arinc615aOptionsTest.cpp
    test/ErrorMessageTest.cpp )

add_subdirectory( clients )
add_subdirectory( servers )
