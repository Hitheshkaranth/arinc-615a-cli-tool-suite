// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615aCommands::Targets::ListTargetsCommand.
 **/

#ifndef ARINC_615A_COMMANDS_TARGETS_LISTTARGETSCOMMAND_HPP
#define ARINC_615A_COMMANDS_TARGETS_LISTTARGETSCOMMAND_HPP

#include <arinc_615a_commands/targets/Targets.hpp>

#include <commands/Commands.hpp>

#include <boost/program_options.hpp>

#include <filesystem>

namespace Arinc615aCommands::Targets {

/**
 * @brief List ARINC 615A Targets Command.
 *
 * Lists the ARINC 615A targets from a user-provided JSON targets file.
 **/
class ARINC_615A_COMMANDS_EXPORT ListTargetsCommand final
{
  public:
    //! Constructs class
    ListTargetsCommand();

    /**
     * @brief Executes the Command.
     *
     * @param[in] parameters
     *   Parameters supplied by User.
     **/
    void execute( const Commands::Parameters &parameters );

    //! Prints command help
    void help() const;

  private:
    //! Program Options
    boost::program_options::options_description optionsDescription;
    //! Targets List JSON Path
    std::filesystem::path targetsListPathV;
};

}

#endif
