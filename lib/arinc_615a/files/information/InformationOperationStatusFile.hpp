// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Files::InformationOperationStatusFile.
 **/

#ifndef ARINC_615A_FILES_INFORMATIONOPERATIONSTATUSFILE_HPP
#define ARINC_615A_FILES_INFORMATIONOPERATIONSTATUSFILE_HPP

#include <arinc_615a/files/Files.hpp>
#include <arinc_615a/files/ProtocolFile.hpp>

#include <arinc_615a/information/Status.hpp>

namespace Arinc615a::Files {

/**
 * @brief ARINC 615A %Information Operation Status File (*.LCS).
 *
 * This file is sent from the *Target Data Loader* periodically within the *Information* operation.
 * This file contains the status information of the operation.
 *
 * @par File Format
 * Name of Field                         | Field Size (bits) | Field Size (bytes)
 * :-------------------------------------|:-----------------:|:-------------------:
 * **File Length**                       | 32                | 4
 * **Protocol %Version**                 | 16                | 2
 * **Counter**                           | 16                | 2
 * **Information Operation Status Code** | 16                | 2
 * **Exception Timer**                   | 16                | 2
 * **Estimated Time**                    | 16                | 2
 * **Status Description Length**         |  8                | 1
 * **Status Description**                |  0 – 2040         | 0 - 255
 *
 * @sa ARINC 615A-4 Section 6.4.3
 *
 * @sa @ref InitializationFile
 * @sa @ref LoadConfigurationListFile
 **/
class ARINC_615A_EXPORT InformationOperationStatusFile final : public ProtocolFile
{
  public:
    //! Default Construction of Information Operation Status File.
    InformationOperationStatusFile() = default;

    /**
     * @brief Constructs the status file from the given parameters.
     *
     * @param[in] protocolVersion
     *   ARINC 615A Protocol Version.
     * @param[in] status
     *   Status Information.
     **/
    explicit InformationOperationStatusFile(
      Arinc615aVersion protocolVersion,
      Information::InformationStatus status = {} );

    /**
     * @brief Constructs the status file from the given raw data.
     *
     * @param[in] rawData
     *   Raw Status File.
     **/
    explicit InformationOperationStatusFile( Helper::ConstRawDataSpan rawData );

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
    InformationOperationStatusFile& operator=( Helper::ConstRawDataSpan rawData );

    /**
     * @name Status Information.
     * @{
     **/

    /**
     * @brief Returns the Status Information.
     *
     * @return Status Information.
     **/
    [[nodiscard]] const Information::InformationStatus& status() const;

    //! @copydoc status() const
    Information::InformationStatus& status();

    /**
     * @brief Updates the Status Information.
     *
     * @param[in] status
     *   Status Information.
     **/
    void status( Information::InformationStatus status );

    /** @} **/

  protected:
    //! @copydoc ProtocolFile::encode
    [[nodiscard]] Helper::RawData encode() const override;

    /**
     * @brief Decodes the Information Operations Status File from the raw data.
     *
     * @param[in] rawData
     *   Raw data.
     **/
    void decode( Helper::ConstRawDataSpan rawData );

  private:
    //! Information Operation Status
    Information::InformationStatus statusV;
};

}

#endif
