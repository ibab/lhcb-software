// $Id: PatVeloTTHybridTool.h,v 1.8 2010-01-09 12:46:27 witekma Exp $
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

static const InterfaceID IID_PatVeloTTHybridTool ( "PatVeloTTHybridTool", 1, 0 );

  /** @class PatVeloTTHybridTool PatVeloTTHybridTool.h
   *
   *  PatVeloTT tool
   *  
   *  @author Mariusz Witek
   *  @date   2007-05-08
   *  @update for A-Team framework 2007-08-20 SHM
   *
   */

class PatVeloTTHybridTool : public GaudiTool,  virtual public ITracksFromTrack,  public IIncidentListener {
public:
  
  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_PatVeloTTHybridTool; }

  /// Standard constructor
  PatVeloTTHybridTool( const std::string& type,
                const std::string& name,
                const IInterface* parent);
  
  virtual ~PatVeloTTHybridTool( ); ///< Destructor
  
  StatusCode initialize ( );
  
  virtual StatusCode tracksFromTrack(const LHCb::Track & velotrack, std::vector<LHCb::Track*>& outtracks );
  void handle ( const Incident& incident );
  void simpleFit( PatVTTHybridTrack& vtt);

protected:
  
  void getCandidates(const LHCb::Track& veloTrack,std::vector<LHCb::Track*>& outtracks );
  bool findHits();
  void clustering();
  void formClusters(bool forward);
  void prepareOutputTracks(std::vector<LHCb::Track*>& outtracks);
  void initEvent();
  bool acceptTrack(const LHCb::Track& track);

  
private:
  class compX  {
  public:
    bool operator() (PatTTHit* first, PatTTHit* second ) {
      return first->hit()->xAtYEq0() < second->hit()->xAtYEq0() ;
    }
  };

  class lowerBoundX  {
    public:
      bool operator() (const PatTTHit* first, const float value ) const {
        return first->x() < value ;
      }
    };
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
  
  Tf::TTStationHitManager<PatTTHit> *      m_utHitManager;
  
  PatTTMagnetTool*    m_PatTTMagnetTool;  ///< Multipupose tool for Bdl and deflection
  bool m_printVariables;
  
  std::vector<PatVTTHybridTrack> m_vuttracks;
  std::array<PatTTHits,8> m_hitsLayers;
  std::array<PatTTHits,4> m_allHits;
  std::vector<PatTTHits> m_allClusters;
  PatTTHits m_clusterCandidate;
  std::array<PatTTHits::iterator,8> m_iterators;
  std::vector<float> m_normFact;
  std::vector<float> m_invNormFact;
  bool m_newEvent;
  std::vector<PatVTTHybridTrack> m_bestCand;
  bool m_foundCand;
  float m_xVelo;
  float m_yVelo;
  float m_zVelo;
  float m_txVelo;
  float m_tyVelo;
  float m_zKink;
  float m_bdl;
  float m_yAtMidTT;
  float m_distToMomentum;
  float m_yAt0;
  bool m_fourLayerSolution;
  
  float m_c11,m_c12,m_c13,m_c21,m_c22,m_c23;
  bool m_passTracks;
  float m_passHoleSize;
  
};

#endif // PATVELOTTTOOL_H
