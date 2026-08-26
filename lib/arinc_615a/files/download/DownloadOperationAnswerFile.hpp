// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Files::DownloadOperationAnswerFile.
 **/

#ifndef ARINC_615A_FILES_DOWNLOADOPERATIONANSWERFILE_HPP
#define ARINC_615A_FILES_DOWNLOADOPERATIONANSWERFILE_HPP

#include <arinc_615a/files/Files.hpp>
#include <arinc_615a/files/ProtocolFile.hpp>

#include <arinc_615a/information/Information.hpp>

namespace Arinc615a::Files {

/**
 * @brief ARINC 615A Operator Defined Download Operation Answer File (*.LNA).
 *
 * This file is sent from the *%Host Data Loader Application* within the *Operator Defined Download* operation.
 * This file contains the information of files to be downloaded selected by the operator.
 *
 * @par File Format
 * Name of Field          | Field Size (bits) | Field Size (bytes)
 * :----------------------|:-----------------:|:------------------:
 * **File Length**        | 32                | 4
 * **Protocol %Version**  | 16                | 2
 * **Number of %Files**   | 16                | 2
 * + **File Name Length** |  8                | 1
 * + **File Name**        |  8 – 2040         | 1 - 255
 *
 * @note
 * The items marked with *+* are repeated for every available file.
 *
 * @sa ARINC 615A-4 Section 6.4.9
 *
 * @sa @ref InitializationFile
 * @sa @ref DownloadOperationStatusFile
 * @sa @ref DownloadOperationListFile
 **/
class ARINC_615A_EXPORT DownloadOperationAnswerFile final : public ProtocolFile
{
  public:
    //! Minimum size of this file. It's the header size + number of files field).
    static constexpr std::size_t MinimumSize{ HeaderSize + sizeof( uint16_t ) };

    //! Default constructor
    DownloadOperationAnswerFile() = default;

    /**
     * @brief Constructs the Download Operation Answer file.
     *
     * @param[in] protocolVersion
     *   ARINC 615A Protocol Version.
     * @param[in] files
     *   Download Answer File Names.
     **/
    explicit DownloadOperationAnswerFile( Arinc615aVersion protocolVersion, Information::DownloadFiles files = {} );

    /**
     * @brief Constructs the status file from the given raw data.
     *
     * @param[in] rawData
     *   Raw Answer File.
     **/
    explicit DownloadOperationAnswerFile( Helper::ConstRawDataSpan rawData );

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
    DownloadOperationAnswerFile& operator=( Helper::ConstRawDataSpan rawData );

    /**
     * @name Downloadable Files.
     * @{
     **/

    /**
     * @brief Returns the Answers.
     *
     * @return File Answers.
     **/
    [[nodiscard]] const Information::DownloadFiles& files() const;

    //! @copydoc DownloadOperationAnswerFile::files() const
    Information::DownloadFiles& files();

    /**
     * @brief Updates the Download Files.
     *
     * @param[in] files
     *   Download Files.
     **/
    void files( Information::DownloadFiles files );

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
     *   When the number of files field is `0`.
     **/
    void decode( Helper::ConstRawDataSpan rawData );

  private:
    //! Download Files
    Information::DownloadFiles filesV;
};

}

#endif
