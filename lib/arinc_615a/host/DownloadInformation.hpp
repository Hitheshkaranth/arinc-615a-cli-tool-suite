// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Struct Arinc615a::Host::DownloadInformation.
 **/

#ifndef ARINC_615A_HOST_DOWNLOADINFORMATION_HPP
#define ARINC_615A_HOST_DOWNLOADINFORMATION_HPP

#include <arinc_615a/host/Host.hpp>

#include <arinc_615a/TargetId.hpp>

#include <arinc_649/CheckValue.hpp>

#include <boost/asio/ip/address.hpp>

#include <chrono>
#include <filesystem>
#include <list>
#include <map>
#include <mutex>
#include <string>
#include <string_view>
#include <tuple>

namespace Arinc615a::Host {

/**
 * @brief Download Information.
 *
 * For each download operation, the Host Data Loader shall provide a set of information.
 * The information should be stored together with the downloaded files.
 *
 * @note
 * In contrast to the ARINC 615A Specification, the implementation does not use an incrementing counter for the filename
 * generation.
 * This implementation uses the current date/time information for filename generation.
 **/
class ARINC_615A_EXPORT DownloadInformation final
{
  public:
    //! Transfer OK Status
    static auto constexpr TransferOk{ "Transfer OK" };
    //! Transfer Error Status
    static auto constexpr TransferError{ "Transfer Error" };

    //! File Transfer Information
    struct FileTransferInformation
    {
      //! Start of file operation
      std::chrono::system_clock::time_point start{};
      //! End Of file operation
      std::chrono::system_clock::time_point end{};
      //! File Path
      std::filesystem::path filePath{};
      //! File Status
      std::string fileStatus{};
      //! File Size
      std::size_t fileSize{};
      //! File Part Number
      std::string partNumber{};
      //! Check Value
      Arinc649::CheckValue checkValue{};
    };

    /**
     * @brief Start of Download Operation indicator.
     *
     * Stores the Target ID, Target Address, and the current time.
     *
     * @sa files()
     * @sa finished()
     *
     * @param[in] targetId
     *   Target ID
     * @param[in] targetAddress
     *   Target Address
     **/
    void start( TargetId targetId, boost::asio::ip::address targetAddress );

    /**
     * @name Download Paths
     *
     * @sa ARINC 615A-4 Section 5.4.4.3 Download Media Organization.
     * @{
     **/

    /**
     * @brief Returns a Download Data Path based on Target ID and Operation Start Time.
     *
     * The _download data path_ is a directory which should contain the downloaded files.
     *
     * @param[in] basePath
     *   Base Path Prepended.
     *
     * @return Base Path appended by a download data path.
     **/
    [[nodiscard]] std::filesystem::path downloadDataPath( const std::filesystem::path &basePath ) const;

    /**
     * @brief Returns a Download Information Path based on Target ID and Operation Start Time.
     *
     * The _download information path_ is a text file which should contain additional information of the Download operation.
     *
     * @param[in] basePath
     *   Base Path Prepended.
     *
     * @return Base Path appended by a download information path.
     **/
    [[nodiscard]] std::filesystem::path downloadInformationPath( const std::filesystem::path &basePath ) const;

    /** @} **/

    /**
     * @brief Stores the files, which will be downloaded.
     *
     * @sa fileStart()
     *
     * @param[in] files
     *   List of download files.
     **/
    void files( std::list< std::string > files );

    /**
     * @brief File Download Start indication.
     *
     * Stores the filename and start of operation.
     * Uses the current time for start of transfer indication.
     *
     * @sa fileFinished()
     *
     * @param[in] filename
     *   Filename
     * @param[in] filePath
     *   File path, where the data is stored to.
     * @param[in] partNumber
     *   File Part Number
     * @param[in] checkValue
     *   File Check Value
     **/
    void fileStart(
      std::string_view filename,
      std::filesystem::path filePath,
      std::string partNumber,
      Arinc649::CheckValue checkValue );

    /**
     * @brief File Download Finished indicator.
     *
     * Uses the current time for completion of the transfer.
     *
     * @sa fileStart()
     *
     * @param[in] filename
     *   Filename
     * @param[in] status
     *   Transfer Status
     * @param[in] fileSize
     *   File Size
     **/
    void fileFinished( std::string_view filename, std::string status, std::size_t fileSize );

    /**
     * @brief Download Operation Finished indication.
     *
     * Stores the operation status, the current time, and overall file size.
     *
     * @sa @ref start()
     *
     * @param[in] status
     *   Final status
     **/
    void finished( std::string status );

    /**
     * @brief Stores the Download Information to file.
     *
     * @param[in] downloadInformationPath
     *   Filename of Download Information File
     * @param[in] checkIntegrity
     *   If set to `true` the check value is used to check the integrity of the files
     *
     * @sa @ref downloadDataPath()
     **/
    void save( const std::filesystem::path &downloadInformationPath, bool checkIntegrity );

  private:
    //! Mutex for concurrent access
    mutable std::mutex mutexV;
    //! Target ID
    TargetId targetIdV;
    //! Target Address
    boost::asio::ip::address targetAddressV;
    //! Start of operation
    std::chrono::system_clock::time_point startV;
    //! Requested files
    std::list< std::string > filesV;
    //! End Of Operation
    std::chrono::system_clock::time_point endV;
    //! File Transfer Status ( filename -> file transfer info )
    std::map< std::string, FileTransferInformation, std::less< > > filesInformationV;
    //! Overall Operation Status < status, transfer size >
    std::tuple< std::string, std::size_t > overallStatusV;
};

}

#endif
