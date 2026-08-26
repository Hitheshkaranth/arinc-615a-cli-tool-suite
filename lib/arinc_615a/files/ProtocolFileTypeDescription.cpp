// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615a::Files::ProtocolFileTypeDescription.
 **/

#include "ProtocolFileTypeDescription.hpp"

namespace Arinc615a::Files {

ProtocolFileTypeDescription::ProtocolFileTypeDescription():
  Description{
    { "Load Configuration Initialization", ProtocolFileType::LoadConfigurationInitialization },
    { "Load Configuration List", ProtocolFileType::LoadConfigurationList },
    { "Load Configuration Status", ProtocolFileType::LoadConfigurationStatus },

    { "Upload Initialization", ProtocolFileType::UploadInitialization },
    { "Upload Request", ProtocolFileType::UploadRequest },
    { "Upload Status", ProtocolFileType::UploadStatus },

    { "Media Defined Download Initialization", ProtocolFileType::MediaDefinedDownloadInitialization },
    { "Media Defined Download Request", ProtocolFileType::MediaDefinedDownloadRequest },

    { "Operator Defined Download Initialization", ProtocolFileType::OperatorDefinedDownloadInitialization },
    { "Operator Defined Download List", ProtocolFileType::OperatorDefinedDownloadList },
    { "Operator Defined Download Answer", ProtocolFileType::OperatorDefinedDownloadAnswer },

    { "Download Status", ProtocolFileType::DownloadStatus } }
{
}

std::ostream& operator<<( std::ostream &stream, const ProtocolFileType protocolFileType )
{
  return stream << ProtocolFileTypeDescription::instance().name( protocolFileType );
}

}
