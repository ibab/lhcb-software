// $Id: ShiftDB.cpp,v 1.2 2010-03-18 20:17:47 robbep Exp $
// Include files

// local
#include "ShiftDB.h"

// STL
#include <iostream>

// Boost
#include "boost/date_time/posix_time/posix_time_types.hpp"
#include "boost/date_time/gregorian/formatters.hpp"
#include <boost/asio.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/find.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/classification.hpp>

//-----------------------------------------------------------------------------
// Implementation file for class : ShiftDB
//
// 2010-03-01 : Patrick Robbe
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ShiftDB::ShiftDB(  ) : m_open( false ) {
  m_shifters.clear() ;
}

//=============================================================================
// Destructor
//=============================================================================
ShiftDB::~ShiftDB() { }

//=============================================================================
// Get content of web page
//=============================================================================
void ShiftDB::readWebFile( const boost::gregorian::date * day ) {
  m_shifters.clear() ;

  boost::asio::ip::tcp::iostream 
    web_stream( "shiftdb.lbdaq.cern.ch" , "http" ) ;

  if ( ! web_stream ) {
    std::cerr << "Cannot open shiftdb script via the web" << std::endl ;
    m_open = false ;
    return ;
  }

  // Send HTTP request to web server
  web_stream << "GET /shiftdb_report.py?format=twiki&"
	     << "sDate=" << boost::gregorian::to_iso_string( *day ) 
	     << " HTTP/1.0\r\n" 
	     << "Host:shiftdb.lbdaq.cern.ch\r\n" 
	     << "\r\n" << std::flush ;

  std::string line ;

  // Check that the web server answers correctly
  std::getline( web_stream , line ) ;
  if ( ! boost::algorithm::find_first( line , "200 OK" ) ) {
    std::cerr << "ShiftDB server does not reply" << std::endl ;
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
std::string ShiftDB::getShifter( const std::string & function , 
				 const int slot ) { 
  if ( ! m_open ) return function ;

  if ( m_shifters.find( function ) == m_shifters.end() ) 
    return function ;

  return m_shifters[ function ][ slot ] ;  
}

//=============================================================================
// Name of data manager on shift
//=============================================================================
std::string ShiftDB::getCurrentDataManager( ) {

  boost::posix_time::ptime now = 
    boost::posix_time::second_clock::local_time() ;
  boost::gregorian::date now_day = now.date() ;

  int slot = 0 ;

  if ( now.time_of_day().hours() <= 7 ) {
    now_day = now.date() - boost::gregorian::date_duration( 1 ) ;
    slot = 2 ;
  } else if ( now.time_of_day().hours() <= 15 ) slot = 0 ;
  else if ( now.time_of_day().hours() <= 23 ) slot = 1 ;
  else slot = 2 ;

  std::map< std::string , std::vector< std::string > > shifters ;
  readWebFile( &now_day ) ;

  return getShifter( "Data Manager" , slot ) ;
}
