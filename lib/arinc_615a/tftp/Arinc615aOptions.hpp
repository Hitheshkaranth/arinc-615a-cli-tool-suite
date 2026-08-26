// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Struct Arinc615a::Tftp::Arinc615aOptions.
 **/

#ifndef ARINC_615A_TFTP_ARINC615AOPTIONS_HPP
#define ARINC_615A_TFTP_ARINC615AOPTIONS_HPP

#include <arinc_615a/tftp/Tftp.hpp>

#include <arinc_649/CheckValue.hpp>

#include <tftp/packets/Packets.hpp>

#include <iosfwd>
#include <optional>
#include <string>
#include <string_view>

namespace Arinc615a::Tftp {

/**
 * @name ARINC 615A TFTP Options
 *
 * @sa @ref KnownOptions
 * @sa @ref Arinc615aOptions
 *
 * @{
 **/

/**
 * @brief ARINC 615A Specialisation of TFTP Option List.
 *
 * This class adds options, which are specific to the ARINC 615A Load Protocol.
 * These are:
 *  - Part Number Option,
 *  - Checksum Options, and
 *  - Port Option.
 *
 * @par ARINC 615A Part Number Option
 * This option should be used in the ARINC Report 615A protocols to identify the part containing the file being
 * requested.
 * This option is required to allow simultaneous uploads of multiple parts where there are duplicate file names across
 * the multiple parts to a target.
 * The option value should be the part number of the part containing the file being requested.
 * It should be used by the target hardware for TFTP client read requests of data and support files of the upload
 * operation.
 *
 * @par ARINC 615A Checksum Option
 * The Checksum Option is a TFTP option that provides the method by which the Target Hardware Loader indicates to the
 * Data Loader the Cyclic Redundancy Check (CRC) of the data file to be transferred as part of a file read or write
 * request.
 * Option Purpose for %Target Hardware %File Read:
 *  - The *Checksum Option* is used to distinguish between two files with the same file name during an upload operation.
 *  - The %Target Hardware should include the 16-bit CRC from the ARINC 665 load header file.
 *  - The Data Loader should use the included CRC to locate the requested file.
 *
 * @note
 * Neither %Target Hardware nor Data Loader is intended to calculate the CRC as part of the upload.
 *
 * Option Purpose for %Target Hardware %File Write:
 *  - The *Checksum Option* provides a way for CRC integrity checking of files written to the Data Loader during a
 *    *Download Operation* or *%Information Operation*.
 *  - The check value could be stored and exported to ground tools for later validation that the data had not been
 *    changed during storage.
 *
 * @par ARINC 615A Port Option
 * It indicates to the %Target Hardware Loader that the Data Loader is requesting that all requests from the %Target
 * Hardware Loader be at the new port value instead of the ARINC 615A default control port.
 * The *Port Option* is either accepted or rejected by the %Target Hardware Loader.
 * If it is rejected by the %Target Hardware, the load operation may fail.
 *
 * The Port Option is only valid for operation initialisation requests sent by the Data Loader.
 * Use of this Port Option can only change the Data Loader control port and cannot change the %Target Hardware control
 * port.
 * This supports the operation of the Data Loader in that the Data Loader always initiates any data load operation by
 * sending a read or write request to the %Target Hardware’s control port.
 **/
struct ARINC_615A_EXPORT Arinc615aOptions
{
  //! Port Option
  std::optional< uint16_t > port;
  //! Part Number
  std::string partNumber;
  //! Checksum Option
  Arinc649::CheckValue checksum;

  /**
   * @brief Returns if any option is set.
   *
   * @return If any option is set
   **/
  explicit operator bool() const noexcept
  {
    return port || !partNumber.empty() || ( Arinc649::CheckValueType::NotUsed != checksum.type() );
  }

  /**
   * @brief Generates TFTP Options for @p options.
   *
   * @return TFTP Options for @p options
   **/
  [[nodiscard]] ::Tftp::Packets::Options options() const;
};

/**
 * @brief Returns the Option String for the given Option.
 *
 * @param[in] option
 *   ARINC 615A TFTP Option.
 *
 * @return Returns the option name.
 **/
[[nodiscard]] ARINC_615A_EXPORT std::string_view Arinc615aOptions_name( KnownOptions option ) noexcept;

/**
 * @brief Returns the Option String for the given Check Value Type.
 *
 * @param[in] type
 *   ARINC 665 Check Value Type.
 *
 * @return Returns the option name.
 **/
[[nodiscard]] ARINC_615A_EXPORT std::string_view Arinc615aOptions_name( Arinc649::CheckValueType type ) noexcept;

/**
 * @brief Returns a string, which describes the ARINC 615A Options.
 *
 * This operation is used for debugging and information.
 *
 * The format is `{Name:Value};`.
 *
 * @param[in] options
 *   ARINC 615A Options.
 *
 * @return Options Description.
 * @retval `(NONE)`
 *   When @p options is empty.
 **/
[[nodiscard]] ARINC_615A_EXPORT std::string Arinc615aOptions_toString( const Arinc615aOptions &options );

/**
 * @brief Tries to decode Checksum Option from the given option list.
 *
 * The @p std::pair< bool, Arinc649::CheckValue > is used to indicate if the checksum option processing was successful
 * and the decoded check value.
 * When no checksum option is provided, success is indicated with an empty check value.
 *
 * @param[in,out] options
 *   TFTP Options list
 *
 * @return Decoded Option
 **/
[[nodiscard]] ARINC_615A_EXPORT std::pair< bool, Arinc649::CheckValue > Arinc615aOptions_checksum(
  ::Tftp::Packets::Options &options );

/**
 * @brief Outputs @p options to @p stream.
 *
 * @param[in,out] stream
 *   Output Stream.
 * @param[in] options
 *   ARINC 615A Options.
 *
 * @return @p stream
 **/
ARINC_615A_EXPORT std::ostream& operator <<( std::ostream &stream, const Arinc615aOptions &options );

/** @} **/

}

#endif
