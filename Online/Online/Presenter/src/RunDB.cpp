// $Id: RunDB.cpp,v 1.4 2010-10-18 07:37:58 marcocle Exp $
// local
#include "RunDB.h"

// STL
#include <iostream>

// Boost
#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/regex.hpp>

//-----------------------------------------------------------------------------
// Implementation file for class : RunDB
//
// 2010-05-10 : Patrick Robbe
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RunDB::RunDB( std::string address, PresenterInformation* pInfo ) :
  m_currentRunNumber( 0 ) ,
  m_currentRunStartTime( "" ) ,
  m_currentRunEndTime( "" ) ,
  m_currentRunDuration( "" ) ,
  m_partition( "LHCb" ) ,
  m_destination( "OFFLINE" )
{
  m_address = address;
  m_presInfo = pInfo;
}

//=============================================================================
// Destructor
//=============================================================================
RunDB::~RunDB() {}

//=============================================================================
// Get last run
//=============================================================================
int RunDB::getLastRun( ) {

  int lastRun = 0 ;

  std::cout << "getLastRun in run database: Open webstream at " << m_address << std::endl;

  boost::asio::ip::tcp::iostream webStream( m_address , "http" ) ;

  if ( ! webStream ) {
    std::cerr << "Cannot open the RunDb Database at " << m_address
              << std::endl ;
    return lastRun ;
  }

  // Send HTTP request to web server
  std::string request =  "GET /api/search/?rows=1" ;
  if ( getPartition() != "" ) request = request + "&partition=" + getPartition() ;
  if ( getDestination() != "" ) request = request + "&destination=" + getDestination() ;
  request = request + " HTTP/1.0\r\nHost:" + m_address + "\r\n\r\n";

  std::cout << "Send '" << request << "'" << std::endl;

  webStream << request << std::flush ;

  std::string line ;

  // Check that the web server answers correctly
  std::getline( webStream , line ) ;
  if ( ! boost::algorithm::find_first( line , "200 OK" ) ) {
    std::cerr << "RunDB server does not answer OK :" << line << std::endl;
    return lastRun ;
  }

  while ( std::getline( webStream , line ) ) {
    lastRun = runFromWebLine( line );
    if ( 0 < lastRun  ) return lastRun;
  }
  //== Protect: If this run has a zero duration, take the previous one
  if ( "00:00:00" == m_currentRunDuration  ) {
    std::cout << "Run " << lastRun << " has 00:00:00 duration. Take previous one" << std::endl;
    lastRun = getPreviousRun();
  }

  return lastRun ;
}

//=============================================================================
// Get next run
//=============================================================================
int RunDB::getNextRun( ) {

  int nextRun = 0 ;
  int currRun = m_currentRunNumber;

  std::cout << "getNextRun in run database: Open webstream at " << m_address << std::endl;

  boost::asio::ip::tcp::iostream webStream( m_address , "http" ) ;

  if ( ! webStream ) {
    std::cerr << "Cannot open the RunDb Database at " << m_address
              << std::endl ;
    return nextRun ;
  }

  // Send HTTP request to web server
  webStream << "GET /api/search/?rows=1" ;
  if ( getPartition() != "" ) webStream << "&partition=" << getPartition() ;
  if ( getDestination() != "" ) webStream << "&destination=" << getDestination() ;

  webStream << "&starttime="
            << boost::posix_time::to_iso_extended_string( boost::posix_time::time_from_string( m_currentRunEndTime ) ) ;
  webStream << " HTTP/1.0\r\n"
            << "Host:" << m_address << "\r\n"
            << "\r\n" << std::flush ;

  std::string line ;

  // Check that the web server answers correctly
  std::getline( webStream , line ) ;
  if ( ! boost::algorithm::find_first( line , "200 OK" ) ) {
    std::cerr << "RunDB server does not answer OK :'" << line << "'" << std::endl;
    return nextRun ;
  }

  // Check first the number of candidate run:
  while ( std::getline( webStream , line ) ) {
    currRun = runFromWebLine( line );
    if ( 0 < nextRun  ) break;
  }
  webStream.close() ;

  return nextRun ;
}

//=============================================================================
// Get next run
//=============================================================================
int RunDB::getPreviousRun( ) {

  int previousRun = 0 ;

  std::cout << "getPreviousRun in run database: Open webstream at " << m_address << std::endl;

  boost::asio::ip::tcp::iostream webStream( m_address , "http" ) ;

  if ( ! webStream ) {
    std::cerr << "Cannot open the RunDb Database at " << m_address
              << std::endl ;
    return previousRun ;
  }

  // Send HTTP request to web server
  webStream << "GET /api/search/?rows=2&start=1" ;
  if ( getPartition() != "" ) webStream << "&partition=" << getPartition() ;
  if ( getDestination() != "" ) webStream << "&destination=" << getDestination() ;
  webStream << "&endtime="
            << boost::posix_time::to_iso_extended_string( boost::posix_time::time_from_string( m_currentRunStartTime ) ) ;
  webStream << " HTTP/1.0\r\n"
            << "Host:" << m_address << "\r\n"
            << "\r\n" << std::flush ;

  std::string line ;

  // Check that the web server answers correctly
  std::getline( webStream , line ) ;
  if ( ! boost::algorithm::find_first( line , "200 OK" ) ) {
    std::cerr << "RunDB server does not answer OK :" << line << std::endl;
    return previousRun ;
  }

  // Check first the number of candidate run:
  while ( std::getline( webStream , line ) ) {
    previousRun = runFromWebLine( line );
    if ( 0 < previousRun  ) return previousRun;
  }
  webStream.close() ;

  return previousRun ;
}

//=============================================================================
// Get a list of runs from the run db
// starting for a given run number with +/-10 runs
//=============================================================================
std::vector< int > RunDB::getListOfRuns( int /*runNumber*/ ) {
  return std::vector< int >() ;
}

//=============================================================================
// Check run
//=============================================================================
bool RunDB::checkRun( int runNumber ) {
  if ( runNumber == getCurrentRunNumber() ) return true ;

  std::cout << "checkRun in run database run " << runNumber << " : Open webstream at " << m_address << std::endl;

  boost::asio::ip::tcp::iostream webStream( m_address , "http" ) ;

  if ( ! webStream ) {
    std::cerr << "Cannot open the RunDb Database at " << m_address
              << std::endl ;
    return false ;
  }

  // Send HTTP request to web server
  webStream << "GET /api/run/" << runNumber << "/" ;
  webStream << " HTTP/1.0\r\n"
            << "Host:" << m_address << "\r\n"
            << "\r\n" << std::flush ;

  std::string line ;

  // Check that the web server answers correctly
  std::getline( webStream , line ) ;
  if ( ! boost::algorithm::find_first( line , "200 OK" ) ) {
    std::cerr << "RunDB server does not answer OK :" << line << std::endl;
    return false;
  }

  // Parse the web server answers
  std::string pattern ;
  pattern = "\\N{left-curly-bracket}(.*)" ;
  pattern += "\\N{right-curly-bracket}" ;

  const boost::regex e( pattern ) ;

  boost::property_tree::ptree run_tree ;

  // Check destination and partition match
  while ( std::getline( webStream , line ) ) {
    int run = runFromWebLine( line, true );
    if ( 0 < run ) return true;
  }

  return false ;
}

//============================================================================
// Convert to usable time format
//============================================================================
std::string RunDB::convertRunTimeToString( boost::posix_time::ptime * st )
  const {
  boost::posix_time::time_facet * outputFacet = new boost::posix_time::time_facet();
  outputFacet->format("%Y-%m-%d %H:%M:%S");
  std::stringstream ss;
  ss.imbue(std::locale(std::locale::classic(), outputFacet));

  ss << (*st) ;

  return ss.str();
}

//=========================================================================
//  Parse the web server answer, extract the run parameters. Return 0 if not found
//=========================================================================
int RunDB::runFromWebLine ( std::string line, bool checkDestAndPart ) {

  std::string pattern( "\\N{left-curly-bracket}(.*)\\N{right-curly-bracket}" ) ;

  const boost::regex e( pattern ) ;

  boost::property_tree::ptree run_tree ;

  // Check that the answer has the correct format (JSON format)
  if ( boost::regex_match( line , e ) ) {
    if ( checkDestAndPart ) {
      line = "{ \"runs\" : ["+line+"]}";
    }
    // Now parse each line (there should be actually only one line
    // in the server answer, otherwise it is over-written
    std::istringstream is( line ) ;
    std::cout << "RVC: " << line << std::endl;
    try {
      boost::property_tree::json_parser::read_json( is , run_tree ) ;
      
      BOOST_FOREACH( boost::property_tree::ptree::value_type &v, run_tree.get_child( "runs" ) ) {
        int runNb = v.second.get< int >( "runid" ) ;
        m_currentRunNumber = runNb ;
        std::string dest   = v.second.get< std::string >( "destination" ) ;
        std::string part   = v.second.get< std::string >( "partitionname" ) ;
        if ( checkDestAndPart ) {
          if ( ( "" != m_destination && dest != m_destination ) || ( part != m_partition ) ) {
            std::cout << "m_destination '" << m_destination << "' dest='" << dest 
                      << "' part='" << part << "'  -> ignore." << std::endl;
            continue;
          }
        }
        m_destination = dest;
        m_partition   = part;
        std::string runType      = v.second.get< std::string >( "runtype",      "Collision11" );
        std::string veloPosition = v.second.get< std::string >( "veloPosition", "Closed" ) ;
        std::string magnetState  = v.second.get< std::string >( "magnetState",  "Down" ) ;
        double ene         = v.second.get< float >( "beamenergy",  3500. ) ;
        std::string start  = v.second.get< std::string >( "starttime" ) ;
        std::string end    = v.second.get< std::string >( "endtime" ) ;
        boost::algorithm::erase_all( start , "-" ) ;
        boost::algorithm::erase_all( start , ":" ) ;
        boost::algorithm::erase_all( end , "-" ) ;
        boost::algorithm::erase_all( end , ":" ) ;
        boost::posix_time::ptime st = boost::posix_time::from_iso_string( start ) ;
        m_currentRunStartTime = convertRunTimeToString( &st ) ;
        boost::posix_time::ptime et = boost::posix_time::from_iso_string( end ) ;
        m_currentRunEndTime = convertRunTimeToString( &et ) ;        
        boost::posix_time::time_duration dur = et - st ;
        m_currentRunDuration = boost::posix_time::to_simple_string( dur ) ;
        if ( NULL != m_presInfo ) {
          m_presInfo->setRunType(      capitalize( runType ) );
          m_presInfo->setVeloPosition( capitalize( veloPosition ) );
          m_presInfo->setMagnetState(  capitalize( magnetState ) );
          char tmp[40];
          sprintf( tmp, "%d", int(ene + 0.5) );
          m_presInfo->setBeamEnergy( std::string( tmp ) );
          int tck =  v.second.get< int >( "tck" ) ;
          sprintf( tmp, "TCK_0x%8.8X", tck );
          m_presInfo->setTCK( std::string( tmp ) );
        }
        return runNb ;
      }
    }
    catch (...) {
      std::cerr << "Error in the problem db server response" << std::endl ;
      return 0 ;
    }
  }
  return 0 ;
}
