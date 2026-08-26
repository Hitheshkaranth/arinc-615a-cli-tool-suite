// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Files::DownloadOperationListFile.
 **/

#ifndef ARINC_615A_FILES_DOWNLOADOPERATIONLISTFILE_HPP
#define ARINC_615A_FILES_DOWNLOADOPERATIONLISTFILE_HPP

#include <arinc_615a/files/Files.hpp>
#include <arinc_615a/files/ProtocolFile.hpp>

#include <arinc_615a/information/DownloadFileInformation.hpp>

namespace Arinc615a::Files {

/**
 * @brief ARINC 615A Operator Defined Download Operation List File (*.LNL).
 *
 * This file is sent from the *Target Data Loader* within the *Operator Defined Download* operation.
 * This file contains the information of downloadable files from the target.
 *
 * @par File Format
 * Name of Field                 | Field Size (bits) | Field Size (bytes)
 * :-----------------------------|:-----------------:|:------------------:
 * **File Length**               | 32                | 4
 * **Protocol %Version**         | 16                | 2
 * **Number of %Files**          | 16                | 2
 * + **File Name Length**        |  8                | 1
 * + **File Name**               |  8 – 2040         | 1 - 255
 * + **File Description Length** |  8                | 1
 * + **File Description**        |  8 – 2040         | 1 - 255
 *
 * @note
 * The items marked with *+* are repeated for every available file.
 *
 * @sa ARINC 615A-4 Section 6.4.8
 *
 * @sa @ref InitializationFile
 * @sa @ref DownloadOperationStatusFile
 * @sa @ref DownloadOperationAnswerFile
 **/
class ARINC_615A_EXPORT DownloadOperationListFile final : public ProtocolFile
{
  public:
    //! Minimum size of this file. It's the header size + number of files field).
    static constexpr auto MinimumSize{ HeaderSize + sizeof( uint16_t ) };

    //! Default constructor
    DownloadOperationListFile() = default;

    /**
     * @brief Constructs the list file from the given parameters.
     *
     * @param[in] protocolVersion
     *   ARINC 615A Protocol Version.
     * @param[in] files
     *   Download Files Information.
     **/
    explicit DownloadOperationListFile(
      Arinc615aVersion protocolVersion,
      Information::DownloadFilesInformation files = {} );

    /**
     * @brief Constructs the list file from the given raw data.
     *
     * @param[in] rawData
     *   Raw Status File.
     **/
    explicit DownloadOperationListFile( Helper::ConstRawDataSpan rawData );

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
    DownloadOperationListFile& operator=( Helper::ConstRawDataSpan rawData );

    /**
     * @name Downloadable Files.
     * @{
     **/

    /**
     * @brief Returns the Download Information List.
     *
     * @return Download Information List.
     **/
    [[nodiscard]] const Information::DownloadFilesInformation& files() const;

    //! @copydoc DownloadOperationListFile::files() const
    Information::DownloadFilesInformation& files();

    /**
     * @brief Updates the Download Information List.
     *
     * @param[in] files
     *   New download information files list.
     **/
    void files( Information::DownloadFilesInformation files );

    /**
     * @brief Add file entry.
     *
     * @param[in] file
     *   File information.
     **/
    void file( Information::DownloadFileInformation file );

    /**
     * @brief Add file entry.
     *
     * @param[in] filename
     *   Filename.
     * @param[in] description
     *   File description.
     **/
    void file( std::string filename, std::string description = {} );

    /** @} **/

  protected:
    //! @copydoc ProtocolFile::encode
    [[nodiscard]] Helper::RawData encode() const override;

    /**
     * @brief Decodes the body from the raw data.
     *
     * @param[in] rawData
     *   Raw data.
     *
     * @throw Arinc615aException
     *   The File size is too small.
     * @throw Arinc615aException
     *   The number of header files field is 0.
     *   @throw Arinc615aException
     *   String decoding fails.
     **/
    void decode( Helper::ConstRawDataSpan rawData );

  private:
    //! Download information list.
    Information::DownloadFilesInformation filesV;
};

}

#endif
