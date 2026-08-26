# SPDX-License-Identifier: MPL-2.0

# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required( VERSION 4.3 )

if( WIN32 )
  # Add Own Library Paths to Directory List
  set( LIBS
    $<TARGET_FILE_DIR:helper>

    $<TARGET_FILE_DIR:arinc_649>

    $<TARGET_FILE_DIR:commands>

    $<TARGET_FILE_DIR:arinc_665>
    $<TARGET_FILE_DIR:arinc_665_commands>

    $<TARGET_FILE_DIR:tftp>

    $<TARGET_FILE_DIR:arinc_615a>
    $<TARGET_FILE_DIR:arinc_615a_commands> )

  set(
    PRE_EXCLUDE_REGEXES
    # Exclude MS Libraries
    [[api-ms-win-.*]]
    [[ext-ms-.*]]
    [[kernel32\.dll]]
    [[hvsifiletrust]] )

  set(
    POST_EXCLUDE_REGEXES
    # Exclude Application Directories
    "$<TARGET_FILE_DIR:arinc_649_check_value>/.*"

    # Exclude windows system32 directory"
    ".*system32.*" )

  # Only built when app/CMakeLists.txt adds the corresponding sub-directory
  if( TARGET arinc_615a_download_request_file )
    list(
      APPEND POST_EXCLUDE_REGEXES
      "$<TARGET_FILE_DIR:arinc_615a_download_request_file>/.*" )
  endif()

  foreach( RUNTIME_DEP_SET IN ITEMS
    helper-runtime-deps
    arinc_649-runtime-deps
    arinc_665-runtime-deps
    commands-runtime-deps
    tftp-runtime-deps
    arinc_615a-runtime-deps )

    install(
      RUNTIME_DEPENDENCY_SET ${RUNTIME_DEP_SET}
        COMPONENT runtime
        DIRECTORIES
          $ENV{PATH}
          # Add our own libraries - will be excluded for installation automatically
          ${LIBS}
        PRE_EXCLUDE_REGEXES ${PRE_EXCLUDE_REGEXES}
        POST_EXCLUDE_REGEXES ${POST_EXCLUDE_REGEXES} )
  endforeach()

endif()

# Set Version to Date + Git Hash if not set
if( NOT CMAKE_PROJECT_VERSION )
  string( TIMESTAMP CMAKE_PROJECT_VERSION "%Y%m%d-${PROJECT_GIT_HASH}" )
endif()

set( CPACK_PACKAGE_NAME "${CMAKE_PROJECT_DESCRIPTION}" )
set( CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${CMAKE_PROJECT_VERSION}" )
set( CPACK_PACKAGE_VENDOR "Thomas Vogt" )
set( CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_DESCRIPTION} )
set( CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR} )
set( CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR} )
set( CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH} )
set( CPACK_PACKAGE_INSTALL_DIRECTORY ${PROJECT_NAME} )
set( CPACK_PACKAGE_CHECKSUM SHA512 )
# copy to .txt to allow automatic .rtf generation with WIX
file( COPY_FILE LICENSE ${CMAKE_CURRENT_BINARY_DIR}/LICENSE.txt )
set( CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_BINARY_DIR}/LICENSE.txt )

set( CPACK_SOURCE_IGNORE_FILES "/cmake-.*" "/\\.idea/" )
set( CPACK_SOURCE_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${CMAKE_PROJECT_VERSION}-Source" )

if( LINUX )
  # Linux specific generator
  set( CPACK_GENERATOR TBZ2 )
  set( CPACK_SOURCE_GENERATOR TBZ2 )
elseif( WIN32 )
  # Windows specific generator
  set( CPACK_GENERATOR ZIP )
  set( CPACK_SOURCE_GENERATOR ZIP )

  list( APPEND CPACK_GENERATOR NSIS64 )
  set( CPACK_NSIS_DISPLAY_NAME ${PROJECT_DESCRIPTION} )
  set( CPACK_NSIS_PACKAGE_NAME ${PROJECT_DESCRIPTION} )
  #set( CPACK_NSIS_IGNORE_LICENSE_PAGE True )

  list( APPEND CPACK_GENERATOR WIX )
  set( CPACK_WIX_ROOT_FEATURE_TITLE ${PROJECT_DESCRIPTION} )
  set( CPACK_WIX_ROOT_FEATURE_DESCRIPTION ${PROJECT_DESCRIPTION} )
  set( CPACK_WIX_UPGRADE_GUID "8b2f55b1-122e-44e7-9cc3-1d6bc4c3724f" )
  set( CPACK_WIX_INSTALL_SCOPE perMachine )
  set( CPACK_WIX_VERSION 4 )
endif()

include( CPack )

cpack_add_component(
  runtime
  DISPLAY_NAME "Runtime Components"
  DESCRIPTION "All necessary executables and configuration files"
  REQUIRED )

cpack_add_component(
  test
  DISPLAY_NAME "Test Components"
  DESCRIPTION "Additional test executables"
  DISABLED )

cpack_add_component(
  development
  DISPLAY_NAME "Development Components"
  DESCRIPTION "Additional files, which are needed for development (headers, libraries and documentation)"
  DISABLED )
