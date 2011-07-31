// $Id: $
#ifndef DAILYREPORT_H 
#define DAILYREPORT_H 1

// Include files
#include <cstdio>
#include "Shifts.h"
#include "HistogramAlarms.h"

/** @class DailyReport DailyReport.h
 *  Generates the Daily Report
 *
 *  @author Olivier Callot
 *  @date   2011-04-13
 */
class DailyReport {
public: 
  /// Standard constructor
  DailyReport( ); 

  virtual ~DailyReport( ); ///< Destructor

  void build( int argc, char** argv );
  
protected:

  void listOnePiquet( std::string name );

  void scanElog( std::string file, std::string system, bool today, bool addHeader=false );

  std::string getLogbookTagValue ( std::string tag, bool advance=false );

  char* html( std::string in );
  
  void extractFromElog( std::string logbook, std::string system, bool addHeader = false );
  
private:
  Shifts* m_shift;
  Shifts* m_yesterday;
  Shifts* m_tomorrow;
  FILE*   m_web;
  HistogramAlarms* m_histAlarms;
  char*   m_rdPtr;
  char    m_html[50000];
  bool    m_firstSummary;
  std::string m_logbook;
  std::string m_logYesterday;
  std::string m_logToday;
};
#endif // DAILYREPORT_H
