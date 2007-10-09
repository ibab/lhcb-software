// $Id: PatPVOffline.cpp,v 1.1.1.1 2007-10-09 18:46:14 smenzeme Exp $
//

// from Gaudi
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/AlgFactory.h"

// from Event
#include "Event/Track.h"
#include "Event/State.h"
#include "Event/RecVertex.h"

// Local
#include "PatPVOffline.h"

DECLARE_ALGORITHM_FACTORY( PatPVOffline );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatPVOffline::PatPVOffline(const std::string& name,
                           ISvcLocator* pSvcLocator)
  : GaudiAlgorithm(name,pSvcLocator),
    m_pvSeedTool(0)
{
  declareProperty("UseVelo",       m_useVelo = true);
  declareProperty("UseLong",       m_useLong = true); 
  declareProperty("UseUpstream",   m_useUpstream = true);
  declareProperty("SaveSeedsAsPV", m_saveSeedsAsPV = false);
  declareProperty("InputTracks",   m_inputTracks);
  declareProperty("OutputVertices",m_outputVertices = LHCb::RecVertexLocation::Primary ); 
}

//=============================================================================
// Destructor
//=============================================================================
PatPVOffline::~PatPVOffline() {
};

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode PatPVOffline::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize();
  if (!sc) return sc;
 
  debug() << "Initialisation" << endmsg;

  // Access PVFitterTool
  m_pvfit = tool<IPVFitterTool>("PVFitterTool", this);
  if( !m_pvfit ) {
    err() << "Unable to retrieve the PVFitterTool" << endmsg;
    return  StatusCode::FAILURE;
  }

  // Access PVSeedTool
  m_pvSeedTool = tool<PVSeedTool>( "PVSeedTool", this );
  if( !m_pvSeedTool ) {
    err() << "Unable to retrieve the PVSeedTool" << endmsg;
    return  StatusCode::FAILURE;
  }
  m_vclusters.reserve(500);

  if(m_inputTracks.size() == 0 ) { 
     m_inputTracks.push_back( LHCb::TrackLocation::Default );
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode PatPVOffline::execute() {

  if( msgLevel(MSG::DEBUG) ) debug() << "Execute" << endmsg;
  
  m_vclusters.clear();

  LHCb::RecVertices* v2tes = new LHCb::RecVertices();
  put(v2tes, m_outputVertices);

  StatusCode sc;

//=============================================================================
// Build a vector of PVTracks from the tracks in the input container
//=============================================================================
  if( msgLevel(MSG::VERBOSE) )
      verbose() << "Reading in tracks for PV search" << endmsg;
  PVTracks pvTracks;  
  sc = readTracks( pvTracks);  
  if (sc.isFailure()) {
    return Error( "Error reading tracks, exiting...", sc );
    return sc;
  } else if( msgLevel(MSG::DEBUG) ) {  
    debug() << "Number of tracks read: " << pvTracks.size() << endmsg;
  }
  
//=============================================================================
// Loop to find successively all PV, removing used tracks
//=============================================================================
  if( msgLevel(MSG::DEBUG) ) debug() << "Primary Vertex search" << endmsg;
  int nVtx = 0;

  std::vector<PVTrack>::const_iterator pvTrack;
  for (pvTrack = pvTracks.begin(); pvTracks.end() != pvTrack; pvTrack++) {
    PVTrack pvTr = (*pvTrack);
    LHCb::Track* refTrack = pvTr.refTrack;
    double zc = pvTr.stateG.z();
    double sigsq; 
    m_pvSeedTool->errorForPVSeedFinding(refTrack->firstState().tx(),
                                        refTrack->firstState().ty(), sigsq);

    vtxCluster clu;
    clu.z = zc;
    clu.sigsq = sigsq;
    clu.sigsqmin = clu.sigsq;
    clu.ntracks = 1;
    m_vclusters.push_back(clu);
  }

  std::vector<double> zseeds;
  if( msgLevel(MSG::DEBUG) ) {
    debug() << "Start seeding: " << endmsg;
    debug() << "m_vclusters.size: " << m_vclusters.size()  << endmsg;
  }
  
  m_pvSeedTool->findClusters(m_vclusters,zseeds);

  // Minitor quality of zseeds only
  if(m_saveSeedsAsPV) {
    storeDummyVertices(zseeds,pvTracks,v2tes);
    return StatusCode::SUCCESS;
  }

  if( msgLevel(MSG::DEBUG) ) {
    debug() << "Nr of seeds: " << zseeds.size() << " " << zseeds << endmsg;
  }
  
  std::vector<double>::iterator itzseed;
  for(itzseed = zseeds.begin(); itzseed != zseeds.end(); itzseed++) {

    double zseed = *itzseed;
    if( msgLevel(MSG::DEBUG) ) {
      debug() << "Vtx search for seed at z:  " << zseed << endmsg;
    }
    
    PVVertex pvVertex;

//=============================================================================
// Collect compatible tracks
//=============================================================================
    sc = collectTracks(pvTracks,pvVertex,zseed);
    if (sc.isFailure()) return sc;

    if( msgLevel(MSG::DEBUG) ) {
      verbose() << "Vertex " << nVtx << " +++" << endmsg;
      debug() << "Fitting vertex starting at z = " 
              << pvVertex.primVtx.position().z()
              << " with " << pvVertex.pvTracks.size() << " tracks" << endmsg;
    }

//=============================================================================
// Fit the vertex, removing tracks if not used for the vertex
//=============================================================================
    StatusCode scvfit = m_pvfit->fit(pvVertex.primVtx, pvVertex.pvTracks);

    if(scvfit == StatusCode::SUCCESS) {

      // Create a new vertex in the container.   
      LHCb::RecVertex* vertex = new LHCb::RecVertex();
      copyTrackVertex(&(pvVertex.primVtx),vertex);

      vertex->setTechnique(LHCb::RecVertex::Primary);
      v2tes->insert(vertex);

      // Remove used tracks from list
      PVTrackPtrs::iterator itTrr;
      for(itTrr = pvVertex.pvTracks.begin(); itTrr != pvVertex.pvTracks.end();
          itTrr++) {
        (*itTrr)->isUsed = true;
      }
      PVTracks::iterator itPVr;
      for(itPVr = pvTracks.begin(); itPVr != pvTracks.end();) {
        if(itPVr->isUsed) {
          itPVr = pvTracks.erase(itPVr);
        } else {
          itPVr++;
        }
      }   
      if( msgLevel(MSG::DEBUG) ) {
        debug() << "Vertex " << nVtx << " successfully fitted "
                << endmsg << endmsg;
      }
      nVtx++;
    }
  }
 
    // debug
  if( msgLevel(MSG::DEBUG) ) {
    debug() << endmsg;
    debug() << "TES location filled with ";
    debug() << v2tes->size() << " PrimVertices" << endmsg;
   
    nVtx = 0;
    for(LHCb::RecVertices::iterator itVtx = v2tes->begin();
        v2tes->end() != itVtx; itVtx++, nVtx++) {
      LHCb::RecVertex* vertex = (*itVtx);
      debug() << " Vertex " << nVtx << endmsg;
      debug() << " x, y, z: " 
              << vertex->position().x() << " "
              << vertex->position().y() << " "
              << vertex->position().z() << endmsg;
      debug() << " Errors : "
              << sqrt(vertex->covMatrix()(0,0)) << " "
              << sqrt(vertex->covMatrix()(1,1)) << " "
              << sqrt(vertex->covMatrix()(2,2)) << endmsg;
      debug() << " Number of tracks: " 
              << vertex->tracks().size() << endmsg;
      debug() << " Chi2/DoF: " << vertex->chi2()/vertex->nDoF() << endmsg;
    }      
    debug() << endmsg;
  }
  
  return StatusCode::SUCCESS;
}


//=============================================================================
// readTracks
//=============================================================================
StatusCode PatPVOffline::readTracks( PVTracks& pvTracks) 
{
  if( msgLevel(MSG::VERBOSE) ) {
    verbose() << "readTracks procedure" << endmsg;
  }
  StatusCode sc = StatusCode::SUCCESS;

  unsigned int ntrall = 0;
  std::vector< std::string >:: iterator itTrName;
  for (itTrName = m_inputTracks.begin(); itTrName != m_inputTracks.end(); itTrName++) {

    // Retrieving tracks
    LHCb::Tracks* stracks = get<LHCb::Tracks>(*itTrName);
    if(!stracks) {
      warning() << "Tracks not found at location: " << *itTrName << endmsg; 
      continue;
    }

    ntrall += stracks->size();

    // Read parameters
    for (LHCb::Tracks::iterator istrack = stracks->begin(); 
         stracks->end() != istrack; istrack++) {
  
      LHCb::Track* str = (*istrack);
  
      if(((str->type() == LHCb::Track::Velo) && m_useVelo) || 
         ((str->type() == LHCb::Track::Long) && m_useLong) || 
         ((str->type() == LHCb::Track::Upstream) && m_useUpstream)) {
  
        // Create a new working track
        sc = m_pvfit->addTrackForPV(str, pvTracks);
        if( !sc.isSuccess() ) continue;
        
      }
    }
    if( msgLevel(MSG::DEBUG) ) {
      debug() << "readTracks: ntracks (accepted / all) = " << pvTracks.size()
              << " / " << ntrall << " " << *itTrName << endmsg;
    }
  }

  if( msgLevel(MSG::DEBUG) ) {
    debug() << "readTracks: ntracks (accepted / all) = " 
            << pvTracks.size() << " / " << pvTracks.size() << endmsg;
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// collectTracks
//=============================================================================
StatusCode PatPVOffline::collectTracks( PVTracks& pvTracks, PVVertex& pvVtx, 
                                        double zseed) 
{
  if( msgLevel(MSG::VERBOSE) ) {
    verbose() << "=> collectTracks " << endmsg;
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

  if( msgLevel(MSG::DEBUG) ) {
    debug() << " Collected " << nTracks << " tracks out of " <<  pvTracks.size()
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
//  Copy original TrackVertex 
//=============================================================================
void PatPVOffline::copyTrackVertex(LHCb::RecVertex* inTrackVertex,
                                         LHCb::RecVertex* outTrackVertex)
{
    if( msgLevel(MSG::DEBUG) ) {
      debug() << "==> Enter copyTrackVertex" << endmsg;
    }
    
    outTrackVertex->setPosition(inTrackVertex->position());
    outTrackVertex->setCovMatrix(inTrackVertex->covMatrix());
    outTrackVertex->setChi2(inTrackVertex->chi2());
    outTrackVertex->setNDoF(inTrackVertex->nDoF());
    outTrackVertex->setTracks(inTrackVertex->tracks());
    if( msgLevel(MSG::DEBUG) ) {
      debug() << "==> Exit copyTrackVertex" << endmsg;
    }
    
}


//=============================================================================
//  storeDummyVertices
//=============================================================================
void PatPVOffline::storeDummyVertices(std::vector<double>& zseeds, 
                                      std::vector<PVTrack>& pvTracks, 
                                      LHCb::RecVertices* v2tes) {

  std::vector<double>::iterator itzseed;
  for (itzseed = zseeds.begin(); itzseed != zseeds.end(); itzseed++) {
    LHCb::RecVertex* tVertex = new LHCb::RecVertex();
    Gaudi::XYZPoint xyz(0.0,0.0,*itzseed);
    tVertex -> setPosition( xyz );
    Gaudi::SymMatrix3x3 errMat;
    for(int i = 0; i < 3; i++) {
      for(int j = 0; j < 3; j++) {
        errMat(i,j) = 1.0;
      }
    }
    tVertex -> setCovMatrix(errMat);
    tVertex->setNDoF(1);
    tVertex -> setChi2(99999.0);
    // Fill close tracks
    std::vector<PVTrack>::const_iterator pvTrack;
    for(pvTrack = pvTracks.begin(); pvTracks.end() != pvTrack; pvTrack++) {
      PVTrack pvTr = (*pvTrack);
      double zc = pvTr.zClose();
      if(fabs(zc - *itzseed) < 3.0 * Gaudi::Units::mm) {
        LHCb::Track* trk = pvTr.refTrack;
        tVertex -> addToTracks(trk);
      }
    }
    v2tes->insert(tVertex);
  }

}
