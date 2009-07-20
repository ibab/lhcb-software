// $Id: TTTrackMonitor.h,v 1.2 2009-07-20 11:17:31 mneedham Exp $
#ifndef TTTRACKMONITOR_H
#define TTTRACKMONITOR_H 1
 
// Include files
 
// from Gaudi
#include "TrackMonitorBase.h"

namespace LHCb {
  class Track ;
  class LHCbID ;
}

/** @class TTTrackMonitor TTTrackMonitor.h "TrackCheckers/TTTrackMonitor"
 * 
 * Class for TT track monitoring
 *  @author M. Needham.
 *  @date   16-1-2009
 */                 
                                                           
class TTTrackMonitor : public TrackMonitorBase {
                                                                             
 public:
                                                                             
  /** Standard construtor */
  TTTrackMonitor( const std::string& name, ISvcLocator* pSvcLocator );
                                                                             
  /** Destructor */
  virtual ~TTTrackMonitor();

  /** Algorithm initialize */
  virtual StatusCode initialize();

  /** Algorithm execute */
  virtual StatusCode execute();

 private:


  void fillHistograms(const LHCb::Track& track, 
                      const std::vector<LHCb::LHCbID>& itIDs) const ;

  unsigned int histoBin(const LHCb::STChannelID& chan) const;

  double m_refZ;
  double m_xMax;
  double m_yMax;

  unsigned int m_minNumTTHits;
  std::string m_clusterLocation;
  
};


#endif // TRACKMONITOR_H
