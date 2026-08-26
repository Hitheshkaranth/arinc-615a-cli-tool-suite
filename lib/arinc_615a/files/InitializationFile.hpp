// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Files::InitializationFile.
 **/

#ifndef ARINC_615A_FILES_INITIALIZATIONFILE_HPP
#define ARINC_615A_FILES_INITIALIZATIONFILE_HPP

#include <arinc_615a/files/Files.hpp>
#include <arinc_615a/files/ProtocolFile.hpp>

#include <arinc_615a/information/InitializationResponse.hpp>

namespace Arinc615a::Files {

/**
 * @brief ARINC 615A Initialization File.
 *
 * This file is transmitted as answer to an ARINC 615A operation request.
 *
 * @par File Format
 * Name of Field                        | Field Size (bits) | Field Size (bytes)
 * :------------------------------------|:-----------------:|:-------------------:
 * **File Length**                      | 32                | 4
 * **Protocol %Version**                | 16                | 2
 * **Operation Acceptance Status Code** | 16                | 2
 * **Status Description Length**        |  8                | 1
 * **Status Description**               |  0 – 2040         | 0 - 255
 *
 * @sa ARINC 615A-4 Section 6.4.1
 **/
class ARINC_615A_EXPORT InitializationFile final : public ProtocolFile
{
  public:
    /**
     * @brief Minimal Size of an ARINC 615A Initialisation File.
     *
     * This includes:
     * - ARINC 615A Protocol File Header,
     * - Operation status (16 bit), and
     * - Minimal Status Description String (8 bit)
     **/
    static constexpr auto MinFileSize{ HeaderSize + 3UZ };

    //! Default construction of Initialisation File
    InitializationFile() = default;

    /**
     * @brief Constructs an initialisation file from the given data.
     *
     * @param[in] protocolVersion
     *   ARINC 615A protocol version.
     * @param[in] response
     *   Initialisation Response Information.
     **/
    explicit InitializationFile( Arinc615aVersion protocolVersion, Information::InitializationResponse response = {} );

    /**
     * @brief Constructs the initialisation file from the given raw data.
     *
     * @param[in] rawData
     *   Raw data representation of the initialisation file.
     **/
    explicit InitializationFile( Helper::ConstRawDataSpan rawData );

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
    InitializationFile& operator=( Helper::ConstRawDataSpan rawData );

    /**
     * @name Response Information
     * @{
     **/

    /**
     * @brief Returns the Initialisation Response Information.
     *
     * @return Response information.
     **/
    [[nodiscard]] const Information::InitializationResponse& response() const;

    //! @copydoc response() const
    [[nodiscard]] Information::InitializationResponse& response();

    /**
     * @brief Updates the Initialisation Response Information.
     *
     * @param[in] response
     *   Initialisation Response Information.
     *   The data is moved to the instance.
     **/
    void response( Information::InitializationResponse response );

    /** @} **/

  protected:
    //! @copydoc ProtocolFile::encode
    [[nodiscard]] Helper::RawData encode() const override;

    /**
     * @brief Decodes the Initialisation File.
     *
     * @param[in] rawData
     *   Raw file data.
     *
     * @throw Arinc615aException
     *   Data packet to small.
     * @throw Arinc615aException
     *   More data than expected.
     **/
    void decode( Helper::ConstRawDataSpan rawData );

  private:
    //! Response Information.
    Information::InitializationResponse responseV;
};

}

#endif
