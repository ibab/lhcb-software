// $Id: Shifts.cpp,v 1.2 2010-11-02 13:52:04 ocallot Exp $
// Include files

// local
#include "Shifts.h"

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
// Implementation file for class : Shifts
//
// 2010-03-01 : Patrick Robbe
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Shifts::Shifts( ) {
  m_shifters.clear();
}

//=============================================================================
// Destructor
//=============================================================================
Shifts::~Shifts() { }

//=========================================================================
//  Load the database information fportoday, or yesterday
//=========================================================================
bool Shifts::loadFromWeb( std::string date ) {

  boost::asio::ip::tcp::iostream web_stream( "lbshiftdb.cern.ch" , "http" ) ;

  if ( ! web_stream ) {
    std::cerr << "Cannot open shiftdb script via the web" << std::endl ;
    return false;
  }

  // Send HTTP request to web server
  web_stream << "GET /shiftdb_report.php?format=twiki&"
             << "sDate=" << date
             << " HTTP/1.0\r\n"
             << "Host:lbshiftdb.cern.ch\r\n"
             << "\r\n" << std::flush ;

  std::string line ;

  // Check that the web server answers correctly
  std::getline( web_stream , line ) ;
  if ( ! boost::algorithm::find_first( line , "200 OK" ) ) {
    std::cerr << "Shiftdb server does not reply" << std::endl ;
    return false;
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
    if ( vec[0].size() > 0 ) vec[0] = vec[0].substr(1);   // remove leading blank
    if ( vec[1].size() > 0 ) vec[1] = vec[1].substr(1);
    if ( vec[2].size() > 0 ) vec[2] = vec[2].substr(1);
    m_shifters[ function ] = vec ;
  }
  return true;  
}
//=========================================================================
//  Name as string for piquet/shift
//=========================================================================
std::string Shifts::name( const std::string & function ) {
  if ( m_shifters.find( function ) == m_shifters.end() ) return function;
  
  return m_shifters[ function ][ 0 ];
}
//=========================================================================
//  Name for shifters
//=========================================================================
const char* Shifts::name( const std::string & function, int slot  ) {
  if ( m_shifters.find( function ) == m_shifters.end() ) return function.c_str();
  
  return m_shifters[ function ][ slot ].c_str();
}
//=============================================================================
