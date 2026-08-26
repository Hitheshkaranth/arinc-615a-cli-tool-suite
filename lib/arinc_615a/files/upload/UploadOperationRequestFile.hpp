// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Files::UploadOperationRequestFile.
 **/

#ifndef ARINC_615A_FILES_UPLOADOPERATIONREQUESTFILE_HPP
#define ARINC_615A_FILES_UPLOADOPERATIONREQUESTFILE_HPP

#include <arinc_615a/files/Files.hpp>
#include <arinc_615a/files/ProtocolFile.hpp>

#include <arinc_615a/information/UploadLoad.hpp>

namespace Arinc615a::Files {

/**
 * @brief ARINC 615A Upload Operation Request File (*.LUR).
 *
 * This file is sent from the *Host Data Loader Application* within the *Upload Download* operation.
 * This file contains the information of files to be downloaded.
 *
 * @par File Format
 * Name of Field                 | Field Size (bits) | Field Size (bytes)
 * :-----------------------------|:-----------------:|:-------------------:
 * **File Length**               | 32               | 4
 * **Protocol %Version**         | 16               | 2
 * **Number of Header %Files**   | 16               | 2
 * + **Header File Name Length** |  8               | 1
 * + **Header File Name**        |  8 – 2040        | 1 - 255
 * + **Load P/N Name Length**    |  8               | 1
 * + **Load P/N Name**           |  8 – 2040        | 1 - 255
 *
 * @note
 * The items marked with *+* are repeated for every available header file.
 *
 * @sa ARINC 615A-4 Section 6.4.4
 *
 * @sa @ref InitializationFile
 * @sa @ref UploadOperationStatusFile
 **/
class ARINC_615A_EXPORT UploadOperationRequestFile final : public ProtocolFile
{
  public:
    /**
     * @brief Minimum size of this file.
     *
     * This includes:
     * - ARINC 615A Protocol File Header size, and
     * - Number of Header Files field,
     **/
    static constexpr std::size_t MinimumSize{ HeaderSize + 2UZ };

    //! Default Construction of Upload Operation Request File.
    UploadOperationRequestFile() = default;

    /**
     * @brief Creates the Upload Operation Request File.
     *
     * @param[in] protocolVersion
     *   ARINC 615A Protocol Version.
     * @param[in] loads
     *   List of Loads.
     **/
    explicit UploadOperationRequestFile( Arinc615aVersion protocolVersion, Information::UploadLoads loads = {} );

    /**
     * @brief Decodes the Upload Operation Request file from the given raw data.
     *
     * @param[in] rawData
     *   Raw data file representation.
     **/
    explicit UploadOperationRequestFile( Helper::ConstRawDataSpan rawData );

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
    UploadOperationRequestFile& operator=( Helper::ConstRawDataSpan rawData );

    /**
     * @name Loads.
     * @{
     **/

    /**
     * @brief Returns the list of upload information.
     *
     * @return A reference to the header file list.
     **/
    [[nodiscard]] const Information::UploadLoads& loads() const;

    /**
     * @brief Returns the list of upload information.
     *
     * @return Returns the modifiable list of header files.
     **/
    [[nodiscard]] Information::UploadLoads& loads();

    /**
     * @brief Replaces the list of upload information with the given version.
     *
     * @param[in] loads
     *   The new list of upload information
     **/
    void loads( Information::UploadLoads loads );

    /** @} **/

  protected:
    //! @copydoc ProtocolFile::encode
    [[nodiscard]] Helper::RawData encode() const override;

    /**
     * @brief Decodes the Upload Operations Request File from the raw data.
     *
     * @param[in] rawData
     *   Raw data.
     *
     * @throw Arinc615aException
     *   When the number of header files field is `0`.
     **/
    void decode( Helper::ConstRawDataSpan rawData );

  private:
    //! upload loads information
    Information::UploadLoads loadsV;
};

}

#endif
