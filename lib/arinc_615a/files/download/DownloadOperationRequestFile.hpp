// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Files::DownloadOperationRequestFile.
 **/

#ifndef ARINC_615A_FILES_DOWNLOADOPERATIONREQUESTFILE_HPP
#define ARINC_615A_FILES_DOWNLOADOPERATIONREQUESTFILE_HPP

#include <arinc_615a/files/Files.hpp>
#include <arinc_615a/files/ProtocolFile.hpp>

#include <arinc_615a/information/Information.hpp>

namespace Arinc615a::Files {

/**
 * @brief ARINC 615A Media Defined Download Operation Request File (*.LNR).
 *
 * This file is sent from the *Host Data Loader Application* within the *Media Defined Download* operation.
 * This file contains the information of files to be downloaded.
 *
 * @par File Format
 * Name of Field                | Field Size (bits) | Field Size (bytes)
 * :----------------------------|:-----------------:|:------------------:
 * **File Length**              | 32                | 4
 * **Protocol %Version**        | 16                | 2
 * **Number of %Files**         | 16                | 2
 * + **File Name Length**       |  8                | 1
 * + **File Name**              |  8 – 2040         | 1 - 255
 * **User Defined Data Length** |  8                | 1
 * **User Defined Data**        |  0 – 2040         | 0 - 255
 *
 * @note
 * The items marked with *+* are repeated for every available file.
 *
 * @sa ARINC 615A-4 Section 6.4.4
 *
 * @sa @ref InitializationFile
 * @sa @ref DownloadOperationStatusFile
 **/
class ARINC_615A_EXPORT DownloadOperationRequestFile final : public ProtocolFile
{
  public:
    //! Minimum size of this file. It's the header size + number of files field + user defined data length.
    static constexpr std::size_t MinimumSize{ HeaderSize + 3UZ };

    //! Default Construction of Download Operation Request File.
    DownloadOperationRequestFile() = default;

    /**
     * @brief Creates the Download Operation Request File.
     *
     * @param[in] protocolVersion
     *   ARINC 615A Protocol Version.
     * @param[in] files
     *   Download Request File Names.
     * @param[in] userDefinedData
     *   User Defined Data.
     **/
    explicit DownloadOperationRequestFile(
      Arinc615aVersion protocolVersion,
      Information::DownloadFiles files = {},
      Helper::RawData userDefinedData = {} );

    /**
     * @brief Constructs the status file from the given raw data.
     *
     * @param[in] rawData
     *   Raw Download Request File.
     **/
    explicit DownloadOperationRequestFile( Helper::ConstRawDataSpan rawData );

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
    DownloadOperationRequestFile& operator=( Helper::ConstRawDataSpan rawData );

    /**
     * @name Downloadable Files.
     * @{
     **/

    /**
     * @brief Returns the Download Request Information.
     *
     * @return Download Request Information.
     **/
    [[nodiscard]] const Information::DownloadFiles& files() const;

    /**
     * @brief Returns the Download Request Information.
     *
     * @return Download Request Information.
     **/
    [[nodiscard]] Information::DownloadFiles& files();

    /**
     * @brief Updates the Download Request Information.
     *
     * @param[in] files
     *   Download Request File Names
     **/
    void files( Information::DownloadFiles files );

    /**
     * @brief Add Request to List.
     *
     * @param[in] file
     *   Download File.
     **/
    void file( std::string file );

    /** @} **/

    /**
     * @name User Defined Data.
     * @{
     **/

    /**
     * @brief Returns the User Defined Data.
     *
     * @return User Defined Data.
     **/
    [[nodiscard]] Helper::ConstRawDataSpan userDefinedData() const;

    //! @copydoc userDefinedData() const
    Helper::RawData& userDefinedData();

    /**
     * @brief Updates the User Defined Data.
     *
     * @param[in] userDefinedData
     *   New User Defined Data.
     **/
    void userDefinedData( Helper::RawData userDefinedData );

    /** @} **/

  protected:
    //! @copydoc ProtocolFile::encode
    [[nodiscard]] Helper::RawData encode() const override;

    /**
     * @brief Decodes the Download Operations Request File from the raw data.
     *
     * @param[in] rawData
     *   Raw data.
     *
     * @throw Arinc615aException
     *   When the number of files field is `0`.
     * @throw Arinc615aException
     *   If user defined data length is invalid
     **/
    void decode( Helper::ConstRawDataSpan rawData );

  private:
    //! Download Files
    Information::DownloadFiles filesV;
    //! User Defined Data.
    Helper::RawData userDefinedDataV;
};

}

#endif
