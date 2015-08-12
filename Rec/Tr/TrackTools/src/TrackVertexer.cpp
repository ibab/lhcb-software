// Include files 

// local
#include "TrackVertexer.h"

// from Event
#include "Event/TwoProngVertex.h"
#include "Event/RecVertex.h"
#include "TrackKernel/TrackStateVertex.h"
#include "TrackKernel/TrackTraj.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TrackVertexer
//
// 2007-11-07 : Wouter HULSBERGEN
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TrackVertexer )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackVertexer::TrackVertexer( const std::string& type,
                              const std::string& name,
                              const IInterface* parent )
: GaudiTool ( type, name , parent ),
  m_stateprovider("TrackStateProvider",this),
  m_pocatool("TrajPoca"),
  m_maxNumIter(10),
  m_maxDChisq(0.01),
  m_computeMomCov(true),
  m_discardFailedFits(false)
{
  declareInterface<ITrackVertexer>(this);
  declareProperty("MaxNumberOfIterations",m_maxNumIter) ;
  declareProperty("MaxDeltaChisqForConvergence",m_maxDChisq) ;
  declareProperty("ComputeMomentumCovariance",m_computeMomCov) ;
  declareProperty("DiscardFailedFits",m_discardFailedFits) ;
  declareProperty("StateProvider",m_stateprovider) ;
}
//=============================================================================
// Destructor
//=============================================================================
TrackVertexer::~TrackVertexer() {} 

StatusCode TrackVertexer::initialize()
{
  StatusCode sc = GaudiTool::initialize() ;
  if(sc.isSuccess() ) {
    sc = m_stateprovider.retrieve() ;
  }
  if(sc.isSuccess() ) {
    sc = m_pocatool.retrieve() ;
  }
  return sc ;
}

StatusCode TrackVertexer::finalize()
{
  m_stateprovider.release().ignore() ;
  m_pocatool.release().ignore() ;
  return GaudiTool::finalize();
}

LHCb::TwoProngVertex*
TrackVertexer::fit(const LHCb::State& stateA, const LHCb::State& stateB) const
{
  LHCb::TwoProngVertex* rc(0) ;
  StateContainer states(2) ;
  states.front() = &stateA ;
  states.back() = &stateB ;
  LHCb::TrackStateVertex vertex( states, m_maxDChisq, m_maxNumIter ) ;
  if( vertex.fitStatus() == LHCb::TrackStateVertex::FitSuccess || !m_discardFailedFits) {
    // create a two prong vertex
    rc = new LHCb::TwoProngVertex(vertex.position()) ;
    rc->setChi2(vertex.chi2()) ;
    rc->setCovMatrix( vertex.covMatrix() ) ;
    rc->setNDoF(vertex.nDoF()) ;
    rc->momA() = vertex.mom(0) ;
    rc->momB() = vertex.mom(1) ;
    if( m_computeMomCov ) {
      rc->momcovA()    = vertex.momCovMatrix(0) ;
      rc->momposcovA() = vertex.momPosCovMatrix(0) ;
      rc->momcovB()    = vertex.momCovMatrix(1) ;
      rc->momposcovB() = vertex.momPosCovMatrix(1) ;
      rc->mommomcov()  = vertex.momMomCovMatrix(0,1) ;
    }
  }
  return rc ;
}

LHCb::RecVertex*
TrackVertexer::fit(const StateContainer& tracks) const
{
  LHCb::RecVertex* recvertex(0) ;
  
  if( tracks.size()>=2 ) {
    LHCb::TrackStateVertex vertex( tracks, m_maxDChisq, m_maxNumIter ) ;
    if( vertex.fitStatus() == LHCb::TrackStateVertex::FitSuccess || !m_discardFailedFits) {
      recvertex = new LHCb::RecVertex(vertex.position()) ;
      recvertex->setChi2AndDoF( vertex.chi2(), vertex.nDoF() ) ;
      recvertex->setCovMatrix( vertex.covMatrix() ) ;
    }
  }
  return recvertex ;
}

LHCb::RecVertex*
TrackVertexer::fit(const TrackContainer& tracks) const
{
  // return if insufficient tracks
  if( tracks.size() < 2 ) return 0 ;

  // get the track slopes to choose the seed tracks. get also all trajectories.
  size_t N(tracks.size()) ;
  std::vector<const LHCb::TrackTraj*> trajectories(N,0) ;
  std::vector<Gaudi::XYZVector> directions(N) ;
  size_t i(0) ;
  for( TrackContainer::const_iterator itrack = tracks.begin() ;
       itrack != tracks.end(); ++itrack,++i ) {
    trajectories[i] = m_stateprovider->trajectory( **itrack ) ;
    directions[i]   = (*itrack)->firstState().slopes().Unit() ;
  }

  // seed: take pair with the largest "opening angle"
  // (or do we want invariant mass?)
  double mincostheta(1) ;
  size_t trk1(0),trk2(0) ;
  for( size_t itrk=0; itrk<N; ++itrk) {
    for( size_t jtrk=0; jtrk<itrk; ++jtrk) {
      double costheta = directions[itrk].Dot( directions[jtrk] ) ;
      if( costheta <mincostheta) {
	trk1 = itrk ;
	trk2 = jtrk ;
	mincostheta = costheta ;  
      }
    }
  }
  
  // use the poca tool to get the poca of the seed tracks
  Gaudi::XYZVector deltaX; 
  double z1(0),z2(0),z(0) ;
  StatusCode sc = m_pocatool->minimize( *(trajectories[trk1]),z1,
					*(trajectories[trk2]),z2,
					deltaX, 0.001*Gaudi::Units::mm );
  if( sc.isSuccess() ) z = 0.5*(z1+z2) ;
  
  // collect the states at the poca
  StateContainer states(N) ;
  states.reserve( N ) ;
  for( size_t itrk=0; itrk<N; ++itrk)
    states[itrk] = new LHCb::State(trajectories[itrk]->state( z  ) );
  
  // fit the states
  LHCb::RecVertex* recvertex = fit( states ) ;
  
  // add the tracks
  if( recvertex ) 
    for( TrackContainer::const_iterator itrack = tracks.begin() ;
	 itrack != tracks.end(); ++itrack ) 
      recvertex->addToTracks( *itrack ) ;
  
  // clean up
  for( size_t itrk=0; itrk<N; ++itrk) {
    delete states[itrk] ;
    //delete trajectories[itrk] ;
  }
  
  return recvertex ;
}

namespace {
  inline Gaudi::Vector3 transform( const Gaudi::XYZVector& vec)
  {
    return Gaudi::Vector3(vec.X(),vec.Y(),vec.Z()) ;
  }
}

bool
TrackVertexer::computeDecayLength(const LHCb::TwoProngVertex& vertex,
				  const LHCb::RecVertex& pv,
				  double& chi2,
				  double& decaylength,
				  double& decaylengtherr) const
{
  // This calculation is basically a 1-iteration beamspot fit. The
  // constraint is 
  //
  //    r = x - lambda p/|p| - xbs
  // 
  // where x and p are the position of the decay vertex of the
  // candidate and its momentum, lambda is the decaylength and xbs
  // the position of the beamspot. The covariance in the constraint
  // is
  //
  //    V = Vbs + Vxx - a * Vxp - a Vxp^T + a^2 * Vpp
  //
  // where a=lambda/|p|^2. It needs an initial estimate for the
  // flightlength, for which we simply take the projection of deltaX
  // on the direction. We now minimize  the chisquare contribution 
  // 
  //     chi^2 = r^T V^{-1} r
  //
  // for lambda.


  const Gaudi::XYZPoint pos = vertex.position() ;
  // I really only need p3, not p4
  Gaudi::SymMatrix6x6 cov6  = vertex.covMatrix6x6() ;
  Gaudi::XYZVector p3 = vertex.p3() ;
  Gaudi::Vector3 dx = transform(pos - pv.position()) ;
  double p3mag = p3.R() ; 
  Gaudi::Vector3 dir = transform(p3.Unit()) ;
  Gaudi::SymMatrix3x3 W = pv.covMatrix() ;
    
   //double a = (ROOT::Math::Transpose(dir)*dx)/p3mag  ;
  double a = ROOT::Math::Dot(dir,dx)/p3mag  ;
  for(size_t row=0; row<3; ++row)
    for(size_t col=0; col<=row; ++col)
      W(row,col) += 
	cov6(row,col)
	+ a*a * cov6(row+3,col+3)
	- a * (cov6(row+3,col) + cov6(col+3,row)) ;
  
  int OK = W.InvertChol() ;
  if( OK==0 ) info() << "inversion error in fitDecayLength" << endmsg ;
  
  double halfdChi2dLam2 = ROOT::Math::Similarity(W,dir) ;
  decaylength    = ROOT::Math::Dot(dir,W*dx)/halfdChi2dLam2 ;
  decaylengtherr = std::sqrt(1/halfdChi2dLam2) ;
  
  Gaudi::Vector3 res = dx - decaylength * dir ;
  
  chi2 = ROOT::Math::Similarity(W,res) ;

  return (OK!=0) ;
}

double
TrackVertexer::ipchi2( const LHCb::Track& track, const LHCb::RecVertex& pv) const
{  
  const LHCb::TrackTraj* traj = m_stateprovider->trajectory( track ) ;
  return traj ? 
    ipchi2( traj->state(pv.position().z()),pv ) : 
    ipchi2( track.firstState(),pv) ;
}

double
TrackVertexer::ipchi2( const LHCb::State& state, const LHCb::RecVertex& pv) const
{  
  double tx = state.tx() ;
  double ty = state.ty() ;
  double dz = pv.position().z() - state.z() ;
  double dx = state.x() + dz * tx - pv.position().x() ;
  double dy = state.y() + dz * ty - pv.position().y() ;
  
  const Gaudi::SymMatrix3x3& pvcov = pv.covMatrix() ;
  const Gaudi::SymMatrix5x5& trkcov = state.covariance() ;
  
  // compute the covariance matrix. first only the trivial parts:
  double cov00 = pvcov(0,0) + trkcov(0,0) ;
  double cov10 = pvcov(1,0) + trkcov(1,0) ;
  double cov11 = pvcov(1,1) + trkcov(1,1) ;
  
  // add the contribution from the extrapolation
  cov00 += dz*dz*trkcov(2,2) + 2*dz*trkcov(2,0) ;
  cov10 += dz*dz*trkcov(3,2) + dz*(trkcov(3,0)+trkcov(2,1)) ;
  cov11 += dz*dz*trkcov(3,3) + 2*dz*trkcov(3,1) ;
  
  // add the contribution from pv Z
  cov00 += tx*tx*pvcov(2,2)  -  2*tx*pvcov(2,0) ;
  cov10 += tx*ty*pvcov(2,2)  -  ty*pvcov(2,0) - tx*pvcov(2,1) ;
  cov11 += ty*ty*pvcov(2,2)  -  2*ty*pvcov(2,1) ;
  
  // invert the covariance matrix
  double D = cov00*cov11 - cov10*cov10 ;
  double invcov00 = cov11 / D ;
  double invcov10 = -cov10 / D ;
  double invcov11 = cov00 / D ;
  
  return dx*dx * invcov00 + 2*dx*dy * invcov10 + dy*dy * invcov11 ;
}
