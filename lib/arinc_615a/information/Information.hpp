// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Namespace Arinc615a::Information.
 **/

#ifndef ARINC_615A_INFORMATION_INFORMATION_HPP
#define ARINC_615A_INFORMATION_INFORMATION_HPP

#include <arinc_615a/Arinc615a.hpp>

#include <list>
#include <string>

/**
 * @dir
 * @brief Declaration/ Definition of Namespace Arinc615a::Information.
 **/

/**
 * @brief @brief ARINC 615A Protocol %Information.
 *
 * This namespace contains classes and definitions, with all known ARINC 615A protocol information.
 **/
namespace Arinc615a::Information {

class InitializationResponse;
class Status;
class Ratio;

//! %Information %Status
using InformationStatus = Status;
struct TargetHardware;
//! Targets Hardware List.
using TargetsHardware = std::list< TargetHardware >;
struct PartNumber;
//! Part Number %Information List.
using PartNumbers = std::list< PartNumber >;

class UploadLoadStatus;
//! Upload Load %Status %Information List.
using UploadLoadsStatus = std::list< UploadLoadStatus >;
class UploadStatus;
struct UploadLoad;
//! Upload Loads List.
using UploadLoads = std::list< UploadLoad >;

class DownloadStatus;
class DownloadFileStatus;
//! Download Files %Status (list)
using DownloadFilesStatus = std::list< DownloadFileStatus >;

struct DownloadFileInformation;
//! Download %Information
using DownloadFilesInformation = std::list< DownloadFileInformation >;

//! Filenames for Download Operations
using DownloadFiles = std::list< std::string >;

}

#endif
