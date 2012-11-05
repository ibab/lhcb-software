#ifndef PRVL_FITLHCBIDS_H 
#define PRVL_FITLHCBIDS_H 1

// Gaudi
#include "GaudiAlg/GaudiTool.h"
// Tr/TrackInterfaces
#include "TrackInterfaces/ITrackFitter.h"
// Local
#include "PrVLHit.h"

/** @class PrVLFitLHCbIDs PrVLFitLHCbIDs.h
 *  Fit a track from LHCbIDs.
 *
 */

class DeVL;

class PrVLFitLHCbIDs : public GaudiTool, virtual public ITrackFitter {

public: 
  /// Standard constructor
  PrVLFitLHCbIDs(const std::string& type, 
                 const std::string& name,
                 const IInterface* parent);
  /// Destructor
  virtual ~PrVLFitLHCbIDs() {}

  virtual StatusCode initialize(); 

  /// Fit the track using the LHCbIDs on it.
  virtual StatusCode fit(LHCb::Track& track, 
                         LHCb::ParticleID pid = LHCb::ParticleID(211));

private:
  DeVL* m_det;
  std::vector<PrVLHit> m_pool;
  std::vector<PrVLHit>::iterator m_nextInPool;
  unsigned int m_nZonesR;
  unsigned int m_nZonesPhi;
  double m_msFactor;
  bool m_stateAtBeam;

};
#endif 
