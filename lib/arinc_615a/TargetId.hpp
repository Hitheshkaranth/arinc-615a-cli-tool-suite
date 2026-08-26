// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::TargetId.
 **/

#ifndef ARINC_615A_TARGETID_HPP
#define ARINC_615A_TARGETID_HPP

#include <arinc_615a/Arinc615a.hpp>

#include <format>
#include <iosfwd>
#include <string>
#include <string_view>
#include <tuple>

namespace Arinc615a {

/**
 * @brief ARINC 615A %Target ID.
 *
 * A _ARINC 615A %Target ID_ is formed by the string `[THW_ID]_[POSITION]`.
 *
 * Validation is decoupled from storage:
 * you can construct an invalid _%Target ID_ and later query compliance via operator bool(), rather than throwing on
 * construction.
 **/
class ARINC_615A_EXPORT TargetId final
{
  public:
    //! Minimal Size of THW ID
    static constexpr std::size_t ThwIdSizeMin{ 4U };

    //! Maximum Size of THW ID
    static constexpr std::size_t ThwIdSizeMax{ 15U };

    //! Minimal Size of Position
    static constexpr std::size_t PositionSizeMin{ 0U };

    //! Maximum Size of Position
    static constexpr std::size_t PositionSizeMax{ 8U };

    /**
     * @brief Splits a %Target ID to its *THW ID* and *Position* Parts.
     *
     * @param[in] targetId
     *   %Target ID, which shall be split.
     *
     * @return std::tuple with [THW ID, Position].
     *   The result is based on @p targetId and must be valid beyond usage.
     * @retval {}
     *   When the target ID cannot be split (in-compliant).
     **/
    [[nodiscard]] static std::tuple< std::string_view, std::string_view > splitTargetId( std::string_view targetId );

    /**
     * @brief Test the %Target ID splitting.
     *
     * @param[in] targetId
     *   %Target ID, which shall be tested.
     *
     * @return If the split was successful.
     **/
    [[nodiscard]] static bool isTargetId( std::string_view targetId ) noexcept;

    /**
     * @brief Test if the given String is a valid %Target Hardware ID.
     *
     * @param[in] thwId
     *   %Target Hardware ID, which shall be tested.
     *
     * @return If @p thwId is a valid %Target Hardware ID.
     **/
    [[nodiscard]] static bool isThwId( std::string_view thwId ) noexcept;

    /**
     * @brief Test if the given String is a valid %Target Hardware Position.
     *
     * @param[in] position
     *   %Target Hardware Position, which shall be tested.
     *
     * @return If the @p position is a valid %Target Hardware Position.
     **/
    [[nodiscard]] static bool isPosition( std::string_view position ) noexcept;

    //! Constructor
    TargetId() = default;

    /**
     * @brief Copy Constructor.
     *
     * @param[in] targetId
     *   Assigned %Target ID
     **/
    TargetId( const TargetId &targetId ) = default;

    /**
     * @brief Move Constructor.
     *
     * @param[in] targetId
     *   Assigned %Target ID
     **/
    TargetId( TargetId &&targetId ) noexcept = default;

    /**
     * @brief Constructs the %Target ID with the given string.
     *
     * @param[in] targetId
     *   %Target ID.
     **/
    explicit TargetId( std::string_view targetId );

    /**
     * @brief Constructs the %Target ID with the given Strings.
     *
     * @param[in] thwId
     *   %Target Hardware ID (THW ID)
     * @param[in] position
     *   %Target Position
     **/
    TargetId( std::string thwId, std::string position );

    //! Destructor
    ~TargetId() noexcept = default;

    /**
     * @name %Target Hardware ID (THW ID)
     * @{
     **/

    /**
     * @brief Returns the %Target Hardware ID (THW ID).
     *
     * @return %Target Hardware ID (THW ID)
     **/
    [[nodiscard]] std::string_view thwId() const;

    /**
     * @brief Updates the %Target Hardware ID (THW ID).
     *
     * @param[in] thwId
     *   The new %Target Hardware ID (THW ID).
     **/
    void thwId( std::string thwId );

    //! @}

    /**
     * @name %Target Position
     * @{
     **/

    /**
     * @brief Returns the %Target Position.
     *
     * @return %Target Position.
     **/
    [[nodiscard]] std::string_view position() const;

    /**
     * @brief Updates the %Target Position.
     *
     * @param[in] position
     *   %Target Position.
     **/
    void position( std::string position );

    //! @}

    /**
     * @brief Returns the %Target ID as a @p std::string.
     *
     * @return %Target ID.
     **/
    [[nodiscard]] std::string toString() const;

    /**
     * @brief Cast-operator to std::string.
     *
     * @return %Target ID as string.
     **/
    explicit operator std::string() const;

    /**
     * @brief Returns if the %Target ID is compliant.
     *
     * @return If the %Target ID is compliant.
     **/
    explicit operator bool() const;

    /**
     * @brief Assignment Operator.
     *
     * @param[in] other
     *   Assigned %Target ID
     *
     * @return *this
     **/
    TargetId& operator=( const TargetId &other ) = default;

    /**
     * @brief Move-assignment Operator.
     *
     * @param[in] other
     *   Assigned %Target ID
     *
     * @return *this
     **/
    TargetId& operator=( TargetId &&other ) noexcept = default;

    /**
     * @brief String-assignment Operator.
     *
     * @param[in] other
     *   Assigned %Target ID
     *
     * @return *this
     **/
    TargetId& operator=( std::string_view other );

    /**
     * @brief Three-way Comparison Operator.
     *
     * @param[in] other
     *   %Target ID to compare with.
     *
     * @return Comparison result of @p this and @p other.
     **/
    inline auto operator<=>( const TargetId &other ) const;

    /**
     * @brief Equal Comparison Operation.
     *
     * @param[in] other
     *   The right-hand side operand of the operator.
     *
     * @return The result of the equal comparison operation between @p lhs and @p rhs.
     **/
    bool operator==( const TargetId &other ) const = default;

    /**
     * @brief Unequal Comparison Operation.
     *
     * @param[in] other
     *   The right-hand side operand of the operator.
     *
     * @return The result of the unequal comparison operation between @p lhs and @p rhs.
     **/
    bool operator!=( const TargetId &other ) const = default;

    /**
     * @brief String-comparison operator.
     *
     * @param[in] other
     *   %Target ID to compare with.
     *
     * @return Equality of @p this and @p other.
     * @retval true
     *   @p this and @p other are equal.
     * @retval false
     *   @p this and @p other are different.
     **/
    bool operator==( std::string_view other ) const;

  private:
    /**
     * @brief Checks String Compliance.
     *
     * Checks if:
     * - length of @p stringToCheck is in range [@p minLength, @p maxLength], and
     * - @p stringToCheck only contains alphanumeric characters.
     *
     * @param[in] stringToCheck
     *   String to be validated.
     * @param[in] minLength
     *   Minimal allowed length of @p stringToCheck.
     * @param[in] maxLength
     *   Maximum allowed length of @p stringToCheck.
     *
     * @pre
     * This operation must be called with @p minLength <= @p maxLength
     *
     * @return If the compliance check is passed.
     **/
    [[nodiscard]] static bool isCompliant( std::string_view stringToCheck, size_t minLength, size_t maxLength );

    //! %Target Hardware ID
    std::string thwIdV;
    //! Position
    std::string positionV;
};

inline auto TargetId::operator <=>( const TargetId &other ) const
{
  return toString() <=> other.toString();
}

/**
 * @name %Target ID Operators
 *
 * @sa @ref TargetId
 *
 * @{
 **/

/**
 * @brief String-comparison operator.
 *
 * @param[in] lhs
 *   %Target ID (left-hand side) to compare with.
 * @param[in] rhs
 *   %Target ID (right-hand side) to compare with.
 *
 * @return Equality of @p lhs and @p rhs.
 * @retval true
 *   @p lhs and @p rhs are equal.
 * @retval false
 *   @p lhs and @p rhs are different.
 **/
auto operator <=>( std::string_view lhs, const TargetId &rhs );

/**
 * @brief Outputs the %Target ID to Stream.
 *
 * @param[in,out] stream
 *   Output stream.
 * @param[in] targetId
 *   %Target ID.
 *
 * @return @p stream
 **/
ARINC_615A_EXPORT std::ostream& operator<<( std::ostream &stream, const TargetId &targetId );

/**
 * @brief Reads the %Target ID from Stream.
 *
 * @param[in,out] stream
 *   Output stream.
 * @param[out] targetId
 *   %Target ID.
 *
 * @return @p stream
 **/
ARINC_615A_EXPORT std::istream& operator>>( std::istream &stream, TargetId &targetId );

/** @} **/

inline auto operator <=>( const std::string_view lhs, const TargetId &rhs )
{
  return lhs <=> rhs.toString();
}

}

namespace std {

/**
 * @brief Specialisation of @p std::formatter for @ref Arinc615a::TargetId.
 *
 * @sa @ref Arinc615a::TargetId
 **/
template <>
struct formatter< Arinc615a::TargetId > : std::formatter< std::string_view >
{
  /**
   * @brief Arinc615a::TargetId format routine.
   *
   * @tparam FmtContext
   *   Formatting Context
   * @param[in] targetId
   *   Target ID
   * @param[in,out] ctx
   *   Formatting Context
   *
   * @return Iterator to the end of the output.
   **/
  template< class FmtContext >
  FmtContext::iterator format( const Arinc615a::TargetId &targetId, FmtContext &ctx ) const
  {
    return std::formatter< string_view >::format( targetId.toString(), ctx );
  }
};

}

#endif
