// Include files
// from Event
#include "Event/Track.h"
#include "Event/State.h"
#include "Event/RecVertex.h"
#include "TrackInterfaces/ITrackExtrapolator.h"
// Local
#include "LSAdaptPVFitter.h"

DECLARE_TOOL_FACTORY(LSAdaptPVFitter)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
LSAdaptPVFitter::LSAdaptPVFitter(const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent)
  : base_class(type,name,parent),
    m_minTr(0),
    m_maxIterations(0),
    m_maxChi2(0.),
    m_maxDeltaZ(0.),
    m_maxDeltaChi2NDoF(0.),
    m_minTrackWeight(0.),
    m_trackChi(3.),
    m_linExtrapolator(0),
    m_fullExtrapolator(0),
    m_myZero(1E-12)
{
  declareInterface<IPVFitter>(this);
  // Minimum number of tracks in vertex
  declareProperty("MinTracks", m_minTr = 5)
  ->declareUpdateHandler( [=](Property&) {
      // do not allow less than 3 tracks in a vertex
      if  ( this->m_minTr < 3 ) {
        this->m_minTr = 3;
        this->warning() << "MinTracks parameter set to 3" << endmsg;
      }
  });
  // Number of iterations
  declareProperty("Iterations", m_maxIterations = 50);
  // Chi2 of completely wrong tracks
  declareProperty("maxChi2", m_maxChi2 = 400.0);
  // Fit convergence condition delta z
  declareProperty("maxDeltaZ", m_maxDeltaZ = 0.0005 * Gaudi::Units::mm);
  // Fit convergence condition delta chi2/ndof if not converged by dz
  declareProperty("maxDeltaChi2NDoF", m_maxDeltaChi2NDoF = 0.002);
  // Value of the Tukey's weight to accept a track
  declareProperty("acceptTrack", m_minTrackWeight = 0.00000001);
  // Max chi2 track to accept track in PV fit
  auto p = declareProperty("trackMaxChi2", m_trackMaxChi2 = 9.);
  p->declareUpdateHandler( [=](Property&) { this->m_trackChi = std::sqrt(m_trackMaxChi2); });
  p->useUpdateHandler();
  // Max chi2 tracks to be removed from next PV search
  declareProperty("trackMaxChi2Remove", m_trackMaxChi2Remove = 25.);
  // Min number of iterations
  declareProperty("minIter", m_minIter = 5);
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
LSAdaptPVFitter::~LSAdaptPVFitter() = default;

//=============================================================================
// Least square adaptive fitting method
//=============================================================================
StatusCode LSAdaptPVFitter::fitVertex(const Gaudi::XYZPoint& seedPoint,
                                      const std::vector<const LHCb::Track*>& rTracks,
                                      LHCb::RecVertex& vtx,
                                      std::vector<const LHCb::Track*>& tracks2remove) const
{
  if(msgLevel(MSG::VERBOSE)) {
    verbose() << "fitVertex method" << endmsg;
  }

  PVTracks pvTracks;
  PVVertex pvVertex;

  for(auto itr = rTracks.begin(); itr != rTracks.end(); itr++) {
    const LHCb::Track* track = *itr;
    addTrackForPV(track, pvTracks, seedPoint.z()).ignore();
  }

  initVertex(pvTracks,pvVertex,seedPoint);

  // Initial track cleaning
  if(msgLevel(MSG::VERBOSE)) {
    verbose() << "clean tracks" << endmsg;
  }
  for(auto itrack = pvVertex.pvTracks.begin();
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
      debug() << "Too few tracks to fit PV" << endmsg;
    }
    vtx = pvVertex.primVtx;
    vtx.clearTracks();
    vtx.setTechnique(LHCb::RecVertex::Primary);
    return StatusCode::FAILURE;
  }

  StatusCode scvfit = fit(pvVertex.primVtx, pvVertex.pvTracks, tracks2remove);
  if (!scvfit.isSuccess() ) {
    if(msgLevel(MSG::DEBUG)) debug() << "PV fit failed" << endmsg;
  }

  vtx = pvVertex.primVtx;
  vtx.setTechnique(LHCb::RecVertex::Primary);
  return scvfit;
}

//=============================================================================
// Least square adaptive PV fit
//=============================================================================
StatusCode LSAdaptPVFitter::fit(LHCb::RecVertex& vtx,
                                std::vector<PVTrack*>& pvTracks, std::vector<const LHCb::Track*>& tracks2remove) const
{
  tracks2remove.clear();

  if(msgLevel(MSG::VERBOSE)) {
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
  double chi2previous = 1e12;
  double zVtx = 0.0;
  double chi2Vtx = 0.0;
  double delta_zVtx = 0.0;
  double delta_chi2Vtx = 0.0;
  double maxdz = m_maxDeltaZ;
  bool converged = false;
  int nbIter = 0;
  // Iteration loop. Require at least m_minIter iterations to reach final weight.
  while( (nbIter < m_minIter) || (!converged && nbIter < m_maxIterations) )
  {
    if(msgLevel(MSG::DEBUG)) {
      debug() <<"Iteration nr: " << nbIter << endmsg;
    }
    zVtx = vtx.position().z();
    prepareVertex(vtx,pvTracks,hess,d0vec,nbIter);

    int ntr=0;
    for(PVTrackPtrs::iterator itrack = pvTracks.begin();
         itrack != pvTracks.end(); itrack++) {
       PVTrack* pvTrack = *itrack;
       if (pvTrack->weight > 0.) ntr++;
    }
    if (ntr < 3 ) {
      if(msgLevel(MSG::DEBUG)) debug() << "# tracks too low. ntr = " << ntr << endmsg;
      break;
    }

    // loose convergence criteria if close to end of iterations
    if ( 1.*nbIter > 0.8*m_maxIterations ) {
      maxdz = 10.*m_maxDeltaZ;
    }

    StatusCode sc = outVertex(vtx,pvTracks,hess,d0vec);
    if(sc != StatusCode::SUCCESS) {
      break;
    }
    zVtx = vtx.position().z();
    chi2Vtx = vtx.chi2()/vtx.nDoF();

    delta_zVtx = fabs(zVtx - zPrevious);
    delta_chi2Vtx = fabs(chi2Vtx-chi2previous);

    if( delta_zVtx < maxdz) {
      if ( nbIter >= m_minIter ) converged = true;
    }

    if( msgLevel(MSG::DEBUG) )
    {
      if ( converged )
      {
        debug() << format(" converged with delta chi2 and dz %8.5f %8.5f %4d %4d ",
                          delta_chi2Vtx, delta_zVtx, vtx.nDoF(), nbIter) << endmsg;
      }
    }
    zPrevious = zVtx;
    chi2previous = chi2Vtx;

    ++nbIter;
  }

  if (nbIter >= m_maxIterations) {
    if( msgLevel(MSG::DEBUG) ) {
      debug() << " Reached max # iterations without convergence " << nbIter << endmsg;
    }

    // check if delta chi2/ndof acceptable to confirm convergence anyway
    if ( delta_chi2Vtx < m_maxDeltaChi2NDoF ) converged = true;
  }

  if ( ! converged ) {
    return StatusCode::FAILURE;
  }


  setChi2(vtx,pvTracks);

  // fill tracks arounf PV to remove from next PV search
  for(PVTrackPtrs::iterator itrack = pvTracks.begin();
      itrack != pvTracks.end(); itrack++) {

    if( (*itrack)->chi2 < m_trackMaxChi2Remove) {
       tracks2remove.push_back( (*itrack)->refTrack);
    }

    if( msgLevel(MSG::DEBUG) )
    {

    if (  (*itrack)->chi2 < 100.) {
        const LHCb::Track * lbtrack = (*itrack)->refTrack;
        int back = lbtrack->checkFlag(LHCb::Track::Backward);
        int velo = ( lbtrack->checkType(LHCb::Track::Velo) ||  lbtrack->checkType(LHCb::Track::Upstream) );
        int longtr = lbtrack->checkType(LHCb::Track::Long);
        int hasmom = fabs( lbtrack->firstState().qOverP() )>0. ;

        debug() <<  format("track significance dump: %3d %3d %3d %3d %7.2f",
                   back, velo, longtr, hasmom, std::sqrt((*itrack)->chi2) ) << endmsg;
      }
    }

  }

  // Check the weight of tracks to be accepted for PV candidate
  int outTracks = 0;
  for(PVTrackPtrs::iterator iFitTr = pvTracks.begin(); iFitTr != pvTracks.end(); iFitTr++) {
    PVTrack* fitTrack = *iFitTr;
    double weight = fitTrack->weight;
    if(weight > m_minTrackWeight) {
      outTracks++;
    }
  }

  if(outTracks < m_minTr) {
    if(msgLevel(MSG::VERBOSE)) {
      verbose() << "Too few tracks after PV fit" << endmsg;
    }
    return StatusCode::FAILURE;
  }


  return StatusCode::SUCCESS;
}

//=============================================================================
// Add track for PV
//=============================================================================
StatusCode LSAdaptPVFitter::addTrackForPV(const LHCb::Track* pvtr,
                                          PVTracks& pvTracks, double zseed) const
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
                                 const Gaudi::XYZPoint seedPoint)  const
{
  if(msgLevel(MSG::VERBOSE)) {
    verbose() << "initVertex method" << endmsg;
  }
  for(auto pvTrack = pvTracks.begin(); pvTracks.end() != pvTrack; pvTrack++) {
         pvVtx.pvTracks.push_back(&(*pvTrack));
         pvVtx.primVtx.addToTracks(pvTrack->refTrack);
  }

  int nTracks = pvTracks.size();
  if(msgLevel(MSG::DEBUG)) {
    debug() << " Collected " << nTracks << " for this vtx search" << endmsg;
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
                                   int iter) const
{
  int nbIter = iter;
  // Reset hessian and d0 vector
  for(int i = 0; i < 3; i++) {
    d0vec[i] = 0.0;
    for(int j = 0; j < 3; j++) {
      hess(i,j) = 0.0;
    }
  }
  if(msgLevel(MSG::VERBOSE)) {
    verbose() << "Extrapolate tracks to the vertex at z = "
              << vtx.position().z()
              << endmsg;
  }
  // Add whole track vector to hessian and d0 vector
  for(auto itrack = pvTracks.begin(); itrack != pvTracks.end();) {
    PVTrack* pvTrack = *itrack;
    // Extrapolate tracks
    StatusCode sc = trackExtrapolate(pvTrack,vtx);
    if(!sc.isSuccess()) {
      if(msgLevel(MSG::VERBOSE)) {
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
                                             const LHCb::RecVertex& vtx) const
{
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
    if(msgLevel(MSG::VERBOSE)) {
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
  if(pvTrack->err2d0>m_myZero) pvTrack->chi2 = (pvTrack->d0 * pvTrack->d0)/ pvTrack->err2d0;
  else
  {
    if(msgLevel(MSG::DEBUG)) debug() << "trackExtrapolate: pvTrack error is too small for computation" << endmsg;
    pvTrack->chi2 = (pvTrack->d0 * pvTrack->d0)/ m_myZero;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Add track
//=============================================================================
void LSAdaptPVFitter::addTrack(PVTrack* pTrack,
                               Gaudi::SymMatrix3x3& hess,
                               ROOT::Math::SVector<double,3>& d0vec) const
{
  addsubTrack(pTrack,hess,d0vec,+1.0);
}

//=============================================================================
// Remove track
//=============================================================================
void LSAdaptPVFitter::removeTrack(PVTrack* pTrack,
                                  Gaudi::SymMatrix3x3& hess,
                                  ROOT::Math::SVector<double,3>& d0vec) const
{
  addsubTrack(pTrack,hess,d0vec,-1.0);
}

//=============================================================================
// Add subtrack
//=============================================================================
void LSAdaptPVFitter::addsubTrack(PVTrack* pvTrack,
                                  Gaudi::SymMatrix3x3& hess,
                                  ROOT::Math::SVector<double,3>& d0vec,
                                  double invs) const
{
  if(pvTrack->err2d0>m_myZero) invs *= (2.0 / pvTrack->err2d0) * pvTrack->weight;
  else {
    if(msgLevel(MSG::DEBUG)) debug() << "trackExtrapolate: pvTrack error is too small for computation" << endmsg;
    invs *= (2.0 / m_myZero) * pvTrack->weight;
  }

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
                                      ROOT::Math::SVector<double,3>& d0vec) const
{
  if(msgLevel(MSG::VERBOSE)) {
    verbose() << "Computing new vertex position" << endmsg;
  }
  // Solve the linear equations to get a vertex
  int fail;
  hess.Inverse(fail);
  if (0 != fail) {
    if(msgLevel(MSG::DEBUG)) debug() << "Error inverting hessian matrix" << endmsg;
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

  if(msgLevel(MSG::DEBUG)) {
    printTracks(pvTracks);
  }

  // Set covariance matrix
  hess = hess * 2.0;
  vtx.setCovMatrix(hess);
  // Set tracks
  vtx.clearTracks();
  for (PVTrackPtrs::iterator itrack = pvTracks.begin();
                             itrack != pvTracks.end(); itrack++) {
    if((*itrack)->weight > m_minTrackWeight) {
      vtx.addToTracks( (*itrack)->refTrack, (float) (*itrack)->weight );
    }
  }
  setChi2(vtx,pvTracks);

  return StatusCode::SUCCESS;
}

//=============================================================================
// Compute PV chi2
//=============================================================================
void LSAdaptPVFitter::setChi2(LHCb::RecVertex& vtx,
                                PVTrackPtrs& pvTracks) const
{
  int nDoF = -3;
  double chi2 = 0.0;
  for(PVTrackPtrs::iterator itrack = pvTracks.begin();
      itrack != pvTracks.end(); itrack++) {
    if((*itrack)->weight > m_minTrackWeight) {
      chi2 += (*itrack)->chi2 * (*itrack)->weight;
      nDoF += 2;
    }
  }
  if(msgLevel(MSG::VERBOSE)) {
    verbose() << "Compute chi2 of this vertex: " << chi2
              << " for " << nDoF << " DoF ( "
              << chi2 / nDoF << " / DoF)"
             << endmsg;
  }
  vtx.setChi2(chi2);
  vtx.setNDoF(nDoF);
}

//=============================================================================
// Compute PV chi2
//=============================================================================
void LSAdaptPVFitter::printTracks(PVTrackPtrs& pvTracks) const
{
  if(msgLevel(MSG::VERBOSE)) {
    verbose() << " dump tracks for this vertex" << endmsg;

    for(PVTrackPtrs::iterator iFitTrPtr = pvTracks.begin(); iFitTrPtr != pvTracks.end(); iFitTrPtr++) {
      int invx = 0;
      PVTrack* iFitTr = *iFitTrPtr;
      if( iFitTr->weight > m_minTrackWeight) invx = 1;
      verbose() << format( "chi2 d0 w zc %7.2f %7.2f %7.2f %7.2f %3d",
                           iFitTr->chi2, iFitTr->d0, iFitTr->weight, iFitTr->refTrack->firstState().z(),invx) << endmsg;
    }
  }
}

//=============================================================================
// Get Tukey's weight
//=============================================================================
double LSAdaptPVFitter::getTukeyWeight(double trchi2, int iter) const
{
  if (iter<1 ) return 1.;
  double ctrv = m_trackChi * (m_minIter -  iter);
  if (ctrv < m_trackChi) ctrv = m_trackChi;
  double cT2 = trchi2 / (ctrv*ctrv);
  double weight = 0.;
  if(cT2 < 1.) {
    weight = 1. - cT2;
    weight = weight * weight;
  }
  return weight;
}
