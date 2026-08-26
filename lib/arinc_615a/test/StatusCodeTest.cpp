// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of unit tests for module Arinc615a StatusCode.
 **/

#include <arinc_615a/Arinc615aException.hpp>
#include <arinc_615a/StatusCode.hpp>

#include <boost/test/unit_test.hpp>

namespace Arinc615a {

BOOST_AUTO_TEST_SUITE( Arinc615aTest )
BOOST_AUTO_TEST_SUITE( StatusCodeTest )

//! Status Code conversion test
BOOST_AUTO_TEST_CASE( statusCodeTest )
{
  BOOST_CHECK(
    statusCode( std::to_underlying( StatusCode::OperationAccepted ) )
      == StatusCode::OperationAccepted );
  BOOST_CHECK(
    statusCode( std::to_underlying( StatusCode::OperationNotAccepted ) )
      == StatusCode::OperationNotAccepted );
  BOOST_CHECK(
    statusCode( std::to_underlying( StatusCode::OperationNotSupported ) )
      == StatusCode::OperationNotSupported );
  BOOST_CHECK(
    statusCode( std::to_underlying( StatusCode::OperationInProgress ) )
      == StatusCode::OperationInProgress );
  BOOST_CHECK(
    statusCode( std::to_underlying( StatusCode::OperationCompleted ) )
      == StatusCode::OperationCompleted );
  BOOST_CHECK(
    statusCode( std::to_underlying( StatusCode::OperationInProgressAdditionalInfo ) )
      == StatusCode::OperationInProgressAdditionalInfo );
  BOOST_CHECK(
    statusCode( std::to_underlying( StatusCode::OperationAbortedByTargetHw ) )
      == StatusCode::OperationAbortedByTargetHw );
  BOOST_CHECK(
    statusCode( std::to_underlying( StatusCode::OperationAbortedByDlp ) )
      == StatusCode::OperationAbortedByDlp );
  BOOST_CHECK(
    statusCode( std::to_underlying( StatusCode::OperationAbortedByOperator ) )
      == StatusCode::OperationAbortedByOperator );
  BOOST_CHECK(
    statusCode( std::to_underlying( StatusCode::LoadPartNumberOrDownloadFileFailed ) )
      == StatusCode::LoadPartNumberOrDownloadFileFailed );

  BOOST_CHECK_THROW( boost::ignore_unused( statusCode(
    std::to_underlying( StatusCode::OperationDeferred ) ) ), Arinc615aException );
  BOOST_CHECK_THROW( boost::ignore_unused( statusCode(
    std::to_underlying( StatusCode::Invalid ) ) ), Arinc615aException );

  BOOST_CHECK_THROW( boost::ignore_unused( statusCode( 0x1234 ) ), Arinc615aException );
}

//! Operation Acceptance Status Code Test
BOOST_AUTO_TEST_CASE( operationAcceptanceStatusCodeTest )
{
  BOOST_CHECK(
    operationAcceptanceStatusCode( std::to_underlying( OperationAcceptanceStatusCode::OperationAccepted ) )
      == OperationAcceptanceStatusCode::OperationAccepted );
  BOOST_CHECK(
    operationAcceptanceStatusCode( std::to_underlying( OperationAcceptanceStatusCode::OperationDenied ) )
      == OperationAcceptanceStatusCode::OperationDenied );
  BOOST_CHECK(
    operationAcceptanceStatusCode( std::to_underlying( OperationAcceptanceStatusCode::OperationNotSupported ) )
      == OperationAcceptanceStatusCode::OperationNotSupported );

  BOOST_CHECK_THROW( boost::ignore_unused( operationAcceptanceStatusCode(
    std::to_underlying( OperationAcceptanceStatusCode::OperationDeferred ) ) ), Arinc615aException );

  BOOST_CHECK_THROW( boost::ignore_unused( operationAcceptanceStatusCode( 0x1234 ) ), Arinc615aException );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
