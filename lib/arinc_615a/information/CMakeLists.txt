# SPDX-License-Identifier: MPL-2.0

# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required( VERSION 4.3 )

target_sources(
  arinc_615a

  PUBLIC
    FILE_SET HEADERS
      FILES
        DownloadFileInformation.hpp
        DownloadFileStatus.hpp
        DownloadStatus.hpp
        Information.hpp
        InitializationResponse.hpp
        PartNumber.hpp
        Ratio.hpp
        Status.hpp
        TargetHardware.hpp
        UploadLoad.hpp
        UploadLoadStatus.hpp
        UploadStatus.hpp

  PRIVATE
    DownloadFileStatus.cpp
    DownloadStatus.cpp
    InitializationResponse.cpp
    PartNumber.cpp
    Ratio.cpp
    Status.cpp
    TargetHardware.cpp
    UploadLoadStatus.cpp
    UploadStatus.cpp )

target_sources(
  arinc_615a_test

  PRIVATE
    test/InitializationResponseTest.cpp
    test/RatioTest.cpp )
