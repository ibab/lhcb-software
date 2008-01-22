// $Id: TrackMonitor.h,v 1.3 2008-01-22 12:02:22 smenzeme Exp $
#ifndef TRACKMONITOR_H
#define TRACKMONITOR_H 1
 
// Include files
 
// from Gaudi
#include "TrackMonitorBase.h"
#include "Event/Track.h"

/** @class TrackMonitor TrackMonitor.h "TrackCheckers/TrackMonitor"
 * 
 * Class for track monitoring
 *  @author M. Needham.
 *  @date   6-5-2007
 */                 
                                                           
class TrackMonitor : public TrackMonitorBase {
                                                                             
 public:
                                                                             
  /** Standard construtor */
  TrackMonitor( const std::string& name, ISvcLocator* pSvcLocator );
                                                                             
  /** Destructor */
  virtual ~TrackMonitor();

  /** Algorithm initialize */
  virtual StatusCode initialize();

  /** Algorithm execute */
  virtual StatusCode execute();

 private:

  std::string algorithm(LHCb::Track::History history ) const;

  void fillHistograms(const LHCb::Track* aTrack, 
                      const std::string& type);

  void fillNtuple(const LHCb::Track* aTrack, 
		  const std::string& type);
  double m_refZ;
  double m_xMax;
  double m_yMax;
  bool m_splitByAlgorithm;
  

};


#endif // TRACKMONITOR_H
