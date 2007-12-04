// $Id: LSIterPVFitter.cpp,v 1.1 2007-12-04 08:46:52 witekma Exp $
// Include files 
// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
// from GSL
#include "gsl/gsl_math.h"
// from Event
#include "Event/Track.h"
#include "Event/State.h"
#include "Event/RecVertex.h"
#include "TrackInterfaces/ITrackExtrapolator.h"
// Local
#include "LSIterPVFitter.h"

DECLARE_TOOL_FACTORY(LSIterPVFitter);

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
LSIterPVFitter::LSIterPVFitter(const std::string& type,
                               const std::string& name,
                               const IInterface* parent)
  : GaudiTool(type,name,parent) {
  declareInterface<IPVFitter>(this);
  // Cut-off of chi2 for single track
  declareProperty("Chi2Cut", m_Chi2Cut = 16.0);
  // Number of iterations
  declareProperty("Iterations", m_Iterations = 200);
  // Minimum number of tracks in vertex  
  declareProperty("MinTracks", m_minTr = 6);
  // Chi2 of completely wrong tracks
  declareProperty("maxChi2", m_maxChi2 = 256.0);
  // Limit in R for linear extrapolation
  declareProperty("LinExtrapolation", m_extrapRCut = 5.0 * Gaudi::Units::mm);
}

//=========================================================================
// Initialize
//=========================================================================
StatusCode LSIterPVFitter::initialize()
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
  m_fullExtrapolator = tool<ITrackExtrapolator>("TrackHerabExtrapolator",this);
  if(!m_fullExtrapolator) {
    err() << "Unable to get TrackHerabExtrapolator" << endmsg;
    return  StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
// Destructor
//=============================================================================
LSIterPVFitter::~LSIterPVFitter() {}

//=============================================================================
// Least square iterative fitting method
//=============================================================================
StatusCode LSIterPVFitter::fitVertex(const Gaudi::XYZPoint seedPoint, 
                                     std::vector<const LHCb::Track*>& rTracks, 
				     LHCb::RecVertex& vtx)
{
  StatusCode sc = StatusCode::SUCCESS;
  m_pvTracks.clear();
  PVVertex pvVertex;
  std::vector<const LHCb::Track*>::iterator itr;
  for(itr = rTracks.begin(); itr != rTracks.end(); itr++) {    
    const LHCb::Track* track = *itr;
    sc = addTrackForPV(track,m_pvTracks);
    if(!sc.isSuccess()) continue;
  }
  collectTracks(m_pvTracks,pvVertex,seedPoint.z());
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
    copyTrackVertex(&(pvVertex.primVtx),&vtx);
    vtx.setTechnique(LHCb::RecVertex::Primary);
  }
  return scvfit;
}

//=============================================================================
// Least square iterative PV fit
//=============================================================================
StatusCode LSIterPVFitter::fit(LHCb::RecVertex& vtx, 
                               std::vector<PVTrack*>& pvTracks) 
{
  if(msgLevel(MSG::DEBUG)) {
    verbose() << "Least square iterative PV fit" << endmsg;
  }
  if(msgLevel(MSG::DEBUG)) {
    debug() << pvTracks.size() << " tracks before iteration loop" << endmsg;
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
  double chi2Cut = m_Chi2Cut;
  StatusCode scfit = StatusCode::SUCCESS;
  int nbIter = 0;
  // Iteration loop
  while(1) {
    nbIter++;
    if(msgLevel(MSG::DEBUG)) {
      debug() <<"Iteration nr: " << nbIter << endmsg;
    }
    if (nbIter > m_Iterations) {
      scfit = StatusCode::FAILURE;
      break;
    }
    if( (int)pvTracks.size() < m_minTr) {
      scfit = StatusCode::FAILURE;
      break;
    }
    // Move the tracks and compute chi2 at the new vertex
    prepareVrtcs(vtx,pvTracks,hess,d0vec);
    StatusCode sc = outVertex(vtx,pvTracks,hess,d0vec);
    if(sc != StatusCode::SUCCESS) {
      scfit = StatusCode::FAILURE;
      break;
    }
    setChi2(vtx,pvTracks);
    // Remove track with highest chi2
    int nRejected = removeMaxChi2Track(pvTracks,hess,d0vec,chi2Cut); 
    if (nRejected == 0) {
      break;
    }
  } 
  if(msgLevel(MSG::DEBUG)) {
    debug() <<  pvTracks.size() << " tracks after iteration loop" << endmsg;
  }
  // Check result
  if(scfit != StatusCode::SUCCESS || (int) pvTracks.size() < m_minTr) {
    if(msgLevel(MSG::DEBUG)) {
      debug() << "PV fit failed" << endmsg; 
    }
    return StatusCode::FAILURE;
  } 
  return scfit;
}

//=============================================================================
// Add track for PV
//=============================================================================
StatusCode LSIterPVFitter::addTrackForPV(const LHCb::Track* pvtr,
                                         PVTracks& pvTracks) 
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
  double r = sqrt(mstate.x() * mstate.x() + mstate.y() * mstate.y());
  if(r > 5.0 * Gaudi::Units::mm) {
    double zclose = 0.5 * (mstate.z() - mstate.x() / mstate.tx() +
                           mstate.z() - mstate.y() / mstate.ty()); 
    if(fabs(mstate.qOverP()) > 0) {
      StatusCode sc = m_fullExtrapolator->propagate(mstate,zclose);
      if(sc.isFailure()) return sc;
    } else {
      Warning("Extrapolation of track with no momentum",0);
      // Assign average pt = 400 MeV and update error
      double p = 400.0 * Gaudi::Units::MeV * 
                 sqrt(1.0 + 1.0 / (gsl_pow_2(mstate.tx()) + 
                                   gsl_pow_2(mstate.ty())));
       mstate.setQOverP(1.0 / p);
       double errqp2 = gsl_pow_2((200.0 * Gaudi::Units::MeV) / (p * p));
       mstate.setErrQOverP2(errqp2);
       double zclose = 0.5 * (mstate.z() - mstate.x() / mstate.tx() +
                              mstate.z() - mstate.y() / mstate.ty()); 
       StatusCode sc = m_fullExtrapolator->propagate(mstate,zclose);
       if(sc.isFailure()) return sc;
    }
  }
  pvtrack.stateG = mstate; 
  pvtrack.unitVect = pvtrack.stateG.slopes().Unit();
  pvTracks.push_back(pvtrack);
  return StatusCode::SUCCESS;
}

//=============================================================================
// Collect tracks
//=============================================================================
StatusCode LSIterPVFitter::collectTracks(PVTracks& pvTracks, PVVertex& pvVtx, 
                                        double zseed) 
{
  if(msgLevel(MSG::VERBOSE)) {
    verbose() << "collectTracks method" << endmsg;
  }
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
  pvVtx.primVtx.setPosition(Gaudi::XYZPoint(0.0,0.0,zseed));
  pvVtx.primVtx.setNDoF(2 * nTracks - 3);
  Gaudi::SymMatrix3x3 hsm;
  for(int i = 0; i < 3; i++) {
    for(int j = 0; j < 3; j++) {
      hsm(i,j) = 0.0;
    }
  }
  pvVtx.primVtx.setCovMatrix(hsm);
  pvVtx.primVtx.setChi2(0.0);
  return StatusCode::SUCCESS;
}

//=============================================================================
// Copy original TrackVertex 
//=============================================================================
void LSIterPVFitter::copyTrackVertex(LHCb::RecVertex* inTrackVertex,
                                     LHCb::RecVertex* outTrackVertex)
{
  if(msgLevel(MSG::DEBUG)) {
    debug() << "copyTrackVertex method" << endmsg;
  }
  outTrackVertex->setPosition(inTrackVertex->position());
  outTrackVertex->setCovMatrix(inTrackVertex->covMatrix());
  outTrackVertex->setChi2(inTrackVertex->chi2());
  outTrackVertex->setNDoF(inTrackVertex->nDoF());
  outTrackVertex->setTracks(inTrackVertex->tracks());
  if(msgLevel(MSG::DEBUG)) {
    debug() << "Exit copyTrackVertex method" << endmsg;
  } 
}

//=============================================================================
// Prepare vertices
//=============================================================================
void LSIterPVFitter::prepareVrtcs(LHCb::RecVertex& vtx, 
                                  PVTrackPtrs& pvTracks, 
                                  Gaudi::SymMatrix3x3& hess,
                                  ROOT::Math::SVector<double,3>& d0vec)
{
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
      addTrack(pvTrack,hess,d0vec);
      itrack++;
    }
  }
}

//=============================================================================
// Track extrapolation
//=============================================================================
StatusCode LSIterPVFitter::trackExtrapolate(PVTrack* pvTrack,
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
  // Use the extrapolator to move the track to the desired position
  ITrackExtrapolator* iExtrapolator;
  if(msgLevel(MSG::DEBUG)) {
    verbose() << " Extrapolating track " << pvTrack->refTrack->key()
              << " using ";
  }
  if(sqrt(gsl_pow_2(trkPoint.x()) + gsl_pow_2(trkPoint.y())) < m_extrapRCut) {
    // Use the linear extrapolator
    if(msgLevel(MSG::DEBUG)) verbose() << "linear";
    iExtrapolator = m_linExtrapolator;
  } else {
      if(msgLevel(MSG::DEBUG)) verbose() << "full";
      iExtrapolator = m_fullExtrapolator;
  }
  if(msgLevel(MSG::DEBUG)) verbose() << " extrapolator" << endmsg;
  LHCb::State stateToMove = pvTrack->stateG;
  StatusCode sc = iExtrapolator->propagate(stateToMove,vtx.position().z() 
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
  pvTrack->chi2 = gsl_pow_2(pvTrack->d0) / pvTrack->err2d0;
  return StatusCode::SUCCESS;
}

//=============================================================================
// Add track
//=============================================================================
void LSIterPVFitter::addTrack(PVTrack* pTrack, 
                              Gaudi::SymMatrix3x3& hess,
                              ROOT::Math::SVector<double,3>& d0vec)
{
  addsubTrack(pTrack,hess,d0vec,+1.0);
}

//=============================================================================
// Remove track
//=============================================================================
void LSIterPVFitter::removeTrack(PVTrack* pTrack, 
                                 Gaudi::SymMatrix3x3& hess,
                                 ROOT::Math::SVector<double,3>& d0vec)
{
  addsubTrack(pTrack,hess,d0vec,-1.0);
}

//=============================================================================
// Add subtrack
//=============================================================================
void LSIterPVFitter::addsubTrack(PVTrack* pvTrack, 
                                 Gaudi::SymMatrix3x3& hess,
                                 ROOT::Math::SVector<double,3>& d0vec,
                                 double invs)
{
  invs *= 2.0 / pvTrack->err2d0;
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
StatusCode LSIterPVFitter::outVertex(LHCb::RecVertex& vtx,
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
double LSIterPVFitter::setChi2(LHCb::RecVertex& vtx, 
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
  return chi2 / nDoF;
}

//=============================================================================
//  removeMaxChi2Track
//=============================================================================
int LSIterPVFitter::removeMaxChi2Track(PVTrackPtrs& pvTracks, 
                                       Gaudi::SymMatrix3x3& hess,
                                       ROOT::Math::SVector<double,3>& d0vec,
                                       double chi2Cut)
{
  int nRejected = 0;
  PVTrackPtrs::iterator itrack_maxchi2 = pvTracks.end();  
  double maxchi2 = 0.;
  // Loop over all tracks    
  for(PVTrackPtrs::iterator itrack = pvTracks.begin(); 
      itrack != pvTracks.end(); itrack++) {
    PVTrack* pvTrack = *itrack;
    if(pvTrack->chi2 > chi2Cut && pvTrack->chi2 > maxchi2) {
      itrack_maxchi2 = itrack;
      maxchi2 = pvTrack->chi2;
    }
  }
  if(itrack_maxchi2 != pvTracks.end()) {
    PVTrack* pvTrack = *itrack_maxchi2;
    removeTrack(pvTrack,hess,d0vec);
    pvTracks.erase(itrack_maxchi2);   
    // Set the track as not yet extrapolated
    pvTrack->err2d0 = 0;
    pvTrack->chi2 = 0;
    nRejected = 1;
  }
  return nRejected;
}

