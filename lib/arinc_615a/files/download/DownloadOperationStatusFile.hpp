// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Files::DownloadOperationStatusFile.
 **/

#ifndef ARINC_615A_FILES_UPLOADOPERATIONSTATUSFILE_HPP
#define ARINC_615A_FILES_UPLOADOPERATIONSTATUSFILE_HPP

#include <arinc_615a/files/Files.hpp>
#include <arinc_615a/files/ProtocolFile.hpp>

#include <arinc_615a/information/DownloadStatus.hpp>

namespace Arinc615a::Files {

/**
 * @brief ARINC 615A Download Operation Status File (*.LNS).
 *
 * This file is sent from the *Target Data Loader* periodically within the *Download* operation.
 * This file contains the status information of the operation.
 *
 * @par File Format
 * Name of Field                          | Field Size (bits) | Field Size (bytes)
 * :--------------------------------------|:-----------------:|:-------------------:
 * **File Length**                        | 32                | 4
 * **Protocol %Version**                  | 16                | 2
 * **Download Operation Status Code**     | 16                | 2
 * **Download Status Description Length** |  8                | 1
 * **Download Status Description**        |  0 – 2040         | 0 - 255
 * **Counter**                            | 16                | 2
 * **Exception Timer**                    | 16                | 2
 * **Estimated Time**                     | 16                | 2
 * **Download List Ratio**                | 24                | 3
 * **Number of %Files**                   | 16                | 2
 * + **File Name Length**                 |  8                | 1
 * + **File Name**                        |  8 – 2040         | 1 - 255
 * + **File Status**                      | 16                | 2
 * + **File Status Description Length**   |  8                | 1
 * + **File Status Description**          |  0 – 2040         | 0 - 255
 *
 * @note
 * The items marked with *+* are repeated for every available file.
 *
 * @sa ARINC 615A-4 Section 6.4.7
 *
 * @sa @ref InitializationFile
 * @sa @ref DownloadOperationListFile
 * @sa @ref DownloadOperationAnswerFile
 * @sa @ref DownloadOperationRequestFile
 **/
class ARINC_615A_EXPORT DownloadOperationStatusFile final : public ProtocolFile
{
  public:
    //! Default Construction of Download Operation Status File.
    DownloadOperationStatusFile() = default;

    /**
     * @brief Constructs the status file from the given parameters.
     *
     * @param[in] protocolVersion
     *   ARINC 615A Protocol Version.
     * @param[in] status
     *   Download Status Information.
     **/
    explicit DownloadOperationStatusFile( Arinc615aVersion protocolVersion, Information::DownloadStatus status = {} );

    /**
     * @brief Constructs the status file from the given raw data.
     *
     * @param[in] rawData
     *   Raw Status File.
     **/
    explicit DownloadOperationStatusFile( Helper::ConstRawDataSpan rawData );

    /**
     * @brief Assigns raw data to the file.
     *
     * Tries to decode the given raw data as this file type.
     *
     * @param[in] rawData
     *   Raw data file.
     *
     * @return *this
     **/
    DownloadOperationStatusFile& operator=( Helper::ConstRawDataSpan rawData );

    /**
     * @name Status Information.
     * @{
     **/

    /**
     * @brief Returns the Status Information.
     *
     * @return Status Information.
     **/
    [[nodiscard]] const Information::DownloadStatus& status() const;

    //! @copydoc status() const
    Information::DownloadStatus& status();

    /**
     * @brief Updates the Status Information.
     *
     * @param[in] status
     *   Status Information.
     **/
    void status( Information::DownloadStatus status );

    /** @} **/

  protected:
    //! @copydoc ProtocolFile::encode
    [[nodiscard]] Helper::RawData encode() const override;

    /**
     * @brief Decodes the Download Operations Status File from the raw data.
     *
     * @param[in] rawData
     *   Raw data.
     **/
    void decode( Helper::ConstRawDataSpan rawData );

  private:
    //! Download Status information.
    Information::DownloadStatus statusV;
};

}

#endif
