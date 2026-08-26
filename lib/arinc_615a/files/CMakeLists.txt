# SPDX-License-Identifier: MPL-2.0

# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required( VERSION 4.3 )

target_sources(
  arinc_615a

  PUBLIC
    FILE_SET HEADERS
      FILES
        Files.hpp
        InitializationFile.hpp
        ProtocolFile.hpp
        ProtocolFileLogger.hpp
        ProtocolFilename.hpp
        ProtocolFileStatistic.hpp
        ProtocolFileTypeDescription.hpp
        Ratio.hpp
        String.hpp

        information/InformationOperationStatusFile.hpp
        information/LoadConfigurationListFile.hpp

        download/DownloadOperationStatusFile.hpp
        download/DownloadOperationRequestFile.hpp
        download/DownloadOperationListFile.hpp
        download/DownloadOperationAnswerFile.hpp

        upload/UploadOperationRequestFile.hpp
        upload/UploadOperationStatusFile.hpp

  PRIVATE
    InitializationFile.cpp
    ProtocolFile.cpp
    ProtocolFileLogger.cpp
    ProtocolFilename.cpp
    ProtocolFileStatistic.cpp
    ProtocolFileTypeDescription.cpp
    Ratio.cpp
    String.cpp

    information/InformationOperationStatusFile.cpp
    information/LoadConfigurationListFile.cpp

    download/DownloadOperationStatusFile.cpp
    download/DownloadOperationRequestFile.cpp
    download/DownloadOperationListFile.cpp
    download/DownloadOperationAnswerFile.cpp

    upload/UploadOperationRequestFile.cpp
    upload/UploadOperationStatusFile.cpp )

target_sources(
  arinc_615a_test

  PRIVATE
    test/InitializationFileTest.cpp
    test/ProtocolFilenameTest.cpp
    test/RatioTest.cpp
    test/StringTest.cpp

    download/test/DownloadOperationAnswerFileTest.cpp
    download/test/DownloadOperationListFileTest.cpp
    download/test/DownloadOperationRequestFileTest.cpp
    download/test/DownloadOperationStatusFileTest.cpp

    information/test/InformationOperationStatusFileTest.cpp
    information/test/LoadConfigurationListFileTest.cpp

    upload/test/UploadOperationRequestFileTest.cpp
    upload/test/UploadOperationStatusFileTest.cpp )
