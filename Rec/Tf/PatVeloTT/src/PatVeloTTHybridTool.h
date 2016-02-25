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
  
  std::unique_ptr<LHCb::Track> getCandidate(const LHCb::Track& veloTrack) const;
  bool findHits() const;
  void clustering() const;
  void formClusters() const;
  std::unique_ptr<LHCb::Track> prepareOutputTrack(const LHCb::Track& veloTrack) const;
  void initEvent() const;

  template<std::size_t nHits>
  void simpleFit( const PatTTHits& theHits ) const;
  

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
  int   m_minHighThres;
  

  Tf::TTStationHitManager<PatTTHit> *      m_utHitManager;
  
  PatTTMagnetTool*    m_PatTTMagnetTool;  ///< Multipupose tool for Bdl and deflection
  bool m_printVariables;
  bool m_passTracks;
  
  mutable std::array<PatTTHits,8> m_hitsLayers;
  mutable std::array<PatTTHits,4> m_allHits;
  mutable std::array<float,4> m_normFact;
  mutable std::array<float,4> m_invNormFact;
  mutable bool m_newEvent;
  mutable PatTTHits m_clusterCandidate;
  mutable PatTTHits m_bestCandHits;
  mutable std::array<float,4> m_bestParams;
  mutable float m_xVelo;
  mutable float m_yVelo;
  mutable float m_zVelo;
  mutable float m_txVelo;
  mutable float m_tyVelo;
  float m_zKink;
  mutable float m_yAtMidTT;
  mutable float m_yAt0;
  mutable bool m_fourLayerSolution;
  mutable float m_minP;
  mutable float m_xMid;
  mutable float m_wb;
  mutable float m_invKinkVeloDist;

  float m_sigmaVeloSlope;
  float m_invSigmaVeloSlope;

};

//=========================================================================
// A kind of global track fit in VELO and TT
// The pseudo chi2 consists of two contributions:
//  - chi2 of Velo track x slope
//  - chi2 of a line in TT
// The two track segments go via the same (x,y) point
// at z corresponding to the half Bdl of the track
//
// Only q/p and chi2 of outTr are modified
//
//=========================================================================
template<std::size_t nHits>
void PatVeloTTHybridTool::simpleFit( const PatTTHits& theHits ) const{
  
  const float zDiff = 0.001*(m_zKink-m_zMidTT);
  float mat[3] = { m_wb, m_wb*zDiff, m_wb*zDiff*zDiff };
  float rhs[2] = { m_wb*m_xMid, m_wb*m_xMid*zDiff };
  
  // -- Scale the z-component, to not run into numerical problems
  // -- with floats
  int nHighThres = 0;
  for ( std::size_t i = 0; i < nHits; ++i){
    
    const PatTTHit* hit = theHits[i];
    
    if(  hit->hit()->sthit()->cluster().highThreshold() ) ++nHighThres;
    
    const float ui = hit->x();
    const float ci = hit->hit()->cosT();
    const float dz = 0.001*(hit->z() - m_zMidTT);
    const float wi = hit->hit()->weight();
    
    mat[0] += wi * ci;
    mat[1] += wi * ci * dz;
    mat[2] += wi * ci * dz * dz;
    rhs[0] += wi * ui;
    rhs[1] += wi * ui * dz;
  }
  
  // -- Veto hit combinations with no high threshold hit
  // -- = likely spillover
  if( nHighThres < m_minHighThres ) return;
  
  
  ROOT::Math::CholeskyDecomp<float, 2> decomp(mat);
  if( UNLIKELY(!decomp)) {
    return;
  } else {
    decomp.Solve(rhs);
  }
  
  const float xSlopeTTFit = 0.001*rhs[1];
  const float xTTFit = rhs[0];
  
  // new VELO slope x
  const float xb = xTTFit+xSlopeTTFit*(m_zKink-m_zMidTT);
  const float xSlopeVeloFit = (xb-m_xVelo)*m_invKinkVeloDist;
  const float chi2VeloSlope = (m_txVelo - xSlopeVeloFit)*m_invSigmaVeloSlope;
  
  float chi2TT = 0;
  for ( std::size_t i = 0; i < nHits; ++i){
    
    const PatTTHit* hit = theHits[i];
    
    const float zd    = hit->z();
    const float xd    = xTTFit + xSlopeTTFit*(zd-m_zMidTT);
    const float du    = xd - hit->x();
    
    chi2TT += (du*du)*hit->hit()->weight();
    
  }
  
  chi2TT += chi2VeloSlope*chi2VeloSlope;
  chi2TT /= (nHits + 1 - 2);
  
  if( chi2TT < m_bestParams[1] ){
    
    // calculate q/p
    const float sinInX  = xSlopeVeloFit*vdt::fast_isqrt(1.+xSlopeVeloFit*xSlopeVeloFit);
    const float sinOutX = xSlopeTTFit*vdt::fast_isqrt(1.+xSlopeTTFit*xSlopeTTFit);
    const float qp = (sinInX-sinOutX);
    
    m_bestParams = { qp, chi2TT, xTTFit,xSlopeTTFit };
    m_bestCandHits = theHits;
  }
  
}

#endif // PATVELOTTTOOL_H
