// $Id: ROShifter.cpp,v 1.2 2010-11-02 13:52:04 ocallot Exp $
// Include files

// local
#include "ROShifter.h"

// STL
#include <iostream>

// Boost
#include <boost/asio.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/find.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/regex.hpp>

//-----------------------------------------------------------------------------
// Implementation file for class : ROShifter
//
// 2010-03-01 : Patrick Robbe
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ROShifter::ROShifter(  ) : m_open( false ) { 
  m_shifters.clear() ;
  
  boost::posix_time::ptime now = boost::posix_time::second_clock::local_time() ;
  m_lastDay  = now.date();
  m_lastSlot = -1;
}

//=============================================================================
// Destructor
//=============================================================================
ROShifter::~ROShifter() { }

//=============================================================================
// Get content of web page
//=============================================================================
void ROShifter::readWebFile( ) {
  m_shifters.clear() ;

  boost::asio::ip::tcp::iostream 
    web_stream( "lbshiftdb.cern.ch" , "http" ) ;

  if ( ! web_stream ) {
    std::cerr << "Cannot open shiftdb script via the web" << std::endl ;
    m_open = false ;
    return ;
  }

  // Send HTTP request to web server
  web_stream << "GET /shiftdb_report.php?format=twiki&"
	     << "sDate=" << boost::gregorian::to_iso_string( m_lastDay ) 
	     << " HTTP/1.0\r\n" 
	     << "Host:lbshiftdb.cern.ch\r\n" 
	     << "\r\n" << std::flush ;

  std::string line ;

  // Check that the web server answers correctly
  std::getline( web_stream , line ) ;
  if ( ! boost::algorithm::find_first( line , "200 OK" ) ) {
    std::cerr << "Shiftdb server does not reply" << std::endl ;
    m_open = false ;
    return ;
  }

  // Parse the answer of the web server with | as separator
  boost::char_separator< char > sep( "|" , "" , boost::keep_empty_tokens ) ;
  boost::tokenizer< boost::char_separator< char > > tok( line , sep ) ;
  boost::tokenizer< boost::char_separator< char > >::iterator it ;

  std::string pattern ;
  pattern = "\\N{vertical-line}(.*)" ;
  pattern += "\\N{vertical-line}(.*)" ;
  pattern += "\\N{vertical-line}(.*)" ;
  pattern += "\\N{vertical-line}(.*)" ; 
  pattern += "\\N{vertical-line}(.*)" ;
  pattern += "\\N{vertical-line}(.*)" ;
  pattern += "\\N{vertical-line}" ;

  const boost::regex e( pattern ) ;

  while( std::getline( web_stream , line ) ) {
    // Removes spaces at the beginning of the line
    boost::algorithm::trim_left_if( line , boost::algorithm::is_space() ) ;
    // Use only lines with correct format
    if ( ! boost::regex_match( line , e ) ) continue ;
    // Separate fields
    tok.assign( line ) ;
    it = tok.begin() ; ++it ; ++it ;
    std::string function = (*it) ; ++it ;
    // Remove spaces before and after
    boost::algorithm::trim_left_if( function , boost::algorithm::is_space() ) ;
    boost::algorithm::trim_right_if( function , 
				     boost::algorithm::is_space() ) ;
    // Get Names
    std::vector< std::string > vec ;
    vec.push_back( *(++it ) ) ;
    vec.push_back( *(++it ) ) ;
    vec.push_back( *(++it ) ) ;
    
    m_shifters[ function ] = vec ;
  }

  m_open = true ;
}

//=============================================================================
// Get name of the person on shift for a given function and a given slot 
//=============================================================================
std::string ROShifter::getShifter( const std::string & function  ) { 
  if ( ! m_open ) return function ;

  if ( m_shifters.find( function ) == m_shifters.end() ) return function ;

  return m_shifters[ function ][ m_lastSlot ] ;  
}

//=============================================================================
// Name of data manager on shift
//=============================================================================
std::string ROShifter::getDataManager( ) {

  return getShifter( "Data Manager" ) ;
}

//=============================================================================
// Name of shift leader on shift
//=============================================================================
std::string ROShifter::getShiftLeader( ) {

  return getShifter( "Shift Leader" ) ;
}

//=============================================================================
// Name of production manager on shift
//=============================================================================
std::string ROShifter::getProduction( ) {

  return getShifter( "Production" ) ;
}


//=============================================================================
// Name of production manager on shift
//=============================================================================
std::string ROShifter::getSLIMOS( ) {

  return getShifter( "SLIMOS Shutdown" ) ;
}

//=========================================================================
//  Has the time slot changed for shift ?
//=========================================================================
bool ROShifter::hasChanged ( ) {

  boost::posix_time::ptime now = boost::posix_time::second_clock::local_time() ;
  boost::gregorian::date nowDay = now.date() ;

  int slot = 0 ;

  int hour = now.time_of_day().hours();
  if ( now.time_of_day().minutes() >= 30 ) hour++;  // shift start at 6:30, 14:30, 22:30
  
  if ( hour < 7 ) {
    nowDay = now.date() - boost::gregorian::date_duration( 1 ) ;
    slot = 2 ;
  } else if ( hour < 15 ) slot = 0 ;
  else if ( hour < 23 ) slot = 1 ;
  else slot = 2 ;

  //== Special if no shift leader...
  if ( getShiftLeader() == "" ) {
    slot = 1;
    if ( hour < 14 ) slot = 0;
    if ( now.time_of_day().hours() < 8 ||
         now.time_of_day().hours() > 19 ) slot = 2;  // no crew...
  }
  

  if ( nowDay == m_lastDay && slot == m_lastSlot ) return false;
  
  m_lastDay = nowDay;
  m_lastSlot = slot;    
  
  readWebFile( ) ;
  return true;
}
