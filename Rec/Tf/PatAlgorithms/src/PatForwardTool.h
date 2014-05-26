// $Id: PatForwardTool.h,v 1.3 2008-07-17 13:16:49 smenzeme Exp $
#ifndef PATFORWARDTOOL_H
#define PATFORWARDTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/extends.h"

#include "TrackInterfaces/IPatForwardTool.h"
#include "Event/STCluster.h"
#include "TrackInterfaces/IAddTTClusterTool.h"
#include "TrackInterfaces/ITracksFromTrack.h"

#include "STDet/DeSTDetector.h"

#include "Event/Track.h"

#include "PatFwdTool.h"
#include "PatRange.h"
#include "PatFwdTrackCandidate.h"

#include "PatKernel/PatTStationHitManager.h"
#include "PatKernel/PatForwardHit.h"


/** @class PatForwardTool PatForwardTool.h
 *  Tool to extend one Velo (VeloTT) track through the magnet
 *
 *  @author Olivier Callot
 *  @date   2005-10-04 Initial version
 *  @date   2007-08-20 Update for A-Team framework
 */

class PatForwardTool : public extends2<GaudiTool,IPatForwardTool,ITracksFromTrack> {
public:


  /// Standard constructor
  PatForwardTool( const std::string& type,
                  const std::string& name,
                  const IInterface* parent);

  ~PatForwardTool( ) override = default; ///< Destructor

  void forwardTrack( const LHCb::Track* track, LHCb::Tracks* output ) override;

  StatusCode tracksFromTrack( const LHCb::Track& seed,
                              std::vector<LHCb::Track*>& tracks ) override;

  StatusCode initialize() override;

  // added for NNTools
  void setNNSwitch( bool nnSwitch)  { m_nnSwitch = nnSwitch;}
  bool nnSwitch()       const       { return m_nnSwitch;}

private:
  void buildXCandidatesList( PatFwdTrackCandidate& track );

  class XInterval {
      double m_xscale,m_offset,m_xmin,m_xmax;
  public:
      XInterval(double xScale, double xOffset, double xMin, double xMax) 
          : m_xscale{xScale}, m_offset{xOffset}, m_xmin{xMin}, m_xmax{xMax} {}
      double xKick(double z) const { return m_xscale*z-m_offset; }
      double xMin() const { return m_xmin; }
      double xMax() const { return m_xmax; }
  };

  double dSlope_kick( double sinTrack ) const {
      return sinTrack * m_magnetKickParams.first / 
           ( m_WrongSignPT - sinTrack*m_magnetKickParams.second );
  }

  XInterval make_XInterval(const PatFwdTrackCandidate& track) const {
      double xExtrap = track.xStraight( m_fwdTool->zReference() );
      //== calculate if minPt or minMomentum sets the window size
      double minMom = m_minPt / track.sinTrack();
      //== calculate center of magnet from Velo track
      const double zMagnet =  m_fwdTool->zMagnet( track );
      const double dSlope =  m_magnetKickParams.first / ( minMom - m_magnetKickParams.second ) ;
      double maxRange = dSlope*( m_fwdTool->zReference() - zMagnet);
      double xMin = xExtrap - maxRange;
      double xMax = xExtrap + maxRange;
      
      //== based on momentum a wrong-charge sign window size is defined
      if (m_useMomentumEstimate && !m_withoutBField && track.qOverP() != 0 ) {
        bool useKick { m_UseWrongSignWindow && track.track()->pt()>m_WrongSignPT };
        double kickRange = useKick ? dSlope_kick(track.sinTrack())*( m_fwdTool->zReference() - zMagnet)
                         : 0;
        if ( UNLIKELY( msgLevel( MSG::DEBUG ) ) ) {
          debug() << "   xExtrap = " << xExtrap
                  << " q/p " << track.qOverP()
                  << " predict " << xExtrap + kickRange << endmsg;
        }
        if ( std::signbit( track.qOverP() ) != std::signbit( m_fwdTool->magscalefactor() ) ) {
          xMin = xExtrap - kickRange;
        } else {
          xMax = xExtrap + kickRange;
        }
      }
      
      // compute parameters of deltaX as a function of z
      double deltaXScale  = maxRange  / ( m_fwdTool->zReference() - zMagnet );
      double deltaXOffset = deltaXScale*zMagnet;
      return { deltaXScale, deltaXOffset, xMin, xMax };
  };


  void fillXList( PatFwdTrackCandidate& track );

  bool fillStereoList( PatFwdTrackCandidate& track, double tol );

  void debugFwdHits( const PatFwdTrackCandidate& track ) const;

  void debugFwdHits( const PatFwdTrackCandidate& track, MsgStream& msg ) const;


  bool driftInRange( const PatFwdHit& hit )  const {  
      auto drift = hit.driftDistance(); 
     return m_minOTDrift < drift && drift < m_maxOTDrift ; 
  }

  bool driftOK(const PatFwdHit& hit) const {
    return (hit.hit()->type() != Tf::RegionID::OT) || driftInRange(hit);
  }

  double allowedXSpread(const PatFwdHit *hit, double xExtrap ) const { 
    double spreadSl = ( hit->projection() - xExtrap ) * m_maxSpreadSlopeX;
    double spread = m_maxSpreadX + fabs( spreadSl );
    if (hit->hit()->type() == Tf::RegionID::OT)  spread += 1.5;  // OT drift ambiguities...
    return spread;
  }

  double allowedStereoSpread(const PatFwdHit *hit) const { 
    double maxSpread = 3.;
    // in case of OT, add 1.5 to account for OT drift ambiguities...
    return  (hit->hit()->type() != Tf::RegionID::OT) ? maxSpread : ( maxSpread +1.5 ) ; 
  }

  bool inCenter(const PatFwdTrackCandidate& cand) const {
    return  m_centerOTYSize > fabs( cand.y( 0. ) );
  }

  double computeQOverP( const PatFwdTrackCandidate& c) const {
    double qOverP = 1000. * m_fwdTool->qOverP( c );  // in 1/GeV
    if (m_withoutBField) {
      if (m_minMomentum !=0)  qOverP = 1/m_minMomentum;
      else                    qOverP = 1;
    }
    return qOverP;
  }

  double computeQuality(const PatFwdTrackCandidate& c, double qOverP) const {
    double quality = 0.;
    quality  = 5. * fabs(  m_fwdTool->changeInY( c ) ) / ( m_maxDeltaY + qOverP * qOverP * m_maxDeltaYSlope );
    quality += c.chi2PerDoF() / 10.;
    quality += 10 * fabs(qOverP);  // low momentum are worse
    return quality;
  }

  double computeStereoTol( double qOverP) const {
     return m_maxSpreadY + m_maxSpreadSlopeY * qOverP *  qOverP;
  }

  bool hasEnoughStereo( const PatFwdTrackCandidate& c) {
    // Enough stereo planes
    PatFwdPlaneCounter fullCount( std::begin(c), std::end(c) );
    int nbY = fullCount.nbStereo();
    if ( nbY < 4 ) {
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
        debug() << "Not enough Y planes : " << nbY << endmsg;
      return false;
    }
    return true;
  }

  bool passMomentum(const PatFwdTrackCandidate& c, double sinTrack) const {
          const double momentum=1.0/fabs(m_fwdTool->qOverP( c ));
          const double pt = sinTrack*momentum;
          //== reject if below threshold
          return  m_withoutBField || ( momentum>m_minMomentum && pt>m_minPt) ;
  }



  PatFwdTool*                                 m_fwdTool;        ///< Tool to compute extrapolations of tracks
  Tf::TStationHitManager <PatForwardHit> *    m_tHitManager;    ///< Tool to provide hits
  IAddTTClusterTool*                          m_addTTClusterTool;
  std::string                                 m_addTtToolName;
  std::string                                 m_addUtToolName;

  //== Parameters of the algorithm
  bool   m_secondLoop;
  bool   m_useMomentumEstimate;
  double m_momentumEstimateError;
  double m_zAfterVelo;
  double m_yCompatibleTol;
  double m_yCompatibleTolFinal;
  double m_minOTDrift;
  double m_maxOTDrift;
  double m_maxSpreadX;
  double m_maxSpreadY;
  double m_maxSpreadSlopeX;
  double m_maxSpreadSlopeY;
  int    m_minXPlanes;
  int    m_minPlanes;
  double m_minPt;
  double m_minMomentum;
  double m_maxChi2;
  double m_maxChi2Track;
  int    m_minHits;
  int    m_minOTHits;
  double m_centerOTYSize;
  double m_maxDeltaY;
  double m_maxDeltaYSlope;
  int    m_maxXCandidateSize;

  std::pair<double,double>  m_magnetKickParams ;
  double m_minRange;

  // setting the cov matrix of the state
  double m_stateErrorX2;
  double m_stateErrorY2;
  double m_stateErrorTX2;
  double m_stateErrorTY2;
  double m_stateErrorP;

  PatFwdHits  m_xHitsAtReference;
  std::vector<PatFwdTrackCandidate> m_candidates;

  bool  m_withoutBField;
  bool  m_Preselection;
  float m_PreselectionPT;
  bool  m_UseWrongSignWindow;
  float m_WrongSignPT;
  bool  m_FlagUsedSeeds;              // flag velo seeds as used if a track is upgraded
  bool  m_nnSwitch;                   // switch on or off NN var. writing
};

#endif // PATFORWARDTOOL_H
