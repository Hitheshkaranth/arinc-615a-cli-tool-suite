# SPDX-License-Identifier: MPL-2.0

# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required( VERSION 4.3 )

find_package( Boost REQUIRED )
find_package( spdlog REQUIRED )

# prepare Version.cpp
configure_file( Version.hpp.in Version.hpp )

add_library( arinc_615a )

include( GenerateExportHeader )
generate_export_header( arinc_615a )

target_sources(
  arinc_615a

  PUBLIC
    FILE_SET HEADERS
      BASE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${CMAKE_CURRENT_BINARY_DIR}/..

      FILES
        Arinc615a.hpp
        Arinc615aConfiguration.hpp
        Arinc615aException.hpp
        Arinc615aVersionDescription.hpp
        OperationTypeDescription.hpp
        StatusCode.hpp
        StatusCodeDescription.hpp
        TargetId.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/arinc_615a_export.h
        ${CMAKE_CURRENT_BINARY_DIR}/Version.hpp

  PRIVATE
    Arinc615a.cpp
    Arinc615aConfiguration.cpp
    Arinc615aVersionDescription.cpp
    OperationTypeDescription.cpp
    StatusCode.cpp
    StatusCodeDescription.cpp
    TargetId.cpp )

target_compile_features( arinc_615a PUBLIC cxx_std_23 )

target_compile_definitions(
  arinc_615a

  PUBLIC
    # deactivate Boost ASIO deprecated interfaces
    BOOST_ASIO_NO_DEPRECATED

  PRIVATE
    # Disable C++17 deprecation warning for BOOST in MSVC (Version 1.67)
    $<$<CXX_COMPILER_ID:MSVC>:_SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING>
    # Activate STL assertions
    $<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:Debug>>:_GLIBCXX_ASSERTIONS>
    $<$<AND:$<CXX_COMPILER_ID:Clang>,$<CONFIG:Debug>>:_GLIBCXX_ASSERTIONS> )

target_compile_options(
  arinc_615a

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

    $<$<CXX_COMPILER_ID:Clang>:-Wall>
    $<$<CXX_COMPILER_ID:Clang>:-Wextra>
    $<$<CXX_COMPILER_ID:Clang>:-Wpedantic> )

target_link_libraries(
  arinc_615a

  PUBLIC
    tftp
    arinc_649
    helper
    Boost::boost

  PRIVATE
    spdlog::spdlog_header_only
    # Windows Socket must be explicitly linked in MinGW
    $<$<PLATFORM_ID:Windows>:wsock32 ws2_32> )

add_library( arinc_615a_test OBJECT EXCLUDE_FROM_ALL )

target_sources(
  arinc_615a_test

  PRIVATE
    test/StatusCodeTest.cpp
    test/TargetIdTest.cpp
    test/VersionTest.cpp )

target_compile_features( arinc_615a_test PUBLIC cxx_std_23 )

target_compile_options(
  arinc_615a_test

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

    $<$<CXX_COMPILER_ID:Clang>:-Wall>
    $<$<CXX_COMPILER_ID:Clang>:-Wextra>
    $<$<CXX_COMPILER_ID:Clang>:-Wpedantic>
    # Disable new Clang-22 Warning for usage of c2y-extensions (__COUNTER__) within boost::test
    $<$<CXX_COMPILER_ID:Clang>:-Wno-c2y-extensions> )

target_link_libraries(
  arinc_615a_test

  PRIVATE
    arinc_615a
    # Windows Socket must be explicitly linked in MinGW
    $<$<PLATFORM_ID:Windows>:wsock32 ws2_32> )

add_subdirectory( find )
add_subdirectory( files )
add_subdirectory( information )
add_subdirectory( tftp )
add_subdirectory( host )
add_subdirectory( target )

set_property(
  DIRECTORY ${PROJECT_SOURCE_DIR}
  APPEND
  PROPERTY DOC_PATHS
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/arinc_615a_export.h
    ${CMAKE_CURRENT_BINARY_DIR}/Version.hpp )

install(
  TARGETS arinc_615a
  RUNTIME_DEPENDENCY_SET arinc_615a-runtime-deps
  EXPORT arinc_615a-config
  COMPONENT runtime
  ARCHIVE COMPONENT development
  FILE_SET HEADERS COMPONENT development )

install(
  EXPORT arinc_615a-config
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake
  COMPONENT development )
