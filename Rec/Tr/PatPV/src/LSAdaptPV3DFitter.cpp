// $Id: LSAdaptPV3DFitter.cpp,v 1.7 2010-01-20 13:46:48 rlambert Exp $
// Include files
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
// from Event
#include "Event/Track.h"
#include "Event/State.h"
#include "Event/RecVertex.h"
// Local
#include "LSAdaptPV3DFitter.h"

DECLARE_TOOL_FACTORY(LSAdaptPV3DFitter);

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
LSAdaptPV3DFitter::LSAdaptPV3DFitter(const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent)
  : GaudiTool(type,name,parent),
    m_minTr(0),
    m_Iterations(0),
    m_maxIP2PV(0.),
    m_maxDeltaZ(0.),
    m_minTrackWeight(0.),
    m_TrackErrorScaleFactor(0.),
    m_x0MS(0.),
    m_scatCons(0.),
    m_pvTracks(0)
{
  declareInterface<IPVFitter>(this);
  // Minimum number of tracks in vertex
  declareProperty("MinTracks", m_minTr = 5);
  // Number of iterations
  declareProperty("Iterations", m_Iterations = 20);
  // Maximum IP of a track to accept track
  declareProperty("maxIP2PV", m_maxIP2PV = 5.0 * Gaudi::Units::mm);
  // Fit convergence condition
  declareProperty("maxDeltaZ", m_maxDeltaZ = 0.003 * Gaudi::Units::mm);
  // Minimum Tukey's weight to accept a track
  declareProperty("minTrackWeight", m_minTrackWeight = 0.00001);
  declareProperty( "x0MS"            , m_x0MS          =  0.02          );
  declareProperty("TrackErrorScaleFactor", m_TrackErrorScaleFactor = 1.0 );
}

//=========================================================================
// Initialize
//=========================================================================
StatusCode LSAdaptPV3DFitter::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if(!sc) return sc;

  double X0 = m_x0MS;
  m_scatCons = (13.6*sqrt(X0)*(1.+0.038*log(X0))) / ( 0.400 * Gaudi::Units::GeV );

  return StatusCode::SUCCESS;
}

//=============================================================================
// Destructor
//=============================================================================
LSAdaptPV3DFitter::~LSAdaptPV3DFitter() {}

//=============================================================================
// Least square adaptive fitting method
//=============================================================================
StatusCode LSAdaptPV3DFitter::fitVertex(const Gaudi::XYZPoint seedPoint,
                                      std::vector<const LHCb::Track*>& rTracks,
                                      LHCb::RecVertex& vtx)
{
  if(msgLevel(MSG::DEBUG)) debug() << "================Test==================" << endmsg;
  
  Gaudi::XYZPoint xyzvtx = seedPoint;
  // prepare tracks
  m_pvTracks.clear();

  std::vector<const LHCb::Track*>::iterator itr;
  for(itr = rTracks.begin(); itr != rTracks.end(); itr++) {
    const LHCb::Track* track = *itr;
    if ( !(track->hasVelo()) ) continue;
    addTrackForPV(track, m_pvTracks, seedPoint);
  }

  if((int) m_pvTracks.size() < m_minTr) {
    if(msgLevel(MSG::DEBUG)) {
      verbose() << "Too few tracks to fit PV" << endmsg;
    }
    return StatusCode::FAILURE;
  }

  Gaudi::SymMatrix3x3 hess;
  ROOT::Math::SVector<double,3> d0vec;

  bool converged = false;
  int nbIter = 0;
  while( (nbIter < 2) || (!converged && nbIter < m_Iterations) )
  {
    ++nbIter;

    for(int i = 0; i < 3; i++) {
      d0vec[i] = 0.0;
      for(int j = 0; j < 3; j++) {
        hess(i,j) = 0.0;
      }
    }

    int ntrin=0;
    for(PVTracks::iterator pvTrack = m_pvTracks.begin(); pvTrack != m_pvTracks.end(); pvTrack++) {
      double unitVectStd[3];
      unitVectStd[0] = pvTrack->unitVect.x();
      unitVectStd[1] = pvTrack->unitVect.y();
      unitVectStd[2] = pvTrack->unitVect.z();
      Gaudi::XYZVector ipVector =
        impactParameterVector(xyzvtx, pvTrack->stateG.position(), pvTrack->unitVect);
      pvTrack->vd0 = ipVector;
      pvTrack->d0 = ipVector.Mag2();
      if(pvTrack->err2d0<myzero) 
      {
        if(msgLevel(MSG::DEBUG)) debug() << "fitVertex: pvTrack error is too small for computation" << endmsg;
        pvTrack->chi2=pvTrack->d0/myzero;
      }
      else pvTrack->chi2 = pvTrack->d0/ pvTrack->err2d0;

      pvTrack->weight = getTukeyWeight(pvTrack->chi2, nbIter);

      if (  pvTrack->weight > m_minTrackWeight ) ntrin++;

      double invs = 1.0;
      if(pvTrack->err2d0<myzero) 
      {
        invs = (2.0 / myzero) * pvTrack->weight;
      }
      else invs = (2.0 / pvTrack->err2d0) * pvTrack->weight;

      double vd0Std[3];
      vd0Std[0] = ipVector.x();
      vd0Std[1] = ipVector.y();
      vd0Std[2] = ipVector.z();
      for(int k = 0; k < 3; k++) {
        double vd0 = 0.;
        for (int l = 0; l < 3; l++) {
          double delta_kl = (k == l) ? 1.0 : 0.0;
          double val = delta_kl - unitVectStd[k] * unitVectStd[l];
          vd0 += vd0Std[l] * val * invs;
          if(l <= k) {
            hess(k,l) += val * invs;
          }
        }
        d0vec[k] += vd0;
      }

    }

    // check nr of tracks that entered the fit
    if(ntrin < 2) return StatusCode::FAILURE;
    int fail;
    hess.Inverse(fail);
    if (0 != fail) {
      warning() << "Error inverting hessian matrix" << endmsg;
      return StatusCode::FAILURE;
    } else {
      hess.InvertFast();
    }
    ROOT::Math::SVector<double,3> delta;
    delta = hess * d0vec;

    if(msgLevel(MSG::DEBUG)) {
      debug() <<  " delta " <<  delta[0] << " " <<  delta[1] << " " <<  delta[2]  << endmsg;
    }

    xyzvtx.SetX(xyzvtx.X()+delta[0]);
    xyzvtx.SetY(xyzvtx.Y()+delta[1]);
    xyzvtx.SetZ(xyzvtx.Z()+delta[2]);

    if(fabs(delta[2]) < m_maxDeltaZ) {
      converged = true;
    }
  } // end iteration loop
  if(!converged) return StatusCode::FAILURE;
  // Check number of tracks and calculate chi2
  int outTracks = 0;
  int nDoF = -3;
  double chi2 = 0.0;
  for(PVTracks::iterator iFitTr = m_pvTracks.begin(); iFitTr != m_pvTracks.end(); iFitTr++) {
    if( iFitTr->weight > m_minTrackWeight) {
      outTracks++;
      chi2 += iFitTr->chi2;
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

  vtx.setPosition(xyzvtx);
  hess = hess * 2.0;
  vtx.setCovMatrix(hess);
  // Set tracks
  vtx.clearTracks();
  for(PVTracks::iterator iFitTr = m_pvTracks.begin(); iFitTr != m_pvTracks.end(); iFitTr++) {
    if( iFitTr->weight > m_minTrackWeight) {
      vtx.addToTracks( iFitTr->refTrack);
    }
  }
  vtx.setTechnique(LHCb::RecVertex::Primary);

  if(msgLevel(MSG::DEBUG)) {
    debug() << "Vertex" << endmsg;
    debug() << "===================" << endmsg;
    debug() << format( "chi2/ndof %7.2f  %7.2f %5d %7.3f %7.3f %7.3f", vtx.chi2()/vtx.nDoF(), vtx.chi2(), vtx.nDoF(),
            vtx.position().X(), vtx.position().Y(), vtx.position().Z())
            << endmsg << endmsg;
    debug() << "Tracks in this vertex" << endmsg;
    debug() << "---------------------" << endmsg;
    for(PVTracks::iterator iFitTr = m_pvTracks.begin(); iFitTr != m_pvTracks.end(); iFitTr++) {
      int invx = 0;
      if( iFitTr->weight > m_minTrackWeight) invx = 1;
        debug() << format( "chi2 d0 w zc %7.2f %7.2f %7.2f %7.2f %3d",
                   iFitTr->chi2, iFitTr->d0, iFitTr->weight, iFitTr->refTrack->firstState().z(),invx) << endmsg;

    }
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
// Add track for PV
//=============================================================================
void LSAdaptPV3DFitter::addTrackForPV(const LHCb::Track* pvtr,
                                      PVTracks& pvTracks, Gaudi::XYZPoint seed)
{
  // Add new PVTrack
  PVTrack pvtrack;
  pvtrack.isUsed = false;
  pvtrack.stateG = pvtr->firstState();
  pvtrack.unitVect = pvtrack.stateG.slopes().Unit();
  pvtrack.d0 = ( (impactParameterVector(seed,  pvtrack.stateG.position(),  pvtrack.unitVect)).Mag2() );
  if(pvtrack.d0 > m_maxIP2PV*m_maxIP2PV) return;
  pvtrack.err2d0 = err2d0(pvtr);
  pvtrack.chi2 = 0;
  //does it make sense to add such an error... why is chi2 0 for this case? it should be a very big number
  if(pvtrack.err2d0 > myzero) pvtrack.chi2 = (pvtrack.d0)/pvtrack.err2d0;
  else
  {
    if(msgLevel(MSG::DEBUG)) debug() << "addTrackForPV: pvTrack error is too small for computation" << endmsg;
    pvtrack.chi2=pvtrack.d0/myzero;
  }
  // Keep reference to the original track
  pvtrack.refTrack = pvtr;
  pvTracks.push_back(pvtrack);
  return;
}

//=============================================================================
//  err2d0
//=============================================================================
double LSAdaptPV3DFitter::err2d0(const LHCb::Track* track) {

  double x     = track->firstState().x();
  double y     = track->firstState().y();
  double r     = sqrt(x*x + y*y);
  double corr2 = (m_scatCons*r)*(m_scatCons*r);

  double ex2 = track->firstState().errX2() + corr2;
  double ey2 = track->firstState().errY2();

  return (ex2+ey2);
}


Gaudi::XYZVector LSAdaptPV3DFitter::impactParameterVector(const Gaudi::XYZPoint& vertex,
                                        const Gaudi::XYZPoint& point,
                                        const Gaudi::XYZVector& direction) {
  Gaudi::XYZVector udir = direction.unit();
  Gaudi::XYZVector distance = point - vertex;
  return udir.Cross(distance.Cross(udir));
}

//=============================================================================
// Get Tukey's weight
//=============================================================================
double LSAdaptPV3DFitter::getTukeyWeight(double trchi2, int iter)
{
  double ctrv = 9. - 3. * iter;
  if (ctrv < 3.) ctrv = 3.;
  double cT2 = trchi2 / (ctrv*ctrv*m_TrackErrorScaleFactor*m_TrackErrorScaleFactor);
  double weight = 0.;
  if(cT2 < 1.) {
    weight = 1. - cT2;
    weight = weight * weight;
  }
  return weight;
}
