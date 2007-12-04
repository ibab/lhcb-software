// $Id: LSAdaptPVFitter.cpp,v 1.3 2007-12-04 21:48:36 witekma Exp $
// Include files 
// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
// from Event
#include "Event/Track.h"
#include "Event/State.h"
#include "Event/RecVertex.h"
#include "TrackInterfaces/ITrackExtrapolator.h"
// Local
#include "LSAdaptPVFitter.h"

DECLARE_TOOL_FACTORY(LSAdaptPVFitter);

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
LSAdaptPVFitter::LSAdaptPVFitter(const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent)
  : GaudiTool(type,name,parent) {
  declareInterface<IPVFitter>(this);
  // Minimum number of tracks in vertex  
  declareProperty("MinTracks", m_minTr = 5);
  // Number of iterations
  declareProperty("Iterations", m_Iterations = 50);
  // Chi2 of completely wrong tracks
  declareProperty("maxChi2", m_maxChi2 = 256.0);
  // Limit in R for linear extrapolation
  declareProperty("LinExtrapolation", m_extrapRCut = 5.0 * Gaudi::Units::mm);
  // Fit convergence condition 
  declareProperty("maxDeltaZ", m_maxDeltaZ = 0.0005 * Gaudi::Units::mm);
  // Value of the Tukey's weight to accept a track
  declareProperty("acceptTrack", m_acceptTrack = 0.00001);
}

//=========================================================================
// Initialize
//=========================================================================
StatusCode LSAdaptPVFitter::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if(!sc) return sc;
  // Linear track extrapolator
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
// Destructor
//=============================================================================
LSAdaptPVFitter::~LSAdaptPVFitter() {}

//=============================================================================
// Least square adaptive fitting method
//=============================================================================
StatusCode LSAdaptPVFitter::fitVertex(const Gaudi::XYZPoint seedPoint, 
                                      std::vector<const LHCb::Track*>& rTracks, 
                                      LHCb::RecVertex& vtx)
{
  StatusCode sc = StatusCode::SUCCESS;
  m_pvTracks.clear();
  PVVertex pvVertex;
  std::vector<const LHCb::Track*>::iterator itr;
  for(itr = rTracks.begin(); itr != rTracks.end(); itr++) {    
    const LHCb::Track* track = *itr;
    if ( !(track->hasVelo()) ) continue;
    sc = addTrackForPV(track, m_pvTracks, seedPoint.z());
    if(!sc.isSuccess()) continue;
  }
  initVertex(m_pvTracks,pvVertex,seedPoint);
  // Initial track cleaning
  for(PVTrackPtrs::iterator itrack = pvVertex.pvTracks.begin(); 
      itrack != pvVertex.pvTracks.end();) {
    PVTrack* pvTrack = *itrack;
    StatusCode sc = trackExtrapolate(pvTrack,pvVertex.primVtx);
    if((!sc.isSuccess()) || (pvTrack->chi2 >= m_maxChi2)) {
      itrack = pvVertex.pvTracks.erase(itrack);
    } else {
      itrack++;
    }
  }
  // Check the number of tracks for PV candidate
  if((int)pvVertex.pvTracks.size() < m_minTr) {
    if(msgLevel(MSG::DEBUG)) {
      verbose() << "Too few tracks to fit PV" << endmsg;
    }
    return StatusCode::FAILURE;
  }
  StatusCode scvfit = fit(pvVertex.primVtx,pvVertex.pvTracks);
  if(scvfit == StatusCode::SUCCESS) {
    vtx = pvVertex.primVtx;
    vtx.setTechnique(LHCb::RecVertex::Primary);
  }
  return scvfit;
}

//=============================================================================
// Least square adaptive PV fit
//=============================================================================
StatusCode LSAdaptPVFitter::fit(LHCb::RecVertex& vtx, 
                                std::vector<PVTrack*>& pvTracks) 
{
  if(msgLevel(MSG::DEBUG)) {
    verbose() << "Least square adaptive PV fit" << endmsg;
  }
  // Reset hessian and d0 vector
  Gaudi::SymMatrix3x3 hess;
  ROOT::Math::SVector<double,3> d0vec;
  for(int i = 0; i < 3; i++) {
    d0vec[i] = 0.0;
    for(int j = 0; j < 3; j++) {
      hess(i,j) = 0.0;
    }
  }
  double zPrevious = 99999.0;
  double zVtx = 0.0;
  bool converged = false;
  int nbIter = 0;
  // Iteration loop. Require at least 3 iterations to reach final weight.
  while( (nbIter < 2) || (!converged && nbIter < m_Iterations) ) 
  {
    if(msgLevel(MSG::DEBUG)) {
      debug() <<"Iteration nr: " << nbIter << endmsg;
    }
    zVtx = vtx.position().z();
    prepareVertex(vtx,pvTracks,hess,d0vec,nbIter);
    StatusCode sc = outVertex(vtx,pvTracks,hess,d0vec);
    if(sc != StatusCode::SUCCESS) {
      break;
    }
    zVtx = vtx.position().z();
    if(fabs(zVtx - zPrevious) < m_maxDeltaZ) {
      converged = true;
    } else {
      zPrevious = zVtx;
    }
    ++nbIter;
  }

  if (nbIter >= m_Iterations) {
    warning() << " Reached max # iterations without convergence " 
              << nbIter << endreq;
  }

  setChi2(vtx,pvTracks);
  // Check the weight of tracks to be accepted for PV candidate
  int outTracks = 0;
  for(PVTrackPtrs::iterator iFitTr = pvTracks.begin(); 
      iFitTr != pvTracks.end(); iFitTr++) {
    PVTrack* fitTrack = *iFitTr;
    double weight = fitTrack->weight;
    if(weight > m_acceptTrack) {
      outTracks++;
    }
  }
  if(outTracks < m_minTr) {
    if(msgLevel(MSG::DEBUG)) {
      verbose() << "Too few tracks after PV fit" << endmsg;
    }
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;;
}

//=============================================================================
// Add track for PV
//=============================================================================
StatusCode LSAdaptPVFitter::addTrackForPV(const LHCb::Track* pvtr,
                                          PVTracks& pvTracks, double zseed) 
{
  // Create a new PVTrack to be put on the vecctor
  PVTrack pvtrack;
  pvtrack.isUsed = false;
  pvtrack.chi2 = 0;
  pvtrack.d0 = 0;
  pvtrack.err2d0 = 0.0;
  // Keep reference to the original track
  pvtrack.refTrack = pvtr;
  LHCb::State mstate;
  mstate =  pvtr->firstState();
  if ( mstate.checkLocation(LHCb::State::ClosestToBeam) != true ) {
    // extrapolate
    if(fabs(mstate.qOverP()) > 0) {
      StatusCode sc = m_fullExtrapolator->propagate(mstate,zseed);
      if(sc.isFailure()) return sc;
    } else {
       StatusCode sc = m_linExtrapolator->propagate(mstate,zseed);
       if(sc.isFailure()) return sc;
    }
  }
  pvtrack.stateG = mstate; 
  pvtrack.unitVect = pvtrack.stateG.slopes().Unit();
  pvTracks.push_back(pvtrack);
  return StatusCode::SUCCESS;
}

//=============================================================================
// initialize vertex
//=============================================================================
void LSAdaptPVFitter::initVertex(PVTracks& pvTracks, PVVertex& pvVtx,
                                 const Gaudi::XYZPoint seedPoint) 
{
  if(msgLevel(MSG::VERBOSE)) {
    verbose() << "initVertex method" << endmsg;
  }
  double zseed = seedPoint.z();
  double startzs = zseed - 30.0 * Gaudi::Units::mm;
  double endzs = zseed + 30.0 * Gaudi::Units::mm;
  int nTracks = 0;
  PVTracks::iterator pvTrack;
  for(pvTrack = pvTracks.begin(); pvTracks.end() != pvTrack; pvTrack++) {
    if(pvTrack->isUsed != true) {
      if((pvTrack->zClose() >= startzs) && (pvTrack->zClose() <= endzs)) {
         nTracks++;
         pvVtx.pvTracks.push_back(&(*pvTrack));
         pvVtx.primVtx.addToTracks(pvTrack->refTrack);
      } else {
        continue;
      }
    } else {
      continue;
    }
    
  }
  
  if(msgLevel(MSG::DEBUG)) {
    debug() << " Collected " << nTracks << " tracks out of " << pvTracks.size()
            << " for this vtx search" << endmsg;
  }
  pvVtx.primVtx.setPosition(seedPoint);
  pvVtx.primVtx.setNDoF(2 * nTracks - 3);
  Gaudi::SymMatrix3x3 hsm;
  for(int i = 0; i < 3; i++) {
    for(int j = 0; j < 3; j++) {
      hsm(i,j) = 0.0;
    }
  }
  pvVtx.primVtx.setCovMatrix(hsm);
  pvVtx.primVtx.setChi2(0.0);
}

//=============================================================================
// Prepare vertices
//=============================================================================
void LSAdaptPVFitter::prepareVertex(LHCb::RecVertex& vtx, 
                                   PVTrackPtrs& pvTracks, 
                                   Gaudi::SymMatrix3x3& hess,
                                   ROOT::Math::SVector<double,3>& d0vec, 
                                   int iter)
{
  int nbIter = iter;
  // Reset hessian and d0 vector
  for(int i = 0; i < 3; i++) {
    d0vec[i] = 0.0;
    for(int j = 0; j < 3; j++) {
      hess(i,j) = 0.0;
    }
  }
  if(msgLevel(MSG::DEBUG)) {
    verbose() << "Extrapolate tracks to the vertex at z = " 
              << vtx.position().z()
              << endmsg;
  }
  // Add whole track vector to hessian and d0 vector     
  for(PVTrackPtrs::iterator itrack = pvTracks.begin(); 
      itrack != pvTracks.end();) {
    PVTrack* pvTrack = *itrack;
    // Extrapolate tracks
    StatusCode sc = trackExtrapolate(pvTrack,vtx);
    if(!sc.isSuccess()) {
      if(msgLevel(MSG::DEBUG)) {
        verbose() << "Track " << pvTrack->refTrack->key()
                  << " could not be extrapolated to the vertex" << endmsg;
      }      
      itrack = pvTracks.erase(itrack);
    } else {
      pvTrack->weight = getTukeyWeight(pvTrack->chi2,nbIter);
      addTrack(pvTrack,hess,d0vec);
      itrack++;
    }
  }
}

//=============================================================================
// Track extrapolation
//=============================================================================
StatusCode LSAdaptPVFitter::trackExtrapolate(PVTrack* pvTrack,
                                             const LHCb::RecVertex& vtx) {
  Gaudi::XYZPoint trkPoint(pvTrack->stateG.x(),
                           pvTrack->stateG.y(),
                           pvTrack->stateG.z());
  // Set the new origin at the current vertex
  Gaudi::XYZVector diffVect = trkPoint - vtx.position();
  // Compute the distance from the vertex to the track
  pvTrack->vd0 = pvTrack->unitVect.Cross(diffVect.Cross(pvTrack->unitVect));
  // Compute impact parameter d0
  pvTrack->d0 = sqrt(pvTrack->vd0.Mag2());
  // Use linear extrapolator to move the track to the desired position
  LHCb::State stateToMove = pvTrack->stateG;
  StatusCode sc = m_linExtrapolator->propagate(stateToMove,vtx.position().z() 
                                           + pvTrack->vd0.z());
  if(!sc.isSuccess()) {
    if(msgLevel(MSG::DEBUG)) {
      verbose() << "Error propagating the track state" << endmsg;
    }
    return StatusCode::FAILURE;
  }
  // Compute the error on the track impact parameter
  Gaudi::XYZVector vd0Unit = pvTrack->vd0.unit();
  ROOT::Math::SVector<double,2> xyVec;
  xyVec[0] = vd0Unit.x();
  xyVec[1] = vd0Unit.y();
  Gaudi::SymMatrix2x2 covMatrix =
                      stateToMove.covariance().Sub<Gaudi::SymMatrix2x2>(0,0);
  ROOT::Math::SVector<double,2> covMatrix_xyVec_product;
  covMatrix_xyVec_product = covMatrix * xyVec;
  pvTrack->err2d0 = xyVec[0] * covMatrix_xyVec_product[0] 
                  + xyVec[1] * covMatrix_xyVec_product[1];
  pvTrack->chi2 = (pvTrack->d0 * pvTrack->d0)/ pvTrack->err2d0;
  return StatusCode::SUCCESS;
}

//=============================================================================
// Add track
//=============================================================================
void LSAdaptPVFitter::addTrack(PVTrack* pTrack, 
                               Gaudi::SymMatrix3x3& hess,
                               ROOT::Math::SVector<double,3>& d0vec)
{
  addsubTrack(pTrack,hess,d0vec,+1.0);
}

//=============================================================================
// Remove track
//=============================================================================
void LSAdaptPVFitter::removeTrack(PVTrack* pTrack, 
                                  Gaudi::SymMatrix3x3& hess,
                                  ROOT::Math::SVector<double,3>& d0vec)
{
  addsubTrack(pTrack,hess,d0vec,-1.0);
}

//=============================================================================
// Add subtrack
//=============================================================================
void LSAdaptPVFitter::addsubTrack(PVTrack* pvTrack, 
                                  Gaudi::SymMatrix3x3& hess,
                                  ROOT::Math::SVector<double,3>& d0vec,
                                  double invs)
{
  invs *= (2.0 / pvTrack->err2d0) * pvTrack->weight;
  double unitVectStd[3];
  unitVectStd[0] = pvTrack->unitVect.x();
  unitVectStd[1] = pvTrack->unitVect.y();
  unitVectStd[2] = pvTrack->unitVect.z(); 
  double vd0Std[3];
  vd0Std[0] = pvTrack->vd0.x();
  vd0Std[1] = pvTrack->vd0.y();
  vd0Std[2] = pvTrack->vd0.z();
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

//=============================================================================
// Output Vertex
//=============================================================================
StatusCode LSAdaptPVFitter::outVertex(LHCb::RecVertex& vtx,
                                      PVTrackPtrs& pvTracks,
                                      Gaudi::SymMatrix3x3& hess,
                                      ROOT::Math::SVector<double,3>& d0vec)
{
  if(msgLevel(MSG::DEBUG)) {
    verbose() << "Computing new vertex position" << endmsg;
  }
  // Solve the linear equations to get a vertex
  int fail;
  hess.Inverse(fail);
  if (0 != fail) {
    Error("Error inverting hessian matrix",0);
    return StatusCode::FAILURE;
  } else {
    hess.Invert();
  }
  ROOT::Math::SVector<double,3> delta;
  for(int i = 0; i < 3; i++) {
    delta[i] = 0.0;
  }
  delta = hess * d0vec;
  if(msgLevel(MSG::DEBUG)) {
    debug() << "Displacement found: " 
            << delta[0] << " , " 
            << delta[1] << " , " 
            << delta[2] << " , " << endmsg;
  }
  Gaudi::XYZPoint newVtx(vtx.position().x() + delta[0], 
                         vtx.position().y() + delta[1], 
                         vtx.position().z() + delta[2]);
  vtx.setPosition(newVtx);
  if(msgLevel(MSG::DEBUG)) {
    debug() << " New vertex position: "
            << vtx.position().x() << " , "
            << vtx.position().y() << " , " 
            << vtx.position().z() << endmsg;
  }
  // Set covariance matrix
  hess = hess * 2.0;
  vtx.setCovMatrix(hess);
  // Set tracks
  vtx.clearTracks();
  for (PVTrackPtrs::iterator itrack = pvTracks.begin(); 
                             itrack != pvTracks.end(); itrack++) {
    vtx.addToTracks((*itrack)->refTrack);
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
// Compute PV chi2
//=============================================================================
void LSAdaptPVFitter::setChi2(LHCb::RecVertex& vtx, 
                                PVTrackPtrs& pvTracks)
{
  int nDoF = -3;
  double chi2 = 0.0;
  for(PVTrackPtrs::iterator itrack = pvTracks.begin(); 
      itrack != pvTracks.end(); itrack++) {  
    chi2 += (*itrack)->chi2;
    nDoF += 2;
  }
  if( msgLevel(MSG::DEBUG) ) {
    verbose() << "Compute chi2 of this vertex: " << chi2 
              << " for " << nDoF << " DoF ( " 
              << chi2 / nDoF << " / DoF)"
             << endmsg;
  }
  vtx.setChi2(chi2);
  vtx.setNDoF(nDoF);
}

//=============================================================================
// Get Tukey's weight
//=============================================================================
double LSAdaptPVFitter::getTukeyWeight(double& trchi2,int& iter)
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
