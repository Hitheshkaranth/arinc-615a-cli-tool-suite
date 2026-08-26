// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Information::Ratio.
 **/

#ifndef ARINC_615A_INFORMATION_RATIO_HPP
#define ARINC_615A_INFORMATION_RATIO_HPP

#include <arinc_615a/information/Information.hpp>

#include <string>
#include <iosfwd>

namespace Arinc615a::Information {

/**
 * @brief Represents the ratio within the status files.
 *
 * The Ratio element is used within the _ARINC 615A Download_ and _ARINC 615A Upload_ status information to indicate the
 * operation and load upload progress.
 *
 * @sa @ref DownloadStatus
 * @sa @ref UploadStatus
 * @sa @ref UploadLoadStatus
 **/
class ARINC_615A_EXPORT Ratio final
{
  public:
    //! Default Initialisation of Ratio.
    Ratio() noexcept = default;

    /**
     * @brief Creates a ratio with the initial ratio.
     *
     * @param[in] ratio
     *   Initial ratio [0..100]
     *
     * @throw Arinc615aException
     *   If the value is out of range.
     **/
    explicit Ratio( unsigned int ratio );

    /**
     * @brief Returns the ratio as an unsigned integer.
     *
     * @return Ratio as integer.
     **/
    [[nodiscard]] unsigned int value() const noexcept;

    /**
     * @brief Returns the ratio as an unsigned integer.
     *
     * @return Ratio as integer.
     **/
    explicit operator unsigned int() const noexcept;

    /**
     * @brief Assigns new ratio.
     *
     * @param[in] ratio
     *   Initial ratio [0..100]
     * @return *this
     *
     * @throw Arinc615aException
     *   If the value is out of range.
     **/
    Ratio& operator =( unsigned int ratio );

  private:
    //! Ratio Value.
    unsigned int ratioV{ 0U };
};

/**
 * @brief Ratio Stream Output Operator.
 *
 * @sa @ref Ratio::value()
 *
 * @param[in,out] stream
 *   Output Stream
 * @param ratio
 *   Ratio
 *
 * @return @p stream for chaining
 **/
ARINC_615A_EXPORT std::ostream &operator<<( std::ostream &stream, const Ratio &ratio );

}

namespace std {

/**
 * @brief Specialisation of @p std::formatter for @ref Arinc615a::Information::Ratio
 **/
template <>
struct formatter< Arinc615a::Information::Ratio > : std::formatter< std::string_view >
{
  /**
   * @brief Arinc615a::Information::Ratio format routine.
   *
   * @tparam FmtContext
   *   Formatting Context
   * @param[in] ratio
   *   Ratio
   * @param[in,out] ctx
   *   Formatting Context
   *
   * @return Iterator to the end of output.
   **/
  template< class FmtContext >
  FmtContext::iterator format( const Arinc615a::Information::Ratio &ratio, FmtContext &ctx ) const
  {
    return std::formatter< string_view >::format( std::format( "{}%", ratio.value() ), ctx );
  }
};

}

#endif
