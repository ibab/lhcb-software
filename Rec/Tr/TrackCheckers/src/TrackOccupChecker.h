// $Id: TrackOccupChecker.h,v 1.1 2009-11-12 15:42:23 kholubye Exp $
#ifndef TRACKOCCUPCHECKER_H
#define TRACKOCCUPCHECKER_H 1
 
// Include files
 
// from Gaudi
#include "TrackCheckerBase.h"

#include "Event/Track.h"

// for occupancy studies
#include "TrackInterfaces/IPatSeedingTool.h"
#include "PatKernel/PatForwardHit.h"
#include "TfKernel/TStationHitManager.h"

//#include "TfKernel/VeloHitManager.h"
//#include "TfKernel/DefaultVeloHitManager.h"
#include "TfKernel/DefaultVeloRHitManager.h"
#include "TfKernel/DefaultVeloPhiHitManager.h"


#include "TfKernel/RegionID.h"
#include "TfKernel/VeloSensorHits.h"
#include "TfKernel/TfIDTypes.h"

// detector stuff
#include "STDet/DeSTDetector.h"
#include "OTDet/DeOTDetector.h"
#include "VeloDet/DeVelo.h"


class IHistoTool ;

namespace LHCb{
  class MCParticle;
  class State;
  class Measurement;
  class STCluster;
  class OTTime;
}

/** @class TrackOccupChecker TrackOccupChecker.h
 * 
 * Class for occupancy measurements
 *  @author K. Holubyev.
 *  @date   5-10-2009
 */                 
                                                           
class TrackOccupChecker : public TrackCheckerBase {
                                                                             
 public:
                                                                             
  /** Standard construtor */
  TrackOccupChecker( const std::string& name, ISvcLocator* pSvcLocator );
                                                                             
  /** Destructor */
  virtual ~TrackOccupChecker();

  /** Algorithm initialize */
  virtual StatusCode initialize();

  /** Algorithm execute */
  virtual StatusCode execute();

  /** Algorithm finalize */
  virtual StatusCode finalize();


  
 private:


  void occupInfo();


  typedef Tf::TStationHitManager<PatForwardHit>::HitRange HitRange;
  Tf::TStationHitManager <PatForwardHit> *  m_tHitManager;
  Tf::DefaultVeloRHitManager* m_rHitManager;
  Tf::DefaultVeloPhiHitManager* m_phiHitManager;
  
  static const unsigned int m_nSta = Tf::RegionID::OTIndex::kNStations;
  static const unsigned int m_nLay = Tf::RegionID::OTIndex::kNLayers;
  static const unsigned int m_nRegOT = Tf::RegionID::OTIndex::kNRegions;
  static const unsigned int m_nRegIT = Tf::RegionID::ITIndex::kNRegions;
  static const unsigned int m_nReg = m_nRegOT + m_nRegIT;

  // Detectors
  DeVelo *m_Velo;
  DeSTDetector *m_IT;
  DeOTDetector *m_OT;

  unsigned int m_ITChannels;
  unsigned int m_OTChannels;

  inline bool isRegionOT(const unsigned reg) const
  { return (reg == 0 || reg == 1); }

  /// true if region reg is IT region
  inline bool isRegionIT(const unsigned reg) const
  { return reg >= Tf::RegionID::OTIndex::kNRegions; }

  /// true if region reg is IT horizontal
  inline bool isRegionIThor(const unsigned reg) const
  { return (reg == 2 || reg == 3); }
  /// true if region reg is IT region
  inline bool isRegionITver(const unsigned reg) const
  { return (reg == 4 || reg == 5); }
  
 
};

#endif // TRACKOCCUPCHECKER_H
