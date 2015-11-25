#ifndef STADCTRACKMONITOR_H
#define STADCTRACKMONITOR_H 1
 
// Include files

#include <string>
#include <map>


// from Gaudi
#include "TrackMonitorTupleBase.h"

// from Kernel
#include "Kernel/ISTReadoutTool.h"
#include "Kernel/STToolBase.h"

// STTELL1Event
#include "Event/STTELL1Data.h"

namespace LHCb {
  class Track ;
  class State ;
}
class DeVelo ;
class DeOTDetector ;
class IHitExpectation;
class IVeloExpectation;
class ITrackExtraplator;
class DeSTDetector;

/** @class STADCTrackMonitor STADCTrackMonitor.h "TrackCheckers/TrackMonitor"
 * 
 * Class for monitoring of hits by extrapolated tracks with NZS data
 *  @author Ch. Elsasser
 *  @date   17-6-2013
 */                 
                                                           
class STADCTrackMonitor : public TrackMonitorTupleBase {
                                                                             
 public:
                                                                             
  /** Standard construtor */
  STADCTrackMonitor( const std::string& name, ISvcLocator* pSvcLocator );
                                                                             
  /** Destructor */
  virtual ~STADCTrackMonitor();

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

  int  GetADCValue(LHCb::STTELL1Data* board,
                   unsigned int stripID,
                   int nStripsBeetle,
                   int summedADCValue,
                   int nNeigh
                   );

  LHCb::StateVector* GetTTHit(const LHCb::Track* track);
  LHCb::StateVector* GetITHit(const LHCb::Track* track);


  const DeVelo*        m_veloDet ;
  const DeSTDetector*  m_STDet;

  std::string m_detType;
  std::string m_layer;
  std::string m_rawDataLocCent;
  std::string m_odinLoc;

  int m_maxNeighbours;


  
  
  
  ISTReadoutTool* m_readoutTool;

  IVeloExpectation* m_veloExpectation;
  IHitExpectation* m_ttExpectation;
  IHitExpectation* m_itExpectation;
  IHitExpectation* m_otExpectation;

  ITrackExtrapolator* m_extrapolator;
  
  
  

  
  typedef  std::map<std::string, unsigned int> MultiplicityMap ;
  MultiplicityMap m_multiplicityMap;

  std::map<std::string, unsigned int> m_layerMap;
  
  
};


#endif // STADCTRACKMONITOR_H
