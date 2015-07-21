#include "FastVeloTrack.h"
#include "FastVeloKalmanTrack.h"
#include "Event/Track.h"
#include "Event/State.h"
#include "Event/StateParameters.h"

FastVeloKalmanTrack::FastVeloKalmanTrack(const FastVeloTrack& track)
  :
  m_track(&track),
  m_hits(track.rHits().size()+track.phiHits().size())
{
  // makre sure hits are sorted in the positive z-direction. I was
  // using that r and phi hits were individually sorted, but that's
  // only true for the 'fitlhcbids': in fastvelotracking they can be
  // sorted in different directions. since the list is short anyway,
  // I'll just sort it ...

  const FastVeloHits& rhits = track.rHits() ;
  const FastVeloHits& phihits = track.phiHits() ;
  std::copy(rhits.begin(),rhits.end(),m_hits.begin()) ;
  std::copy(phihits.begin(),phihits.end(), m_hits.begin()+rhits.size()) ;
  std::sort(m_hits.begin(),m_hits.end(),FastVeloHit::IncreasingByZ()) ;
  // // make a single list of hits. r and phi hits are already sorted, so use std::merge.
  // if( m_hitsDirectionZ > 0 ) 
  //   std::merge(phihits.begin(),phihits.end(),rhits.begin(),rhits.end(),m_hits.begin(),FastVeloHit::IncreasingByZ()) ;
  // else 
  //   std::merge(phihits.begin(),phihits.end(),rhits.begin(),rhits.end(),m_hits.begin(),FastVeloHit::DecreasingByZ()) ;
}


//===============================================================================
// Fit the track with a Kalman filter, allowing for some scattering at
// every hit. Function arguments:
//  state: state at the last filtered hit.
//  direction=+1 : filter in positive z direction (not normally what you want)
//  direction=-1 : filter in negative z direction
//  noise2PerLayer: scattering contribution (squared) to tx and ty
// ===============================================================================
  
void FastVeloKalmanTrack::fit(LHCb::State& state,
			      double& chi2,
			      int& ndof,
			      const int direction,
                              const float noise2PerLayer) const
{
  // FIXME: can we think of a solution  such that we don't need to copy the list?
  std::vector<const FastVeloHit*> hits(m_hits) ;

  // initialize the parameters and the covariance with the current state of the track
  // (this is also state used for computing residual derivatives)
  auto& pars = state.stateVector() ;
  auto& cov  = state.covariance() ;
  const double firstz = hits.front()->z() ;
  pars(2) = m_track->tx() ;
  pars(3) = m_track->ty() ;
  pars(0) = m_track->x0() + pars(2) * firstz ;
  pars(1) = m_track->y0() + pars(3) * firstz ;
  state.setZ( firstz ) ;
  
  // initialize the covariance matrix. leave the qOverP element alone.
  for(int irow=0; irow<4; ++irow) {
    for(int icol=0; icol<=irow; ++icol )
      cov(irow,icol) = 0 ;
    cov(irow,4) = 0 ;
  }
  cov(0,0) = cov(1,1) = 1  ; //mm^2
  cov(2,2) = cov(3,3) = 0.01  ; //rad^2
  cov(4,4) = 0.0001  ; // reflects the default behaviour of the default track fitter
  
  // reset the chi2
  const int nhits = hits.size() ;
  chi2 = 0 ;
  ndof = nhits - 4 ;
  
  // use a constant direction for computing scattering
  const double reftx = pars(2) ;
  const double refty = pars(3) ;
  
  // start filtering
  // don't know how else to iterate in both directions ...
  const int beginhit = Forward==direction? 0     : nhits-1 ;
  const int endhit   = Forward==direction? nhits : -1 ;
  const int delta    = Forward==direction? +1    : -1 ;
  for( auto ihit = beginhit ; ihit != endhit; ihit += delta ) {
    const FastVeloHit* hit = m_hits[ihit] ;
    // we can compute residuals ourselves, using trajectory from
    // sensor, but that all makes it a bit slow. in the end, it is
    // just easier to reuse Olivier's linearization in the xy
    // plane. It isn't quite right, but it all seems to make little
    // difference.

    // propagate the current state to the z position of the hit
    const double zhit = hit->z() ;
    state.linearTransportTo( zhit ) ;

    // derivatives of residual, in the xy plane only.
    const double Hx = hit->a() ;
    const double Hy = hit->b() ;

    // predicted residual
    const double r = Hx*state.x() + Hy*state.y() + hit->c() ;

    // variance in residual.
    // for some reason Olivier has multiplied pitch by pseudosize. but that cannot be right for 1 or 2 strip clusters.
    const double f = 1.0/hit->cluster().pseudoSize() ;
    //const double V = n2/hit->weight() *f*f; // sigma(hit)^2 x correction factor for track angle
    const double V = f*f/hit->weight() ;
    const double R = V + Hx*Hx*cov(0,0) + 2*Hx*Hy*cov(1,0) + Hy*Hy*cov(1,1) ;

    // chi2 contribution
    chi2 += r*r/R ;
    
    // update the state vector and cov matrix
    // K = C * H^T * R^-1
    double CHT[4] ;
    CHT[0] = ( cov(0,0) * Hx + cov(0,1) * Hy ) ;
    CHT[1] = ( cov(1,0) * Hx + cov(1,1) * Hy ) ;
    CHT[2] = ( cov(2,0) * Hx + cov(2,1) * Hy ) ;
    CHT[3] = ( cov(3,0) * Hx + cov(3,1) * Hy ) ;
    for(int i=0; i<4; ++i) {
      pars(i) -= CHT[i] * r / R ;
    }

    for(int i=0; i<4; ++i) {
      for(int j=0; j<=i; ++j)
        cov(i,j) += -CHT[i]*CHT[j]/R ;
    }
    
    // finally, add the noise, using the direction of the reference.
    cov(2,2) += noise2PerLayer * (1 + reftx*reftx) ;
    cov(3,3) += noise2PerLayer * (1 + refty*refty) ;
    cov(3,2) += noise2PerLayer * reftx*refty ;
  }
}

namespace {
  void addOrReplace( LHCb::Track& track, const LHCb::State& state )
  {
    LHCb::State* stateOnTrack = track.stateAt( state.location() ) ;
    if( stateOnTrack ) {
      *stateOnTrack = state ;
    } else {
      track.addToStates( state ) ;
    }
  }
}


void FastVeloKalmanTrack::addStates(LHCb::Track& track,
				    const LHCb::State& basestate,
				    double transverseMomentumForScattering,
            std::vector<double> scatteringNoiseParameters,
				    bool addStateBeamLine,
				    bool addStateLastMeasurement,
				    bool addStateEndVelo) const
{
  bool backward       = track.checkFlag( LHCb::Track::Backward) ;

  // compute the scattering contributions:
  // Without momentum, this worked quite well:
  //const double scat2perplane = 3.0e-9 + 1.4e-6 * t2 ;
  //const double scat2rffoil   = 1.4e-7 ;
  // This is what I extracted from MC
  //   const double scat2perplane = 1.21 * ( 1. + 17. * t2 ) / scatmom2 ;
  //   const double scat2rffoil   = 2.35 * ( 1. + 0.011 / t2 ) / scatmom2 ;
  // That gives pulls that are far too narrow. This may be because
  // of measurement errors, but because I don't want to change
  // those, I adjusted the scattering until things looked reasonable:
  
  const double tx = m_track->tx() ;
  const double ty = m_track->ty() ;
  const double t2 = tx * tx + ty * ty ;

  double scatmom2 = 0 ;
  if( transverseMomentumForScattering > 0) {
    scatmom2 = transverseMomentumForScattering*transverseMomentumForScattering * (1+t2)/t2 ;
  } else {
    scatmom2 = basestate.p() * basestate.p() ;
  }

  if (scatteringNoiseParameters.empty()){
    // Use defaults
    scatteringNoiseParameters.push_back(0.5);
    scatteringNoiseParameters.push_back(0.002);
    scatteringNoiseParameters.push_back(2.0);
    scatteringNoiseParameters.push_back(0.004);
  }else if (scatteringNoiseParameters.size()!=4){
    // Make it fail...
    return;
  }
  
  
  const double scat2PerPlane = scatteringNoiseParameters[0] * (1.0 + scatteringNoiseParameters[1]/t2) / scatmom2 ;
  const double scat2RFFoil   = scatteringNoiseParameters[2] * (1.0 + scatteringNoiseParameters[3]/t2) / scatmom2 ;
  
  // Call the fit
  double chi2 ;
  int ndof ;
  LHCb::State state = basestate ;
  int direction = track.checkFlag(LHCb::Track::Backward) ? +1 : -1 ;
  fit( state,chi2,ndof,direction,scat2PerPlane ) ;
  track.setChi2AndDoF( chi2, ndof ) ;
  state.setLocation( LHCb::State::FirstMeasurement );
  addOrReplace(track,state );
  if( addStateBeamLine ) {
    // add the noise
    state.covariance()(2,2) += scat2RFFoil * (1 + state.tx()*state.tx() ) ;
    state.covariance()(3,3) += scat2RFFoil * (1 + state.ty()*state.ty() ) ;
    state.covariance()(3,2) += scat2RFFoil * state.tx() * state.ty() ;
    state.linearTransportTo( m_track->zBeam() ) ;
    state.setLocation( LHCb::State::ClosestToBeam );
    addOrReplace(track,state );
  }
  addStateEndVelo = addStateEndVelo &&!backward ;
  if( addStateLastMeasurement || addStateEndVelo ) {
    fit( state,chi2,ndof,-direction,scat2PerPlane ) ;
    if( addStateLastMeasurement) {
      state.setLocation( LHCb::State::LastMeasurement );
      addOrReplace(track,state) ;
    }
    if( addStateEndVelo) {
      state.linearTransportTo(StateParameters::ZEndVelo) ;
      state.setLocation( LHCb::State::EndVelo );
      addOrReplace(track,state );
    }
  }

}
