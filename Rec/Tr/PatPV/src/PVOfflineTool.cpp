// Include files:
// from Gaudi
#include "GaudiKernel/SystemOfUnits.h"
// from Event
#include "Event/Track.h"
#include "Event/State.h"
#include "Event/RecVertex.h"
#include "DetDesc/Condition.h"
#include "GaudiKernel/IUpdateManagerSvc.h"
// Local
#include "PVOfflineTool.h"

DECLARE_TOOL_FACTORY(PVOfflineTool)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PVOfflineTool::PVOfflineTool(const std::string& type,
                             const std::string& name,
                             const IInterface* parent)
  : GaudiTool(type,name,parent),
    m_requireVelo(false),
    m_saveSeedsAsPV(false),
    m_outputVertices(""),
    m_inputVertices(0),
    m_pvfit(0),
    m_pvSeedTool(0),
    m_pvRecalc(0),
    m_inputTracks(0),
    m_pvFitterName(""),
    m_pvSeedingName(""),
    m_pvsChi2Separation(0.0),
    m_pvsChi2SeparationLowMult(0.0),
    m_useBeamSpotRCut(false),
    m_beamSpotRCut(0.3),
    m_beamSpotX(0.),
    m_beamSpotY(0.),
    m_beamSpotCond(""),
    m_veloClosed(false),
    m_timerTool(0)
{
  declareInterface<IPVOfflineTool>(this);
  declareProperty("RequireVelo"      , m_requireVelo   = true);
  declareProperty("SaveSeedsAsPV"    , m_saveSeedsAsPV = false);
  declareProperty("InputTracks"      , m_inputTracks);
  declareProperty("InputVerticesName", m_inputVerticesName = LHCb::RecVertexLocation::Velo3D);
  declareProperty("PVFitterName"     , m_pvFitterName  = "LSAdaptPVFitter");
  declareProperty("PVSeedingName"    , m_pvSeedingName = "PVSeed3DTool");
  declareProperty("LookForDisplaced" , m_lookForDisplaced = false);
  declareProperty("PVsChi2Separation", m_pvsChi2Separation = 25.);
  declareProperty("PVsChi2SeparationLowMult", m_pvsChi2SeparationLowMult = 91.);
  declareProperty("UseBeamSpotRCut",   m_useBeamSpotRCut  = false );
  declareProperty("BeamSpotRCut",      m_beamSpotRCut = 0.3 );
  declareProperty("ResolverBound",     m_resolverBound = 5 * Gaudi::Units::mm );
  declareProperty("TimingMeasurement", m_doTiming       = false     );
  
}

//=========================================================================
// Initialize
//=========================================================================
StatusCode PVOfflineTool::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if (!sc) return sc;

  m_beamSpotCond = "/dd/Conditions/Online/Velo/MotionSystem";
  if (m_useBeamSpotRCut ) {
    IUpdateManagerSvc* m_updMgrSvc = svc<IUpdateManagerSvc>("UpdateManagerSvc", true);
    m_updMgrSvc->registerCondition(this, m_beamSpotCond, &PVOfflineTool::UpdateBeamSpot);
    StatusCode scu = m_updMgrSvc->update(this);
    if(!scu.isSuccess()) 
      return Error("Failed to update conditions!",StatusCode::FAILURE);
  }  

  // Access PVFitterTool
  m_pvfit = tool<IPVFitter>(m_pvFitterName,this);
  if(!m_pvfit) {
    err() << "Unable to retrieve the PVFitterTool" << endmsg;
    return  StatusCode::FAILURE;
  }
  // Access PVSeedTool
  m_pvSeedTool = tool<IPVSeeding>(m_pvSeedingName,this);
  if(!m_pvSeedTool) {
    err() << "Unable to retrieve the PV seeding tool " << m_pvSeedingName << endmsg;
    return  StatusCode::FAILURE;
  }
  // Access PVOFflineRecalculate tool
  m_pvRecalc = tool<PVOfflineRecalculate>("PVOfflineRecalculate", this);
  //  m_pvRecalc = tool<PVOfflineRecalculateWG1>("PVOfflineRecalculateWG1", this);
  if(!m_pvRecalc) {
    err() << "Unable to retrieve the PV recalculation tool " << endmsg;
    return  StatusCode::FAILURE;
  }
  

  // Input tracks
  if(m_inputTracks.size() == 0) { 
    m_inputTracks.push_back(LHCb::TrackLocation::Default);
  }

  if ( m_doTiming) {
    m_timerTool = tool<ISequencerTimerTool>( "SequencerTimerTool/Timer", this );
    m_timeTotal   = m_timerTool->addTimer( "PatPV total" );
    m_timerTool->increaseIndent();
    m_timeSeeding = m_timerTool->addTimer( "PatPV seeding" );
    m_timeFitting = m_timerTool->addTimer( "PatPV fitting" );
    m_timerTool->decreaseIndent();
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Destructor
//=============================================================================
PVOfflineTool::~PVOfflineTool() {}

//=============================================================================
// reconstruct PV for a given seed
//=============================================================================
StatusCode PVOfflineTool::reconstructSinglePV(const Gaudi::XYZPoint xyzseed,
                                              LHCb::RecVertex& outvtx)  
{
  std::vector<const LHCb::Track*> tracks2remove;
  std::vector<const LHCb::Track*> rtracks;
  
  readTracks(rtracks); 

  StatusCode scvfit = m_pvfit->fitVertex( xyzseed, rtracks, outvtx, tracks2remove );
  return scvfit;
}
//=============================================================================
// reconstruct PV for a given seed using tracks from the list
//=============================================================================
StatusCode PVOfflineTool::reconstructSinglePVFromTracks(const Gaudi::XYZPoint xyzseed,
                                                        std::vector<const LHCb::Track*>& tracks2use,
                                                        LHCb::RecVertex& outvtx)  
{
  std::vector<const LHCb::Track*> tracks2remove;
  StatusCode scvfit = m_pvfit->fitVertex( xyzseed, tracks2use, outvtx, tracks2remove );
  return scvfit;
}

//=============================================================================
// reconstruct PV for a given seed with a list of tracks to be excluded
//=============================================================================
StatusCode PVOfflineTool::reDoSinglePV(const Gaudi::XYZPoint xyzseed,
                                       std::vector<const LHCb::Track*>& tracks2exclude,
                                       LHCb::RecVertex& outvtx)
{

  std::vector<const LHCb::Track*> rtracks;
  readTracks(rtracks); 
  if(tracks2exclude.size()>0) {
    removeTracksByLHCbIDs(rtracks, tracks2exclude);
  }
  std::vector<const LHCb::Track*> tracks2remove;
  StatusCode scvfit = m_pvfit->fitVertex( xyzseed, rtracks, outvtx, tracks2remove );
  return scvfit;

}


//=============================================================================
// multi vtx search and fit. Return new vtx after track removal
//=============================================================================
StatusCode PVOfflineTool::reDoMultiPV(const LHCb::RecVertex& invtx, 
                                      std::vector<const LHCb::Track*>& tracks2exclude, 
                                      LHCb::RecVertex& outvtx)
{
  std::vector<LHCb::RecVertex> outvtxVec;
  std::vector<const LHCb::Track*> rtracks;
  readTracks(rtracks); 

  if(tracks2exclude.size()>0) {
    removeTracksByLHCbIDs(rtracks, tracks2exclude);
  }

  StatusCode scvfit = reconstructMultiPVFromTracks(rtracks,outvtxVec); 

  if(scvfit != StatusCode::SUCCESS) return scvfit;
  // check which vtx corresponds to input vtx
  StatusCode scmatch = matchVtxByTracks(invtx, outvtxVec, outvtx);
  return scmatch;
}

//=============================================================================
// multi vtx search and fit with tracks from default location
//=============================================================================
StatusCode PVOfflineTool::reconstructMultiPV(std::vector<LHCb::RecVertex>& outvtxvec)  
{
  std::vector<const LHCb::Track*> rtracks;
  readTracks(rtracks); 
  StatusCode scvfit = reconstructMultiPVFromTracks(rtracks, outvtxvec); 
  return scvfit;
}


//=============================================================================
// multi vtx search and fit with tracks specified
//=============================================================================
StatusCode PVOfflineTool::reconstructMultiPVFromTracks(std::vector<const LHCb::Track*>& tracks2use,
                                                       std::vector<LHCb::RecVertex>& outvtxvec)
{

  if ( m_doTiming ){
    m_timerTool->start( m_timeTotal );
  }

  std::vector<const LHCb::Track*> rtracks;
  rtracks = tracks2use;
  
  outvtxvec.clear();
  
  if (m_saveSeedsAsPV) {
    std::vector<Gaudi::XYZPoint> seeds;
    Gaudi::XYZPoint beamspot(m_beamSpotX, m_beamSpotY, 0.0);
    m_pvSeedTool->getSeeds(rtracks, beamspot, seeds); 
    storeDummyVertices(seeds, rtracks, outvtxvec);
    return StatusCode::SUCCESS;
  }
  
  int nvtx_before = -1;
  int nvtx_after  =  0;
  while ( nvtx_after > nvtx_before ) {
    nvtx_before = outvtxvec.size();
    // reconstruct vertices
    std::vector<Gaudi::XYZPoint>::iterator is;

    std::vector<Gaudi::XYZPoint> seeds;
    Gaudi::XYZPoint beamspot(m_beamSpotX, m_beamSpotY, 0.0);

    if ( m_doTiming ){
      m_timerTool->start( m_timeSeeding );
    }

    // seeding
    m_pvSeedTool->getSeeds(rtracks, beamspot, seeds);  


    if ( m_doTiming ){
      m_timerTool->stop( m_timeSeeding );
    }

    //TIMEBOMB fixed
    if (m_pvfit==NULL) 
      return Warning("m_pvfit is null ", StatusCode::FAILURE);

    if(msgLevel(MSG::DEBUG)) {
      debug() << " seeds  " << seeds.size()
              << " pos: " << seeds << endmsg;
    }
    for (is = seeds.begin(); is != seeds.end(); is++) {
      LHCb::RecVertex recvtx;
      //    Gaudi::XYZPoint *pxyz = &is;
      if(msgLevel(MSG::VERBOSE)) {
        verbose() << "ready to fit" << endmsg;
      }
      
      std::vector<const LHCb::Track*> tracks2remove;
      if ( m_doTiming ){
        m_timerTool->start( m_timeFitting );
      }

      // fitting
      StatusCode scvfit = m_pvfit->fitVertex( *is, rtracks, recvtx, tracks2remove );

      if ( m_doTiming ){
        m_timerTool->stop( m_timeFitting );
      }

      if(scvfit == StatusCode::SUCCESS) {
        bool isSepar = separatedVertex(recvtx,outvtxvec);
        bool inR = true;
        if ( m_useBeamSpotRCut && m_veloClosed ) {
          double dx = recvtx.position().x() - m_beamSpotX; 
          double dy = recvtx.position().y() - m_beamSpotY; 
          if ( std::sqrt(dx*dx + dy*dy) > m_beamSpotRCut ) inR = false;
        }
        if ( isSepar && inR ) {
          outvtxvec.push_back(recvtx);
          removeTracks(rtracks, tracks2remove);
        }
      }
    }//iterate on seeds
    nvtx_after = outvtxvec.size();   
  }//iterate on vtx

  if ( m_doTiming ) m_timerTool->stop( m_timeTotal );

  return StatusCode::SUCCESS;

}


bool PVOfflineTool::separatedVertex( LHCb::RecVertex& rvtx, std::vector<LHCb::RecVertex>& outvtxvec) {

  if(outvtxvec.size() < 1 ) return true;

  bool vsepar = true;
  double chi2min = 1e10;
  
  std::vector<LHCb::RecVertex>::iterator itv, itvclosest;
  for (itv = outvtxvec.begin(); itv != outvtxvec.end(); itv++) {
    double distz = (rvtx.position().z() - (*itv).position().z());
    double dist2z = distz*distz;
    double sigma2z_1 =   rvtx.covMatrix()(2,2); 
    double sigma2z_2 = (*itv).covMatrix()(2,2); 
    double chi2 = dist2z/(sigma2z_1+sigma2z_2);
    if ( chi2 < chi2min ) {
      itvclosest = itv;  
      chi2min = chi2;
    }
  }

  if ( chi2min < m_pvsChi2Separation ) vsepar = false;
  // protect secondary vertices of B signal
  if ( chi2min < m_pvsChi2SeparationLowMult && rvtx.tracks().size() < 7 ) vsepar = false;
  return vsepar;

}

//=============================================================================
// Read tracks
//=============================================================================
void PVOfflineTool::readTracks(std::vector<const LHCb::Track*>& rtracks) 
{

  if(msgLevel(MSG::VERBOSE)) {
    verbose() << "readTracks method" << endmsg;
  }

  unsigned int ntrall = 0;
  std::vector<std::string>::iterator itTrName;
  for(itTrName = m_inputTracks.begin(); itTrName != m_inputTracks.end(); 
      itTrName++) {
    // Retrieving tracks
    LHCb::Tracks* stracks = get<LHCb::Tracks>(*itTrName);
    if(!stracks) {
      warning() << "Tracks not found at location: " << *itTrName << endmsg; 
      continue;
    }
    ntrall += stracks->size();
    // Read parameters
    for(LHCb::Tracks::iterator istrack = stracks->begin(); 
        stracks->end() != istrack; istrack++) {
      const LHCb::Track* str = (*istrack);
      if ( str->hasVelo() ) {
     	  rtracks.push_back(*istrack);
      } else if ( ! m_requireVelo ) {
        rtracks.push_back(*istrack);
      }
    }
  }

  //remove tracks used by the other vertices to look for displaced vertices
  if(m_lookForDisplaced){
    m_inputVertices = get<LHCb::RecVertices>(m_inputVerticesName);
    if(!m_inputVertices) {
      warning() << "Vertices not found at location: " << m_inputVerticesName << endmsg;
    }
    for (LHCb::RecVertices::iterator ivt = m_inputVertices->begin(); ivt != m_inputVertices->end(); ivt++){
      LHCb::RecVertex* vrt = *ivt;
      removeTracksUsedByVertex(rtracks, *vrt);
    }
  }

  if(msgLevel(MSG::DEBUG)) {
    debug() << "readTracks: " << rtracks.size() << endmsg;
  }
  
}

//=============================================================================
// removeTracks
//=============================================================================
void PVOfflineTool::removeTracks(std::vector<const LHCb::Track*>& tracks, 
                                 std::vector<const LHCb::Track*>& tracks2remove)
{
  if(msgLevel(MSG::VERBOSE)) {
    verbose() << " removeTracks method" << endmsg;
  }

  if(msgLevel(MSG::DEBUG)) {
    debug() << "removeTracks. Input number of tracks: " << tracks.size() 
            << "  To remove:" << tracks2remove.size() << endmsg;
  }

  std::vector<const LHCb::Track*>::iterator itr;
  for(itr = tracks2remove.begin(); itr != tracks2remove.end(); itr++) {
    const LHCb::Track* ptr = (*itr);
    std::vector<const LHCb::Track*>::iterator itf = std::find(tracks.begin(),tracks.end(), ptr); 
    if(itf != tracks.end()) {
      tracks.erase(itf);
    }
  }

  if(msgLevel(MSG::DEBUG)) {
    debug() << "removeTracks. Output number of tracks: " << tracks.size() << endmsg;
  }
}

//=============================================================================
// removeTracks
//=============================================================================
void PVOfflineTool::removeTracksByLHCbIDs(std::vector<const LHCb::Track*>& tracks, 
                                          std::vector<const LHCb::Track*>& tracks2remove)
{
  if(msgLevel(MSG::VERBOSE)) {
    verbose() << " removeTracks method" << endmsg;
  }

  if(msgLevel(MSG::DEBUG)) {
    debug() << "removeTracksByLHCbIDs. Input number of tracks: " << tracks.size() << endmsg;
  }

  std::vector<const LHCb::Track*>::iterator itr1,itr2;
  for(itr1 = tracks2remove.begin(); itr1 != tracks2remove.end(); itr1++) {
    const LHCb::Track* ptr1 = (*itr1);

    for (itr2 = tracks.begin(); itr2 != tracks.end(); itr2++) {
      const LHCb::Track* ptr2 = (*itr2);

      // Calculate the number of common LHCbIDs
      const std::vector< LHCb::LHCbID > & ids1 = ptr1->lhcbIDs();
      const std::vector< LHCb::LHCbID > & ids2 = ptr2->lhcbIDs();

      std::vector< LHCb::LHCbID >::const_iterator ih1,ih2;

      int nhit2 = 0;
      int nCommon =0;
      for ( ih2 = ids2.begin(); ih2 != ids2.end(); ++ih2 ) {
        if ( !ih2->isVelo()) continue;
        nhit2++;
        for ( ih1 = ids1.begin(); ih1 != ids1.end(); ++ih1 ) {
          if ( !ih1->isVelo()) continue;
          //          if ( ids1[i1].channelID() == ids2[i2].channelID() ) {
          if ( *ih1 == *ih2 ) {
            ++nCommon;
            break;
          }
        }
      }

      double rate = 0.;
      if(nhit2>0) rate = 1.0*nCommon/nhit2;
      if(rate>0.99) {
        tracks.erase(itr2);
        break;          
      }
    } // over tracks
  } // over tracks2remove

  if(msgLevel(MSG::DEBUG)) {
    debug() << "removeTracksByLHCbIDs. Output number of tracks: " << tracks.size() << endmsg;
  }
}

//=============================================================================
// removeTracksUsedByVertex
//=============================================================================
void PVOfflineTool::removeTracksUsedByVertex(std::vector<const LHCb::Track*>& tracks,
                                             LHCb::RecVertex& rvtx)

{
  if(msgLevel(MSG::VERBOSE)) {
    verbose() << " removeTracksUsedByVertex method" << endmsg;
  }

  std::vector<const LHCb::Track*> tracks2remove;
  //  std::vector<LHCb::Track*>::iterator it;
  //  LHCb::Tracks::const_iterator it;
  SmartRefVector< LHCb::Track >  vtx_tracks = rvtx.tracks();
  for (unsigned int it = 0; it < vtx_tracks.size(); it++ ) {
    const LHCb::Track* ptr =  vtx_tracks[it];
    tracks2remove.push_back(ptr); 
  }
  removeTracks(tracks, tracks2remove);
  tracks2remove.clear();
  
  if(msgLevel(MSG::VERBOSE)) {
    verbose() << " leaving removeTracksUsedByVertex method" << endmsg;
  }
}

//=============================================================================
// Store dummy vertices
//=============================================================================
void PVOfflineTool::storeDummyVertices(std::vector<Gaudi::XYZPoint>& seeds,
                                       std::vector<const LHCb::Track*> rtracks, 
                                       std::vector<LHCb::RecVertex>& outvtxvec)
{
  std::vector<Gaudi::XYZPoint>::iterator itseed;
  for (itseed = seeds.begin(); itseed != seeds.end(); itseed++) {
    LHCb::RecVertex tVertex;
    tVertex.setPosition( *itseed );
    Gaudi::SymMatrix3x3 errMat;
    for(int i = 0; i < 3; i++) {
      for(int j = 0; j < 3; j++) {
        errMat(i,j) = 1.0;
      }
    }
    tVertex.setCovMatrix(errMat);
    tVertex.setNDoF(1);
    tVertex.setChi2(99999.0);
    // Fill close tracks
    std::vector<const LHCb::Track*>::iterator it;
    for(it = rtracks.begin(); rtracks.end() != it; it++) {
      const LHCb::Track * ptr = (*it);
      double zc = zCloseBeam(ptr);
      if(fabs(zc - itseed->Z()) < 3.0 * Gaudi::Units::mm) {
        tVertex. addToTracks(ptr);
      }
    }
    outvtxvec.push_back(tVertex);
  }
}

double PVOfflineTool::zCloseBeam(const LHCb::Track* track){
   

  Gaudi::XYZVector unitVect;
  unitVect = track->firstState().slopes().Unit();
  LHCb::State& stateG = track->firstState(); 

  double zclose = stateG.z() - unitVect.z() * 
    (unitVect.x() * stateG.x() + unitVect.y() * stateG.y()) /
    (1.0 - pow(unitVect.z(),2)); 

  return zclose;

}



//=============================================================================
// Match vtx in vector of vtx by matching tracks
//=============================================================================
StatusCode PVOfflineTool::matchVtxByTracks(const LHCb::RecVertex& invtx,  
                                           std::vector<LHCb::RecVertex>& outvtxvec, 
                                           LHCb::RecVertex& outvtx) 
{
  typedef const SmartRefVector<LHCb::Track>      PVTRACKS ;
  PVTRACKS& tracksIn =  invtx.tracks();
  PVTRACKS::const_iterator itin,itout;
  int ntrin = tracksIn.size();

  if( ntrin == 0 ) return StatusCode::FAILURE;

  double lastrate = 0.;
  std::vector<LHCb::RecVertex>::iterator itv, itbest;
  itbest = outvtxvec.end();

  for (itv = outvtxvec.begin(); itv != outvtxvec.end(); itv++) {
    int imatch = 0;
    PVTRACKS& tracksOut =  (*itv).tracks();
    for ( itin = tracksIn.begin(); itin !=tracksIn.end(); itin++) {
      itout = std::find(tracksOut.begin(),tracksOut.end(), *itin );
      if(itout != tracksOut.end() ) imatch++;
    }
    double rate = 1.0*imatch/ntrin;
    if (rate > lastrate ) {
      lastrate = rate;
      itbest = itv;
    }
  }

  if ( itbest == outvtxvec.end() && lastrate < 0.3 ) {
    return StatusCode::FAILURE;
  }

  outvtx = *itbest;
  debug() << " vtx succesfully matched at tracks rate: " << lastrate << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode PVOfflineTool::removeTracksAndRecalculatePV(const LHCb::RecVertex* pvin,
					const std::vector<const LHCb::Track*>& tracks2remove,
					LHCb::RecVertex& vtx) 
{
  return m_pvRecalc->RecalculateVertex(pvin, tracks2remove, vtx);  
}

//=============================================================================
// Update of the beam spot position
//=============================================================================
StatusCode PVOfflineTool::UpdateBeamSpot()
{
  if (! exist<Condition>(detSvc(), m_beamSpotCond )){ 
    Warning( "Unable to locate beam spot condition" ) ;
    return StatusCode::FAILURE;
  }
  Condition *myCond =  get<Condition>(detSvc(), m_beamSpotCond );
  //
  const double xRC = myCond -> paramAsDouble ( "ResolPosRC" ) ;
  const double xLA = myCond -> paramAsDouble ( "ResolPosLA" ) ;
  const double   Y = myCond -> paramAsDouble ( "ResolPosY"  ) ;
  //
  m_beamSpotX = ( xRC + xLA ) / 2;
  m_beamSpotY = Y ;

  m_veloClosed = (std::abs ( xRC - m_beamSpotX ) < m_resolverBound && 
                  std::abs ( xLA - m_beamSpotX ) < m_resolverBound);

  return StatusCode::SUCCESS;
}
