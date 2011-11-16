// $Id: $
// Include files 
#include <cstdio>
#include <cstdlib>
#include <algorithm>

#include "OnlineHistDB/OMAMessage.h"

// local
#include "HistogramAlarms.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HistogramAlarms
//
// 2011-04-15 : Olivier Callot
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HistogramAlarms::HistogramAlarms( std::string& aa, std::string& bb, std::string& cc ) {
  m_histogramDB = new OnlineHistDB( aa, bb, cc );
}
//=============================================================================
// Destructor
//=============================================================================
HistogramAlarms::~HistogramAlarms() {} 


//=========================================================================
//  Retrieve all alarms since the specified time
//=========================================================================
void HistogramAlarms::retrieveAlarms ( std::string date ) {
  std::vector<int> messageIds;
  unsigned int minDate = atoi( date.c_str() );
  minDate = 10000 * (minDate - 20000000) + 700;  // after yesterday 7 AM
  std::cout << "Min date = " << minDate << std::endl;
  
  m_histogramDB->getMessages( messageIds );
  for ( std::vector<int>::iterator itI = messageIds.begin(); messageIds.end() != itI; ++itI ) {
    OMAMessage* message = new OMAMessage( *itI, *m_histogramDB );
    if ( !message->isAbort() ) {
      int day, mon, year, hh, mm, ss;
      char monC[4];
      char dumC[4];
      std::string lastTime( message->humanlastTime() );
      lastTime = lastTime.substr( 0, 24 );
      sscanf( lastTime.c_str(), "%3s %3s %2d %2d:%2d:%2d %4d", dumC, monC, &day, &hh, &mm, &ss, &year );
      std::string month( monC );
      mon = 0;
      if ( "Jan" == month ) mon = 1;
      if ( "Feb" == month ) mon = 2;
      if ( "Mar" == month ) mon = 3;
      if ( "Apr" == month ) mon = 4;
      if ( "May" == month ) mon = 5;
      if ( "Jun" == month ) mon = 6;
      if ( "Jul" == month ) mon = 7;
      if ( "Aug" == month ) mon = 8;
      if ( "Sep" == month ) mon = 9;
      if ( "Oct" == month ) mon = 10;
      if ( "Nov" == month ) mon = 11;
      if ( "Dec" == month ) mon = 12;
      if ( 0 == mon ) {
        std::cout << "Time " << message->humanTime() << " -> " << year << " " << month << " " << day
                  << " " << hh << ":" << mm << ":" << ss << std::endl;
      }
      unsigned int messageDate = ( (year-2000) * 10000 + mon * 100 + day) * 10000 + 100* hh + mm ;
      if ( messageDate < minDate ) continue;

      histAlarm temp;
      temp.system  = std::string( message->concernedSystem() );
      temp.message = std::string(  message->msgtext() );
      temp.date    = message->humanlastTime();
      temp.date    = temp.date.substr( 4, 15 );
      temp.timeStamp = messageDate;
      m_alarms.push_back( temp );

      std::cout << "Stamp " << temp.timeStamp << " Date " << temp.date 
                << " system '" << temp.system
                << "' text='" << temp.message
                << "'" << std::endl;
    }
  }
  std::sort( m_alarms.begin(), m_alarms.end(), LowerByStamp() );
}

//=========================================================================
//  
//=========================================================================
void HistogramAlarms::retrieveAlarms( std::vector<std::string>& alarms, const std::string& system ){
  for ( std::vector<histAlarm>::iterator itA = m_alarms.begin(); m_alarms.end() != itA; ++itA ) {
    if ( (*itA).system == system ) {
      std::string message = (*itA).date + " : " + (*itA).message;
      alarms.push_back( message );
    }
  }
}
//=============================================================================
