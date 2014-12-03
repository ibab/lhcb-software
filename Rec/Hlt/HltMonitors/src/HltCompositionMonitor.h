// $Id: $
#ifndef HLTCOMPOSITIONMONITOR_H 
#define HLTCOMPOSITIONMONITOR_H 1

// STL
#include <map>

// boost

// from Gaudi
#include "HltMonitorBase.h"

namespace AIDA {
   class IProfile1D;
}

namespace LHCb {
   class HltDecReports;
}

/** @class HltCompositionMonitor HltCompositionMonitor.h
 *  Algorithm to monitor the composition of the Hlt output offline.
 *  The most important property is Regexes. It is a list of regexes and for 
 *  every regex, a composition histogram will be filled. Every bin in the 
 *  histogram will represent a decision which matches the regex.
 *  The default is "Hlt2Express.*Decision", which monitors the Hlt2Express
 *  line composition.
 *
 *  @author Roel Aaij
 *  @date   2010-08-24
 */
class HltCompositionMonitor : public HltMonitorBase {
public: 
   /// Standard constructor
   HltCompositionMonitor( const std::string& name, ISvcLocator* pSvcLocator );

   ~HltCompositionMonitor( ) override = default; ///< Destructor

   StatusCode initialize() override;    ///< Algorithm initialization
   StatusCode execute   () override;    ///< Algorithm execution
   StatusCode finalize  () override;    ///< Algorithm finalization

private:

   std::vector<std::string> m_decReportsLocations;
   std::map< std::string, unsigned int > m_accepts;
   
   void init( const LHCb::HltDecReports* decReports );

};
#endif // HLTCOMPOSITIONMONITOR_H
