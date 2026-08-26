# SPDX-License-Identifier: MPL-2.0

# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required( VERSION 4.3 )

target_sources(
  arinc_615a

  PUBLIC
    FILE_SET HEADERS
      FILES
        Operation.hpp
        ReadOperation.hpp
        Server.hpp
        Servers.hpp
        WriteOperation.hpp

  PRIVATE
    Server.cpp
    implementation/ReadOperationImpl.hpp
    implementation/ReadOperationImpl.cpp
    implementation/ServerImpl.hpp
    implementation/ServerImpl.cpp
    implementation/WriteOperationImpl.hpp
    implementation/WriteOperationImpl.cpp )

#target_sources( arinc_615a_test PRIVATE )
