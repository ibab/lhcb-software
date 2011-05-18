// $Id: $
#ifndef HISTOGRAMALARMS_H 
#define HISTOGRAMALARMS_H 1

// Include files
#include "OnlineHistDB/OnlineHistDB.h"

struct histAlarm {
  unsigned int timeStamp;
  std::string system;
  std::string date;
  std::string message;
};

/** @class HistogramAlarms HistogramAlarms.h
 *  
 *
 *  @author Olivier Callot
 *  @date   2011-04-15
 */
class HistogramAlarms {
public: 
  /// Standard constructor
  HistogramAlarms( std::string& aa, std::string& bb, std::string& cc ); 

  virtual ~HistogramAlarms( ); ///< Destructor
  
  void retrieveAlarms ( std::string date );

  struct LowerByStamp  {
    bool operator() ( histAlarm a, histAlarm b) const { return a.timeStamp < b.timeStamp; }
  };
  
  void retrieveAlarms( std::vector<std::string>& alarms, const std::string& system );

protected:

private:
  OnlineHistDB* m_histogramDB;
  std::vector<histAlarm> m_alarms;
};
#endif // HISTOGRAMALARMS_H
