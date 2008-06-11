// $Id: LSAdaptPV3DFitter.cpp,v 1.1 2008-06-11 19:28:24 witekma Exp $
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
  : GaudiTool(type,name,parent) {
  declareInterface<IPVFitter>(this);
  // Minimum number of tracks in vertex  
  declareProperty("MinTracks", m_minTr = 5);
  // Number of iterations
  declareProperty("Iterations", m_Iterations = 20);
  // Maximum IP of a track to accept track
  declareProperty("maxIP2PV", m_maxIP2PV = 2.0 * Gaudi::Units::mm);
  // Fit convergence condition 
  declareProperty("maxDeltaZ", m_maxDeltaZ = 0.0005 * Gaudi::Units::mm);
  // Minimum Tukey's weight to accept a track
  declareProperty("minTrackWeight", m_minTrackWeight = 0.00001);

  declareProperty( "DetectorResolutionCoeff",      m_detectorResolutionCoeff =15e-3 );
  declareProperty( "MultipleScatteringCoeff",      m_multipleScatteringCoeff =0.0001 ) ;
}

//=========================================================================
// Initialize
//=========================================================================
StatusCode LSAdaptPV3DFitter::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if(!sc) return sc;
  // -> square as errors are quadratically summed
  m_detectorResolutionCoeff *= m_detectorResolutionCoeff;
  m_multipleScatteringCoeff *= m_multipleScatteringCoeff;

  //== Get Velo detector element, to get the R sector angle
  m_velo = getDet<DeVelo>( DeVeloLocation::Default );
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

  
  StatusCode sc = StatusCode::SUCCESS;

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
      pvTrack->d0 = sqrt(ipVector.Mag2());
      pvTrack->chi2 = (pvTrack->d0 * pvTrack->d0)/ pvTrack->err2d0;

      pvTrack->weight = getTukeyWeight(pvTrack->chi2, nbIter);
      
      if (  pvTrack->weight > m_minTrackWeight ) ntrin++;

      double invs = (2.0 / pvTrack->err2d0) * pvTrack->weight;

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
      hess.Invert();
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
  pvtrack.d0 = sqrt( (impactParameterVector(seed,  pvtrack.stateG.position(),  pvtrack.unitVect)).Mag2() );
  if(pvtrack.d0 > m_maxIP2PV) return;
  pvtrack.err2d0 = err2d0(pvtr);
  pvtrack.chi2 = 0;
  if(pvtrack.err2d0 > 1.e-12) pvtrack.chi2 = (pvtrack.d0*pvtrack.d0)/pvtrack.err2d0;
  // Keep reference to the original track
  pvtrack.refTrack = pvtr;
  pvTracks.push_back(pvtrack);
  return;
}

//=============================================================================
//  err2d0
//=============================================================================
double LSAdaptPV3DFitter::err2d0(const LHCb::Track* track) {

    double z = track->firstState().z();

    double dist, distVELO;
    int nbSta;
    computeErrorParameters( track, z, dist, nbSta, distVELO );

    double distRatio = dist / distVELO;

    double slX = track->firstState().tx();
    double slY = track->firstState().ty();

    double tanTheta = slX * slX + slY * slY;

    double cosTheta = 1 / sqrt( 1 + tanTheta );

    // Error coefficients tuned by hand
    double detectorResolution = m_detectorResolutionCoeff * 
      ( distRatio * distRatio + ( 1 + distRatio ) * ( 1 + distRatio ) );
      
    // The sinTheta dependence does not appear
    // => p used instead of pt
    // Reason: difficulty to get good vertex resolution with pt
    double multipleScattering = m_multipleScatteringCoeff * 
      nbSta / cosTheta * dist * dist / cosTheta / cosTheta;
      
    double ex2 = track->firstState().errX2() + detectorResolution + multipleScattering;
      
    double ey2 = track->firstState().errY2() + detectorResolution + multipleScattering;

    return (ex2+ey2);
}

//=============================================================================
// computeErrorParameters
//=============================================================================
void LSAdaptPV3DFitter::computeErrorParameters( const LHCb::Track* track, double z,
                                      double & minZDistToVELOCluster,
                                      int & nbStaBeforeLastVELOCluster,
                                      double & distFirstLastVELOCluster ) {

    double minzDist = 10000;

    int nbFirstSta = 84;
    int nbLastSta  = 0;

    double zMax = - 100000;
    double zMin = 100000;

    std::vector<LHCb::LHCbID>::const_iterator it;


    for ( it = track->lhcbIDs().begin();
          track->lhcbIDs().end() != it; it++ ) {
      if ( ! it->isVelo() ) continue;
      LHCb::VeloChannelID id      = (*it).veloID();
      int           sensor  = id.sensor();
      double        zSensor = m_velo -> sensor(id)->z();
      double        dist    = fabs( zSensor - z );

      if( dist < minzDist )
        minzDist = dist;

      if ( sensor < nbFirstSta )
        nbFirstSta = sensor;
      else if ( sensor > nbLastSta )
        nbLastSta = sensor;

      if ( zSensor > zMax )
        zMax = zSensor;
      else if ( zSensor < zMin )
        zMin = zSensor;
    }

    minZDistToVELOCluster      = minzDist;
    nbStaBeforeLastVELOCluster = int(float(nbLastSta-nbFirstSta)/4.);
    distFirstLastVELOCluster   = zMax - zMin;
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
  double cT2 = trchi2 / (ctrv*ctrv);
  double weight = 0.;
  if(cT2 < 1.) {
    weight = 1. - cT2;
    weight = weight * weight;
  }
  return weight;
}
