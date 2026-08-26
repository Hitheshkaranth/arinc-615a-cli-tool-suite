# SPDX-License-Identifier: MPL-2.0

# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required( VERSION 4.3 )

target_sources(
  arinc_615a_commands

  PUBLIC
    FILE_SET HEADERS
      FILES
        AdhocUploadOperationCommand.hpp
        BatchUploadOperationCommand.hpp
        InformationOperationCommand.hpp
        MediaDefinedDownloadOperationCommand.hpp
        Operations.hpp
        OperatorDefinedDownloadOperationCommand.hpp
        UploadLoadsOperationCommand.hpp
        UploadOperationCommand.hpp

  PRIVATE
    AdhocUploadOperationCommand.cpp
    BatchUploadOperationCommand.cpp
    InformationOperationCommand.cpp
    MediaDefinedDownloadOperationCommand.cpp
    Operations.cpp
    OperatorDefinedDownloadOperationCommand.cpp
    UploadLoadsOperationCommand.cpp
    UploadOperationCommand.cpp )
