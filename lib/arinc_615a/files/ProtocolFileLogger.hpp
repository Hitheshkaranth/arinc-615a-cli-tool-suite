// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Files::ProtocolFileLogger.
 **/

#ifndef ARINC_615A_FILES_PROTOCOLFILELOGGER_HPP
#define ARINC_615A_FILES_PROTOCOLFILELOGGER_HPP

#include <arinc_615a/files/Files.hpp>

#include <helper/RawData.hpp>

#include <filesystem>
#include <string_view>

namespace Arinc615a::Files {

/**
 * @brief Protocol File Logger.
 *
 * The _Protocol File Logger_ is used to log the content of the reception and transmission of
 * _ARINC 615A Protocol Files_ to a persistent file storage.
 * It is used for debugging the ARINC 615A protocol implementation.
 **/
class ProtocolFileLogger final
{
  public:
    //! Default Constructor
    ProtocolFileLogger() = default;

    /**
     * @name Protocol FIle Logger Configuration.
     *
     * Uses method chaining for simpler usage.
     *
     * @{
     **/

    /**
     * @brief Sets the Logging Directory.
     *
     * Defines the base-directory where the protocol files are stored.
     *
     * @param[in] loggingDirectory
     *   Base Logging Directory.
     *
     * @return @p *this for chaining.
     **/
    ProtocolFileLogger& loggingDirectory(
      std::filesystem::path loggingDirectory = std::filesystem::temp_directory_path() );

    /**
     * @brief Sets the Logging Enabled Flag.
     *
     * @param[in] loggingEnabled
     *   If logging is enabled.
     *
     * @return @p *this for chaining.
     **/
    ProtocolFileLogger& loggingEnabled( bool loggingEnabled );

    /**
     * @brief Sets the Operation.
     *
     * @param[in] operation
     *   Active Operation.
     *
     * @return @p *this for chaining.
     **/
    ProtocolFileLogger& operation( OperationType operation );

    /** @} **/

    /**
     * @brief Logs a Received Protocol File.
     *
     * @param[in] filename
     *   Protocol File Filename
     * @param[in] file
     *   Received Protocol File.
     **/
    void receivedProtocolFile( std::string_view filename, Helper::ConstRawDataSpan file );

    /**
     * @brief Logs a Transmitted Protocol File.
     *
     * @param[in] filename
     *   Protocol File Filename
     * @param[in] file
     *   Transmitted Protocol File.
     **/
    void transmitProtocolFile( std::string_view filename, Helper::ConstRawDataSpan file );

  private:
    /**
     * @brief Logs the protocol file.
     *
     * @param[in] prefix
     *   Log Filename Prefix
     * @param[in] filename
     *   Protocol File Filename
     * @param[in] file
     *   Transmitted Protocol File.
     */
    void logProtocolFile( std::string_view prefix, std::string_view filename, Helper::ConstRawDataSpan file );

    //! Logging Directory
    std::filesystem::path loggingDirectoryV{ std::filesystem::temp_directory_path() };
    //! Logging Enabled
    bool loggingEnabledV{ false };
    //! Operation
    OperationType operationV{ OperationType::Invalid };
};

}

#endif
