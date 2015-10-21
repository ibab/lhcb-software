// Include files
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"

// Interfaces
#include "IPVFitter.h"
// Track info
#include "Event/Track.h"
#include "Event/State.h"
#include "Event/RecVertex.h"

class AdaptivePV3DFitter : public GaudiTool, virtual public IPVFitter {

public:
  // Standard constructor
  AdaptivePV3DFitter(const std::string& type,
                  const std::string& name,
                  const IInterface* parent);
  // Destructor
  virtual ~AdaptivePV3DFitter();
  // Initialize
  virtual StatusCode initialize() ;
  // Fitting
  StatusCode fitVertex(const Gaudi::XYZPoint seedPoint,
                       std::vector<const LHCb::Track*>& tracks,
                       LHCb::RecVertex& vtx, 
                       std::vector<const LHCb::Track*>& tracks2remove);
private:
  size_t m_minTr;         // Minimum number of tracks to make a vertex
  int    m_Iterations;    // Number of iterations for minimisation
  int    m_minIter;       // iterate at least m_minIter times
  double m_maxDeltaZ;     // Fit convergence condition
  double m_minTrackWeight;// Minimum Tukey's weight to accept a track
  double m_TrackErrorScaleFactor;
  double m_maxChi2 ;      // max chi2 of track-to-vtx to be considered for fit
  double m_trackMaxChi2;  // maximum chi2 track to accept track in PV fit
  double m_trackChi ;     // sqrt of trackMaxChi2
  double m_trackMaxChi2Remove; // Max chi2 tracks to be removed from next PV search
  double m_maxDeltaZCache; // Update cache if distance of reference is more than this
  
  // Get Tukey's weight
  double getTukeyWeight(double trchi2, int iter);
};

DECLARE_TOOL_FACTORY(AdaptivePV3DFitter)

namespace {
  class AdaptivePVTrack
  {
  public:
    AdaptivePVTrack(const LHCb::Track& track, const Gaudi::XYZPoint& vtx) ;
    void updateCache( const Gaudi::XYZPoint& vtx ) ;
    double weight() const { return m_weight ; }
    void setWeight(double w) { m_weight = w ;}
    const Gaudi::SymMatrix3x3&  halfD2Chi2DX2() const { return m_halfD2Chi2DX2 ; }
    const Gaudi::Vector3&  halfDChi2DX() const { return m_halfDChi2DX ; }
    const double& chi2() const { return m_chi2 ; }
    inline double chi2( const Gaudi::XYZPoint& vtx ) const ;
    const LHCb::Track* track() const { return m_track ; }
  private:
    double m_weight ;
    const LHCb::Track* m_track ;
    LHCb::State m_state ;
    Gaudi::SymMatrix2x2 m_invcov ;
    Gaudi::SymMatrix3x3 m_halfD2Chi2DX2 ;
    Gaudi::Vector3 m_halfDChi2DX ;
    double m_chi2 ;
    ROOT::Math::SMatrix<double,3,2> m_H;
  } ;

  AdaptivePVTrack::AdaptivePVTrack(const LHCb::Track& track, const Gaudi::XYZPoint& vtx)
    : m_track(&track)
  {
    // get the state
    m_state = track.firstState() ;
    if( m_state.location()!=LHCb::State::ClosestToBeam ) {
      const LHCb::State* closestToBeam = track.stateAt(LHCb::State::ClosestToBeam) ;
      if(closestToBeam) m_state = *closestToBeam ;
    }
    // do here things we could evaluate at z_seed. may add cov matrix here, which'd save a lot of time.
    m_H(0,0) = m_H(1,1) = 1 ;
    m_H(2,0) = - m_state.tx() ;
    m_H(2,1) = - m_state.ty() ;
    // update the cache
    updateCache( vtx ) ;
  }

  void AdaptivePVTrack::updateCache(const Gaudi::XYZPoint& vtx)
  {
    // transport to vtx z
    m_state.linearTransportTo( vtx.z() ) ;

    // invert cov matrix
    m_invcov = m_state.covariance().Sub<Gaudi::SymMatrix2x2>(0,0) ;
    m_invcov.InvertChol() ;

    // The following can all be written out, omitting the zeros, once
    // we know that it works.
    Gaudi::Vector2 res ;
    res(0) = vtx.x() - m_state.x() ;
    res(1) = vtx.y() - m_state.y() ;
    ROOT::Math::SMatrix<double,3,2> HW = m_H*m_invcov ;
    ROOT::Math::AssignSym::Evaluate(m_halfD2Chi2DX2, HW*ROOT::Math::Transpose(m_H) ) ;
    //m_halfD2Chi2DX2 = ROOT::Math::Similarity(H, invcov ) ;
    m_halfDChi2DX   = HW * res ;
    m_chi2          = ROOT::Math::Similarity(res,m_invcov) ; 
  }

  inline double AdaptivePVTrack::chi2( const Gaudi::XYZPoint& vtx ) const
  {
    Gaudi::Vector2 res ;
    double dz = vtx.z() - m_state.z() ;
    res(0) = vtx.x() - (m_state.x() + dz*m_state.tx()) ;
    res(1) = vtx.y() - (m_state.y() + dz*m_state.ty()) ;
    return ROOT::Math::Similarity(res,m_invcov) ;
  }
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
AdaptivePV3DFitter::AdaptivePV3DFitter(const std::string& type,
                                     const std::string& name,
                                     const IInterface* parent)
  : GaudiTool(type,name,parent)
{
  declareInterface<IPVFitter>(this);
  // Minimum number of tracks in vertex
  declareProperty("MinTracks", m_minTr = 4);
  // Number of iterations
  declareProperty("Iterations", m_Iterations = 20);
  // Chi2 of completely wrong tracks
  declareProperty("maxChi2", m_maxChi2 = 400.0);
  // Fit convergence condition
  declareProperty("maxDeltaZ", m_maxDeltaZ = 0.0005 * Gaudi::Units::mm); //m_maxDeltaZ = 0.001 * Gaudi::Units::mm);
  // Minimum Tukey's weight to accept a track
  declareProperty("minTrackWeight", m_minTrackWeight = 0.00000001); //0.00001);
  declareProperty("TrackErrorScaleFactor", m_TrackErrorScaleFactor = 1.0 );
  declareProperty("trackMaxChi2", m_trackMaxChi2 = 12.);
  // Max chi2 tracks to be removed from next PV search
  declareProperty("trackMaxChi2Remove", m_trackMaxChi2Remove = 25.);
  // Min number of iterations
  declareProperty("minIter", m_minIter = 5);
  // Minimal displacement in z to update derivatives
  declareProperty("maxDeltaZCache", m_maxDeltaZCache = 1 * Gaudi::Units::mm ) ;

}

//=============================================================================
// Destructor
//=============================================================================
AdaptivePV3DFitter::~AdaptivePV3DFitter() {}

//=========================================================================
// Initialize
//=========================================================================
StatusCode AdaptivePV3DFitter::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if(!sc) return sc;
  m_trackChi = std::sqrt(m_trackMaxChi2);
  return StatusCode::SUCCESS;
}
//=============================================================================
// Least square adaptive fitting method
//=============================================================================
StatusCode AdaptivePV3DFitter::fitVertex(const Gaudi::XYZPoint seedPoint,
				     std::vector<const LHCb::Track*>& rTracks,
				     LHCb::RecVertex& vtx,
				     std::vector<const LHCb::Track*>& tracks2remove)
{
  tracks2remove.clear();

  // position at which derivatives are evaluated
  Gaudi::XYZPoint refpos = seedPoint ;
  
  // prepare tracks
  std::vector<AdaptivePVTrack> pvTracks ;
  pvTracks.reserve( rTracks.size() ) ;
  for( const auto& track : rTracks )
    if( track->hasVelo() ) {
      AdaptivePVTrack trk(*track, refpos ) ;
      if( trk.chi2() <  m_maxChi2 )
	pvTracks.push_back( trk ) ;
    }

  if( pvTracks.size() < m_minTr ) {
    if(msgLevel(MSG::DEBUG)) {
      verbose() << "Too few tracks to fit PV" << endmsg;
    }
    return StatusCode::FAILURE;
  }
  
  // current vertex position
  Gaudi::XYZPoint vtxpos = refpos ;
  // vertex covariance matrix
  Gaudi::SymMatrix3x3 vtxcov ;
  bool converged = false;
  double maxdz = m_maxDeltaZ;
  int nbIter = 0;
  while( (nbIter < m_minIter) || (!converged && nbIter < m_Iterations) )
  {
    ++nbIter;

    Gaudi::SymMatrix3x3 halfD2Chi2DX2 ;
    Gaudi::Vector3 halfDChi2DX ;
    // update cache if too far from reference position. this is the slow part.
    if( std::abs(refpos.z() - vtxpos.z()) > m_maxDeltaZCache ) {
      refpos = vtxpos ;
      for( auto& trk : pvTracks ) trk.updateCache( refpos ) ;
    }

    // add contribution from all tracks
    double chi2(0) ;
    size_t ntrin(0) ;    
    for( auto& trk : pvTracks ) {
      // compute weight
      double trkchi2 = trk.chi2(vtxpos) ;
      double weight = getTukeyWeight(trkchi2, nbIter) ;
      trk.setWeight(weight) ;
      // add the track
      if ( weight > m_minTrackWeight ) {
        ++ntrin;
	halfD2Chi2DX2 += weight * trk.halfD2Chi2DX2() ;
	halfDChi2DX   += weight * trk.halfDChi2DX() ;
	chi2 += weight * trk.chi2() ;
      }
    }

    // check nr of tracks that entered the fit
    if(ntrin < m_minTr) {
      if(msgLevel(MSG::DEBUG))
	debug() << "Too few tracks after PV fit" << endmsg;
      return StatusCode::FAILURE;
    }
    
    // compute the new vertex covariance
    vtxcov = halfD2Chi2DX2 ;
    int OK = vtxcov.InvertChol() ;
    if (!OK) {
      if(msgLevel(MSG::DEBUG))
        debug() << "Error inverting hessian matrix" << endmsg;
      return StatusCode::FAILURE;
    }
    // compute the delta w.r.t. the reference
    Gaudi::Vector3 delta = -1.0 * vtxcov * halfDChi2DX ;
    
    // note: this is only correct if chi2 was chi2 of reference!
    chi2  += ROOT::Math::Dot(delta,halfDChi2DX) ;

    // deltaz needed for convergence
    const double deltaz = refpos.z() + delta(2) - vtxpos.z() ;
    
    // update the position
    vtxpos.SetX( refpos.x() + delta(0) ) ;
    vtxpos.SetY( refpos.y() + delta(1) ) ;
    vtxpos.SetZ( refpos.z() + delta(2) ) ;
    vtx.setChi2AndDoF( chi2, 2*ntrin-3 ) ;

    //std::cout << "Iter, chi2: " 
    //<< nbIter << " " << chi2 << " " << refpos.z() << " " << vtxpos.z() << std::endl ;

    // loose convergence criteria if close to end of iterations
    if ( 1.*nbIter > 0.8*m_Iterations ) {
      maxdz = 10.*m_maxDeltaZ;
    }
    converged = std::abs(deltaz) < maxdz ;
    
  } // end iteration loop
  if(!converged) return StatusCode::FAILURE;
  
  // set position and covariance
  vtx.setPosition( vtxpos ) ;
  vtx.setCovMatrix( vtxcov ) ;
  // Set tracks. Compute final chi2.
  vtx.clearTracks();
  for( const auto& trk : pvTracks ) {
    if( trk.weight() > m_minTrackWeight) 
      vtx.addToTracks( trk.track(), trk.weight() ) ;
    // remove track for next PV search
    if( trk.chi2(vtxpos) < m_trackMaxChi2Remove)
      tracks2remove.push_back( trk.track() );
  }
  vtx.setTechnique(LHCb::RecVertex::Primary);
  return StatusCode::SUCCESS;
}

//=============================================================================
// Get Tukey's weight
//=============================================================================
double AdaptivePV3DFitter::getTukeyWeight(double trchi2, int iter)
{
  if (iter<1 ) return 1.;

  double ctrv = m_trackChi * (m_minIter -  iter);
  if (ctrv < m_trackChi) ctrv = m_trackChi;
  double cT2 = trchi2 / (ctrv*ctrv*m_TrackErrorScaleFactor*m_TrackErrorScaleFactor);
  double weight = 0.;
  if(cT2 < 1.) {
    weight = 1. - cT2;
    weight = weight * weight;
  }
  return weight;
}

