// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615aCommands::Operations::InformationOperationCommand.
 **/

#ifndef ARINC_615A_COMMANDS_OPERATIONS_INFORMATIONOPERATIONCOMMAND_HPP
#define ARINC_615A_COMMANDS_OPERATIONS_INFORMATIONOPERATIONCOMMAND_HPP

#include <arinc_615a_commands/operations/Operations.hpp>

#include <arinc_615a/information/Information.hpp>

#include <arinc_615a/host/InformationOperationHandler.hpp>

#include <arinc_615a/Arinc615aConfiguration.hpp>

#include <commands/Commands.hpp>

#include <filesystem>
#include <latch>
#include <string>

namespace Arinc615aCommands::Operations {

/**
 * @brief ARINC 615A Information Operation Command.
 *
 * Requests the Information Operation at the Target Hardware and waits for receiving the *Load Configuration List*.
 * When received prints the information to the console.
 *
 * @sa @ref Arinc615a::Host::InformationOperation
 * @sa @ref Arinc615a::Information::TargetsHardware
 * @sa @ref Arinc615a::Information::TargetHardware
 **/
class ARINC_615A_COMMANDS_EXPORT InformationOperationCommand final :
  private Arinc615a::Host::InformationOperationHandler
{
  public:
    /**
     * @brief Constructs the ARINC 615A Information Operation.
     *
     * @param[in] ioContext
     *   I/O Context used of operations.
     * @param[in] abortSignal
     *   Signal raised when application is aborted.
     * @param[in] terminateSignal
     *   Signal raised when application is terminated.
     **/
    InformationOperationCommand(
      boost::asio::io_context &ioContext,
      AbortTerminateSignal &abortSignal,
      AbortTerminateSignal &terminateSignal );

    /**
     * @brief Executes the Information Operation.
     *
     * @param[in] parameters
     *   Command parameters.
     **/
    void execute( const Commands::Parameters &parameters );

    //! Prints command help
    void help() const;

  private:
    //! @copydoc Arinc615a::Host::InformationOperationHandler::initialisationDeferred
    void initialisationDeferred( std::chrono::seconds waitTime ) override;

    //! @copydoc Arinc615a::Host::InformationOperationHandler::initialisationResponse
    void initialisationResponse( const Arinc615a::Information::InitializationResponse &response ) override;

    //! @copydoc Arinc615a::Host::InformationOperationHandler::finished
    void finished( Arinc615a::StatusCode code, std::string_view description ) override;

    //! @copydoc Arinc615a::Host::InformationOperationHandler::status
    void status( const Arinc615a::Information::InformationStatus &status ) override;

    //! @copydoc Arinc615a::Host::InformationOperationHandler::targetInformation
    void targetInformation( const Arinc615a::Information::TargetsHardware &targetsHardware, bool integrity ) override;

    //! ASIO I/O context
    boost::asio::io_context &ioContextV;

    //! Abort Signal
    AbortTerminateSignal &abortSignalV;
    //! Terminate Signal
    AbortTerminateSignal &terminateSignalV;
    //! Operation Done Indicator for Synchronisation.
    std::latch done{ 1 };

    //! Program Options Description
    boost::program_options::options_description optionsDescriptionV;

    //! %Target address
    boost::asio::ip::address targetAddressV;
    //! Targets List JSON Path
    std::filesystem::path targetsListPathV;
    //! Target ID
    std::string targetIdV;
    //! DLP Timeout
    std::chrono::seconds dlpTimeoutV{};
    //! Use Port Option
    bool portOptionV{ false };

    //! ARINC 615A Data Loader Configuration
    Arinc615a::Arinc615aConfiguration configurationV;

    //! Operation
    Arinc615a::Host::InformationOperationPtr operationV;
};

}

#endif
