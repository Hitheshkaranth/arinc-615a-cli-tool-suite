// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615aCommands::Operations::BatchUploadOperationCommand.
 **/

#include "BatchUploadOperationCommand.hpp"

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
#include <arinc_615a/TargetId.hpp>

#include <arinc_665/media/Batch.hpp>
#include <arinc_665/media/Load.hpp>
#include <arinc_665/media/MediaSet.hpp>

#include <arinc_665/utils/MediaSetManager.hpp>

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
#include <ranges>

namespace Arinc615aCommands::Operations {

BatchUploadOperationCommand::BatchUploadOperationCommand(
  boost::asio::io_context &ioContext,
  AbortTerminateSignal &abortSignal,
  AbortTerminateSignal &terminateSignal ) :
  ioContextV{ ioContext },
  abortSignalV{ abortSignal },
  terminateSignalV{ terminateSignal },
  optionsDescriptionV{ "ARINC 615A Batch Upload Operation Options" }
{
  optionsDescriptionV.add_options()
  (
    "log-level,l",
    boost::program_options::value< spdlog::level::level_enum >()
      ->default_value( spdlog::level::level_enum::warn, "warn" )
      ->value_name( "log-level" )
      ->notifier( []( const auto &logLevel ) {
        spdlog::set_level( logLevel );
        Arinc615aCommands::setLogLevel( logLevel );
        Arinc615a::setLogLevel( logLevel );
        Tftp::setLogLevel( logLevel );
        Arinc665::setLogLevel( logLevel );
        Arinc649::setLogLevel( logLevel );
        Helper::setLogLevel( logLevel );
      } ),
    Helper::SeverityLevelDescription::instance().allLevels().c_str()
  );
  optionsDescriptionV.add( configurationV.options() );
  optionsDescriptionV.add_options()
  (
    "targets-list,l",
    boost::program_options::value( &targetsListPathV )
      ->value_name( "file" )
      ->required(),
    "List of targets JSON File to map Target IDs to IP addresses.\n"
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
    "media-set-manager-dir,d",
    boost::program_options::value( &mediaSetManagerDirectoryV )
      ->required()
      ->value_name( "Directory" ),
    "ARINC 665 Media Set Manager directory.\n"
    "Required."
  )
  (
    "check-media-set-manager-integrity",
    boost::program_options::value( &checkMediaSetManagerIntegrityV )
      ->default_value( true, "true" )
      ->implicit_value( true, "true" )
      ->value_name( "true|false" ),
    "Check Media Set Manager integrity during initialisation."
  )
  (
    "media-set-pn",
    boost::program_options::value( &mediaSetPartNumberV )
      ->required()
      ->value_name( "Media Set P/N" ),
    "Part Number of Media Set containing Loads.\n"
    "Required."
  )
  (
    "batch-file,f",
    boost::program_options::value( &batchFileV )
      ->required()
      ->value_name( "Filename" ),
    "Batch filename.\n"
    "Required."
  );
}

void BatchUploadOperationCommand::execute( const Commands::Parameters &parameters )
{
  try
  {
    std::cout << "ARINC 615A Batch Upload Operation\n";

    boost::program_options::variables_map variablesMap;
    boost::program_options::store(
      boost::program_options::command_line_parser( parameters ).options( optionsDescriptionV ).run(),
      variablesMap );
    boost::program_options::notify( variablesMap );

    boost::property_tree::ptree targetsListPTree{};
    boost::property_tree::read_json( targetsListPathV.string(), targetsListPTree );

    targetsV = Arinc615a::Find::TargetsAddressInformationMap_fromTargetsAddressInformation(
      Arinc615a::Find::TargetInformation::targetsAddressInformation( targetsListPTree ) );

    mediaSetManagerV = Arinc665::Utils::MediaSetManager::load(
      mediaSetManagerDirectoryV,
      checkMediaSetManagerIntegrityV,
      std::bind_front( &BatchUploadOperationCommand::loadProgress, this ) );
    assert( mediaSetManagerV );

    protocolV = Arinc615a::Host::Protocol::instance( ioContextV );
    assert( protocolV );

    const auto mediaSet{ mediaSetManagerV->mediaSet( mediaSetPartNumberV ) };

    // check for media set
    if ( !mediaSet )
    {
      BOOST_THROW_EXCEPTION(
        boost::program_options::error{ std::format( "Media Set '{}' does not exist", mediaSetPartNumberV ) } );
    }

    mediaSetV = mediaSet->first;
    checkValuesV = mediaSet->second;

    const auto batchesFound{ mediaSetV->recursiveBatches( batchFileV ) };

    if ( batchesFound.empty() )
    {
      BOOST_THROW_EXCEPTION( boost::program_options::error{ std::format( "Batch '{}' does not exist", batchFileV ) } );
    }

    if ( batchesFound.size() > 1U )
    {
      BOOST_THROW_EXCEPTION(
        boost::program_options::error{ std::format( "Batch '{}' exist multiple times", batchFileV ) } );
    }

    batchV = batchesFound.front();
    batchTargetsV = batchV->targets();

    SPDLOG_INFO( "Batch: '{}' '{}'", batchV->partNumber(), batchV->comment() );

    // Check for alle targets be in the target list
    for ( const auto &targetId : batchTargetsV | std::views::keys )
    {
      // check for target in targets list
      if ( !targetsV.contains( targetId ) )
      {
        BOOST_THROW_EXCEPTION(
          boost::program_options::error{ std::format( "Target '{}' must exist in target list", targetId ) } );
      }
    }

    // start target transfers
    nextBatchTarget();

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
    std::cerr << std::format( "Batch Upload Operation failed: {}\n", boost::diagnostic_information( e ) );
  }
  catch ( const std::exception &e )
  {
    std::cerr << std::format( "Batch Upload Operation failed: {}\n", e.what() );
  }
  catch ( ... )
  {
    std::cerr << "Batch Upload Operation failed: UNKNOWN EXCEPTION\n";
  }
}

void BatchUploadOperationCommand::help() const
{
  std::cout
    << "Batch Upload Operation\n"
    << optionsDescriptionV;
}

void BatchUploadOperationCommand::loadProgress(
  std::pair< std::size_t, std::size_t > mediaSet,
  std::string_view partNumber,
  std::pair< Arinc665::MediumNumber, Arinc665::MediumNumber > medium )
{
  std::cout << std::format(
    "Loading: {}/{} {} {}:{}\n",
    mediaSet.first,
    mediaSet.second,
    partNumber,
    medium.first,
    medium.second );
}

void BatchUploadOperationCommand::nextBatchTarget()
{
  if ( batchTargetsV.empty() )
  {
    done.count_down();
    return;
  }

  const auto target{ batchTargetsV.extract( batchTargetsV.begin() ) };
  const auto targetInformation{ targetsV.find( target.key() ) };
  const Arinc615a::TargetId targetId{ target.key() };

  std::cout << std::format(
    "Start loading of target {} ({})\n",
    targetId,
    std::get< 0 >( targetInformation->second ).to_string() );

  loadListTransmittedV = false;
  loadsV = target.mapped();

  batchUploadOperationProxyV = std::make_unique< Arinc615a::Host::BatchUploadOperationProxy >(
    targetId,
    static_cast< Arinc615a::Host::BatchUploadOperationHandler & >( *this ) );
  assert( batchUploadOperationProxyV );

  operationV = protocolV->uploadOperation(
    Arinc615a::Host::UploadOperationConfiguration{
      .dataLoaderConfiguration = configurationV,
      .handler = *batchUploadOperationProxyV,
      .targetAddress = std::get< 0 >( targetInformation->second ),
      .targetId = Arinc615a::TargetId{ target.key() },
      .dlpTimeout = dlpTimeoutV,
      .portOption = portOptionV
    } );
  assert( operationV );

  abortSignalV.connect(
    std::bind_front(
      &Arinc615a::Host::UploadOperation::abort,
      operationV,
      Arinc615a::Host::UploadOperation::AbortReason::Operator ) );
  terminateSignalV.connect(
    std::bind_front(
      &Arinc615a::Host::UploadOperation::terminate,
      operationV,
      Arinc615a::Host::UploadOperation::AbortReason::Operator ) );

  // execute operation
  operationV->start();
}

void BatchUploadOperationCommand::initialisationDeferred(
  const Arinc615a::TargetId &targetId,
  std::chrono::seconds waitTime )
{
  std::cout << std::format( "{}: Operation deferred for: {}\n", targetId, waitTime );
}

void BatchUploadOperationCommand::initialisationResponse(
  const Arinc615a::TargetId &targetId,
  const Arinc615a::Information::InitializationResponse &response )
{
  std::cout << std::format(
    "{}: Initialisation Response:\n"
    "\tInitialisation Code: {}\n"
    "\tStatus Description:  '{}'\n",
    targetId,
    response.code(),
    response.description() );
}

void BatchUploadOperationCommand::finished(
  const Arinc615a::TargetId &targetId,
  const Arinc615a::StatusCode code,
  std::string_view description )
{
  std::cout << std::format(
    "{}: Operation finished:\n"
    "\tFinal Status Code:        {}\n"
    "\tFinal Status Description: '{}'\n",
    targetId,
    code,
    description );

  operationV.reset();

  if ( Arinc615a::StatusCode::OperationCompleted != code )
  {
    // abort
    done.count_down();
    return;
  }

  // initiate transfer of the next target
  nextBatchTarget();
}

void BatchUploadOperationCommand::status(
  const Arinc615a::TargetId &targetId,
  const Arinc615a::Information::UploadStatus &status )
{
  assert( operationV );

  std::cout << std::format(
    "{}: Upload Status:\n"
    "\tCounter:            {}\n"
    "\tStatus Code:        {}\n"
    "\tStatus Description: '{}'\n"
    "\tRatio:              {}\n"
    "\tException Timer:    {}\n"
    "\tEstimated Time:     {}\n",
    targetId,
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

void BatchUploadOperationCommand::fileRequest(
  const Arinc615a::TargetId &targetId,
  const boost::asio::ip::udp::endpoint &remote,
  const std::string_view filename,
  const Tftp::Packets::TftpOptions &clientTftpOptions,
  const std::string_view loadPartNumber,
  const Arinc649::CheckValue &checkValue )
{
  SPDLOG_INFO(
    "{}: Request file '{}' Load Part Number '{}' Check Value '{}'",
    targetId.toString(),
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

  auto realFilename{ mediaSetManagerV->filePath( file ) };

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
    ->completionHandler( std::bind_front( &BatchUploadOperationCommand::fileCompleted, this, fileOperation ) );

  fileOperationsV.emplace_front( std::move( fileOperation ) )->start();
}

void BatchUploadOperationCommand::fileCompleted(
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
