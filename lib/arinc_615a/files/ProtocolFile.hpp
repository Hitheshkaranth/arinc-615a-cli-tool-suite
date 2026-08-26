// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Files::ProtocolFile.
 **/

#ifndef ARINC_615A_FILES_PROTOCOLFILE_HPP
#define ARINC_615A_FILES_PROTOCOLFILE_HPP

#include <arinc_615a/files/Files.hpp>

#include <helper/RawData.hpp>

namespace Arinc615a::Files {

/**
 * @brief ARINC 615A protocol file
 *
 * @li `[THW_ID_POS].LCI` - Load Configuration Initialization
 * @li `[THW_ID_POS].LCL` - Load Configuration List
 * @li `[THW_ID_POS].LCS` - Load Configuration Status
 *
 * @li `[THW_ID_POS].LUI` - Load Upload Initialization
 * @li `[THW_ID_POS].LUR` - Load Upload Request
 * @li `[THW_ID_POS].LUS` - Load Upload Status
 *
 * @li `[THW_ID_POS].LND` - Load Download Media defined Initialization
 * @li `[THW_ID_POS].LNO` - Load Download Operator defined Initialization
 * @li `[THW_ID_POS].LNR` - Load Download Media defined Request
 * @li `[THW_ID_POS].LNS` - Load Download Status
 * @li `[THW_ID_POS].LNL` - Load Download List
 * @li `[THW_ID_POS].LNA` - Load Download Answer
 *
 * @sa @ref InitializationFile
 * @sa @ref InformationOperationStatusFile
 * @sa @ref LoadConfigurationListFile
 * @sa @ref UploadOperationRequestFile
 * @sa @ref UploadOperationStatusFile
 * @sa @ref DownloadOperationStatusFile
 * @sa @ref DownloadOperationRequestFile
 * @sa @ref DownloadOperationListFile
 * @sa @ref DownloadOperationAnswerFile
 **/
class ARINC_615A_EXPORT ProtocolFile
{
  public:
    //! Header size of each protocol file (File Length + %Version).
    static constexpr auto HeaderSize{ 6UZ };

    /**
     * @brief Copy Assignment Operator.
     *
     * @param[in] other
     *   Other protocol file.
     *
     * @return *this
     **/
    ProtocolFile& operator=( const ProtocolFile &other ) = default;

    /**
     * @brief Move Assignment Operator.
     *
     * @param[in] other
     *   Other protocol file.
     *
     * @return *this
     **/
    ProtocolFile& operator=( ProtocolFile &&other ) = default;

    /**
     * @brief Returns the protocol file as raw data.
     *
     * @return the protocol file as raw data.
     **/
    explicit operator Helper::RawData() const;

    /**
     * @name Protocol Version
     * @{
     **/

    /**
     * @brief Returns the ARINC 615A version of this protocol file.
     *
     * @return The ARINC 615A version of this protocol file.
     **/
    [[nodiscard]] Arinc615aVersion protocolVersion() const;

    /**
     * @brief Updates the ARINC 615A version of this protocol file.
     *
     * @param[in] protocolVersion
     *    The ARINC 615A version of this protocol file.
     **/
    void protocolVersion( Arinc615aVersion protocolVersion );

    /** @} **/

  protected:
    //! Default construction of Protocol File.
    ProtocolFile() = default;

    /**
     * @brief Constructs a protocol file.
     *
     * @param[in] protocolVersion
     *   Protocol version.
     **/
    explicit ProtocolFile( Arinc615aVersion protocolVersion );

    /**
     * @brief Copy Constructor.
     *
     * @param[in] other
     *   Other protocol file.
     **/
    ProtocolFile( const ProtocolFile &other ) = default;

    /**
     * @brief Move Constructor.
     *
     * @param[in] other
     *   Other protocol file.
     **/
    ProtocolFile( ProtocolFile &&other ) = default;

    //! Default virtual destructor.
    virtual ~ProtocolFile() noexcept = default;

    /**
     * @brief Get the binary representation of the file.
     *
     * @return Binary packet data
     **/
    [[nodiscard]] virtual Helper::RawData encode() const = 0;

    /**
     * @brief Insert the header data to the raw file.
     *
     * Can be used by child classes to assemble a raw file.
     * The size of @p rawData must be at least @p HeaderSize.
     *
     * @note
     * The header should be written after all other data is writen, as this opreation also updates the _size_ field.
     *
     * @param[in,out] rawData
     *   Raw packet, which will be filled with the appropriate data.
     **/
    void insertHeader( Helper::RawDataSpan rawData ) const;

    /**
     * @brief Decodes the header.
     *
     * Extracts all fields from the header and performs some basic checks:
     * - Length validity
     * - magic number integrity.
     *
     * @param[in] rawData
     *   Raw file data.
     *
     * @return Remaining protocol file data without the header.
     *
     * @throw Arinc615aException
     *   If the data size is smaller than expected.
     * @throw Arinc615aException
     *   If the data size is unequal to the size stored in the header.
     **/
    Helper::ConstRawDataSpan decodeHeader( Helper::ConstRawDataSpan rawData );

  private:
    //! Protocol Version
    Arinc615aVersion protocolVersionV{ Arinc615aVersion::Arinc615a34 };
};

}

#endif
