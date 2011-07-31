// $Id: ProblemDB.cpp,v 1.7 2010-10-18 07:37:58 marcocle Exp $

// Include files
// This file
#include "ProblemDB.h"

// STL
#include <iostream>

// Boost
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/formatters.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/regex.hpp>

//-----------------------------------------------------------------------------
// Implementation file for class : ProblemDB
//
// 2010-03-22 : Olivier Callot
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ProblemDB::ProblemDB( std::string address, std::string port ) {
  m_address = address;
  m_port = port;
  m_monthName.push_back( "Jan" );
  m_monthName.push_back( "Feb" );
  m_monthName.push_back( "Mar" );
  m_monthName.push_back( "Apr" );
  m_monthName.push_back( "May" );
  m_monthName.push_back( "Jun" );
  m_monthName.push_back( "Jul" );
  m_monthName.push_back( "Aug" );
  m_monthName.push_back( "Sep" );
  m_monthName.push_back( "Oct" );
  m_monthName.push_back( "Nov" );
  m_monthName.push_back( "Dec" );
}
//=============================================================================
// Destructor
//=============================================================================
ProblemDB::~ProblemDB() {}

//=========================================================================
//
//=========================================================================
std::string  ProblemDB::urlEncode ( std::string src) {
  unsigned char *pd, *p;
  std::string str;
  str.reserve( 3* src.size() );

  pd = (unsigned char *) str.c_str();
  p  = (unsigned char *) src.c_str();
  while ( *p ) {
    if (strchr("%&=#?+/", *p) || *p > 127 || *p < 32) {
      sprintf((char *) pd, "%%%02X", *p);
      pd += 3;
      p++;
    } else if (*p == ' ') {
      *pd++ = '+';
      p++;
    } else {
      *pd++ = *p++;
    }
  }
  *pd = '\0';
  str = std::string( str.c_str() );
  return str;
}
//=============================================================================
//
//=============================================================================
void ProblemDB::getListOfProblems( std::vector< std::vector< std::string > > &problems ,
                                   const std::string & systemName ) {

  boost::asio::ip::tcp::iostream webStream( m_address , m_port ) ;

  if ( ! webStream ) {
    std::cout << "Cannot open the Problem Database at " << m_address  << std::endl ;
    return ;
  }

  /*
  webStream << "GET /api/activity/  HTTP/1.0\r\n"
            << "Host:" << m_address << "\r\n"
            << "\r\n" << std::flush ;
  */
  std::cout << "** get problems from database **" << std::endl;
  
  webStream << "GET /api/search/?_inline=True&system_visible=True" ;

  // Take date of tomorrow to have list of opened problems
  boost::posix_time::ptime now =
    boost::posix_time::second_clock::local_time() ;
  boost::gregorian::date day = now.date() + boost::gregorian::date_duration( 1 ) ;

  webStream << "&open_or_closed_gte=" << boost::gregorian::to_iso_extended_string( day )
            << " HTTP/1.0\r\n"
            << "Host:" << m_address << "\r\n"
            << "\r\n" << std::flush ;

  std::string line ;

  // Check that the web server answers correctly
  std::getline( webStream , line ) ;
  if ( ! boost::algorithm::find_first( line , "200 OK" ) ) {
    std::cerr << "ProblemDB server does not reply" << std::endl ;
    std::cout << line << std::endl;
    return ;
  }

  // Parse the web server answers
  std::string pattern ;
  pattern = "\\N{left-square-bracket}\\N{left-curly-bracket}(.*)" ;
  pattern += "\\N{right-curly-bracket}\\N{right-square-bracket}" ;

  const boost::regex e( pattern ) ;

  std::vector< std::string > single_line ;
  boost::property_tree::ptree problem_tree ;

  while ( std::getline( webStream , line ) ) {
    // Check that the answer has the correct format (JSON format)
    if ( boost::regex_match( line , e ) ) {
      // Now parse each line (there should be actually only one line
      // in the server answer, otherwise it is over-written
      std::istringstream is( "{\"problem\":" + line + "}" ) ;

      std::cout << line << std::endl;

      try {
        boost::property_tree::json_parser::read_json( is , problem_tree ) ;

        BOOST_FOREACH( boost::property_tree::ptree::value_type &v,
                       problem_tree.get_child("problem")) {
          single_line.clear() ;
          std::string systn = v.second.get< std::string >( "system" ) ;
          if ( "" != systemName )
            if ( systemName.find( systn ) == std::string::npos )
              continue ;
          single_line.push_back( systn ) ;
          single_line.push_back( v.second.get< std::string >( "severity" ) ) ;
          std::string date = v.second.get< std::string >( "started_at" );
          int yy, mm, dd;
          sscanf( date.c_str(), "%4d-%2d-%2d", &yy, &mm, &dd );
          char tmp[40];
          sprintf( tmp, "%s %d %s", m_monthName[mm-1].c_str(), dd, date.substr(11).c_str() );
          date = std::string( tmp );
          single_line.push_back( date ) ;
          single_line.push_back( v.second.get< std::string >( "id" ) ) ;
          single_line.push_back( v.second.get< std::string >( "title" ) ) ;
          problems.push_back( single_line ) ;
        }
      }
      catch (...) {
        std::cerr << "Error in the problem db server response" << std::endl ;
      }
    }
  }
}
