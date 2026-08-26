// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615aCommands::Operations::OperatorDefinedDownloadOperationCommand.
 **/

#include "OperatorDefinedDownloadOperationCommand.hpp"

#include <arinc_615a/find/TargetInformation.hpp>

#include <arinc_615a/host/OperationConfiguration.hpp>
#include <arinc_615a/host/OperatorDefinedDownloadOperation.hpp>
#include <arinc_615a/host/Protocol.hpp>

#include <arinc_615a/information/DownloadStatus.hpp>
#include <arinc_615a/information/DownloadFileInformation.hpp>
#include <arinc_615a/information/InitializationResponse.hpp>

#include <arinc_615a/tftp/servers/WriteOperation.hpp>

#include <arinc_615a/tftp/Arinc615aOptions.hpp>

#include <arinc_615a/Arinc615aException.hpp>
#include <arinc_615a/StatusCode.hpp>
#include <arinc_615a/StatusCodeDescription.hpp>

#include <arinc_649/CheckValueGenerator.hpp>

#include <tftp/packets/TftpOptions.hpp>
#include <tftp/packets/PacketStatistic.hpp>

#include <tftp/files/StreamFile.hpp>

#include <helper/BoostAsioProgramOptions.hpp>
#include <helper/Exception.hpp>
#include <helper/Filenames.hpp>
#include <helper/SeverityLevelDescription.hpp>

#include <spdlog/spdlog.h>

#include <boost/exception/all.hpp>

#include <boost/program_options.hpp>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include <format>
#include <iostream>

namespace Arinc615aCommands::Operations {

OperatorDefinedDownloadOperationCommand::OperatorDefinedDownloadOperationCommand(
  boost::asio::io_context &ioContext,
  AbortTerminateSignal &abortSignal,
  AbortTerminateSignal &terminateSignal ) :
  ioContextV{ ioContext },
  abortSignalV{ abortSignal },
  terminateSignalV{ terminateSignal },
  optionsDescriptionV{ "ARINC 615A Operator Defined Download Operation Options" }
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
        Arinc649::setLogLevel( logLevel );
        Helper::setLogLevel( logLevel );
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
    "download-all",
    boost::program_options::bool_switch( &downloadAllV ),
    "If set, all advertised files will automatically be downloaded."
  )
  (
    "file,f",
    boost::program_options::value( &requestedFilesV )
      ->composing()
      ->value_name( "file" ),
    "Requested files (if empty, operation is cancelled after list transfer."
  )
  (
    "download-base-directory",
    boost::program_options::value( &downloadBaseDirectoryV )
      ->default_value( std::filesystem::current_path() )
      ->value_name( "download-dir" ),
    "Base directory, where the received files shall be written to.\n"
    "A sub-directory is created within to contain all download files."
  )
  (
    "no-check-validity,c",
    boost::program_options::bool_switch( &noVerifyDownloadFilesV ),
    "Disables the validity check of the downloaded files."
  )
  (
    "no-download-directory,n",
    boost::program_options::bool_switch( &noDownloadDirectoryV ),
    "Don't create specific directory for downloaded files.\n"
    "download-base-directory is used as target download directory."
  );
}

void OperatorDefinedDownloadOperationCommand::execute( const Commands::Parameters &parameters )
{
  try
  {
    std::cout << "ARINC 615A Operator Defined Download Operation\n";

    boost::program_options::variables_map variablesMap;
    boost::program_options::store(
      boost::program_options::command_line_parser( parameters ).options( optionsDescriptionV ).run(),
      variablesMap );
    boost::program_options::notify( variablesMap );

    if ( !targetsListPathV.empty() )
    {
      boost::property_tree::ptree targetsListPTree{};
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
        boost::program_options::error{ "Target IP address invalid, not provided, or not in target list." } );
    }

    const Arinc615a::TargetId targetId{ targetIdV };

    downloadInformationV.start( targetId, targetAddressV );

    downloadDataPathV =
      noDownloadDirectoryV ? downloadBaseDirectoryV : downloadInformationV.downloadDataPath( downloadBaseDirectoryV );

    std::error_code errorCode;
    std::filesystem::create_directories( downloadDataPathV, errorCode );
    if ( errorCode )
    {
      BOOST_THROW_EXCEPTION(
        Arinc615a::Arinc615aException{}
          << Helper::AdditionalInfo{ errorCode.message() }
          << boost::errinfo_file_name{ downloadDataPathV.string() } );
    }

    const auto protocol{ Arinc615a::Host::Protocol::instance( ioContextV ) };
    assert( protocol );

    operationV = protocol->operatorDefinedDownloadOperation(
      Arinc615a::Host::OperatorDefinedDownloadOperationConfiguration{
        .dataLoaderConfiguration = configurationV,
        .handler = *this,
        .targetAddress = targetAddressV,
        .targetId = targetId,
        .dlpTimeout = dlpTimeoutV,
        .portOption = portOptionV
      } );
    assert( operationV );

    boost::signals2::scoped_connection abortConnection{ abortSignalV.connect(
      std::bind_front(
        &Arinc615a::Host::OperatorDefinedDownloadOperation::abort,
        operationV,
        Arinc615a::Host::OperatorDefinedDownloadOperation::AbortReason::Operator ) ) };
    boost::signals2::scoped_connection terminateConnection{ terminateSignalV.connect(
      std::bind_front(
        &Arinc615a::Host::OperatorDefinedDownloadOperation::terminate,
        operationV,
        Arinc615a::Host::OperatorDefinedDownloadOperation::AbortReason::Operator ) ) };

    // execute operation
    operationV->start();

    // Wait for completion
    done.wait();

    // Print TFTP Statistic
    std::cout
      << "TFTP RX:\n" << Tftp::Packets::PacketStatistic::globalReceive() << "\n"
      << "TFTP TX:\n" << Tftp::Packets::PacketStatistic::globalTransmit() << "\n";

    downloadInformationV.save(
      downloadInformationV.downloadInformationPath( downloadBaseDirectoryV ),
      !noVerifyDownloadFilesV );
  }
  catch ( const boost::program_options::error & )
  {
    // Command line parsing errors are handled within command handler
    throw;
  }
  catch ( const boost::exception &e )
  {
    std::cerr << std::format( "Operator Defined Download Operation failed: {}\n", boost::diagnostic_information( e ) );
  }
  catch ( const std::exception &e )
  {
    std::cerr << std::format( "Operator Defined Download Operation failed: \n", e.what() );
  }
  catch ( ... )
  {
    std::cerr << "Operator Defined Download Operation failed: UNKNOWN EXCEPTION\n";
  }
}

void OperatorDefinedDownloadOperationCommand::help() const
{
  std::cout << "Operator Defined Download Operation\n";
  std::cout << optionsDescriptionV;
}

void OperatorDefinedDownloadOperationCommand::initialisationDeferred( const std::chrono::seconds waitTime )
{
  std::cout << std::format( "Operation deferred for: {}\n", waitTime );
}

void OperatorDefinedDownloadOperationCommand::initialisationResponse(
  const Arinc615a::Information::InitializationResponse &response )
{
  std::cout << std::format(
    "Initialisation Response:\n"
    "\tInitialisation Code: {}\n"
    "\tStatus Description:  '{}'\n",
    response.code(),
    response.description() );
}

void OperatorDefinedDownloadOperationCommand::finished( const Arinc615a::StatusCode code, std::string_view description )
{
  std::cout << std::format(
    "Operation finished:\n"
    "\tFinal Status Code:        {}\n"
    "\tFinal Status Description: '{}'\n",
    code,
    description );

  downloadInformationV.finished( Arinc615a::status( Arinc615a::OperationClass::Download, code, description ) );

  done.count_down();
}

void OperatorDefinedDownloadOperationCommand::status( const Arinc615a::Information::DownloadStatus &status )
{
  assert( operationV );

  std::cout << std::format(
    "Download Status:\n"
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

  for ( const auto &fileInfo : status.files() )
  {
    std::cout << std::format(
      "\tFile Info:\n"
      "\t\tFilename:           '{}'\n"
      "\t\tStatus Code:        {}\n"
      "\t\tStatus Description: '{}'\n",
      fileInfo.filename(),
      fileInfo.code(),
      fileInfo.description() );
  }

  std::cout << std::flush;
}

void OperatorDefinedDownloadOperationCommand::fileRequest(
  const boost::asio::ip::udp::endpoint &remote,
  std::string_view filename,
  const Tftp::Packets::TftpOptions &clientTftpOptions,
  std::string_view partNumber,
  const Arinc649::CheckValue &checkValue )
{
  const std::filesystem::path filePath{ downloadDataPathV / Helper::normaliseFilename( filename ) };

  SPDLOG_INFO(
    "Request file '{}' Part Number '{}' Check Value '{}' to {}",
    filename,
    partNumber,
    checkValue.format(),
    filePath.string() );

  downloadInformationV.fileStart( filename, filePath, std::string{ partNumber }, checkValue );

  const auto streamFile{
    std::make_shared< Tftp::Files::StreamFile >( Tftp::Files::StreamFile::Operation::Receive, filePath ) };
  assert( streamFile );

  auto fileOperation{
    operationV->fileTransfer( streamFile, remote, clientTftpOptions, std::string{ partNumber }, checkValue ) };
  assert( fileOperation );

  fileOperation->completionHandler(
    std::bind_front(
      &OperatorDefinedDownloadOperationCommand::fileCompleted,
      this,
      std::string{ filename },
      filePath,
      clientTftpOptions.transferSize,
      fileOperation ) );

  fileOperationsV.emplace_front( std::move( fileOperation ) )->start();
}

void OperatorDefinedDownloadOperationCommand::downloadingList( const Arinc615a::Information::DownloadFilesInformation &files )
{
  std::cout << "Received File List:\n";

  // iterate over files
  for ( const auto &file : files )
  {
    std::cout
      << std::format(
        "\tFilename:    {}\n"
        "\tDescription: {}\n",
        file.filename,
        file.description );
  }

  // add all files, if requested by user
  if ( downloadAllV )
  {
    requestedFilesV.clear();
    for ( const auto &file : files )
    {
      requestedFilesV.emplace_back( file.filename );
    }
  }

  // if no files submitted -> abort
  if ( requestedFilesV.empty() )
  {
    operationV->abort(
      Arinc615a::Host::OperatorDefinedDownloadOperation::AbortReason::Operator );
  }
  else
  {
    // send answer
    Arinc615a::Information::DownloadFiles downloadFiles{};

    for ( auto &file : requestedFilesV )
    {
      downloadFiles.push_back( std::move( file ) );
    }

    downloadInformationV.files( downloadFiles );

    operationV->answer( std::move( downloadFiles ) );
  }
}

void OperatorDefinedDownloadOperationCommand::fileCompleted(
  std::string_view filename,
  const std::filesystem::path &filePath,
  const std::optional< const std::size_t > &expectedFileSize,
  const Arinc615a::Tftp::Servers::WriteOperationPtr &operation,
  const Tftp::TransferStatus status )
{
  if ( 1U != fileOperationsV.remove( operation ) )
  {
    SPDLOG_ERROR( "File operation completed, which was not created" );
    operationV->abort( Arinc615a::Host::Operation::AbortReason::Protocol );
  }

  if ( Tftp::TransferStatus::Successful != status )
  {
    SPDLOG_WARN( "File transfer failed - Ignore it from host side" );

    downloadInformationV.fileFinished( filename, Arinc615a::Host::DownloadInformation::TransferError, 0U );

    return;
  }

  const auto fileSize{ std::filesystem::file_size( filePath ) };

  if ( expectedFileSize && ( fileSize != expectedFileSize.value() ) )
  {
    SPDLOG_WARN( "File size differs to expected one" );
  }

  downloadInformationV.fileFinished( filename, Arinc615a::Host::DownloadInformation::TransferOk, fileSize );
}

}
