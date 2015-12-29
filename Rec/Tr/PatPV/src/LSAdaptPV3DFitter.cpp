// Include files
// from Event
#include "Event/Track.h"
#include "Event/State.h"
#include "Event/RecVertex.h"
#include "TrackInterfaces/ITrackExtrapolator.h"
// Local
#include "LSAdaptPV3DFitter.h"
namespace {

Gaudi::XYZVector impactParameterVector(const Gaudi::XYZPoint& vertex,
                                       const Gaudi::XYZPoint& point,
                                       const Gaudi::XYZVector& direction) {
  auto udir     = direction.unit();
  auto distance = point - vertex;
  return udir.Cross(distance.Cross(udir));
}
}
DECLARE_TOOL_FACTORY(LSAdaptPV3DFitter)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
LSAdaptPV3DFitter::LSAdaptPV3DFitter(const std::string& type,
                                     const std::string& name,
                                     const IInterface* parent)
: base_class(type,name,parent),
  m_minTr(0),
  m_Iterations(0),
  m_maxDeltaZ(0.),
  m_minTrackWeight(0.),
  m_TrackErrorScaleFactor(0.),
  m_x0MS(0.),
  m_trackChi(3.),
  m_scatCons(0.),
  m_myZero(1E-12)
{
  declareInterface<IPVFitter>(this);
  // Minimum number of tracks in vertex
  declareProperty("MinTracks", m_minTr = 5);
  // Number of iterations
  declareProperty("Iterations", m_Iterations = 20);
  // Chi2 of completely wrong tracks
  declareProperty("maxChi2", m_maxChi2 = 400.0);
  // Fit convergence condition
  declareProperty("maxDeltaZ", m_maxDeltaZ = 0.0005 * Gaudi::Units::mm); //m_maxDeltaZ = 0.001 * Gaudi::Units::mm);
  // Minimum Tukey's weight to accept a track
  declareProperty("minTrackWeight", m_minTrackWeight = 0.00000001); //0.00001);
  declareProperty( "x0MS"            , m_x0MS          =  0.02          )
  ->declareUpdateHandler( [=](Property&) {
    double X0 = this->m_x0MS;
    this->m_scatCons = (13.6*std::sqrt(X0)*(1.+0.038*log(X0))) / ( 3.0 * Gaudi::Units::GeV );
    this->m_scatConsNoMom = (13.6*std::sqrt(X0)*(1.+0.038*log(X0)));
  });
  declareProperty("TrackErrorScaleFactor", m_TrackErrorScaleFactor = 1.0 );
  declareProperty("CalculateMultipleScattering", m_CalculateMultipleScattering = true );
  declareProperty("UseFittedTracks", m_UseFittedTracks = false );
  declareProperty("AddMultipleScattering", m_AddMultipleScattering = true );
  declareProperty("trackMaxChi2", m_trackMaxChi2 = 9.)
  ->declareUpdateHandler( [=](Property&) { this->m_trackChi = std::sqrt(m_trackMaxChi2); });
  // Max chi2 tracks to be removed from next PV search
  declareProperty("trackMaxChi2Remove", m_trackMaxChi2Remove = 25.);
  // Min number of iterations
  declareProperty("minIter", m_minIter = 5);
  declareProperty("zVtxShift", m_zVtxShift = 0.0 );

}

//=========================================================================
// Initialize
//=========================================================================
StatusCode LSAdaptPV3DFitter::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if(!sc) return sc;

  m_linExtrapolator = tool<ITrackExtrapolator>("TrackLinearExtrapolator",this);
  if(!m_linExtrapolator) {
    err() << "Unable to retrieve the TrackLinearExtrapolator" << endmsg;
    return  StatusCode::FAILURE;
  }
  // Full track extrapolator
  m_fullExtrapolator = tool<ITrackExtrapolator>("TrackMasterExtrapolator",this);
  if(!m_fullExtrapolator) {
    err() << "Unable to get TrackMasterExtrapolator" << endmsg;
    return  StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Least square adaptive fitting method
//=============================================================================
StatusCode LSAdaptPV3DFitter::fitVertex(const Gaudi::XYZPoint& seedPoint,
                                        const std::vector<const LHCb::Track*>& rTracks,
                                        LHCb::RecVertex& vtx,
                                        std::vector<const LHCb::Track*>& tracks2remove) const
{
  if(msgLevel(MSG::DEBUG)) debug() << "================Test==================" << endmsg;

  tracks2remove.clear();

  Gaudi::XYZPoint xyzvtx = seedPoint;
  // prepare tracks
  PVTracks pvTracks;
  pvTracks.reserve(rTracks.size());
  for(const auto& track : rTracks ) {
    if ( !(track->hasVelo()) ) continue;
    addTrackForPV(track, pvTracks, seedPoint);
  }

  if((int) pvTracks.size() < m_minTr) {
    if(msgLevel(MSG::DEBUG)) {
      verbose() << "Too few tracks to fit PV" << endmsg;
    }
    return StatusCode::FAILURE;
  }

  Gaudi::SymMatrix3x3 hess;
  ROOT::Math::SVector<double,3> d0vec;

  bool converged = false;
  int nbIter = 0;
  while( (nbIter < m_minIter) || (!converged && nbIter < m_Iterations) )
  {
    ++nbIter;

    std::fill( d0vec.begin(), d0vec.end(), 0. );
    std::fill( hess.begin(), hess.end(), 0. );

    int ntrin=0;
    for(auto& pvTrack : pvTracks ) {
      std::array<double,3> unitVect;
      pvTrack.unitVect.GetCoordinates( unitVect.data() );

      pvTrack.vd0 = impactParameterVector(xyzvtx, pvTrack.stateG.position(), pvTrack.unitVect);
      pvTrack.d0  = std::sqrt(pvTrack.vd0.Mag2());

      if(UNLIKELY(pvTrack.err2d0<m_myZero))
      {
        if(msgLevel(MSG::DEBUG)) debug() << "fitVertex: pvTrack error is too small for computation" << endmsg;
        pvTrack.chi2=(pvTrack.d0*pvTrack.d0)/m_myZero;
      }
      else pvTrack.chi2 = (pvTrack.d0*pvTrack.d0)/ pvTrack.err2d0;

      if(msgLevel(MSG::DEBUG)) debug() << format( "itr %d new track position: chi2 d0 w zc %7.2f %7.2f %7.2f %7.2f",
                                                  nbIter, pvTrack.chi2, pvTrack.d0, pvTrack.err2d0, pvTrack.stateG.z()) << endmsg;


      pvTrack.weight = getTukeyWeight(pvTrack.chi2, nbIter);

      if (  pvTrack.weight > m_minTrackWeight ) {
        ntrin++;

        double invs = 1.0;
        if(pvTrack.err2d0<m_myZero)
        {
          invs = (2.0 / m_myZero) * pvTrack.weight;
        }
        else invs = (2.0 / pvTrack.err2d0) * pvTrack.weight;

        std::array<double,3> ipVec;
        pvTrack.vd0.GetCoordinates( ipVec.data() );
        for(int k = 0; k < 3; ++k) {
          for (int l = 0; l < 3; ++l) {
            double delta_kl = (k == l) ? 1.0 : 0.0;
            double val = (delta_kl - unitVect[k] * unitVect[l])*invs;
            d0vec[k] += ipVec[l] * val;
            if(l <= k) hess(k,l) += val;
          }
        }
      }
    }

    // check nr of tracks that entered the fit
    if(ntrin < 2) {
      return StatusCode::FAILURE;
    }

    if ( !hess.InvertChol() ) {
      if(msgLevel(MSG::DEBUG)) {
        debug() << "Error inverting hessian matrix" << endmsg;
        // typically 2 tracks (clones) left with identical slopes
        // dump all tracks used in hessian calculation
        int dntrin=0;
        for(const auto& fitTr : pvTracks ) {
          if (  fitTr.weight > m_minTrackWeight ) {
            ++dntrin;
            info() << format( "chi2 d0 w %12.6e %12.6e %12.6e xyz %12.6e %12.6e %12.6e tx ty %12.6e %12.6e",
                              fitTr.chi2, fitTr.d0, fitTr.weight,
                              fitTr.refTrack->firstState().x(),
                              fitTr.refTrack->firstState().y(),
                              fitTr.refTrack->firstState().z(),
                              fitTr.refTrack->firstState().tx(),
                              fitTr.refTrack->firstState().ty()) << endmsg;
          }
        }
        debug() << "nr of track in hessian failure: " << dntrin++ << endmsg;
      }
      return StatusCode::FAILURE;
    }
    ROOT::Math::SVector<double,3> delta = hess * d0vec;

    if(msgLevel(MSG::DEBUG)) {
      debug() <<  " delta " <<  delta[0] << " " <<  delta[1] << " " <<  delta[2]  << endmsg;
    }

    xyzvtx += Gaudi::XYZVector{ delta[0], delta[1], delta[2] };

    // loose convergence criteria if close to end of iterations
    auto maxdz = ( 1.*nbIter > 0.8*m_Iterations  ? 10.*m_maxDeltaZ : m_maxDeltaZ );
    if(fabs(delta[2]) < maxdz) converged = true;

  } // end iteration loop
  if(!converged) return StatusCode::FAILURE;
  int outTracks = 0;
  int nDoF = -3;
  double chi2 = 0.0;
  for (const auto& fitTr : pvTracks ) {
    if( fitTr.weight > m_minTrackWeight) {
      ++outTracks;
      chi2 += fitTr.chi2 * fitTr.weight ;
      nDoF += 2;
    }
  }

  if(outTracks < m_minTr) {
    if(msgLevel(MSG::DEBUG)) {
      debug() << "Too few tracks after PV fit" << endmsg;
    }
    return StatusCode::FAILURE;
  }

  // accepted PV
  vtx.setChi2(chi2);
  vtx.setNDoF(nDoF);


  xyzvtx.SetZ(xyzvtx.z() + m_zVtxShift);
  vtx.setPosition(xyzvtx);
  hess *= 2;
  vtx.setCovMatrix(hess);
  // Set tracks
  vtx.clearTracks();
  for(const auto& fitTr : pvTracks ) {
    if( fitTr.weight > m_minTrackWeight) {
      vtx.addToTracks( fitTr.refTrack, fitTr.weight );
    }
  }
  vtx.setTechnique(LHCb::RecVertex::Primary);

  // fill tracks to remove from next PV search
  for(const auto& fitTr : pvTracks ) {
    if( fitTr.chi2 < m_trackMaxChi2Remove) {
      tracks2remove.push_back( fitTr.refTrack);
    }
  }

  if(msgLevel(MSG::DEBUG)) {
    debug() << "Vertex" << endmsg;
    debug() << "===================" << endmsg;
    debug() << format( "chi2/ndof %7.2f  %7.2f / %5d xyz %7.3f %7.3f %7.3f  err xyz: %7.4f %7.4f %7.4f",
                       vtx.chi2()/vtx.nDoF(), vtx.chi2(), vtx.nDoF(),
                       vtx.position().X(), vtx.position().Y(), vtx.position().Z(),
                       std::sqrt(vtx.covMatrix()(0,0)), std::sqrt(vtx.covMatrix()(1,1)), std::sqrt(vtx.covMatrix()(2,2)) )
            << endmsg << endmsg;
    debug() << "Tracks in this vertex" << endmsg;
    debug() << "---------------------" << endmsg;
    for(const auto& fitTr : pvTracks) {
      int invx = 0;
      if( fitTr.weight > m_minTrackWeight) invx = 1;
      debug() << format( "chi2 d0 w zc %7.2f %7.2f %7.2f %7.2f %7.2f %3d",
                         fitTr.chi2, fitTr.d0, fitTr.weight, fitTr.refTrack->firstState().z(),fitTr.refTrack->pt(), invx) << endmsg;

    }
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
// Add track for PV
//=============================================================================
void LSAdaptPV3DFitter::addTrackForPV(const LHCb::Track* pvtr,
                                      PVTracks& pvTracks, const Gaudi::XYZPoint& seed) const
{
  // Add new PVTrack
  PVTrack pvtrack;
  pvtrack.isUsed = false;
  pvtrack.stateG = pvtr->firstState();
  pvtrack.unitVect = pvtrack.stateG.slopes().Unit();
  pvtrack.vd0 = impactParameterVector(seed,  pvtrack.stateG.position(),  pvtrack.unitVect);
  pvtrack.d0 = std::sqrt( pvtrack.vd0.Mag2() );

  if ( m_UseFittedTracks ) {
    Gaudi::XYZVector vd0Unit = pvtrack.vd0.unit();
    ROOT::Math::SVector<double,2> xyVec( vd0Unit.x(),vd0Unit.y() );
    pvtrack.err2d0 = Similarity(pvtrack.stateG.covariance().Sub<Gaudi::SymMatrix2x2>(0,0),  xyVec );
  } else {
    pvtrack.err2d0 = err2d0(pvtr, seed);
  }
  pvtrack.chi2 = 0;
  if(pvtrack.err2d0 > m_myZero) {
    pvtrack.chi2 = (pvtrack.d0*pvtrack.d0)/pvtrack.err2d0;
  } else {
    if (msgLevel(MSG::DEBUG)) debug() << "addTrackForPV: pvTrack error is too small for computation" << endmsg;
    pvtrack.chi2=pvtrack.d0/m_myZero;
  }
  // Keep reference to the original track
  pvtrack.refTrack = pvtr;
  if ( pvtrack.chi2 < m_maxChi2 ) pvTracks.push_back(pvtrack);
}

//=============================================================================
//  err2d0
//=============================================================================
double LSAdaptPV3DFitter::err2d0(const LHCb::Track* track, const Gaudi::XYZPoint& seed) const {

  // fast parametrization of track parameters
  double z     = track->firstState().z();
  double tx    = track->firstState().tx();
  double ty    = track->firstState().ty();

  double ex2 = track->firstState().errX2();
  double ey2 = track->firstState().errY2();

  double tr2   = tx*tx+ty*ty;
  double fcos2 = 1./(1.+tr2);
  double fsin2 = tr2/(1.+tr2);
  double err2 = (ex2+ey2)*fcos2;

  bool backward = track->checkFlag( LHCb::Track::Backward);

  double z_seed = seed.z();
  double dz_pv = z - z_seed;
  if ( backward ) dz_pv *= -1.;
  double et2 = 0.;
  if (dz_pv > 0. ) {
    double etx2 = track->firstState().errTx2();
    double ety2 = track->firstState().errTy2();
    et2 = dz_pv*dz_pv*(etx2+ety2);
    err2 += et2;
  }

  // add multiple scattering if track errors don't have it (HLT case)
  double corr2 = 0.;
  double l_scat2 = 0.;

  if( m_AddMultipleScattering ) {
    if ( m_CalculateMultipleScattering) {
      // mutliple scattering from RF foil at r = 10 mm

      double r_ms = 10.;
      l_scat2 = r_ms*r_ms/fsin2;
      corr2 = m_scatCons*m_scatCons*l_scat2;

      if(msgLevel(MSG::DEBUG)) {
        debug() << " Track printout "
                << track->type() << " " << track->firstState().qOverP() << " "
                << endmsg;
      }
      err2 += corr2;
    }
    else {
      err2 += 0.05*0.05;
    }


    if(msgLevel(MSG::DEBUG)) {
      debug() << " Track printout "
              << track->type() << " " << track->firstState().qOverP() << " " << corr2 << " " << err2 << " "
              << endmsg;

    }
  }
  return err2;
}


