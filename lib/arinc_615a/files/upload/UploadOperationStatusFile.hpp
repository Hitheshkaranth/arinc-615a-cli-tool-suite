// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Files::UploadOperationStatusFile.
 **/

#ifndef ARINC_615A_FILES_UPLOADOPERATIONSTATUSFILE_HPP
#define ARINC_615A_FILES_UPLOADOPERATIONSTATUSFILE_HPP

#include <arinc_615a/files/Files.hpp>
#include <arinc_615a/files/ProtocolFile.hpp>

#include <arinc_615a/information/UploadStatus.hpp>

namespace Arinc615a::Files {

/**
 * @brief ARINC 615A Upload Operation Status File (*.LUS).
 *
 * This file is sent from the *Target Data Loader* periodically within the *Upload* operation.
 * This file contains the status information of the operation.
 *
 * @par File Format
 * Name of Field                        | Field Size (bits) | Field Size (bytes)
 * :------------------------------------|:-----------------:|:-------------------:
 * **File Length**                      | 32                | 4
 * **Protocol %Version**                | 16                | 2
 * **Upload Operation Status Code**     | 16                | 2
 * **Upload Status Description Length** |  8                | 1
 * **Upload Status Description**        |  0 – 2040         | 0 - 255
 * **Counter**                          | 16                | 2
 * **Exception Timer**                  | 16                | 2
 * **Estimated Time**                   | 16                | 2
 * **Load List Ratio**                  | 24                | 3
 * **Number of Header %Files**          | 16                | 2
 * + **Header File Name Length**        |  8                | 1
 * + **Header File Name**               |  8 – 2040         | 1 - 255
 * + **Load P/N Name Length**           |  8                | 1
 * + **Load P/N Name**                  |  8 – 2040         | 1 - 255
 * + **Load Ratio**                     | 24                | 3
 * + **Load Status**                    | 16                | 2
 * + **Load Status Description Length** |  8                | 1
 * + **Load Status Description**        |  0 – 2040         | 0 - 255
 *
 * @note
 * The items marked with *+* are repeated for every available header file.
 *
 * @sa ARINC 615A-4 Section 6.4.5
 *
 * @sa @ref InitializationFile
 * @sa @ref UploadOperationRequestFile
 **/
class ARINC_615A_EXPORT UploadOperationStatusFile final : public ProtocolFile
{
  public:
    //! Default Construction of Upload Operation Status File.
    UploadOperationStatusFile() = default;

    /**
     * @brief Constructs the Upload Operation Status File from the given parameters.
     *
     * @param[in] protocolVersion
     *   ARINC 615A Protocol Version.
     * @param[in] status
     *   Status Information.
     **/
    explicit UploadOperationStatusFile( Arinc615aVersion protocolVersion, Information::UploadStatus status = {} );

    /**
     * @brief Constructs the Upload Operation Status File from the given Raw Data.
     *
     * @param[in] rawData
     *   Raw Status File.
     **/
    explicit UploadOperationStatusFile( Helper::ConstRawDataSpan rawData );

    /**
     * @brief Assigns Raw Data to the Upload Operation Status File.
     *
     * Tries to decode the given raw data as this file type.
     *
     * @param[in] rawData
     *   Raw data file.
     *
     * @return *this
     **/
    UploadOperationStatusFile& operator=( Helper::ConstRawDataSpan rawData );

    /**
     * @name Upload Status Information.
     * @{
     **/

    /**
     * @brief Returns the Status Information.
     *
     * @return Status Information.
     **/
    [[nodiscard]] const Information::UploadStatus& status() const;

    //! @copydoc status() const
    [[nodiscard]] Information::UploadStatus& status();

    /**
     * @brief Updates the Status Information.
     *
     * @param[in] status
     *   Status Information.
     **/
    void status( Information::UploadStatus status );

    /** @} **/

  protected:
    //! @copydoc ProtocolFile::encode
    [[nodiscard]] Helper::RawData encode() const override;

    /**
     * @brief Decodes the Upload Operations Status File from the raw data.
     *
     * @param[in] rawData
     *   Raw data.
     **/
    void decode( Helper::ConstRawDataSpan rawData );

  private:
    //! Upload Status information
    Information::UploadStatus statusV;
};

}

#endif
