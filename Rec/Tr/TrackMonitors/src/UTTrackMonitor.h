// $Id: UTTrackMonitor.h,v 1.3 2010-03-19 14:12:07 wouter Exp $
#ifndef UTTRACKMONITOR_H
#define UTTRACKMONITOR_H 1
 
// Include files
 
// from Gaudi
#include "TrackMonitorBase.h"

namespace LHCb {
  class Track ;
  class LHCbID ;
}

/** @class UTTrackMonitor UTTrackMonitor.h "TrackCheckers/UTTrackMonitor"
 * 
 * Class for UT track monitoring
 *  @author M. Needham.
 *  @date   16-1-2009
 */                 
                                                           
class UTTrackMonitor : public TrackMonitorBase {
                                                                             
 public:
                                                                             
  /** Standard construtor */
  UTTrackMonitor( const std::string& name, ISvcLocator* pSvcLocator );
                                                                             
  /** Destructor */
  virtual ~UTTrackMonitor();

  /** Algorithm initialize */
  virtual StatusCode initialize();

  /** Algorithm execute */
  virtual StatusCode execute();

 private:


  void fillHistograms(const LHCb::Track& track, 
                      const std::vector<LHCb::LHCbID>& itIDs,
		      const std::string& type) const ;

  unsigned int histoBin(const LHCb::STChannelID& chan) const;

  double m_refZ;
  double m_xMax;
  double m_yMax;

  unsigned int m_minNumUTHits;
  std::string m_clusterLocation;
  
};


#endif // UTTRACKMONITOR_H
