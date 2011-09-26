// $Id: $
// Include files
#include <iostream>
#include <ctime>
#include <cstring>

// local
#include "DailyReport.h"
#include "ProblemDB.h"
#include "RunDB.h"

//-----------------------------------------------------------------------------
// Implementation file for class : DailyReport
//
// 2011-04-13 : Olivier Callot
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DailyReport::DailyReport(  ) {
  m_firstSummary = true;
}
//=============================================================================
// Destructor
//=============================================================================
DailyReport::~DailyReport() {}

//=========================================================================
//
//=========================================================================
void DailyReport::build ( int argc, char** argv ) {

  std::cout << "Argc = " << argc << std::endl;

  std::string logFormat( "/%y%m%da.log" );

  char buf[200];
  time_t now = ::time(0);
  ::strftime( buf, 20, "%Y%m%d", ::localtime(&now) );
  std::string today( buf );
  ::strftime( buf, 200, logFormat.c_str(), ::localtime(&now) );
  m_logToday = std::string( buf );

  time_t before = now - 24 * 3600;
  tm* start = ::localtime(&before);
  int minYear  = start->tm_year + 1900;
  int minMonth = start->tm_mon  + 1;
  int minDay   = start->tm_mday;
  int minHour  = start->tm_hour;
  //int weekDay  = start->tm_wday;

  std::cout << "start : " << minYear << " " << minMonth << " " << minDay << " " << minHour  //<< " weekday : " << weekDay 
            << std::endl;

  ::strftime( buf, 20, "%Y%m%d", ::localtime(&before) );
  std::string yesterday( buf );
  ::strftime( buf, 20, "%d %B %Y", ::localtime(&before) );
  std::string yesterdayNice( buf );
  ::strftime( buf, 200, logFormat.c_str(), ::localtime(&before) );
  m_logYesterday = std::string( buf );

  time_t after = now + 24 * 3600;
  ::strftime( buf, 20, "%Y%m%d", ::localtime(&after) );
  std::string tomorrow( buf );
  ::strftime( buf, 20, "%d %B %Y", ::localtime(&after) );
  std::string tomorrowNice( buf );

  //== Make a dummy file for the 'tomorrow' link
  std::string path = "/group/online/www/DailyReport/";

  bool runChief = false;

  if ( 1 < argc ) {
    path = path + std::string( argv[1]) + "-";
    if ( "RunChief" == std::string( argv[1] ) ) runChief = true;
  }  

  std::string nextFileName = path + tomorrow + ".html";

  FILE* next = fopen( nextFileName.c_str(), "w" );
  fprintf( next, "<!DOCTYPE HTML PUBLIC ""-//IETF//DTD//HTML 2.0//EN> <HTML>" );
  fprintf( next, "<HEAD><TITLE>Not such Daily Report Yet</TITLE></HEAD>");
  fprintf( next, "<BODY>Wait for tomorrow!</BODY></HTML>" );
  fclose(  next );

  //== Open today's file.
  std::string fileName = path + today + ".html";
  m_web = fopen( fileName.c_str(), "w" );

  //== Get shifters
  m_shift = new Shifts();
  m_shift->loadFromWeb( today );
  m_yesterday = new Shifts();
  m_yesterday->loadFromWeb( yesterday );
  m_tomorrow = new Shifts();
  m_tomorrow->loadFromWeb( tomorrow );

  ::strftime( buf, 40, "%d %B %Y %H:%M", ::localtime( &now ) );

  std::string s_SL = "Shift Leader";
  std::string s_DM = "Data Manager";
  std::string s_PR = "Production";

  fprintf( m_web, "<!DOCTYPE HTML PUBLIC ""-//IETF//DTD//HTML 2.0//EN> <HTML>" );
  fprintf( m_web, "<HEAD><TITLE>Daily Report</TITLE></HEAD><BODY>" );

  fprintf( m_web, "<TABLE width=90%% align=center><TR><TD align = left><A href=""./%s.html"">%s</A></TD>",
           yesterday.c_str(), yesterdayNice.c_str() );
  fprintf( m_web, "<TD align=center><FONT size=+3 color=blue >LHCb Daily Report, %s</FONT></TD>", buf );
  fprintf( m_web, "<TD align=right><A href=""./%s.html"">%s</A></TD></TR>", tomorrow.c_str(), tomorrowNice.c_str() );
  if ( !runChief ) {
    fprintf( m_web, "<TR><TD/><TD align=center><A href=""./RunChief-today.html"">");
    fprintf( m_web, "Logbook extract at 10 AM for Run Chief</A></TD></TR>" );
  }
  fprintf( m_web, "</TABLE>" );

  fprintf( m_web, "<TABLE width=70%% cellspacing=5 cellpading=2 border=2 align=center><FONT size=+2>" );
  fprintf( m_web, "<tr><th/><th>%-20s</th><th>%-20s</th><th>%-20s</th></tr>", s_SL.c_str(), s_DM.c_str(), s_PR.c_str() );
  fprintf( m_web, "<tr><td>Yesterday night</td><td>%-20s</td><td>%-20s</td><td>%-20s</td></tr>",
           m_yesterday->name( s_SL, 2 ), m_yesterday->name( s_DM, 2 ), m_yesterday->name( s_PR, 2) );
  fprintf( m_web,"<tr><td>Today morning</td><td>%-20s</td><td>%-20s</td><td>%-20s</td></tr>",
           m_shift->name( s_SL, 0 ), m_shift->name( s_DM, 0 ), m_shift->name( s_PR, 0 ) );
  fprintf( m_web,"<tr><td>Today afternoon</td><td>%-20s</td><td>%-20s</td><td>%-20s</td></tr>",
           m_shift->name( s_SL, 1 ), m_shift->name( s_DM, 1 ), m_shift->name( s_PR, 1 ) );
  fprintf( m_web,"<tr><td>Today night</td><td>%-20s</td><td>%-20s</td><td>%-20s</td></tr>",
           m_shift->name( s_SL, 2 ), m_shift->name( s_DM, 2 ), m_shift->name( s_PR, 2 ) );
  fprintf( m_web,"<tr><td>Tomorrow morning</td><td>%-20s</td><td>%-20s</td><td>%-20s</td></tr>",
           m_tomorrow->name( s_SL, 0 ), m_tomorrow->name( s_DM, 0 ), m_tomorrow->name( s_PR, 0 ) );
  fprintf( m_web, "</TABLE></FONT><HR>" );

  std::string aa( "reader" );
  std::string bb( "HIST_READER" );
  std::string cc( "HISTDB" );

  m_histAlarms = new HistogramAlarms( aa, bb, cc );
  m_histAlarms->retrieveAlarms( yesterday );

  //== Get the fill informations
  //RunDB* myRunDb = new RunDB( "devrundb.lbdaq.cern.ch" );
  RunDB* myRunDb = new RunDB( "lbrundb.cern.ch" );
  myRunDb->setMinDate( minYear, minMonth, minDay, minHour );
  myRunDb->fillInformation( 0 );
  while( 0 < myRunDb->nextFill() ) {
    myRunDb->fillInformation( myRunDb->nextFill() );
  }
  myRunDb->fillInformation( -1 );
  myRunDb->publish( m_web );

  extractFromElog( "Shift", "Shift Summary", true );

  listOnePiquet( "Run Chief" );
  listOnePiquet( "VELO Piquet" );
  listOnePiquet( "ST Piquet" );
  listOnePiquet( "OT Piquet" );
  listOnePiquet( "RICH Piquet" );
  listOnePiquet( "Calo Piquet" );
  listOnePiquet( "Muon Piquet" );
  listOnePiquet( "L0 Piquet" );
  listOnePiquet( "HLT Piquet" );
  listOnePiquet( "Online Piquet" );
  listOnePiquet( "DQ Piquet" );
  listOnePiquet( "VELO DQ Piquet" );
  listOnePiquet( "Production" );

  fprintf( m_web, "</body></html>" );

  fclose( m_web );

  std::string defaultFile  = path+"today.html";
  std::string command = "rm " + defaultFile;
  system( command.c_str() );
  command = "ln -s " + fileName + " " + defaultFile;
  system( command.c_str() );
}

//=========================================================================
//
//=========================================================================
void DailyReport::listOnePiquet ( std::string name ) {

  std::cout << "Get problem and logbook for " << name << std::endl;

  std::string piquet = m_shift->name( name );
  std::string before = m_yesterday->name( name );
  if ( name == "OT Piquet" ) {
    std::string piquetDay = m_shift->name( "OT Piquet 9-21h" );
    std::string piquetNxt = m_shift->name( "OT Piquet 21-9h" );
    before    = m_yesterday->name( "OT Piquet 21-9h" );
    if ( piquetDay == piquetNxt ) {
      piquet = piquetDay;
    } else {
      piquet = piquetDay + " (night: " + piquetNxt + ")";
    }
  } else if ( name == "Production" ) {
    std::string after( m_shift->name( name, 1 ) );
    std::string afterY( m_yesterday->name( name, 1 ) );
    if ( after != "" ) {
      if ( piquet != "" ) piquet += ", ";
      piquet += after;
    }
    if ( afterY != "" ) {
      if ( before != "" ) before += ", ";
      before += afterY;
    }
  }

  piquet = html( piquet );
  if ( piquet != before ) piquet = std::string( html( before ) ) + " &rarr; " + piquet;
  fprintf( m_web, "<H3>%s : %s </H3>", name.c_str(), piquet.c_str() );

  std::vector< std::vector< std::string > > problems;
  //ProblemDB pbdb( "devweb01.lbdaq.cern.ch", "8000" ); //"lbproblems.cern.ch" );
  ProblemDB pbdb( "lbproblems.cern.ch" );
  std::vector< std::string > histAlarms;

  if ( name == "VELO Piquet" ) {
    pbdb.getListOfProblems( problems, "VELO" );
    m_histAlarms->retrieveAlarms( histAlarms, "VELO" );
    extractFromElog( "VELO", "Daily Summary" );

  } else if ( name == "ST Piquet" ) {
    pbdb.getListOfProblems( problems, "TT" );
    pbdb.getListOfProblems( problems, "IT" );
    m_histAlarms->retrieveAlarms( histAlarms, "IT" );
    m_histAlarms->retrieveAlarms( histAlarms, "TT" );
    extractFromElog( "ST", "Daily Report" );

  } else if ( name == "OT Piquet" ) {
    pbdb.getListOfProblems( problems, "OT" );
    m_histAlarms->retrieveAlarms( histAlarms, "OT" );
    extractFromElog( "OT", "Piquet Report" );

  } else if ( name == "RICH Piquet" ) {
    pbdb.getListOfProblems( problems, "RICH1" );
    pbdb.getListOfProblems( problems, "RICH2" );
    m_histAlarms->retrieveAlarms( histAlarms, "RICH1" );
    m_histAlarms->retrieveAlarms( histAlarms, "RICH2" );
    extractFromElog( "RICH", "Daily Report" );

  } else if ( name == "Calo Piquet" ) {
    pbdb.getListOfProblems( problems, "SPD" );
    pbdb.getListOfProblems( problems, "PRS" );
    pbdb.getListOfProblems( problems, "ECAL" );
    pbdb.getListOfProblems( problems, "HCAL" );
    m_histAlarms->retrieveAlarms( histAlarms, "SPD" );
    m_histAlarms->retrieveAlarms( histAlarms, "PRS" );
    m_histAlarms->retrieveAlarms( histAlarms, "ECAL" );
    m_histAlarms->retrieveAlarms( histAlarms, "HCAL" );
    m_histAlarms->retrieveAlarms( histAlarms, "CALO" );
    extractFromElog( "CALO", "Piquet Report" );

  } else if ( name == "Muon Piquet" ) {
    pbdb.getListOfProblems( problems, "MUON" );
    m_histAlarms->retrieveAlarms( histAlarms, "MUON" );
    extractFromElog( "MUON", "Piquet report" );

  } else if ( name == "L0 Piquet" ) {
    pbdb.getListOfProblems( problems, "L0" );
    m_histAlarms->retrieveAlarms( histAlarms, "L0" );
    extractFromElog( "L0 Trigger", "Daily Report" );

  } else if ( name == "HLT Piquet" ) {
    pbdb.getListOfProblems( problems, "HLT" );
    m_histAlarms->retrieveAlarms( histAlarms, "HLT" );
    extractFromElog( "HLT Trigger", "piquet report" );

  } else if ( name == "Online Piquet" ) {
    pbdb.getListOfProblems( problems, "DAQ" );
    extractFromElog( "Online", "Piquet report" );

  } else if ( name == "DQ Piquet" ) {
    pbdb.getListOfProblems( problems, "Alignment" );
    pbdb.getListOfProblems( problems, "Calibration" );
    pbdb.getListOfProblems( problems, "Offline" );
    extractFromElog( "Data Quality", "Piquet Report" );

  } else if ( name == "Production" ) {
    extractFromElog( "Operations", "Shift report" );

  }

  if ( problems.size() > 0 ) {
    fprintf( m_web, "<UL>" );
    for ( std::vector<std::vector<std::string> >::iterator pbIt = problems.begin(); problems.end() != pbIt; ++pbIt  ) {
      fprintf( m_web, "<LI><A href=""http://lbproblems.cern.ch/problems/%s"">Problem %s</A> in %s (%s) since %s : '%s'",
               (*pbIt)[3].c_str(), (*pbIt)[3].c_str(), (*pbIt)[0].c_str(),
               (*pbIt)[1].c_str(), (*pbIt)[2].c_str(), (*pbIt)[4].c_str() );
    }
    fprintf ( m_web, "</UL>" );
  }
  if ( histAlarms.size() > 0 ) {
    fprintf( m_web, "<UL>" );
    for ( std::vector<std::string>::iterator itA = histAlarms.begin(); histAlarms.end() != itA; ++itA ) {
      fprintf( m_web, "<LI><FONT color=red>Histogram analysis</FONT> %s", (*itA).c_str() );
    }
    fprintf( m_web, "</UL>" );
  }
  fprintf( m_web, "<HR>" );
}

//=========================================================================
//  Scan the logbook file and extract the relevant items in the daily report
//=========================================================================
void DailyReport::scanElog( std::string file, std::string system, bool today, bool addHeader ) {
  FILE* logbook = fopen( file.c_str(), "r" );
  if ( 0 != logbook ) {
    char buffer[50000];
    int size = 0;
    char* inPtr = &buffer[0];
    m_rdPtr = &buffer[0];
    buffer[0] = 0;
    while ( NULL != m_rdPtr ) {
      strcpy( &buffer[0], m_rdPtr );
      inPtr = &buffer[strlen( &buffer[0] )];
      if ( !feof( logbook ) ) {
        int free = &buffer[50000] - inPtr - 1;
        size = fread( inPtr, 1, free, logbook );
        *(inPtr+size) = 0;  // terminate the string!
      }
      m_rdPtr = &buffer[0];
      std::string itemString = getLogbookTagValue( "$@MID@$:", true );
      std::cout << "Item " << itemString << std::endl;
      std::string date = getLogbookTagValue( "Date:" );
      std::cout << "Date : '" << date << "'";
      date = date.substr( 5, 6 ) + date.substr( 16, 9);
      std::cout << "-> '" << date << "'" << std::endl;
      if ( today || atoi( date.substr(7,2).c_str() ) > 6 ) {  // exclude reports before 7 AM yesterday.
        std::string systemName = getLogbookTagValue( "System:" );
        std::cout << "System '" << systemName << "'" << std::endl;
        if ( systemName.find( system ) != std::string::npos ) {
          std::cout << "Found a wanted entry " << itemString << " at '" << date << "'" << std::endl;
          std::string author = getLogbookTagValue( "Author:" );
          std::string text("") ;
          char* begText = strstr( m_rdPtr, "=====\n" );
          if ( 0 == begText ) {
            std::cout << "Begin text not found " << std::endl;
          } else {
            begText += 6;
            char* endText = strstr( m_rdPtr, "$@MID@$:" );
            if ( 0 != endText ) {
              text = std::string( begText, endText - begText );
            } else {
              text = std::string( begText );
            }
          }
          text = html( text );
          std::cout << "Text : '" << text << "'" << std::endl;
          if ( m_firstSummary ) {
            if ( addHeader ) fprintf( m_web, "<H3>%s entries in the logbook</H3>", system.c_str() );
            fprintf( m_web, "<TABLE width = 90%% cellspacing=5 cellpading=2 border=2 align=center>" );
            m_firstSummary = false;
          }
          fprintf( m_web, "<TR><TD width=10%% valign=top><a href=""http://lblogbook.cern.ch/%s/%s"">%s</a></TD>",
                   m_logbook.c_str(),itemString.c_str(), date.c_str() );
          fprintf( m_web, "<TD width=15%% valign=top>%s</TD><TD>%s</TD></TR>", author.c_str(), text.c_str() );
        }
      }
      m_rdPtr = strstr( m_rdPtr, "$@MID@$:" );
    }
    fclose( logbook );
  }
  if ( !m_firstSummary && today ) {
    fprintf( m_web, "</TABLE>" );
    if ( addHeader ) fprintf( m_web, "<HR>" );
    m_firstSummary = true;
  }
}

//=========================================================================
//  Get the value of a tag.
//=========================================================================
std::string DailyReport::getLogbookTagValue ( std::string tag, bool advance ) {
  std::string result("");
  char* begTag = strstr( m_rdPtr, tag.c_str() );
  if ( NULL == begTag ) return result;
  begTag = begTag + tag.size() + 1;  // skip the blank after the column
  char* endLine = strchr( begTag, 10 );  // search the line-feed character
  if ( endLine != 0 ) {
    result = std::string( begTag, endLine - begTag );
    if ( advance ) m_rdPtr = endLine+1;
  }
  return result;
}

//=========================================================================
//
//=========================================================================
char* DailyReport::html( std::string src ) {
  const char* in  = src.c_str();
  char* out = &m_html[0];
  char tmp;
  while ( 0 != (tmp = *in++) ) {
    if ( tmp == 10 )       out = strcpy( out, "<br>" ) + 4;
    else if ( tmp == 13 )  ;
    else if ( tmp == '"' ) out = strcpy( out, "&quot;" ) + 6;
    else if ( tmp == '<' ) out = strcpy( out, "&lt;" ) + 4;
    else if ( tmp == '>' ) out = strcpy( out, "&gt;" ) + 4;
    else if ( tmp == '&' ) out = strcpy( out, "&amp;" ) + 5;
    else if ( tmp == 'h' ) {
      //std::cout << "Test link " << *in << *(in+1) << *(in+2) << *(in+3) << *(in+4) << *(in+5) << std::endl;
      if ( 0 == strncmp( in, "ttp://",  6 ) ||
           0 == strncmp( in, "ttps://", 7 )  ) { /// link!
        std::cout << "Found link h" << in << std::endl;
        size_t len = strcspn( in, " ,;)\n" );
        std::string link( in-1, in+len );
        link = "<a href=""" + link + ">"+link+"</a>";
        strcpy( out, link.c_str() );
        out += link.size();
        in += len;
      } else {
        *out++ = tmp;
      }
    }
    else *out++ = tmp;
  }
  *out = 0;
  return &m_html[0];
}
//=========================================================================
//
//=========================================================================
void DailyReport::extractFromElog ( std::string logbook, std::string system, bool addHeader ) {
  m_logbook = logbook;
  std::string logRoot( "/web/sites/logbook/daemon/elog/data/logbooks/" );
  std::string file = logRoot + logbook + m_logYesterday;
  scanElog( file, system, false, addHeader );
  file =  logRoot + logbook + m_logToday;
  scanElog( file, system, true, addHeader );
}
//=============================================================================

int main(int argc, char** argv) {
  DailyReport report;
  report.build( argc, argv );
  return 1;
}
//=============================================================================

