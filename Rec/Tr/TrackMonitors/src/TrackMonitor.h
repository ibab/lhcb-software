// $Id: TrackMonitor.h,v 1.5 2009-08-11 14:06:59 smenzeme Exp $
#ifndef TRACKMONITOR_H
#define TRACKMONITOR_H 1
 
// Include files
 
// from Gaudi
#include "TrackMonitorBase.h"

namespace LHCb {
  class Track ;
  class State ;
}
class DeVelo ;
class DeOTDetector ;

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

  void fillHistograms(const LHCb::Track& track, 
                      const std::string& type) const ;

  void findRefStates(const LHCb::Track& track,
		     const LHCb::State*& firstMeasurementState,
		     const LHCb::State*& lastMeasurementState) const ;

  double m_xMax;
  double m_yMax;

  const DeVelo*        m_veloDet ;

};


#endif // TRACKMONITOR_H
