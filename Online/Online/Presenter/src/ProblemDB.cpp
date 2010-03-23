// $Id: ProblemDB.cpp,v 1.1 2010-03-23 12:51:41 ocallot Exp $
// Include files 
#include <iostream>
#include <boost/asio.hpp>

// local
#include "ProblemDB.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ProblemDB
//
// 2010-03-22 : Olivier Callot
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ProblemDB::ProblemDB( std::string address ) {
  m_address = address;
}
//=============================================================================
// Destructor
//=============================================================================
ProblemDB::~ProblemDB() {} 


//=========================================================================
//  
//=========================================================================
int ProblemDB::post( std::string system, std::string username, std::string title, 
                     std::string message, std::string logReference ) {

  boost::asio::ip::tcp::iostream webStream( m_address , "http" ) ;

  if ( ! webStream ) {
    std::cout << "Cannot open the Problem Database at " << m_address  << std::endl ;
    return -1;
  }

  // Send HTTP request to web server
  std::string postArg;
  postArg = "apikey=12345&system_name=" + system
    + "&title=" + urlEncode( title )
    + "&initial_comment=" + urlEncode( message )
    + "&author_name=" + urlEncode( username );
  if ( !logReference.empty() ) postArg = postArg + "&link=" + urlEncode( logReference );
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
  if ( line.find( "201 Created") != std::string::npos ) {
    status = 1; 
    while( std::getline( webStream , line )  ) {
      std::cout << line << std::endl;
      if ( line.find( "Location: " ) == 0 ) {
        m_reference = line.substr( 10 );
      }
    }
  } 
  // Check that the web server answers correctly
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
