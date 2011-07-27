// $Id: $
#ifndef EXTRACTFROMTRENDINGFILE_H 
#define EXTRACTFROMTRENDINGFILE_H 1

// Include files
#include <ctime>
#include "Trending/ITrendingTool.h"

/** @class ExtractFromTrendingFile ExtractFromTrendingFile.h
 *  Dump a file
 *
 *  @author Olivier Callot
 *  @date   2011-05-31
 */
class ExtractFromTrendingFile {
public: 
  /// Standard constructor
  ExtractFromTrendingFile( ITrendingTool* trend); 

  virtual ~ExtractFromTrendingFile( ); ///< Destructor

  void extract( std::string file, std::string tag, std::string start, std::string end );
  
protected:
  int timeFromISO( std::string iso ) {
    ::tm aTm;
    int year;
    sscanf( iso.c_str(), "%4d-%2d-%2dT%2d:%2d:%2d", 
            &year, &aTm.tm_mon, &aTm.tm_mday, &aTm.tm_hour, &aTm.tm_min, &aTm.tm_sec );
    aTm.tm_year = year - 1900;
    aTm.tm_mon -= 1;    
    int cTime = int( ::mktime( &aTm ) );
    return cTime;
  }
  
private:
  ITrendingTool* m_trend;
};
#endif // EXTRACTFROMTRENDINGFILE_H
