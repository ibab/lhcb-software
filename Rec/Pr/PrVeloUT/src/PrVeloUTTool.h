// $Id: PrVeloUTTool.h,v 1.8 2010-01-09 12:46:27 witekma Exp $
#ifndef PRVELOUTTOOL_H
#define PRVELOUTTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiAlg/ISequencerTimerTool.h"
#include "Kernel/ILHCbMagnetSvc.h"

#include "Event/Track.h"
#include "TfKernel/UTStationHitManager.h"

// local
#include "PrVUTTrack.h"
#include "PrUTMagnetTool.h"
#include "PrKernel/PrUTHit.h"
#include "IPrVeloUTTool.h"

#include "vdt/sqrt.h"

  /** @class PrVeloUTTool PrVeloUTTool.h
   *
   *  PrVeloUT tool
   *  
   *  @author Mariusz Witek
   *  @date   2007-05-08
   *  @update for A-Team framework 2007-08-20 SHM
   *
   */

class PrVeloUTTool :   public IPrVeloUTTool,  public IIncidentListener {
public:
  
  /// Standard constructor
  PrVeloUTTool( const std::string& type,
                const std::string& name,
                const IInterface* parent);
  
  virtual ~PrVeloUTTool( ); ///< Destructor
  
  StatusCode initialize ( );
  void recoVeloUT(LHCb::Track & velotrack, std::vector<LHCb::Track*>& outtracks );
  void handle ( const Incident& incident );
  
protected:
  
  void getCandidates(LHCb::Track& veloTrack,std::vector<LHCb::Track*>& outtracks );
  bool findHits();
  void clustering();
  void formClusters(bool forward);
  void simpleFit( PrVUTTrack& vtt);
  void prepareOutputTracks(std::vector<LHCb::Track*>& outtracks);
  void initEvent();
  bool acceptTrack(const LHCb::Track& track);

  
private:
  class compX  {
  public:
    bool operator() (PrUTHit* first, PrUTHit* second ) {
      return first->hit()->xAtYEq0() < second->hit()->xAtYEq0() ;
    }
  };

  class lowerBoundX  {
    public:
      bool operator() (const PrUTHit* first, const float value ) const {
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
  float m_zMidUT;
  float m_intraLayerDist;
  
  Tf::UTStationHitManager<PrUTHit> *      m_utHitManager;
  
  PrUTMagnetTool*    m_PrUTMagnetTool;  ///< Multipupose tool for Bdl and deflection
  bool m_PrintVariables;
  
  std::vector<PrVUTTrack> m_vuttracks;
  std::vector<PrUTHits> m_hitsLayers;
  std::vector<PrUTHits> m_allHits;
  std::vector<PrUTHits> m_allClusters;
  PrUTHits m_clusterCandidate;
  std::array<PrUTHits::iterator,8> m_iterators;
  std::vector<float> m_normFact;
  std::vector<float> m_invNormFact;
  bool m_newEvent;
  std::vector<PrVUTTrack> m_bestCand;
  bool m_foundCand;
  float m_xVelo;
  float m_yVelo;
  float m_zVelo;
  float m_txVelo;
  float m_tyVelo;
  float m_zKink;
  float m_bdl;
  float m_yAtMidUT;
  float m_distToMomentum;
  float m_yAt0;
  bool m_fourLayerSolution;
  
  float m_c11,m_c12,m_c13,m_c21,m_c22,m_c23;

  bool m_PassTracks;
  float m_passHoleSize;
  
};

#endif // PRVELOUTTOOL_H
