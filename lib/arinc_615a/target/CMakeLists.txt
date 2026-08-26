# SPDX-License-Identifier: MPL-2.0

# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required( VERSION 4.3 )

target_sources(
  arinc_615a

  PUBLIC
    FILE_SET HEADERS
      FILES
        DownloadOperation.hpp
        DownloadOperationHandler.hpp
        ErrorOperation.hpp
        ErrorOperationConfiguration.hpp
        InformationOperation.hpp
        InformationOperationHandler.hpp
        MediaDefinedDownloadOperation.hpp
        MediaDefinedDownloadOperationHandler.hpp
        Operation.hpp
        OperationConfiguration.hpp
        OperationHandler.hpp
        OperatorDefinedDownloadOperation.hpp
        OperatorDefinedDownloadOperationHandler.hpp
        Protocol.hpp
        ProtocolConfiguration.hpp
        Target.hpp
        UploadOperation.hpp
        UploadOperationHandler.hpp

  PRIVATE
    Protocol.cpp

    implementation/ErrorOperationImpl.hpp
    implementation/ErrorOperationImpl.cpp
    implementation/InformationOperationImpl.hpp
    implementation/InformationOperationImpl.cpp
    implementation/MediaDefinedDownloadOperationImpl.hpp
    implementation/MediaDefinedDownloadOperationImpl.cpp
    implementation/OperationImpl.hpp
    implementation/OperationImpl.cpp
    implementation/OperatorDefinedDownloadOperationImpl.hpp
    implementation/OperatorDefinedDownloadOperationImpl.cpp
    implementation/ProtocolImpl.hpp
    implementation/ProtocolImpl.cpp
    implementation/UploadOperationImpl.hpp
    implementation/UploadOperationImpl.cpp )

#target_sources(
#  arinc_615a_test
#
#  PRIVATE)
