#ifndef PATVELOTTHYBRIDTOOL_H
#define PATVELOTTHYBRIDTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiAlg/ISequencerTimerTool.h"
#include "Kernel/ILHCbMagnetSvc.h"

#include "Event/Track.h"
#include "TfKernel/TTStationHitManager.h"

// local
#include "PatVTTHybridTrack.h"
#include "PatTTMagnetTool.h"
#include "PatKernel/PatTTHit.h"
#include "TrackInterfaces/ITracksFromTrack.h"

#include "vdt/sqrt.h"

  /** @class PatVeloTTHybridTool PatVeloTTHybridTool.h
   *
   *  PatVeloTT tool
   *  
   *  @author Mariusz Witek
   *  @date   2007-05-08
   *  @update for A-Team framework 2007-08-20 SHM
   *
   */

class PatVeloTTHybridTool : public extends<GaudiTool, ITracksFromTrack, IIncidentListener> {
public:
  /// Standard constructor
  PatVeloTTHybridTool( const std::string& type,
                const std::string& name,
                const IInterface* parent);
  
  ~PatVeloTTHybridTool( ) override; ///< Destructor
  StatusCode initialize ( ) override;
  
  StatusCode  tracksFromTrack(const LHCb::Track & velotrack, std::vector<LHCb::Track*>& out) const override;
  void handle ( const Incident& incident ) override;

private:
  void simpleFit( PatVTTHybridTrack& vtt) const;

  std::unique_ptr<LHCb::Track> getCandidate(const LHCb::Track& veloTrack) const;
  bool findHits() const;
  void clustering() const;
  void formClusters(bool forward) const;
  std::unique_ptr<LHCb::Track> prepareOutputTrack() const;
  void initEvent() const;

  
private:

  float m_minMomentum;
  float m_minPT;
  float m_maxPseudoChi2;
  float m_yTol;
  float m_yTolSlope;
  float m_hitTol1;
  float m_hitTol2;
  float m_deltaTx1;
  float m_deltaTx2;
  float m_maxXSlope;
  float m_maxYSlope;
  float m_centralHoleSize;
  float m_zMidTT;
  float m_intraLayerDist;
  float m_overlapTol;
  float m_distToMomentum;
  float m_passHoleSize;
  
  Tf::TTStationHitManager<PatTTHit> *      m_utHitManager;
  
  PatTTMagnetTool*    m_PatTTMagnetTool;  ///< Multipupose tool for Bdl and deflection
  bool m_printVariables;
  bool m_passTracks;
  
  mutable std::vector<PatVTTHybridTrack> m_vuttracks;
  mutable std::array<PatTTHits,8> m_hitsLayers;
  mutable std::array<PatTTHits,4> m_allHits;
  mutable std::vector<PatTTHits> m_allClusters;
  mutable PatTTHits m_clusterCandidate;
  mutable std::array<PatTTHits::const_iterator,8> m_iterators;
  mutable std::vector<float> m_normFact;
  mutable std::vector<float> m_invNormFact;
  mutable bool m_newEvent;
  mutable std::vector<PatVTTHybridTrack> m_bestCand;
  mutable bool m_foundCand;
  mutable float m_xVelo;
  mutable float m_yVelo;
  mutable float m_zVelo;
  mutable float m_txVelo;
  mutable float m_tyVelo;
  mutable float m_zKink;
  mutable float m_bdl;
  mutable float m_yAtMidTT;
  mutable float m_yAt0;
  mutable bool m_fourLayerSolution;
  mutable float m_c11,m_c12,m_c13,m_c21,m_c22,m_c23;
  
};

#endif // PATVELOTTTOOL_H
