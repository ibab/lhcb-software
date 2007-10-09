// $Id: PVFitterTool.cpp,v 1.1.1.1 2007-10-09 18:46:14 smenzeme Exp $
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

// local
#include "PVUtils.h"
#include "PVFitterTool.h"

DECLARE_TOOL_FACTORY( PVFitterTool );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PVFitterTool::PVFitterTool(const std::string& type,
                           const std::string& name,
                           const IInterface* parent)
  : GaudiTool(type,name,parent) {
  
  declareInterface<IPVFitterTool>(this);

  // Cut-off of chi2 for single track
  declareProperty("Chi2Cut",     m_Chi2Cut = 16.0);
  // Number of iterations
  declareProperty("Iterations",  m_Iterations = 200);
  // Minimum number of tracks in vertex  
  declareProperty("MinTracks",   m_minTr = 6);
  declareProperty("deWeight",    m_deWeight = false);

  // Limit in R for linear extrapolation
  declareProperty("MaxLinearExtrapolation", m_extrapRCut = 5.0 * Gaudi::Units::mm);

}


//=========================================================================
// Initialize
//=========================================================================
StatusCode PVFitterTool::initialize()
{

  StatusCode sc = GaudiTool::initialize();
  if (!sc) return sc;

   info() << "PVFitterTool parameters" << endmsg
          << "Chi2Cut " << m_Chi2Cut << endmsg
          << "Chi2 will be computed using the track's cov. matrix" << endmsg
          << "Minimum number of tracks to fit a vertex: " << m_minTr << endmsg
          << "Maximum number of iterations: " << m_Iterations << endmsg;

  m_pLinearExtrap = tool<ITrackExtrapolator>("TrackLinearExtrapolator", this);
  if( !m_pLinearExtrap ) {
    err() << "Unable to get TrackLinearExtrapolator" << endmsg;
    return  StatusCode::FAILURE;
  }

  m_pFullExtrap = tool<ITrackExtrapolator>("TrackHerabExtrapolator", this);
  if( !m_pFullExtrap ) {
    err() << "Unable to get TrackHerabExtrapolator" << endmsg;
    return  StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}


//=============================================================================
// Destructor
//=============================================================================
PVFitterTool::~PVFitterTool() {}


//=============================================================================
// Fitting method
//=============================================================================
StatusCode PVFitterTool::fit(LHCb::RecVertex& vtx, LHCb::Tracks& tracks)
{
  PVTracks pvTracks;
  for(LHCb::Tracks::iterator itrack = tracks.begin();
      tracks.end() != itrack; itrack++) {
    StatusCode sc = addTrackForPV(*itrack, pvTracks);
    if (sc.isFailure()) return sc;
  }
  return fit(vtx,pvTracks);
}


StatusCode PVFitterTool::fit(LHCb::RecVertex& vtx, std::vector<LHCb::Track*>& tracks)
{
  PVTracks pvTracks;
  for(std::vector<LHCb::Track*>::iterator itrack = tracks.begin();
      tracks.end() != itrack; itrack++) {
    StatusCode sc = addTrackForPV(*itrack, pvTracks);
    if (sc.isFailure()) return sc;
  }
  return fit(vtx,pvTracks);
}


StatusCode PVFitterTool::fit(LHCb::RecVertex& vtx, PVTracks& pvTracks)
{
  PVTrackPtrs pvTrackPtrs;
  for(PVTracks::iterator itrack = pvTracks.begin();
      pvTracks.end() != itrack; itrack++) {
    pvTrackPtrs.push_back(&(*itrack));
  }
  return fit(vtx,pvTrackPtrs);
}


StatusCode PVFitterTool::fit(LHCb::RecVertex& vtx, PVTrackPtrs& pvTracks) 
{

  if((int)pvTracks.size() < m_minTr) {
    if( msgLevel(MSG::DEBUG) ) {
      verbose() << "Too few tracks to fit" << endmsg;
    }
    return StatusCode::FAILURE;
  }

  Gaudi::SymMatrix3x3 hessian;
  ROOT::Math::SVector<double,3> vectorD0;

  // Reset hessian and vectorD0
  for(int i = 0; i < 3; i++) {
    vectorD0[i] = 0.0;
    for(int j = 0; j < 3; j++) {
      hessian(i,j) = 0.0;
    }
  }

  if( msgLevel(MSG::DEBUG) ) {
    verbose() << "Fitting" << endmsg;
  }

  double chi2Cut = m_Chi2Cut;

  if( msgLevel(MSG::DEBUG) ) {
    debug() <<  pvTracks.size() << " tracks before chi2 exclusion:" << endmsg;
  }
  
  // remove completely wrong tracks
  prepareVertices(vtx,pvTracks,hessian,vectorD0);
  chi2Exclusion(pvTracks,hessian,vectorD0,16.*chi2Cut);    

  if( msgLevel(MSG::DEBUG) ) {
    debug() <<  pvTracks.size() << " tracks before iteration loop:" << endmsg;
  }

  StatusCode scfit = StatusCode::SUCCESS;

  int nbIter = 0;

  while ( 1 ) {

    nbIter++;

    if( msgLevel(MSG::DEBUG) ) {
      debug() <<"Iteration nr: " << nbIter << endmsg;
    }
    
    if ( nbIter > m_Iterations ) {
      scfit = StatusCode::FAILURE;
      break;
    }

    if( (int)pvTracks.size() < m_minTr ) {
      scfit = StatusCode::FAILURE;
      break;
    }

    // Move the tracks and compute chi2 at the new vertex
    prepareVertices(vtx,pvTracks,hessian,vectorD0);

    StatusCode sc = newVertex(vtx,pvTracks,hessian,vectorD0);
    if(sc != StatusCode::SUCCESS) {
      scfit = StatusCode::FAILURE;
      break;
    }
    setChi2(vtx,pvTracks);

    // Remove track with highest chi2
    int nrej = removeMaxChi2Track(pvTracks,hessian,vectorD0,chi2Cut); 
    if (nrej == 0 ) {
      break;
    }
 
  } 

  if( msgLevel(MSG::DEBUG) ) {
    debug() <<  pvTracks.size() << " tracks after iteration loop:" << endmsg;
  }
  
  // check result
  if(scfit != StatusCode::SUCCESS || (int) pvTracks.size() < m_minTr ) {
    if( msgLevel(MSG::DEBUG) ) {
      debug() << " PV fit failed:" << endmsg; 
    }
    return StatusCode::FAILURE;
  } 

  if( msgLevel(MSG::DEBUG) ) {
    debug() << " Tracks after iteration loop:" << endmsg; 

    for(PVTrackPtrs::iterator itrack = pvTracks.begin(); 
        itrack != pvTracks.end(); itrack++) {  
  
         PVTrack* pvTrack = *itrack;
          
          debug() << " Track " << pvTrack->refTrack->key() 
                  << " Type "       << pvTrack->refTrack->type() 
                  << " Chi2 "       << pvTrack->chi2 
                  << " D0 "         << pvTrack->d0 
                  << " +- "         << sqrt(pvTrack->err2d0)
                  << " zD0 "        << pvTrack->vd0.z()
                  << endmsg;
    }
  }
  return scfit;
}


StatusCode PVFitterTool::addTrackForPV(LHCb::Track* str,
                                       PVTracks& pvTracks) {

    // Create a new PVTrack to be put on the vecctor
    PVTrack track;
    track.isUsed = false;
    track.chi2 = 0;
    track.d0 = 0;
    track.err2d0 = 0.;
    // Keep reference to the original track
    track.refTrack = str;

    LHCb::State mstate;

    mstate =  str->firstState();
    double r = sqrt(mstate.x()*mstate.x() + mstate.y()*mstate.y());

    if ( r > 5.*Gaudi::Units::mm ) {

      double zclose = 0.5*( mstate.z() - mstate.x()/mstate.tx() +
                            mstate.z() - mstate.y()/mstate.ty() ); 

      if (fabs(mstate.qOverP())>0 ) {

        StatusCode sc = m_pFullExtrap->propagate(mstate, zclose);
        if (sc.isFailure()) return sc;

      } else {
        
        Warning( "Extrapolation of track with no momentum", 0 );
        
        // Assign average pt = 400 MeV and update error
        double p = 400.0 * Gaudi::Units::MeV * 
                   sqrt(1.0 + 1.0 / (gsl_pow_2(mstate.tx()) + 
                                     gsl_pow_2(mstate.ty())));
         mstate.setQOverP(1./p);
         double errqp2 = gsl_pow_2((200.0 * Gaudi::Units::MeV) / (p * p));
         mstate.setErrQOverP2(errqp2);
         
         double zclose = 0.5*( mstate.z() - mstate.x()/mstate.tx() +
                               mstate.z() - mstate.y()/mstate.ty() ); 

         StatusCode sc = m_pFullExtrap->propagate(mstate, zclose);
         if (sc.isFailure()) return sc;
         
      }
      
    }
    
    track.stateG = mstate; 
    track.unitVect = track.stateG.slopes().Unit();
    pvTracks.push_back(track);
    return StatusCode::SUCCESS;
}

//=============================================================================
// Prepare vertices
//=============================================================================
void PVFitterTool::prepareVertices(LHCb::RecVertex& vtx, 
                                   PVTrackPtrs& pvTracks, 
                                   Gaudi::SymMatrix3x3& hessian,
                                   ROOT::Math::SVector<double,3>& vectorD0)
{

  // Reset hessian and vectorD0
  for(int i = 0; i < 3; i++) {
    vectorD0[i] = 0.0;
    for(int j = 0; j < 3; j++) {
      hessian(i,j) = 0.0;
    }
  }

  if( msgLevel(MSG::DEBUG) ) {
    verbose() << "Extrapolate tracks to the vertex at z = " 
              << vtx.position().z()
              << endmsg;
  }
  
  // Add whole track vector to hessian and vectorD0     
  for(PVTrackPtrs::iterator itrack = pvTracks.begin(); 
      itrack != pvTracks.end();) {
    PVTrack* pvTrack = *itrack;

    StatusCode sc = transport(pvTrack,vtx);

    if(!sc.isSuccess()) {
      if( msgLevel(MSG::DEBUG) ) {
        verbose() << "Track " << pvTrack->refTrack->key()
                  << " could not be transported to vertex" << endmsg;
      }      
      itrack = pvTracks.erase(itrack);

    } else {
      addTrack(pvTrack, hessian, vectorD0);
      if( msgLevel(MSG::DEBUG) ) {
        debug() << "Added track " << pvTrack->refTrack->key() 
                << " Chi2 "       << pvTrack->chi2 
                << " D0 "         << pvTrack->d0 
                << " +- "         << sqrt(pvTrack->err2d0)
                << " zD0 "        << pvTrack->vd0.z()
                << endmsg;
      }
      itrack++;
    }
  }
}


//=============================================================================
// Chi2 exclusion
//=============================================================================
int PVFitterTool::chi2Exclusion(PVTrackPtrs& pvTracks, 
                                Gaudi::SymMatrix3x3& hessian,
                                ROOT::Math::SVector<double,3>& vectorD0,
                                double chi2Cut)
{
  int nRej = 0;

  // Loop over all tracks    
  for(PVTrackPtrs::iterator itrack = pvTracks.begin(); 
      itrack != pvTracks.end();) {
    PVTrack* pvTrack = *itrack;
    if(pvTrack->chi2 >= chi2Cut) {
      if( msgLevel(MSG::DEBUG) ) {
        verbose() << "Removed track  " << pvTrack->refTrack->key() 
                  << " with Chi2 " << pvTrack->chi2 << endmsg;
      }
      removeTrack(pvTrack,hessian,vectorD0);

      // Set the track as not yet extrapolated
      pvTrack->err2d0 = 0;
      pvTrack->chi2 = 0;
      itrack = pvTracks.erase(itrack);
      nRej++;
    } else {
      itrack++;
    }
  }
  return nRej;
}


//=============================================================================
//  removeMaxChi2Track
//=============================================================================
int PVFitterTool::removeMaxChi2Track(PVTrackPtrs& pvTracks, 
                                     Gaudi::SymMatrix3x3& hessian,
                                     ROOT::Math::SVector<double,3>& vectorD0,
                                     double chi2Cut)
{
  int nRej = 0;

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
    removeTrack(pvTrack,hessian,vectorD0);
    pvTracks.erase(itrack_maxchi2);   
    // Set the track as not yet extrapolated
    pvTrack->err2d0 = 0;
    pvTrack->chi2 = 0;
    nRej=1;
  }
  return nRej;
}


//=============================================================================
// Transport
//=============================================================================
StatusCode PVFitterTool::transport(PVTrack* pTrack,
                                   const LHCb::RecVertex& vrtx) {

  Gaudi::XYZPoint trkPoint(pTrack->stateG.x(),
                           pTrack->stateG.y(),
                           pTrack->stateG.z());
  
  // Set the new origin at the current vertex
  Gaudi::XYZVector diffVect = trkPoint - vrtx.position();

  // Compute the distance from the vertex to the track
  pTrack->vd0 = pTrack->unitVect.Cross(diffVect.Cross(pTrack->unitVect));

  // Compute impact parameter d0
  pTrack->d0 = sqrt(pTrack->vd0.Mag2());

  // Use the extrapolator to move the track to the desired position
  ITrackExtrapolator* iExtrap;
  if( msgLevel(MSG::DEBUG) ) {
    verbose() << " Extrapolating track " << pTrack->refTrack->key()
              << " using ";
  }

  if(sqrt(gsl_pow_2(trkPoint.x()) + gsl_pow_2(trkPoint.y())) < m_extrapRCut) {
    // Use the linear extrapolator
    if( msgLevel(MSG::DEBUG) ) verbose() << "linear";
    iExtrap = m_pLinearExtrap;
  } else {
      if( msgLevel(MSG::DEBUG) ) verbose() << "full";
      iExtrap = m_pFullExtrap;
  }

  if( msgLevel(MSG::DEBUG) ) verbose() << " extrapolator" << endmsg;
  LHCb::State stateToMove = pTrack->stateG;
  StatusCode sc =
      iExtrap->propagate(stateToMove,vrtx.position().z() + pTrack->vd0.z());
  if(!sc.isSuccess()) {
    if( msgLevel(MSG::DEBUG) ) {
      verbose() << "Error propagating the track stateG" << endmsg;
    }
    return StatusCode::FAILURE;
  }

  // Compute the error on the impact parameter
  Gaudi::XYZVector vd0Unit = pTrack->vd0.unit();
  ROOT::Math::SVector<double,2> xy;
  xy[0] = vd0Unit.x();
  xy[1] = vd0Unit.y();
  Gaudi::SymMatrix2x2 cov =
    //         pTrack->stateG.covariance().Sub<Gaudi::SymMatrix2x2>(0,0);
         stateToMove.covariance().Sub<Gaudi::SymMatrix2x2>(0,0);
  ROOT::Math::SVector<double,2> cov_times_xy;
  cov_times_xy = cov * xy;
  pTrack->err2d0 = xy[0] * cov_times_xy[0] + xy[1] * cov_times_xy[1];
  pTrack->chi2 = gsl_pow_2(pTrack->d0) / pTrack->err2d0;

  return StatusCode::SUCCESS;
}


//=============================================================================
// Add subTrack
//=============================================================================
void PVFitterTool::addsubTrack(PVTrack* pTrack, 
                               Gaudi::SymMatrix3x3& hessian,
                               ROOT::Math::SVector<double,3>& vectorD0,
                               double weight)
{
  // Should be 1, but here we deweight large chi2 tracks
  if(m_deWeight) {
    weight *= 2.0 / (1.0 + pTrack->chi2);
  }
  weight *= 2.0 / pTrack->err2d0;
  
  double unitVectStd[3];
  unitVectStd[0] = pTrack->unitVect.x();
  unitVectStd[1] = pTrack->unitVect.y();
  unitVectStd[2] = pTrack->unitVect.z(); 

  double vd0Std[3];
  vd0Std[0] = pTrack->vd0.x();
  vd0Std[1] = pTrack->vd0.y();
  vd0Std[2] = pTrack->vd0.z();

  for(int k = 0; k < 3; k++) {
    double vd0 = 0.;
    for (int l = 0; l < 3; l++) {
      double delta_kl = (k == l) ? 1.0 : 0.0;
      double val = delta_kl - unitVectStd[k] * unitVectStd[l];
      vd0 += vd0Std[l] * val * weight;
      if(l <= k) {
        hessian(k,l) += val * weight;
      }
    } // End of counting hessian 
    vectorD0[k] += vd0;
  }
}


void PVFitterTool::addTrack(PVTrack* pTrack, 
                            Gaudi::SymMatrix3x3& hessian,
                            ROOT::Math::SVector<double,3>& vectorD0)
{
  addsubTrack(pTrack,hessian,vectorD0,+1.0);
}


void PVFitterTool::removeTrack(PVTrack* pTrack, 
                               Gaudi::SymMatrix3x3& hessian,
                               ROOT::Math::SVector<double,3>& vectorD0)
{
  addsubTrack(pTrack,hessian,vectorD0,-1.0);
}


//=============================================================================
// New Vertex
//=============================================================================
StatusCode PVFitterTool::newVertex(LHCb::RecVertex& vtx,
                                   PVTrackPtrs& pvTracks,
                                   Gaudi::SymMatrix3x3& hessian,
                                   ROOT::Math::SVector<double,3>& vectorD0)
{
  if( msgLevel(MSG::DEBUG) ) {
    verbose() << "Computing new vertex position" << endmsg;
  }
  
  // Solve the linear equations to get a vertex
  int fail;
  hessian.Inverse(fail);
  if (0 != fail) {
    Error( "Error inverting hessian matrix", 0 );
    return StatusCode::FAILURE;
  } else {
    hessian.Invert();
  }
   
  ROOT::Math::SVector<double,3> delta;
  for(int i = 0; i < 3; i++) {
    delta[i] = 0.0;
  }

  delta = hessian * vectorD0;

  if( msgLevel(MSG::DEBUG) ) {
    debug() << "Displacement found: " 
            << delta[0] << " , " 
            << delta[1] << " , " 
            << delta[2] << " , " << endmsg;
  }
  
  Gaudi::XYZPoint newVtx(vtx.position().x() + delta[0], 
                         vtx.position().y() + delta[1], 
                         vtx.position().z() + delta[2]);
  vtx.setPosition(newVtx);
  if( msgLevel(MSG::DEBUG) ) {
    debug() << " New vertex position: "
            << vtx.position().x() << " , "
            << vtx.position().y() << " , " 
            << vtx.position().z() << endmsg;
  }
  
  // Set cov matrix
  hessian = hessian * 2.0;
  vtx.setCovMatrix(hessian);
  // Set tracks
  vtx.clearTracks();
  for (PVTrackPtrs::iterator itrack = pvTracks.begin(); 
                             itrack != pvTracks.end(); itrack++) {
    vtx.addToTracks((*itrack)->refTrack);
  }

  return StatusCode::SUCCESS;

}


//=============================================================================
// Compute chi2
//=============================================================================
double PVFitterTool::setChi2(LHCb::RecVertex& vtx, PVTrackPtrs& pvTracks)
{

  int nDoF = -3;
  double chi2 = 0.0;
  
  for(PVTrackPtrs::iterator itrack = pvTracks.begin(); 
      itrack != pvTracks.end(); itrack++) {  
    chi2 += (*itrack)->chi2;
    nDoF += 2;
  }

  if( msgLevel(MSG::DEBUG) ) {
    verbose() << " Compute chi2 of this vertex: " << chi2 
              << " for " << nDoF << " DoF ( " 
              << chi2 / nDoF << " / DoF)"
              << endmsg;
  }
  
  vtx.setChi2(chi2);
  vtx.setNDoF(nDoF);
  return chi2 / nDoF;
}

