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
class IHitExpectation;
class IVeloExpectation;

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

  const DeVelo*        m_veloDet ;
  double m_maxMomentum ;
  double m_maxChi2Dof ;
  bool m_useUT;
  
  IVeloExpectation* m_veloExpectation;
  IHitExpectation* m_ttExpectation;
  IHitExpectation* m_itExpectation;
  IHitExpectation* m_otExpectation;
  
  typedef  std::map<std::string, unsigned int> MultiplicityMap ;
  MultiplicityMap m_multiplicityMap;
};


#endif // TRACKMONITOR_H
