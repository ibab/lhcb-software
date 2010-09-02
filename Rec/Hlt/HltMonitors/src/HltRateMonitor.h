// $Id: $
#ifndef HLTRATEMONITOR_H 
#define HLTRATEMONITOR_H 1

// Include files
// from Gaudi
#include "HltMonitorBase.h"

class Condition;

/** @class HltRateMonitor HltRateMonitor.h
 *  Algorithm to monitor hlt rates offline.
 *  The most important property is Regexes. It is a list of regexes and for 
 *  every decision which matches a regex, a histogram of its rate is filled.
 *  A histogram is also filled with the combined rate of all decisions which
 *  match a regex. 
 *  The default is "Hlt2Express.*Decision", which monitors the Hlt2Express
 *  lines.
 *
 *  @author Roel Aaij
 *  @date   2010-08-24
 */
class HltRateMonitor : public HltMonitorBase {
public: 

   /// Standard constructor
   HltRateMonitor( const std::string& name, ISvcLocator* pSvcLocator );

   virtual ~HltRateMonitor( ); ///< Destructor

   virtual StatusCode initialize();    ///< Algorithm initialization
   virtual StatusCode execute   ();    ///< Algorithm execution
   virtual StatusCode finalize  ();    ///< Algorithm finalization

private:

   std::string m_decReportsLocation;
   std::string m_ODINLocation;
   std::string m_runParameterLocation;

   Condition* m_runParameters;
   
   bool m_disabled;
   bool m_forceEnable;

   unsigned int m_secondsPerBin;

};
#endif // HLTRATEMONITOR_H
