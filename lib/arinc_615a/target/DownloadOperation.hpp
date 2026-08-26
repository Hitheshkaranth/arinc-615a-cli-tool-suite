// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Target::DownloadOperation.
 **/

#ifndef ARINC_615A_TARGET_DOWNLOADOPERATION_HPP
#define ARINC_615A_TARGET_DOWNLOADOPERATION_HPP

#include <arinc_615a/target/Target.hpp>
#include <arinc_615a/target/Operation.hpp>

#include <arinc_615a/information/Information.hpp>

#include <arinc_615a/tftp/clients/Clients.hpp>

#include <arinc_649/CheckValue.hpp>

#include <string_view>

namespace Arinc615a::Target {

/**
 * @brief ARINC 615A %Target Download %Operation.
 *
 * Provides the interface for Media Defined and Operator Defined Download operation.
 **/
class ARINC_615A_EXPORT DownloadOperation : public Operation
{
  public:
    // make overloaded operations explicitly visible
    using Operation::inProgress;

    /**
     * @brief Updates the operation status.
     *
     * This overload takes the Target Download Operation specific information.
     *
     * @param[in] immediateTransmission
     *   If set to @p true, the status is sent immediately.
     * @param[in] estimatedTime
     *   Estimated time until completion.
     *   If set to @p -1, no information is available.
     * @param[in] description
     *   Status description.
     * @param[in] listRatio
     *   Completion ratio.
     **/
    virtual void inProgress(
      bool immediateTransmission,
      int16_t estimatedTime,
      std::string description,
      Information::Ratio listRatio ) = 0;

    /**
     * @name Download Files Status handling
     * @{
     **/

    /**
     * @brief Updates the Download File Status.
     *
     * Adds the Download File to the list of files with status accepted.
     *
     * @param[in] filename
     *   Download Filename.
     **/
    virtual void fileAccepted( std::string filename ) = 0;

    /**
     * @brief Sets Download File Status to in Progress.
     *
     * @param[in] filename
     *   Download Filename.
     * @param[in] description
     *   Status Description.
     **/
    virtual void fileInProgress( std::string_view filename, std::string description = {} ) = 0;

    /**
     * @brief Completes Download File Status.
     *
     * @param[in] filename
     *   Download Filename.
     * @param[in] finalStatus
     *   Final Status.
     * @param[in] description
     *   Status Description.
     **/
    virtual void fileFinished(
      std::string_view filename,
      FinalStatus finalStatus,
      std::string description = {} ) = 0;

    /**
     * @brief Completes all not-finished download files.
     *
     * @param[in] finalStatus
     *   Final Status.
     * @param[in] description
     *   Status Description.
     **/
    virtual void filesFinished( FinalStatus finalStatus, std::string_view description = {} ) = 0;

    /** @} **/

    /**
     * @brief Transmits the given file to the host.
     *
     * The ARINC 615A *Part Number Option* could be provided by the target hardware loader to identify the file, which
     * will be transmitted.
     * @note
     * The *Part Number Option* is officially not part of the file transfers of Download Operations.
     * This is an extension to the ARINC 615A %Protocol.
     *
     * The ARINC 615A *Checksum Option* could be provided by the target hardware loader to check the integrity of the
     * file, which will be transmitted.
     *
     * @param[in] optionNegotiationHandler
     *   ARINC 615A Option Negotiation Handler.
     * @param[in] completionHandler
     *   Handler which is called on completion of the file transfer operation.
     * @param[in] dataHandler
     *   %File content provider.
     * @param[in] filename
     *   Filename.
     * @param[in] partNumber
     *   Part Number of the file to transmit.
     *   Can be set to empty string if not used.
     * @param[in] checkValue
     *   Check value of the file to transmit.
     *   Set to @p Arinc649::CheckValue::NoCheckValue if not used.
     *
     * @return TFTP Client _WRQ_ %Operation.
     **/
    [[nodiscard]] virtual Tftp::Clients::OperationPtr transferFile(
      Arinc615aOptionNegotiationHandler optionNegotiationHandler,
      Tftp::Clients::OperationCompletedHandler completionHandler,
      Tftp::TransmitDataHandlerPtr dataHandler,
      std::string filename,
      std::string partNumber = {},
      Arinc649::CheckValue checkValue = Arinc649::CheckValue::NoCheckValue ) = 0;
};

}

#endif
