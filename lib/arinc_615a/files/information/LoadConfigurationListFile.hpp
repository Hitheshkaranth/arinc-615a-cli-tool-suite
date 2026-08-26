// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Files::LoadConfigurationListFile.
 **/

#ifndef ARINC_615A_FILES_LOADCONFIGURATIONLISTFILE_HPP
#define ARINC_615A_FILES_LOADCONFIGURATIONLISTFILE_HPP

#include <arinc_615a/files/Files.hpp>
#include <arinc_615a/files/ProtocolFile.hpp>

#include <arinc_615a/information/Information.hpp>

namespace Arinc615a::Files {

/**
 * @brief ARINC 615A %Information Operation Load Configuration List file (*.LCL).
 *
 * This file is sent from the *%Target Data Loader* to the host.
 * This file contains the load configuration information of the target.
 *
 * @par File Format
 * Name of Field                      | Field Size (bits) | Field Size (bytes)
 * :----------------------------------|:-----------------:|:-------------------:
 * **File Length**                    | 32                | 4
 * **Protocol %Version**              | 16                | 2
 * **Number of target hardware**      | 16                | 2
 * # **%Target Hardware Code Length** |  8                | 1
 * # **%Target Hardware Code**        |  8 – 2040         | 1 - 255
 * # **Serial Number Length**         |  8                | 1
 * # **Serial Number**                |  8 – 2040         | 1 - 255
 * # **Number of Part Numbers**       | 16                | 2
 * # + **Part Number Length**         |  8                | 1
 * # + **Part Number**                |  8 – 2040         | 1 - 255
 * # + **Amendment Length**           |  8                | 1
 * # + **Amendment**                  |  0 – 2040         | 0 - 255
 * # + **Part Designation Length**    |  8                | 1
 * # + **Part Designation Text**      |  8 – 2040         | 1 - 255
 *
 * @note
 * The items marked with # are repeated for every target hardware.
 * @note
 * The items marked with *+* are repeated for every part number.
 *
 * @sa ARINC 615A-4 Section 6.4.2
 *
 * @sa @ref InitializationFile
 * @sa @ref InformationOperationStatusFile
 **/
class ARINC_615A_EXPORT LoadConfigurationListFile final : public ProtocolFile
{
  public:
    //! Default Construction of Load Configuration List File.
    LoadConfigurationListFile() = default;

    /**
     * @brief Initialises the instance with the given data.
     *
     * @param[in] protocolVersion
     *   ARINC 615A Protocol Version.
     * @param[in] targetsHardware
     *   Targets Hardware.
     **/
    explicit LoadConfigurationListFile(
      Arinc615aVersion protocolVersion,
      Information::TargetsHardware targetsHardware = {} );

    /**
     * @brief Constructs the Load Configuration List File from the given raw data.
     *
     * @param[in] rawData
     *   Raw file data.
     **/
    explicit LoadConfigurationListFile( Helper::ConstRawDataSpan rawData );

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
    LoadConfigurationListFile& operator=( Helper::ConstRawDataSpan rawData );

    /**
     * @name Targets Hardware.
     * @{
     **/

    /**
     * @brief Returns the targets hardware information.
     *
     * @return Targets Hardware Information.
     **/
    [[nodiscard]] const Information::TargetsHardware& targetsHardware() const;

    //! @copydoc targetsHardware() const
    Information::TargetsHardware& targetsHardware();

    /**
     * @brief Set Targets Hardware Information.
     *
     * @param[in] targetsHardware
     *   New Targets Hardware
     **/
    void targetsHardware( Information::TargetsHardware targetsHardware );

    /** @} **/

  protected:
    //! @copydoc ProtocolFile::encode
    [[nodiscard]] Helper::RawData encode() const override;

    /**
     * @brief Decodes the Load Configuration List File from the raw data.
     *
     * @param[in] rawData
     *   Raw data.
     **/
    void decode( Helper::ConstRawDataSpan rawData );

  private:
    //! Target Hardware List.
    Information::TargetsHardware targetsHardwareV;
};

}

#endif
