# SPDX-License-Identifier: MPL-2.0

# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required( VERSION 4.3 )

find_package( spdlog REQUIRED )

add_library( arinc_615a_commands )

include( GenerateExportHeader )
generate_export_header( arinc_615a_commands )

target_sources(
  arinc_615a_commands

  PUBLIC
    FILE_SET HEADERS
      BASE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${CMAKE_CURRENT_BINARY_DIR}/..

      FILES
        Arinc615aCommands.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/arinc_615a_commands_export.h

  PRIVATE
    Arinc615aCommands.cpp )

target_compile_features( arinc_615a_commands PUBLIC cxx_std_23 )

target_compile_options(
  arinc_615a_commands

  PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    #$<$<CXX_COMPILER_ID:MSVC>:/Wall>
    # Disable Warning for exporting classes with std::* private members
    $<$<CXX_COMPILER_ID:MSVC>:/wd4251>
    # Disable Warning for exporting classes, which derives from std::*
    $<$<CXX_COMPILER_ID:MSVC>:/wd4275>

    $<$<CXX_COMPILER_ID:GNU>:-Wall>
    $<$<CXX_COMPILER_ID:GNU>:-Wextra>
    $<$<CXX_COMPILER_ID:GNU>:-Wpedantic>

    # Big Objects for MinGW under Windows or Cross
    $<$<AND:$<PLATFORM_ID:Windows>,$<CXX_COMPILER_ID:GNU>>:-Wa,-mbig-obj>

    $<$<CXX_COMPILER_ID:Clang>:-Wall>
    $<$<CXX_COMPILER_ID:Clang>:-Wextra>
    $<$<CXX_COMPILER_ID:Clang>:-Wpedantic> )

target_link_libraries(
  arinc_615a_commands

  PUBLIC
    arinc_615a
    commands

  PRIVATE
    arinc_665
    spdlog::spdlog_header_only
    # Windows Socket must be explicitly linked in MinGW
    $<$<PLATFORM_ID:Windows>:wsock32 ws2_32> )

add_subdirectory( operations )
add_subdirectory( targets )

set_property(
  DIRECTORY ${PROJECT_SOURCE_DIR}
  APPEND
  PROPERTY DOC_PATHS
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/arinc_615a_commands_export.h )

install(
  TARGETS arinc_615a_commands
  RUNTIME_DEPENDENCY_SET arinc_615a-runtime-deps
  EXPORT arinc_615a_commands-config
  COMPONENT runtime
  ARCHIVE COMPONENT development
  FILE_SET HEADERS COMPONENT development )

install(
  EXPORT arinc_615a_commands-config
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake
  COMPONENT development )
