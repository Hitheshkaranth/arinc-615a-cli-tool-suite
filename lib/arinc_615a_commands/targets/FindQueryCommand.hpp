// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615aCommands::Targets::FindQueryCommand.
 **/

#ifndef ARINC_615A_COMMANDS_TARGETS_FINDQUERYCOMMAND_HPP
#define ARINC_615A_COMMANDS_TARGETS_FINDQUERYCOMMAND_HPP

#include <arinc_615a_commands/targets/Targets.hpp>

#include <arinc_615a/find/clients/Clients.hpp>

#include <arinc_615a/find/FindConfiguration.hpp>
#include <arinc_615a/find/TargetInformation.hpp>

#include <commands/Commands.hpp>

#include <boost/asio/io_context.hpp>

#include <filesystem>
#include <latch>

namespace Arinc615aCommands::Targets {

/**
 * @brief ARINC 615A FIND Query Command.
 *
 * Request the FIND information and prints the answers to the console.
 * User provided Parameters:
 *  - Destination IP address
 *  - Timeout (@ref Arinc615a::Find::DefaultReceiveTimeout by default)
 *  - Usage of dynamic source port
 *  - Store targets information to file
 *
 * @sa @ref Arinc615a::Find::Clients::Query
 * @sa @ref Arinc615a::Find::TargetInformation
 **/
class ARINC_615A_COMMANDS_EXPORT FindQueryCommand final
{
  public:
    /**
     * @brief Constructs the FIND Query Command.
     *
     * @param[in] ioContext
     *   I/O Context used for operations.
     * @param[in] abortSignal
     *   Signal raised when application is aborted.
     * @param[in] terminateSignal
     *   Signal raised when application is terminated.
     **/
    FindQueryCommand(
      boost::asio::io_context &ioContext,
      AbortTerminateSignal &abortSignal,
      AbortTerminateSignal &terminateSignal );

    /**
     * @brief Executes the FIND operation.
     *
     * @param[in] parameters
     *   Parameters supplied by User.
     **/
    void execute( const Commands::Parameters &parameters );

    //! Prints command help
    void help() const;

  private:
    /**
     * @brief Handles the received FIND response.
     *
     * Prints this information to the console.
     *
     * @param[in] target
     *   Target Address.
     * @param[in] information
     *   Target Information
     **/
    void response( const boost::asio::ip::address &target, const Arinc615a::Find::TargetInformation &information );

    /**
     * @brief FIND operation finished.
     *
     * Finishes the operation.
     **/
    void finishedFind();

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

    //! Target address
    boost::asio::ip::address targetAddressV;
    //! ARINC 615A FIND Configuration
    Arinc615a::Find::FindConfiguration configurationV;
    //! Use local dynamic port
    bool dynamicPortV{ false };
    //! FIND Operation Timeout
    std::chrono::seconds timeoutV{ Arinc615a::Find::DefaultReceiveTimeout };
    //! Targets List JSON Path
    std::filesystem::path targetsListPathV;
    //! Targets List
    Arinc615a::Find::TargetsAddressInformation targetsV;
};

}

#endif
