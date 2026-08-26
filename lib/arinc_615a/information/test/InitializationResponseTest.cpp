// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of unit tests of class Arinc615a::Files::ProtocolFile.
 **/

#include <arinc_615a/information/InitializationResponse.hpp>

#include <arinc_615a/Arinc615aException.hpp>

#include <boost/test/unit_test.hpp>

namespace Arinc615a::Information {

BOOST_AUTO_TEST_SUITE( Arinc615aTest )
BOOST_AUTO_TEST_SUITE( InformationTest )
BOOST_AUTO_TEST_SUITE( InitializationResponseTest )

//! Constructor Test
BOOST_AUTO_TEST_CASE( Constructor1 )
{
  InitializationResponse response{ OperationAcceptanceStatusCode::OperationDenied, "Denied" };

  BOOST_CHECK( response.code() == OperationAcceptanceStatusCode::OperationDenied );
  BOOST_CHECK( response.description() == "Denied" );
}

//! Set/ Get Test
BOOST_AUTO_TEST_CASE( SetGet)
{
  InitializationResponse response{};

  BOOST_CHECK( response.code() == OperationAcceptanceStatusCode::OperationAccepted );
  BOOST_CHECK( response.description().empty() );

  response.code( OperationAcceptanceStatusCode::OperationDenied );
  response.description( "Denied" );

  BOOST_CHECK( response.code() == OperationAcceptanceStatusCode::OperationDenied );
  BOOST_CHECK( response.description() == "Denied" );

  response.code( OperationAcceptanceStatusCode::OperationNotSupported );
  response.description( "Unsupported" );

  BOOST_CHECK( response.code() == OperationAcceptanceStatusCode::OperationNotSupported );
  BOOST_CHECK( response.description() == "Unsupported" );
}

//! Validate
BOOST_AUTO_TEST_CASE( Validate )
{
  InitializationResponse response{};

  BOOST_CHECK_NO_THROW( response.validate() );

  response.description( "FAIL" );

  BOOST_CHECK( response.description() == "FAIL" );
  BOOST_CHECK_THROW( response.validate(), Arinc615aException );
}

//! Compare
BOOST_AUTO_TEST_CASE( Compare)
{
  InitializationResponse response{};

  BOOST_CHECK( response == InitializationResponse{ OperationAcceptanceStatusCode::OperationAccepted } );
  BOOST_CHECK( response != InitializationResponse{ OperationAcceptanceStatusCode::OperationNotSupported } );
  BOOST_CHECK( (response != InitializationResponse{ OperationAcceptanceStatusCode::OperationAccepted, "XX" } ) );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
