// $Id: ITTrackMonitor.h,v 1.2 2008-12-10 12:42:31 cattanem Exp $
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
  double m_refZ;
  double m_xMax;
  double m_yMax;

  unsigned int m_minNumITHits;
  bool m_splitByITType; 
  
};

inline bool ITTrackMonitor::splitByITType() const {
  return m_splitByITType;;
}



#endif // TRACKMONITOR_H
