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

// Presenter
#include "RunDB.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ProblemDB
//
// 2010-03-22 : Olivier Callot
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ProblemDB::ProblemDB( std::string address, std::string rundbAddress ) {
  m_address = address;
  m_port = "http";
  std::string::size_type colon =  m_address.find(":");
  if ( colon != std::string::npos ) {
    m_port    = m_address.substr( colon+1 );
    m_address = m_address.substr( 0, colon );
  }

  getSystems();
  
  m_rundbAddress = rundbAddress;
}
//=============================================================================
// Destructor
//=============================================================================
ProblemDB::~ProblemDB() {}


//=========================================================================
//
//=========================================================================
int ProblemDB::post( std::string system, std::string username, std::string title,
                     std::string message, std::string logReference, std::string severity ) {

  if ( system == "L0CALO" ) system = "L0";
  if ( system == "L0MUON" ) system = "L0";
  if ( system == "L0DU"   ) system = "L0";
  if ( system == "Brunel" ) system = "Alignment";

  if ( std::find( m_systems.begin(), m_systems.end(), system ) == m_systems.end() ) {
    std::cout << "ProblemDB: System '" << system << "' unknown. request ignored." << std::endl;
    m_reference = "Unknown system '" + system +"'";
    return -1;
  }

  boost::asio::ip::tcp::iostream webStream( m_address , m_port ) ;

  if ( ! webStream ) {
    std::cout << "Cannot open the Problem Database at " << m_address  << " port " << m_port << std::endl ;
    m_reference = "Cannot open problem database at " + m_address +" port " + m_port;
    return -1;
  }

  // Send HTTP request to web server
  std::string postArg;
  postArg = "apikey=12345&system_name=" + system
    + "&title=" + urlEncode( title )
    + "&initial_comment=" + urlEncode( message )
    + "&author_name=" + urlEncode( username );
  if ( !logReference.empty() ) postArg = postArg + "&link=" + urlEncode( logReference );
  if ( !severity.empty() )     postArg = postArg + "&severity=" + urlEncode( severity );
  char argLen[20];
  sprintf( argLen, "%d", (int)postArg.size() );

  std::string posting;
  posting = "POST /api/problems/ HTTP/1.0\r\nHost: " + m_address + "\r\nContent-Length: " + argLen + "\r\n\r\n" + postArg;

  std::cout << "Sending '" << posting << "'" << std::endl;

  webStream << posting << std::flush;

  std::string line ;

  int status = 0;
  std::getline( webStream , line );
  std::cout << line << std::endl;
  if ( line.find( "201 Created") != std::string::npos ||
       line.find( "201 CREATED") != std::string::npos ) {
    status = 1;
    while( std::getline( webStream , line )  ) {
      std::cout << line << std::endl;
      if ( line.find( "Location: " ) == 0 ) {
        m_reference = line.substr( 10 );
      }
    }
  } else {
    m_reference = line;
  }
  
  // Check that the web server answers correctly, log the messages
  while( std::getline( webStream , line )  ) {
    std::cout << line << std::endl;
  }
  std::cout << "--- end ---" << std::endl;

  return status;
}
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
std::vector< std::vector< std::string > > ProblemDB::listOfProblems( const std::string & systemName ,
                                                                     int runNumber ) {
  std::vector< std::vector< std::string > > problems;

  std::cout << "Accessing the Problem Database" << std::endl;
  
  boost::asio::ip::tcp::iostream webStream( m_address , m_port ) ;

  if ( ! webStream ) {
    std::cout << "Cannot open the Problem Database at " << m_address  << std::endl ;
    return problems;
  }

  webStream << "GET /api/search/?_inline=True&system_visible=True" ;

  if ( 0 == runNumber ) {
    // Take date of tomorrow to have list of opened problems
    boost::posix_time::ptime now =
      boost::posix_time::second_clock::local_time() ;
    boost::gregorian::date day = now.date() + boost::gregorian::date_duration( 1 ) ;

    webStream << "&open_or_closed_gte=" << boost::gregorian::to_iso_extended_string( day )
              << " HTTP/1.0\r\n"
              << "Host:" << m_address << "\r\n"
              << "\r\n" << std::flush ;
  } else {
    RunDB runDb( m_rundbAddress ) ;
    if ( ! runDb.checkRun( runNumber ) ) {
      std::cerr << "Run number " << runNumber << "was not found in db"
                << std::endl ;
      return problems;
    }
    std::string st( runDb.currentStartTime() ) ;
    std::string et( runDb.currentEndTime() ) ;
    boost::algorithm::erase_all( st , " " ) ;
    boost::algorithm::erase_all( et , " " ) ;
    webStream << "&open_or_closed_gte=" << st
              << "&started_lte=" << et
              << " HTTP/1.0\r\n"
              << "Host:" << m_address << "\r\n"
              << "\r\n" << std::flush ;
  }

  std::string line ;

  // Check that the web server answers correctly
  std::getline( webStream , line ) ;
  if ( ! boost::algorithm::find_first( line , "200 OK" ) ) {
    std::cerr << "ProblemDB server does not reply" << std::endl ;
    return problems;
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
          single_line.push_back( v.second.get< std::string >( "started_at" ) ) ;
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
  return problems;
}

//=========================================================================
//  Read teh list of valid systems from the web.
//=========================================================================
void ProblemDB::getSystems ( ) {

  boost::asio::ip::tcp::iostream webStream( m_address , m_port ) ;

  if ( ! webStream ) {
    std::cout << "Cannot open the Problem Database at " << m_address  << " port " << m_port << std::endl ;
    return;
  }

  webStream << "GET /api/systems/ HTTP/1.0\r\n"
            << "Host:" << m_address << "\r\n"
            << "\r\n" << std::flush ;

  std::string line ;

  // Check that the web server answers correctly
  std::getline( webStream , line ) ;
  if ( !boost::algorithm::find_first( line , "200 OK" ) ) {
    std::cerr << "Error: ProblemDB replied " << line << std::endl ;
    return;
  }
  while ( std::getline( webStream , line ) ) {
    if ( line.size() == 1 ) break;
  }
  while ( std::getline( webStream , line ) ) {
    while ( "" != line ) {
      std::string::size_type kk = line.find(" ");
      if ( std::string::npos == kk ) {
        m_systems.push_back( line );
        break;
      }
      m_systems.push_back( line.substr( 0, kk ) );
      line = line.substr( kk+1 );
    }
  }
  webStream.close();
  
  //for ( std::vector<std::string>::iterator itS = m_systems.begin(); m_systems.end() != itS ; ++itS ) {
  //  std::cout << "System '" << *itS << "'" << std::endl;
  //}
}
