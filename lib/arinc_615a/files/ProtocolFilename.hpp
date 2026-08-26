// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Files::ProtocolFilename.
 **/

#ifndef ARINC_615A_FILES_PROTOCOLFILENAME_HPP
#define ARINC_615A_FILES_PROTOCOLFILENAME_HPP

#include <arinc_615a/files/Files.hpp>

#include <arinc_615a/TargetId.hpp>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>

#include <string>
#include <string_view>
#include <tuple>

namespace Arinc615a::Files {

/**
 * @brief Helper class for generating and decoding of ARINC 615A protocol filenames.
 *
 * Provides construction, parsing, and validation of protocol filenames used by the ARINC 615A data loader.
 * Supports creating names from @ref TargetId and @ref ProtocolFileType, deriving the correct extension for a file type,
 * mapping extensions back to file types, detecting whether a given string is a protocol filename, and splitting a
 * filename into its target-id and extension parts.
 * Invalid inputs are rejected (e.g. split returns an empty result), ensuring filenames conform to ARINC 615A
 * conventions.
 **/
class ARINC_615A_EXPORT ProtocolFilename final
{
  public:
    /**
     * @brief Extracts the target ID and extension out of the filename
     *
     * If the filename is invalid, an empty tuple is returned.
     *
     * @param[in] filename
     *   Filename, which shall be processed.
     *
     * @return std::tuple< Target ID, Extension>.
     **/
    static std::tuple< std::string_view, std::string_view> split( std::string_view filename );

    /**
     * @brief Determines if the given filename is a ARINC 615A protocol file.
     *
     * @param[in] filename
     *   Filename to check.
     *
     * @return If the filename is a valid ARINC 615A protocol file @p true otherwise @p false.
     **/
    static bool isProtocolFilename( std::string_view filename );

    /**
     * @brief Returns the extension for the given file type.
     *
     * @param[in] fileType
     *   File Type.
     *
     * @return The extension string corresponding to the file type.
     * @retval std::string{}
     *   When an invalid file type is given.
     **/
    static std::string_view extension( ProtocolFileType fileType );

    /**
     * @brief Returns the ARINC 615A file type for the given extension.
     *
     * @param[in] extension
     *   Filename extension
     *
     * @return The ARINC 615a protocol file type.
     * @retval ProtocolFileType::invalid
     *   When the extension does not identify an ARINC 615A protocol file.
     **/
    static ProtocolFileType fileType( std::string_view extension );

    /**
     * @brief Creates a protocol filename with the given parameters.
     *
     * @param[in] thwId
     *   Target Hardware ID.
     * @param[in] position
     *   Position.
     * @param[in] fileType
     *   File type.
     **/
    ProtocolFilename( std::string thwId, std::string position, ProtocolFileType fileType );

    /**
     * @brief Creates a protocol filename with the given parameters.
     *
     * @param[in] targetId
     *   Target ID.
     * @param[in] fileType
     *   File type.
     **/
    ProtocolFilename( std::string_view targetId, ProtocolFileType fileType );

    /**
     * @brief Creates a protocol filename with the given parameters.
     *
     * @param[in] targetId
     *   Target ID.
     * @param[in] fileType
     *   File type.
     **/
    ProtocolFilename( TargetId targetId, ProtocolFileType fileType );

    /**
     * @brief Creates a protocol filename with the given parameters.
     *
     * @param[in] filename
     *   Filename.
     **/
    explicit ProtocolFilename( std::string_view filename );

    /**
     * @name Target ID
     * @{
     **/

    /**
     * @brief Returns the Target ID.
     *
     * @return Target ID.
     **/
    [[nodiscard]] const TargetId& targetId() const;

    /**
     * @brief Sets the Target ID.
     *
     * @param[in] targetId
     *   Target ID.
     **/
    void targetId( std::string_view targetId );

    //! @copydoc targetId(std::string_view)
    void targetId( TargetId targetId );

    /** @} **/

    /**
     * @name Target Hardware (THW) ID
     * @{
     **/

    /**
     * @brief Returns the Target Hardware ID.
     *
     * @return Target Hardware ID.
     **/
    [[nodiscard]] std::string_view thwId() const;

    /**
     * @brief Updates the Target Hardware ID.
     *
     * @param[in] thwId
     *   Target Hardware ID.
     **/
    void thwId( std::string thwId );

    /** @} **/

    /**
     * @name Target Position
     * @{
     **/

    /**
     * @brief Returns the Target Position.
     *
     * @return The position
     **/
    [[nodiscard]] std::string_view position() const;

    /**
     * @brief Updates the Target Position.
     *
     * @param[in] position
     *   Position information.
     **/
    void position( std::string position );

    /** @} **/

    /**
     * @name Protocol File Type
     * @{
     **/

    /**
     * @brief Returns the Protocol File Type.
     *
     * @return The file type.
     **/
    [[nodiscard]] ProtocolFileType fileType() const;

    /**
     * @brief Sets the Protocol File Type.
     *
     * @param[in] fileType
     *   file type.
     **/
    void fileType( ProtocolFileType fileType );

    /** @} **/

    /**
     * @brief @p std::string Operator to get the Filename.
     *
     * @return The filename as @p std::string representation.
     **/
    explicit operator std::string() const;

    /**
     * @brief Returns Protocol Filename Validity Information.
     *
     * Checks if:
     * - The Target ID is valid, and
     * - THe protocol filetype is valid.
     *
     * @return If the Protocol Filename is valid.
     **/
    explicit operator bool() const noexcept;

  private:
    //! Extension to protocol file mapping information
    struct ExtensionTypeInfo
    {
      //! file extension
      std::string_view extension;
      //! corresponding protocol file type
      ProtocolFileType type;
    };

    //! Description index type - we directly link to the base class
    using ExtensionTypeInfoIndex = boost::multi_index_container<
      ExtensionTypeInfo,
      boost::multi_index::indexed_by<

        // Index 0 - extension
        boost::multi_index::ordered_unique<
          boost::multi_index::tag< struct ByExtension>,
          boost::multi_index::member<
            ExtensionTypeInfo,
            std::string_view,
            &ExtensionTypeInfo::extension>>,

        // Index 1 - type
        boost::multi_index::ordered_unique<
          boost::multi_index::tag< struct ByType>,
          boost::multi_index::member<
            ExtensionTypeInfo,
            const ProtocolFileType,
            &ExtensionTypeInfo::type>> >>;

    /**
     * @brief Index that associates file extensions with their corresponding types in the protocol.
     *
     * This index allows mapping between file extensions and the protocol-defined file types, ensuring compatibility
     * when identifying or categorizing files based on their extensions. It supports operations for retrieving file type
     * information from an extension identifier and vice versa.
     *
     * @return The index used for associating extensions with their protocol file types.
     **/
    [[nodiscard]] static const ExtensionTypeInfoIndex& extensionTypeInfoIndex();

    //! Decoded Target ID.
    TargetId targetIdV;
    //! Decoded Protocol File Type.
    ProtocolFileType fileTypeV{ ProtocolFileType::Invalid };
};

}

#endif
