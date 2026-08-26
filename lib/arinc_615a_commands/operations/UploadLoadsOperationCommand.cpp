// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615aCommands::Operations::UploadLoadsOperationCommand.
 **/

#include "UploadLoadsOperationCommand.hpp"

#include <arinc_615a/find/TargetInformation.hpp>

#include <arinc_615a/host/OperationConfiguration.hpp>
#include <arinc_615a/host/Protocol.hpp>
#include <arinc_615a/host/UploadOperation.hpp>

#include <arinc_615a/information/InitializationResponse.hpp>
#include <arinc_615a/information/UploadStatus.hpp>
#include <arinc_615a/information/UploadLoad.hpp>

#include <arinc_615a/tftp/servers/ReadOperation.hpp>

#include <arinc_615a/tftp/Arinc615aOptions.hpp>

#include <arinc_615a/StatusCodeDescription.hpp>

#include <arinc_665/media/Load.hpp>
#include <arinc_665/media/MediaSet.hpp>

#include <arinc_665/utils/FilesystemLoadDecompiler.hpp>

#include <tftp/packets/PacketStatistic.hpp>
#include <tftp/packets/TftpOptions.hpp>

#include <tftp/files/StreamFile.hpp>

#include <helper/BoostAsioProgramOptions.hpp>
#include <helper/SeverityLevelDescription.hpp>

#include <spdlog/spdlog.h>

#include <boost/exception/all.hpp>

#include <boost/program_options.hpp>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include <format>
#include <iostream>

namespace Arinc615aCommands::Operations {

UploadLoadsOperationCommand::UploadLoadsOperationCommand(
  boost::asio::io_context &ioContext,
  AbortTerminateSignal &abortSignal,
  AbortTerminateSignal &terminateSignal ) :
  ioContextV{ ioContext },
  abortSignalV{ abortSignal },
  terminateSignalV{ terminateSignal },
  optionsDescriptionV{ "ARINC 615A Upload Loads Operation Options" }
{
  optionsDescriptionV.add_options()
  (
    "log-level,l",
    boost::program_options::value< spdlog::level::level_enum >()
      ->default_value( spdlog::level::level_enum::warn, "warn" )
      ->value_name( "log-level" )
      ->notifier( []( const auto &logLevel ) {
        spdlog::set_level( logLevel );
      }),
    Helper::SeverityLevelDescription::instance().allLevels().c_str()
  );
  optionsDescriptionV.add( configurationV.options() );
  optionsDescriptionV.add_options()
  (
    "target-address,a",
    boost::program_options::value( &targetAddressV )
      ->value_name( "ip" ),
    "IP address of target hardware."
  )
  (
    "targets-list,l",
    boost::program_options::value( &targetsListPathV )
      ->value_name( "file" ),
    "List of targets JSON File to map Target IDs to IP addresses."
  )
  (
    "target-id,i",
    boost::program_options::value( &targetIdV )
      ->required()
      ->value_name( "target-id" ),
    "ARINC 615A Target ID.\n"
    "Required."
  )
  (
    "dlp-timeout",
    boost::program_options::value< std::chrono::seconds::rep >()
      ->default_value( Arinc615a::DefaultArinc615aDlpTimeout.count() )
      ->value_name( "timeout" )
      ->notifier(
        [ &dlpTimeout = dlpTimeoutV ]( const auto dlpTimeoutInt )
        {
          dlpTimeout = std::chrono::seconds{ dlpTimeoutInt };
        } ),
    "ARINC 615A DLP timeout in seconds."
  )
  (
    "port-option",
    boost::program_options::bool_switch( &portOptionV ),
    "Enables the ARINC 615A-3 Port Option."
  )
  (
    "check-file-integrity,c",
    boost::program_options::value( &checkFileIntegrityV )
      ->default_value( true, "true" )
      ->implicit_value( true, "true" )
      ->value_name( "true|false" ),
    "Check file integrity during initialisation."
  )
  (
    "load-header,f",
    boost::program_options::value( &loadHeadersV )
      ->composing()
      ->required()
      ->value_name( "Filepath" ),
    "Load Header path to send to target.\n"
    "Required."
  );
}

void UploadLoadsOperationCommand::execute( const Commands::Parameters &parameters )
{
  try
  {
    std::cout << "ARINC 615A Upload Loads Operation\n";

    boost::program_options::variables_map variablesMap;
    boost::program_options::store(
      boost::program_options::command_line_parser( parameters ).options( optionsDescriptionV ).run(),
      variablesMap );
    boost::program_options::notify( variablesMap );

    if ( !targetsListPathV.empty() )
    {
      boost::property_tree::ptree targetsListPTree;
      boost::property_tree::read_json( targetsListPathV.string(), targetsListPTree );

      const auto targetsInformation{ Arinc615a::Find::TargetsAddressInformationMap_fromTargetsAddressInformation(
        Arinc615a::Find::TargetInformation::targetsAddressInformation( targetsListPTree ) )};

      if ( const auto targetInformation{ targetsInformation.find( targetIdV ) }; targetInformation != targetsInformation.end() )
      {
        SPDLOG_INFO(
          "Use target address '{}' for '{}'",
          std::get< 0 >( targetInformation->second ).to_string(),
          targetIdV );
        targetAddressV = std::get< 0 >( targetInformation->second );
      }
    }

    if ( targetAddressV.is_unspecified() )
    {
      BOOST_THROW_EXCEPTION(
        boost::program_options::error{ "Target IP address is invalid, not provided, or not in the target list." } );
    }

    const auto protocol{ Arinc615a::Host::Protocol::instance( ioContextV ) };
    assert( protocol );

    // find load headers
    for ( const auto &loadHeader : loadHeadersV )
    {
      spdlog::info( "Processing load header: {}", loadHeader.string() );

      auto loadDecompiler{ Arinc665::Utils::FilesystemLoadDecompiler::create() };
      assert( loadDecompiler );

      loadDecompiler
        ->checkFileIntegrity( checkFileIntegrityV )
        .loadPath( loadHeader );

      auto [ load, mediaSet, checkValues ]{ ( *loadDecompiler )() };

      // save results
      mediaSetPathsV.emplace( std::move( mediaSet ), loadHeader.parent_path() );
      loadsV.emplace_back( std::move( load ) );
      checkValuesV.merge( checkValues );
    }

    operationV = protocol->uploadOperation(
      Arinc615a::Host::UploadOperationConfiguration{
        .dataLoaderConfiguration = configurationV,
        .handler = *this,
        .targetAddress = targetAddressV,
        .targetId = Arinc615a::TargetId{ targetIdV },
        .dlpTimeout = dlpTimeoutV,
        .portOption = portOptionV,
      } );
    assert( operationV );

    boost::signals2::scoped_connection abortConnection{ abortSignalV.connect(
      std::bind_front(
        &Arinc615a::Host::UploadOperation::abort,
        operationV,
        Arinc615a::Host::UploadOperation::AbortReason::Operator ) ) };
    boost::signals2::scoped_connection terminateConnection{ terminateSignalV.connect(
      std::bind_front(
        &Arinc615a::Host::UploadOperation::terminate,
        operationV,
        Arinc615a::Host::UploadOperation::AbortReason::Operator ) ) };

    // execute operation
    operationV->start();

    // Wait for completion
    done.wait();

    // Print TFTP Statistic
    std::cout
      << "TFTP RX:\n" << Tftp::Packets::PacketStatistic::globalReceive() << "\n"
      << "TFTP TX:\n" << Tftp::Packets::PacketStatistic::globalTransmit() << "\n";
  }
  catch ( const boost::program_options::error & )
  {
    // Command line parsing errors are handled within the command handler
    throw;
  }
  catch ( const boost::exception &e )
  {
    std::cerr << std::format( "Upload Loads Operation failed: {}\n", boost::diagnostic_information( e ) );
  }
  catch ( const std::exception &e )
  {
    std::cerr << std::format( "Upload Loads Operation failed: {}\n", e.what() );
  }
  catch ( ... )
  {
    std::cerr << "Upload Loads Operation failed: UNKNOWN EXCEPTION\n";
  }
}

void UploadLoadsOperationCommand::help() const
{
  std::cout << "Upload Operation\n" << optionsDescriptionV;
}

void UploadLoadsOperationCommand::initialisationDeferred( const std::chrono::seconds waitTime )
{
  std::cout << std::format( "Operation deferred for: {}\n", waitTime );
}

void UploadLoadsOperationCommand::initialisationResponse( const Arinc615a::Information::InitializationResponse &response )
{
  std::cout << std::format(
    "Initialisation Response:\n"
    "\tInitialisation Code: {}\n"
    "\tStatus Description:  '{}'\n",
    response.code(),
    response.description() );
}

void UploadLoadsOperationCommand::finished( const Arinc615a::StatusCode code, std::string_view description )
{
  std::cout << std::format(
    "Operation finished:\n"
    "\tFinal Status Code:        {}\n"
    "\tFinal Status Description: '{}'\n",
    code,
    description );

  done.count_down();
}

void UploadLoadsOperationCommand::status( const Arinc615a::Information::UploadStatus &status )
{
  assert( operationV );

  std::cout << std::format(
    "Upload Status:\n"
    "\tCounter:            {}\n"
    "\tStatus Code:        {}\n"
    "\tStatus Description: '{}'\n"
    "\tRatio:              {}\n"
    "\tException Timer:    {}\n"
    "\tEstimated Time:     {}\n",
    status.counter(),
    status.code(),
    status.description(),
    status.listRatio(),
    status.exceptionTimer(),
    status.estimatedTime() );

  for ( const auto &loadInfo : status.loads() )
  {
    std::cout << std::format(
      "\tLoad Info:\n"
      "\t\tHeader Filename:    '{}'\n"
      "\t\tPart Number:        '{}'\n"
      "\t\tRatio:              {}\n"
      "\t\tStatus Code:        {}\n"
      "\t\tStatus Description: '{}'\n",
      loadInfo.headerFilename(),
      loadInfo.partNumber(),
      loadInfo.ratio(),
      loadInfo.code(),
      loadInfo.description() );
  }

  std::cout << std::flush;

  if ( ( status.code() == Arinc615a::StatusCode::OperationAccepted ) && ( !loadListTransmittedV ) )
  {
    // assemble upload load list
    Arinc615a::Information::UploadLoads loadsList;

    for ( const auto &load : loadsV )
    {
      assert( load );
      loadsList.emplace_back( Arinc615a::Information::UploadLoad{
        .headerFilename = std::string{ load->name() },
        .partNumber = std::string{ load->partNumber() } } );
    }

    // send the load list
    operationV->loadList( std::move( loadsList ) );

    loadListTransmittedV = true;
  }
}

void UploadLoadsOperationCommand::fileRequest(
  const boost::asio::ip::udp::endpoint &remote,
  const std::string_view filename,
  const Tftp::Packets::TftpOptions &clientTftpOptions,
  const std::string_view loadPartNumber,
  const Arinc649::CheckValue &checkValue )
{
  SPDLOG_INFO(
    "Request file '{}' Load Part Number '{}' Check Value '{}'",
    filename,
    loadPartNumber,
    checkValue.format() );

  const auto file{ Arinc665::Media::Loads_file( loadsV, checkValuesV, filename, loadPartNumber, checkValue ) };

  if ( !file )
  {
    SPDLOG_ERROR( "File '{}' was not found.", filename );
    operationV->errorOperation(
      remote,
      ::Tftp::Packets::ErrorCode::FileNotFound,
      std::format( "File '{}' was not found.", filename ) );
    return;
  }

  const auto mediaSetPath{ mediaSetPathsV.find( file->mediaSet() ) };

  if ( mediaSetPath == mediaSetPathsV.end() )
  {
    SPDLOG_ERROR( "Path for Media Set '{}' was not found.", file->mediaSet()->partNumber() );
    operationV->errorOperation(
      remote,
      ::Tftp::Packets::ErrorCode::FileNotFound,
      std::format( "File '{}' was not found.", filename ) );
    return;
  }

  const auto realFilename{ ( mediaSetPath->second / file->path().relative_path() ).lexically_normal() };

  SPDLOG_INFO( "Local file '{}'", realFilename.string() );

  if ( !std::filesystem::is_regular_file( realFilename ) )
  {
    SPDLOG_ERROR( "File '{}' was not locally found.", realFilename.string() );
    operationV->errorOperation(
      remote,
      Tftp::Packets::ErrorCode::FileNotFound,
      std::format( "File '{}' was not found.", filename ) );
    return;
  }

  const auto streamFile{ std::make_shared< Tftp::Files::StreamFile >(
    Tftp::Files::StreamFile::Operation::Transmit,
    realFilename,
    std::filesystem::file_size( realFilename ) ) };
  assert( streamFile );

  auto fileOperation{
    operationV->fileTransfer( streamFile, remote, clientTftpOptions, std::string{ loadPartNumber }, checkValue ) };
  assert( fileOperation );

  fileOperation
    ->completionHandler( std::bind_front( &UploadLoadsOperationCommand::fileCompleted, this, fileOperation ) );

  fileOperationsV.emplace_front( std::move( fileOperation ) )->start();
}

void UploadLoadsOperationCommand::fileCompleted(
  const Arinc615a::Tftp::Servers::ReadOperationPtr &operation,
  const Tftp::TransferStatus status )
{
  if ( 1U != fileOperationsV.remove( operation ) )
  {
    SPDLOG_ERROR( "File operation completed, which was not created" );
    operationV->abort( Arinc615a::Host::Operation::AbortReason::Protocol );
  }

  if ( Tftp::TransferStatus::Successful != status )
  {
    SPDLOG_WARN( "File transfer failed - Ignore it from the host side" );
    return;
  }
}

}
