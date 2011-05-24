// $Id: $
// Include files 
#include <iostream>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/formatters.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/regex.hpp>

// local
#include "RunDB.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RunDB
//
// 2011-05-17 : Olivier Callot
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RunDB::RunDB( std::string address ) {
  m_address = address;
  m_nextFill = 0;
  m_minDate  = 0;
}
//=============================================================================
// Destructor
//=============================================================================
RunDB::~RunDB() {} 


//=========================================================================
//  
//=========================================================================
void RunDB::fillInformation ( int fillNb ) {
  boost::asio::ip::tcp::iostream webStream( m_address , "http" ) ;

  if ( ! webStream ) {
    std::cout << "Cannot open the Run Database at " << m_address  << std::endl ;
    return ;
  }

  std::string fillString( "latest" );
  if ( 0 > fillNb ) { 
    fillString = "this_year_totals";
  } else if ( 0 < fillNb ) {
    char fillChar[20];
    sprintf( &fillChar[0], "%d/", fillNb );
    fillString = std::string( fillChar );
  }

  std::cout << "Get fill " << fillString << std::endl;
  
  webStream << "GET /api/fill/" << fillString << " HTTP/1.0\r\n"
            << "Host:" << m_address << "\r\n"
            << "\r\n" << std::flush ;

  std::string line ;

  // Check that the web server answers correctly
  std::getline( webStream , line ) ;
  if ( boost::algorithm::find_first( line , "404 Not Found" ) ) {
    if ( 0 < fillNb ) {
      std::cout << "Fill " << fillNb << " not found" << std::endl;
      m_nextFill = fillNb - 1;
      return;
    }
  }
  if ( ! boost::algorithm::find_first( line , "200 OK" ) ) {
    std::cerr << "RunDB server does not reply 200 OK : "
              << line << std::endl;
    while(   std::getline( webStream , line )  ) {
      std::cout << line << std::endl;
      if ( 2 >  line.size() ) break;
    }
    m_nextFill = 0;
    return ;
  }

  // Parse the web server answers
  std::string pattern ;
  pattern = "\\N{left-curly-bracket}(.*)\\N{right-curly-bracket}" ;

  const boost::regex e( pattern ) ;

  boost::property_tree::ptree fill_tree ;

  while ( std::getline( webStream , line ) ) {
    // Check that the answer has the correct format (JSON format)
    
    if ( boost::regex_match( line , e ) ) {
      // Now parse each line (there should be actually only one line
      // in the server answer, otherwise it is over-written
      std::istringstream is( "{\"fill\":[" + line + "]}" ) ;

      try {
        boost::property_tree::json_parser::read_json( is , fill_tree ) ;

        BOOST_FOREACH( boost::property_tree::ptree::value_type &v,
                       fill_tree.get_child("fill")) {
          Fill myFill;
          if ( 0 <= fillNb ) {            
            myFill.number      = v.second.get<int>( "fill_id" ) ;
            myFill.time        = v.second.get< std::string >( "timestamp" );
            myFill.lumiTot     = 1.e-6 * v.second.get<double>( "lumi_total" );
            myFill.lumiHV      = 1.e-6 * v.second.get<double>( "lumi_hvon" );
            myFill.lumiVelo    = 1.e-6 * v.second.get<double>( "lumi_veloin" );
            myFill.lumiRunning = 1.e-6 * v.second.get<double>( "lumi_running" );
            myFill.lumiLogged  = 1.e-6 * v.second.get<double>( "lumi_logged" );
            m_nextFill         = v.second.get<int>( "prev_fill_id" ) ;
            
            std::cout << "Fill "  << myFill.number
                      << " time " << myFill.time 
                      << " tot "  << myFill.lumiTot
                      << " hv "   << myFill.lumiHV
                      << " velo " << myFill.lumiVelo
                      << " run "  << myFill.lumiRunning
                      << " log "  << myFill.lumiLogged << std::endl;

            int yy, mm, dd, hh;
            sscanf( myFill.time.c_str(), "%4d-%2d-%2dT%2d", &yy, &mm, &dd, &hh );
            int endTag = codeDate( yy, mm, dd, hh );
            std::cout << "Date " << endTag << " min " << m_minDate << std::endl;
            if ( endTag < m_minDate ) m_nextFill = 0;
            if ( 0 < m_nextFill && myFill.lumiTot > 0.0001 ) m_fills.push_back( myFill );
          } else {
            myFill.number      = 0;
            myFill.time        = "Whole year";
            myFill.lumiTot     = 1.e-6 * v.second.get<double>( "lumi_total__sum" );
            myFill.lumiHV      = 1.e-6 * v.second.get<double>( "lumi_hvon__sum" );
            myFill.lumiVelo    = 1.e-6 * v.second.get<double>( "lumi_veloin__sum" );
            myFill.lumiRunning = 1.e-6 * v.second.get<double>( "lumi_running__sum" );
            myFill.lumiLogged  = 1.e-6 * v.second.get<double>( "lumi_logged__sum" );
            m_fills.push_back( myFill );
          }
        }
      }
      catch (...) {
        std::cerr << "Error in the run db server response" << std::endl ;
        m_nextFill = 0;
      }
    }
  }
}

//=========================================================================
//  
//=========================================================================
void RunDB::publish( FILE* web ) {
  fprintf( web, "\n<H3>Luminosity and inefficiencies</H3>");
  fprintf( web, "<TABLE width=70%% cellspacing=1 cellpading=200 border=5 align=center><FONT size=+2>" );
  fprintf( web, "\n<TR><TH>Fill</TH><TH>Delivered</TH><TH>Logged</TH><TH>Inefficiency</TH><TH>HV off</TH>");
  fprintf( web, "<TH>Velo not IN</TH><TH>DAQ not running</TH><TH>Dead time</TH></TR>");
  for ( unsigned int kk = 0 ; kk < m_fills.size() ; ++kk ) {
    if ( 0.000001 > m_fills[kk].lumiTot ) continue;
    if( 0 != m_fills[kk].number ) {
      fprintf( web, "\n<TR><TD align=center>%d</TD>",  m_fills[kk].number );
    } else {
      fprintf( web, "<TR bgcolor=rgb(200,200,200)><TD align=center>Total</TD>" );
    }
    fprintf( web, "<TD align=right>%9.3f pb-1</TD><TD align=right>%9.3f pb-1</TD>", 
             m_fills[kk].lumiTot, m_fills[kk].lumiLogged );
    double effT = 100. * (1. - m_fills[kk].lumiLogged  / m_fills[kk].lumiTot );
    double effH = 100. * (1. - m_fills[kk].lumiHV      / m_fills[kk].lumiTot );
    double effV = 100. * (1. - m_fills[kk].lumiVelo    / (m_fills[kk].lumiHV      + 0.000001) );
    double effR = 100. * (1. - m_fills[kk].lumiRunning / (m_fills[kk].lumiVelo    + 0.000001));
    double effL = 100. * (1. - m_fills[kk].lumiLogged  / (m_fills[kk].lumiRunning + 0.000001) );
    std::string option("align=center width=14%%");
    const char* opt = option.c_str();
    fprintf( web, "<TD %s>%6.2f %%</TD><TD %s>%6.2f %%</TD><TD %s>%6.2f %%</TD>", opt, effT, opt, effH, opt, effV );
    fprintf( web, "<TD %s>%6.2f %%</TD><TD %s>%6.2f %%</TD></TR>", opt, effR, opt, effL );
  }
  fprintf( web, "</TABLE></FONT><HR>\n" );
}
//=============================================================================
