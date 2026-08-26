// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Target::UploadOperation.
 **/

#ifndef ARINC_615A_TARGET_UPLOADOPERATION_HPP
#define ARINC_615A_TARGET_UPLOADOPERATION_HPP

#include <arinc_615a/target/Target.hpp>
#include <arinc_615a/target/Operation.hpp>

#include <arinc_615a/information/Information.hpp>

#include <arinc_615a/tftp/clients/Client.hpp>

#include <arinc_649/CheckValue.hpp>

#include <string_view>

namespace Arinc615a::Target {

/**
 * @brief ARINC 615A %Target %Upload %Operation.
 **/
class ARINC_615A_EXPORT UploadOperation : public Operation
{
  public:
    // make overloaded operations explicit visible
    using Operation::inProgress;

    /**
     * @brief Updates the Operation Status to in Progress.
     *
     * This overload takes the Target Upload Operation specific information.
     *
     * @param[in] immediateTransmission
     *   If set to true, status is sent immediately.
     * @param[in] estimatedTime
     *   Estimated time until completion.
     *   If set to -1 no information is available.
     * @param[in] description
     *   Status description.
     * @param[in] listRatio
     *   List ratio.
     **/
    virtual void inProgress(
      bool immediateTransmission,
      int16_t estimatedTime,
      std::string description,
      Information::Ratio listRatio ) = 0;

    /**
     * @name Load Status handling
     * @{
     **/

    /**
     * @brief Updates the Load Status.
     *
     * Adds the Load to the list of loads with status accepted.
     *
     * @param[in] headerFilename
     *   Load Header Filename.
     * @param[in] partNumber
     *   Load Part Number.
     **/
    virtual void loadAccepted( std::string headerFilename, std::string partNumber ) = 0;

    /**
     * @brief Sets Load Status to in Progress.
     *
     * @param[in] headerFilename
     *   Load Header Filename.
     * @param[in] ratio
     *   Load Ratio.
     * @param[in] description
     *   Status Description.
     **/
    virtual void loadInProgress(
      std::string_view headerFilename,
      Information::Ratio ratio,
      std::string description = {} ) = 0;

    /**
     * @brief Completes Load Status.
     *
     * @param[in] headerFilename
     *   Load Header Filename.
     * @param[in] finalStatus
     *   Final Status.
     * @param[in] description
     *   Status Description.
     **/
    virtual void loadFinished(
      std::string_view headerFilename,
      FinalStatus finalStatus,
      std::string description = {} ) = 0;

    /**
     * @brief Completes all not-finished loads.
     *
     * @param[in] finalStatus
     *   Final Status.
     * @param[in] description
     *   Status Description.
     **/
    virtual void loadsFinished( FinalStatus finalStatus, std::string_view description = {} ) = 0;

    /** @} **/

    /**
     * @brief Transfer the requested file (TFTP RRQ).
     *
     * @param[in] optionNegotiationHandler
     *   ARINC 615A Option Negotiation Handler.
     * @param[in] completionHandler
     *   Handler which is called on completion of the file transfer operation.
     * @param[in] dataHandler
     *   Handler, which receives the requested data.
     * @param[in] filename
     *   Filename of the file to request.
     * @param[in] partNumber
     *   Part Number of the load (part) containing the file being requested.
     * @param[in] checkValue
     *   Check value of the file to request.
     *   Set to @p Arinc649::CheckValue::NoCheckValue if not used.
     *
     * @return TFTP Client Operation.
     **/
    [[nodiscard]] virtual Tftp::Clients::OperationPtr transferFile(
      Arinc615aOptionNegotiationHandler optionNegotiationHandler,
      Tftp::Clients::OperationCompletedHandler completionHandler,
      Tftp::ReceiveDataHandlerPtr dataHandler,
      std::string filename,
      std::string partNumber = {},
      Arinc649::CheckValue checkValue = Arinc649::CheckValue::NoCheckValue ) = 0;
};

}

#endif
