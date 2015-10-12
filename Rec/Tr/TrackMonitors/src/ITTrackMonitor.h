// $Id: ITTrackMonitor.h,v 1.4 2009-07-20 11:17:31 mneedham Exp $
#ifndef ITTRACKMONITOR_H
#define ITTRACKMONITOR_H 1
 
// Include files
 
// from Gaudi
#include "TrackMonitorBase.h"

namespace LHCb {
  class Track ;
  class LHCbID ;
}

/** @class ITTrackMonitor ITTrackMonitor.h "TrackCheckers/ITTrackMonitor"
 * 
 * Class for track monitoring
 *  @author M. Needham.
 *  @date   6-5-2007
 */                 
                                                           
class ITTrackMonitor : public TrackMonitorBase {
                                                                             
 public:
                                                                             
  /** Standard construtor */
  ITTrackMonitor( const std::string& name, ISvcLocator* pSvcLocator );
                                                                             
  /** Destructor */
  virtual ~ITTrackMonitor();

  /** Algorithm initialize */
  virtual StatusCode initialize();

  /** Algorithm execute */
  virtual StatusCode execute();

 private:

  bool splitByITType() const;

  std::string ITCategory(const std::vector<LHCb::LHCbID>& ids) const;

  void fillHistograms(const LHCb::Track& track, 
                      const std::string& type,
                      const std::vector<LHCb::LHCbID>& itIDs) const ;

  unsigned int histoBin(const LHCb::STChannelID& chan) const;

  double ProjectedAngle() const;
  
  double m_refZ;
  double m_xMax;
  double m_yMax;

  unsigned int m_minNumITHits;
  bool m_splitByITType; 
  bool m_plotsByLayer;  
  bool m_2DSummaryHist;  
  bool m_ProfileSummaryHist;  

  std::string m_clusterLocation;

  bool m_plotsBySector;//< individual plots by sector
  bool m_hitsOnTrack;//< plot only hits on tracks

};

inline bool ITTrackMonitor::splitByITType() const {
  return m_splitByITType;;
}



#endif // TRACKMONITOR_H
