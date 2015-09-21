// $Id: PatForwardTool.h,v 1.3 2008-07-17 13:16:49 smenzeme Exp $
#ifndef PATFORWARDTOOL_H
#define PATFORWARDTOOL_H 1

// Include files
#include "boost/range/iterator_range.hpp"
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiAlg/GaudiTupleTool.h"
#include "GaudiKernel/extends.h"
#include "GaudiKernel/IIncidentListener.h"

#include "TrackInterfaces/IPatForwardTool.h"
#include "Event/STCluster.h"
#include "TrackInterfaces/IAddTTClusterTool.h"
#include "TrackInterfaces/ITracksFromTrack.h"
#include "TrackInterfaces/ITrackSelector.h"

#include "Kernel/IUsedLHCbID.h"

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

enum class ImHere{BeforeXFit,AfterXFit,AfterStereoFit};

struct TupInfo{std::string treename; ImHere WhereAmI;};

class IClassifierReader;

class PatForwardTool : public extends2<GaudiTupleTool,IPatForwardTool,ITracksFromTrack>, public IIncidentListener {
public:

  /// Standard constructor
  PatForwardTool( const std::string& type,
                  const std::string& name,
                  const IInterface* parent);

  ~PatForwardTool( ) override = default; ///< Destructor

  StatusCode initialize() override;
  StatusCode finalize() override;

  void forwardTrack( const LHCb::Track* track, LHCb::Tracks* output ) override;

  StatusCode tracksFromTrack( const LHCb::Track& seed,
                              std::vector<LHCb::Track*>& tracks ) override;
  
  bool acceptTrack(const LHCb::Track& track) const;
  
  // added for NNTools
  void setNNSwitch( bool nnSwitch) override { m_nnSwitch = nnSwitch;}
  bool nnSwitch()       const       { return m_nnSwitch;}
  
  void handle ( const Incident& incident );
  void prepareHits();

private:
  void buildXCandidatesList( PatFwdTrackCandidate& track ,
                             boost::iterator_range<typename PatFwdHits::const_iterator> full_XHit_range) const;
  StatusCode make_tuple(const std::vector<PatFwdTrackCandidate> &cands, const TupInfo &tupleinfo)const;

  class XInterval {
    double m_zMagnet,m_xMagnet,m_txMin,m_txMax,m_xmin,m_xmax;
  public:
    XInterval(double zMagnet,double xMagnet, double txMin, double txMax, double xMinRef, double xMaxRef)
      : m_zMagnet{zMagnet}, m_xMagnet{xMagnet}, m_txMin{txMin}, m_txMax{txMax}, m_xmin {xMinRef},m_xmax{xMaxRef} {}
    double xMinAtZ(double z) const { return m_txMin*(z-m_zMagnet)+m_xMagnet; }
    double xMaxAtZ(double z) const { return m_txMax*(z-m_zMagnet)+m_xMagnet; }
    //== This is the range at the reference plane
    double xMin() const { return m_xmin; }
    double xMax() const { return m_xmax; }
    bool inside(double x) const { return m_xmin <= x && x < m_xmax; }
    bool outside(double x) const { return x < m_xmin || m_xmax <= x ; }
    template <typename Range, typename Projection>
    Range inside(const Range& r, Projection p) const {
      // TODO: linear search from the edges is probably faster given the typical input...
      auto f = std::partition_point( std::begin(r), std::end(r),
                                     [&]( const typename Range::value_type& i ) { return p(i)<m_xmin; } );
      auto l = std::partition_point( f, std::end(r),
                                     [&]( const typename Range::value_type& i ) { return p(i)<m_xmax; } );
      return { f, l };
    }
  };


  template <typename T> T dSlope_kick( T pt, T sinTrack ) const {
    return sinTrack * m_magnetKickParams.first / ( pt - sinTrack * m_magnetKickParams.second );
  }

  XInterval make_XInterval(const PatFwdTrackCandidate& track) const {
    double xExtrap = track.xStraight( m_fwdTool->zReference() );
    //== calculate center of magnet from Velo track
    const double zMagnet =  m_fwdTool->zMagnet( track );
    //== calculate if minPt or minMomentum sets the window size
    const double dSlope =  dSlope_kick( m_minPt, track.sinTrack() );
    const double dz     = m_fwdTool->zReference() - zMagnet;
    const double maxRange = dSlope*dz;
    double xMin = xExtrap - maxRange;
    double dSlopeMin = -dSlope;
    double dSlopeMax =  dSlope;
    double xMax = xExtrap + maxRange;

    //== based on momentum a wrong-charge sign window size is defined
    if (m_useMomentumEstimate && !m_withoutBField && track.qOverP() != 0 ) {
      bool useKick { m_UseWrongSignWindow && track.track()->pt()>m_WrongSignPT };
      double kickRange = useKick ? dSlope_kick(m_WrongSignPT, track.sinTrack())*dz : 0;
      if ( UNLIKELY( msgLevel( MSG::DEBUG ) ) ) {
        debug() << "   xExtrap = " << xExtrap
                << " q/p " << track.qOverP()
                << " predict " << xExtrap + kickRange << endmsg;
      }
      //== In the case of a given charge estimate, the search window is not symmetric
      //== around the velo track extrapolation.
      if ( std::signbit( track.qOverP() ) != std::signbit( m_fwdTool->magscalefactor() ) ) {
        xMin = xExtrap - kickRange;
        dSlopeMin = -kickRange/dz;
      } else {
        xMax = xExtrap + kickRange;
        dSlopeMax = kickRange/dz;
      }
    }
    // compute parameters of deltaX as a function of z
    return { zMagnet, track.xStraight( zMagnet ),
          track.slX()+dSlopeMin,
          track.slX()+dSlopeMax,
          xMin, xMax };
  };


  boost::iterator_range<typename PatFwdHits::const_iterator>
  fillXList( PatFwdTrackCandidate& track ) const;

  bool fillStereoList( PatFwdTrackCandidate& track, double tol ) const;

  void debugFwdHits( const PatFwdTrackCandidate& track ) const;

  void debugFwdHits( const PatFwdTrackCandidate& track, MsgStream& msg ) const;


  bool driftInRange( const PatFwdHit& hit )  const {
    auto drift = hit.driftDistance();
    return m_minOTDrift < drift && drift < m_maxOTDrift ;
  }

  double allowedXSpread(const PatFwdHit *hit, double xExtrap ) const {
    auto spreadSl = ( hit->projection() - xExtrap ) * m_maxSpreadSlopeX;
    return m_maxSpreadX + fabs( spreadSl ) + int(hit->isOT()) * 1.5;  // OT drift ambiguities...
  }

  double allowedStereoSpread(const PatFwdHit *hit) const {
    // in case of OT, add 1.5 to account for OT drift ambiguities...
    return  m_StereoSpread + int(hit->isOT()) * 1.5;
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
    double quality  = 5. * fabs(  m_fwdTool->changeInY( c ) ) / ( m_maxDeltaY + qOverP * qOverP * m_maxDeltaYSlope );
    quality += c.chi2PerDoF() / 10.;
    quality += 10 * fabs(qOverP);  // low momentum are worse
    return quality;
  }

  template <typename T> T computeStereoTol( T qOverP) const {
    return m_maxSpreadY + m_maxSpreadSlopeY * qOverP *  qOverP;
  }

  bool hasEnoughStereo( const PatFwdTrackCandidate& c) const {
    // Enough stereo planes
    PatFwdPlaneCounter fullCount( std::begin(c), std::end(c) );
    return fullCount.nbStereo() > 3 ;
  }

  bool passMomentum(const PatFwdTrackCandidate& c, double sinTrack) const {
    const double momentum=1.0/fabs(m_fwdTool->qOverP( c ));
    const double pt = sinTrack*momentum;
    //== reject if below threshold
    return  m_withoutBField || ( momentum>m_minMomentum && pt>m_minPt) ;
  }

  inline float nX ( const PatFwdHits &hits ) const {
    return std::accumulate(hits.begin(),hits.end(),0,[](float last_it, const PatForwardHit *hit){ return last_it + (float)hit->isX(); });
  }

  inline std::pair<double,double> get_mdx_spread ( const PatFwdHits &hits ) const {
    //not pretty, but it has to be done like this, since hits can be rearranged after the stereofit
    auto sum_delta_x = 0.f;    std::vector<double> xproj;
    for(auto hit : hits) if(hit->isX())xproj.push_back(hit->projection());
    std::sort(xproj.begin(),xproj.end());
    for(auto it = xproj.begin(); it != --xproj.end(); it++) sum_delta_x += *next(it) - *it;
    return {sum_delta_x/(nX(hits)-1), xproj.back() - xproj.front()};
  }

  inline double get_cluster_mean ( const PatFwdHits &hits ) const {
    auto sum_x = std::accumulate(hits.begin(),hits.end(),0.0,[](double last_it, const PatForwardHit *hit){ return last_it + hit->isX()*hit->projection(); });
    return sum_x/nX(hits);
  }

  inline double get_cluster_stddev ( const PatFwdHits &hits ) const {
    auto mean = get_cluster_mean(hits);
    auto sqs = std::accumulate(hits.begin(),hits.end(),0.0,[&mean](double last_it, const PatForwardHit *hit){ return last_it + hit->isX()*pow(hit->projection() - mean,2); });
    return sqrt(sqs/nX(hits));
  }

  PatFwdTool*                                 m_fwdTool;        ///< Tool to compute extrapolations of tracks
  Tf::TStationHitManager <PatForwardHit> *    m_tHitManager;    ///< Tool to provide hits
  IAddTTClusterTool*                          m_addTTClusterTool;
  std::string                                 m_addTtToolName;
  std::string                                 m_addUtToolName;

  std::string      m_trackSelectorName;
  ITrackSelector*      m_trackSelector;

  //== Parameters of the algorithm
  bool   m_secondLoop;
  bool   m_writeMCtuples;
  bool   m_useMomentumEstimate;
  double m_momentumEstimateError;
  double m_zAfterVelo;
  double m_YCompatibleTol;
  double m_YCompatibleTolFinal;
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
  double m_maxChi2X;
  double m_maxChi2Y;
  double m_maxXSlope;
  int    m_minHits;
  int    m_minOTHits;
  double m_centerOTYSize;
  double m_maxDeltaY;
  double m_maxDeltaYSlope;
  int    m_maxXCandidateSize;
  double m_StereoSpread;
  double m_fXPlanes;
  double m_fYCompatible;
  double m_YTolOffset;
  double m_MLPcut;
  double m_br1;
  double m_br2;

  std::pair<double,double>  m_magnetKickParams ;
  double m_minRange;

  // setting the cov matrix of the state
  double m_stateErrorX2;
  double m_stateErrorY2;
  double m_stateErrorTX2;
  double m_stateErrorTY2;
  double m_stateErrorP;

  bool m_newEvent;
  
  mutable PatFwdHits  m_xHitsAtReference; // workspace
  mutable std::vector<PatFwdTrackCandidate> m_candidates; // workspace

  bool  m_withoutBField;
  bool  m_Preselection;
  double m_PreselectionPT;
  bool  m_UseWrongSignWindow;
  double m_WrongSignPT;
  //bool  m_FlagUsedSeeds;              // flag velo seeds as used if a track is upgraded
  std::vector<std::string>      m_veloVetoTracksNames;
  bool  m_skipUsedSeeds;              // skip seeds which are flagged as "used"
  bool  m_skipUsedHits;              // skip hits which are flagged as "used"
  std::string                                 m_LHCbIDToolName;
  IUsedLHCbID*                                m_usedLHCbIDTool; ///< Tool to check if hits are already being used

  bool  m_nnSwitch = false;           // switch on or off NN var. writing

  IClassifierReader *m_reader;

};

#endif // PATFORWARDTOOL_H
