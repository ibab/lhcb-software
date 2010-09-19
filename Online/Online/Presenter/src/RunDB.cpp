// $Id: RunDB.cpp,v 1.3 2010-09-19 18:49:53 robbep Exp $
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

//-----------------------------------------------------------------------------
// Implementation file for class : RunDB
//
// 2010-05-10 : Patrick Robbe
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RunDB::RunDB( std::string address ) : 
  m_currentRunNumber( 0 ) , 
  m_currentRunStartTime( "" ) , 
  m_currentRunEndTime( "" ) , 
  m_currentRunDuration( "" ) ,
  m_partition( "LHCb" ) ,
  m_destination( "OFFLINE" ) 
{
  m_address = address;
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

  boost::asio::ip::tcp::iostream webStream( m_address , "http" ) ;

  if ( ! webStream ) {
    std::cerr << "Cannot open the RunDb Database at " << m_address  
	      << std::endl ;
    return lastRun ;
  }

  // Send HTTP request to web server
  webStream << "GET /api/search/?rows=1" ;
  if ( getPartition() != "" ) webStream << "&partition=" << getPartition() ;
  if ( getDestination() != "" ) webStream << "&destination=" << getDestination() ;
  webStream << " HTTP/1.0\r\n"
	    << "Host:" << m_address << "\r\n"
	    << "\r\n" << std::flush ;

  std::string line ;

  // Check that the web server answers correctly
  std::getline( webStream , line ) ;
  if ( ! boost::algorithm::find_first( line , "200 OK" ) ) {
    std::cerr << "RunDB server does not reply" << std::endl ;
    return lastRun ;
  }
  
  // Parse the web server answers
  std::string pattern ;
  pattern = "\\N{left-curly-bracket}(.*)" ;
  pattern += "\\N{right-curly-bracket}" ;
  
  const boost::regex e( pattern ) ;

  boost::property_tree::ptree run_tree ;

  while ( std::getline( webStream , line ) ) {
    // Check that the answer has the correct format (JSON format)
    if ( boost::regex_match( line , e ) ) {
      // Now parse each line (there should be actually only one line
      // in the server answer, otherwise it is over-written
      std::istringstream is( line ) ;

      try {
	boost::property_tree::json_parser::read_json( is , run_tree ) ;
	
	BOOST_FOREACH( boost::property_tree::ptree::value_type &v,
		       run_tree.get_child( "runs" ) ) {
	  lastRun = v.second.get< int >( "runid" ) ;
	  m_currentRunNumber = lastRun ;
	  std::string start = v.second.get< std::string >( "starttime" ) ;
	  std::string end   = v.second.get< std::string >( "endtime" ) ;
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
	  return lastRun ;
	}
      } 
      catch (...) {
	std::cerr << "Error in the problem db server response" << std::endl ;
	return lastRun ;
      }
    }
  }

  return lastRun ;
}

//=============================================================================
// Get next run
//=============================================================================
int RunDB::getNextRun( ) {

  int nextRun = 0 ;

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
    std::cerr << "RunDB server does not reply" << std::endl ;
    return nextRun ;
  }
  
  // Parse the web server answers
  std::string pattern ;
  pattern = "\\N{left-curly-bracket}(.*)" ;
  pattern += "\\N{right-curly-bracket}" ;
  
  const boost::regex e( pattern ) ;

  boost::property_tree::ptree run_tree ;

  int total = 0 ;
  // Check first the number of candidate run:
  while ( std::getline( webStream , line ) ) {
    // Check that the answer has the correct format (JSON format)
    if ( boost::regex_match( line , e ) ) {
      // Now parse each line (there should be actually only one line
      // in the server answer, otherwise it is over-written
      std::istringstream is( line ) ;

      try {
	boost::property_tree::json_parser::read_json( is , run_tree ) ;
	
	total = run_tree.get< int >( "totalResults" ) ;

	if ( 0 >= total ) 
	  // No next run has been found
	  return nextRun ;
      } 
      catch (...) {
	std::cerr << "Error in the problem db server response" << std::endl ;
	return nextRun ;
      }
    }
  }
  webStream.close() ;

  boost::asio::ip::tcp::iostream webStream2( m_address , "http" ) ;

  if ( ! webStream2 ) {
    std::cerr << "Cannot open the RunDb Database at " << m_address  
	      << std::endl ;
    return nextRun ;
  }  

  // Call again web server to get only next run
  // Send HTTP request to web server
  if ( total > 1 ) total = total - 1 ; 
  webStream2 << "GET /api/search/?rows=" << total << "&start=" << (total-1) ;
  if ( getPartition() != "" ) webStream2 << "&partition=" << getPartition() ;
  if ( getDestination() != "" ) webStream2 << "&destination=" << getDestination() ;
  webStream2 << "&starttime=" 
	     << boost::posix_time::to_iso_extended_string( boost::posix_time::time_from_string( m_currentRunEndTime ) ) ; 
  webStream2 << " HTTP/1.0\r\n"
	     << "Host:" << m_address << "\r\n"
	     << "\r\n" << std::flush ;
  
  // Check that the web server answers correctly
  std::getline( webStream2 , line ) ;
  if ( ! boost::algorithm::find_first( line , "200 OK" ) ) {
    std::cerr << "RunDB server does not reply" << std::endl ;
    return nextRun ;
  }

  while ( std::getline( webStream2 , line ) ) {
    // Check that the answer has the correct format (JSON format)
    if ( boost::regex_match( line , e ) ) {
      // Now parse each line (there should be actually only one line
      // in the server answer, otherwise it is over-written
      std::istringstream is( line ) ;

      try {
	boost::property_tree::json_parser::read_json( is , run_tree ) ;
	
	BOOST_FOREACH( boost::property_tree::ptree::value_type &v,
		       run_tree.get_child( "runs" ) ) {
	  nextRun = v.second.get< int >( "runid" ) ;
	  if ( nextRun == m_currentRunNumber ) return 0 ;
	  m_currentRunNumber = nextRun ;
	  std::string start = v.second.get< std::string >( "starttime" ) ;
	  std::string end   = v.second.get< std::string >( "endtime" ) ;
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
	  return nextRun ;
	}
      } 
      catch (...) {
	std::cerr << "Error in the problem db server response" << std::endl ;
	return nextRun ;
      }
    }
  }
  
  return nextRun ;
}

//=============================================================================
// Get next run
//=============================================================================
int RunDB::getPreviousRun( ) {

  int previousRun = 0 ;

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
    std::cerr << "RunDB server does not reply" << std::endl ;
    return previousRun ;
  }
  
  // Parse the web server answers
  std::string pattern ;
  pattern = "\\N{left-curly-bracket}(.*)" ;
  pattern += "\\N{right-curly-bracket}" ;
  
  const boost::regex e( pattern ) ;

  boost::property_tree::ptree run_tree ;

  // Check first the number of candidate run:
  while ( std::getline( webStream , line ) ) {
    // Check that the answer has the correct format (JSON format)
    if ( boost::regex_match( line , e ) ) {
      // Now parse each line (there should be actually only one line
      // in the server answer, otherwise it is over-written
      std::istringstream is( line ) ;

      try {
	boost::property_tree::json_parser::read_json( is , run_tree ) ;

	BOOST_FOREACH( boost::property_tree::ptree::value_type &v,
		       run_tree.get_child( "runs" ) ) {
	  previousRun = v.second.get< int >( "runid" ) ;
	  if ( previousRun == m_currentRunNumber ) return 0 ;
	  m_currentRunNumber = previousRun ;
	  std::string start = v.second.get< std::string >( "starttime" ) ;
	  std::string end   = v.second.get< std::string >( "endtime" ) ;
	  boost::algorithm::erase_all( start , "-" ) ;
	  boost::algorithm::erase_all( start , ":" ) ;
	  boost::algorithm::erase_all( end , "-" ) ;
	  boost::algorithm::erase_all( end , ":" ) ;
	  boost::posix_time::ptime st = boost::posix_time::from_iso_string( start ) ;
	  m_currentRunStartTime = convertRunTimeToString( &st ) ;
	  boost::posix_time::ptime et = boost::posix_time::from_iso_string( end ) ;
	  m_currentRunEndTime   = convertRunTimeToString( &et ) ;

	  boost::posix_time::time_duration dur = et - st ;
	  m_currentRunDuration = boost::posix_time::to_simple_string( dur ) ;
	  return previousRun ;
	}
      } 
      catch (...) {
	std::cerr << "Error in the problem db server response" << std::endl ;
	return previousRun ;
      }
    }
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
  if ( ! boost::algorithm::find_first( line , "200 OK" ) ) return false ;
  
  // Parse the web server answers
  std::string pattern ;
  pattern = "\\N{left-curly-bracket}(.*)" ;
  pattern += "\\N{right-curly-bracket}" ;
  
  const boost::regex e( pattern ) ;

  boost::property_tree::ptree run_tree ;

  // Check destination and partition match
  while ( std::getline( webStream , line ) ) {
    // Check that the answer has the correct format (JSON format)
    if ( boost::regex_match( line , e ) ) {
      // Now parse each line (there should be actually only one line
      // in the server answer, otherwise it is over-written
      std::istringstream is( line ) ;
      
      try {
	boost::property_tree::json_parser::read_json( is , run_tree ) ;
	
	std::string dest = run_tree.get< std::string >( "destination" ) ;
	std::string part = run_tree.get< std::string >( "partitionname" ) ;

	std::string start = run_tree.get< std::string >( "starttime" ) ;
	std::string end   = run_tree.get< std::string >( "endtime" ) ;
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

	if ( ( dest == getDestination() ) && ( part == getPartition() ) ) {
	  m_currentRunNumber = runNumber ;
	  return true ;
	}
      } 
      catch (...) {
	std::cerr << "Error in the problem db server response" << std::endl ;
	return false ;
      }
    }
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
