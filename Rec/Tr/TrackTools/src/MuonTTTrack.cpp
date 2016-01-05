// Include files 

// local
#include "MuonTTTrack.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MuonTTTrack
//
// 2010-09-14 : Michel De Cian
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MuonTTTrack )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonTTTrack::MuonTTTrack( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_ttHitAddingTool(NULL)
  , m_lhcbid2mcparticles(NULL)
  , m_muonPad2MC(NULL)
  , m_trackTool(NULL)
  , m_momentumTool(NULL)
  , m_extrapolator(NULL)
  , m_trackFitter(NULL)
{
  declareProperty( "ToolName",            m_trackToolName     = "MuonNNetRec"             );
  declareProperty( "Extrapolator",        m_extrapolatorName  = "TrackMasterExtrapolator" );
  declareProperty( "MC",                  m_MC                = false                     );
  declareProperty( "AddTTHits",           m_addTTHits         = true                      );
  declareProperty( "FillMuonStubInfo",    m_fillMuonStubInfo  = false                     );
  declareProperty( "Output",              m_outputLoc         = "Rec/"+name+"/Tracks"     );
  declareProperty( "MinNTTHits",          m_minNumberTTHits   = 2                         );
  declareProperty( "PVLocation",          m_pvLoc             = LHCb::RecVertexLocation::Primary          );
}
//=============================================================================
// Destructor
//=============================================================================
MuonTTTrack::~MuonTTTrack() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode MuonTTTrack::initialize() {
  const StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  // -- Tracking tools
  m_trackTool = tool<IMuonTrackRec>(m_trackToolName, this);
  m_extrapolator   = tool<ITrackExtrapolator>( m_extrapolatorName, this );
  m_trackFitter = tool<ITrackFitter>("TrackMasterFitter",this);

  m_momentumTool = tool<IMuonTrackMomRec>("MuonTrackMomRec", this);
  m_ttHitAddingTool = tool<IAddTTClusterTool>("PatAddTTCoord",this);

  // -- MC tools
  if(m_MC){
    m_muonPad2MC = tool<IMuonPad2MCTool>("MuonPad2MCTool", this);
    m_lhcbid2mcparticles = tool<ILHCbIDsToMCParticles>("LHCbIDsToMCParticles", this);
  }
  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode MuonTTTrack::execute() {

  //Chrono chrono( chronoSvc(),name()+"::execute()" );

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // -- Fill PVs
  std::vector<double> PVPos;
  PVPos.reserve(3);
  PVPos.push_back( 0 );
  PVPos.push_back( 0 );
  PVPos.push_back( 0 );
  StatusCode sc = fillPVs(PVPos);
  if (!sc) Warning( "==> Could not find PVs!" ).ignore();

  const std::vector<MuonTrack*>* muonTracks = m_trackTool->tracks();
  std::unique_ptr<LHCb::Tracks> tracks{new LHCb::Tracks()};

  // -- Loop over all Muon Tracks
  for (std::vector<MuonTrack*>::const_iterator it = muonTracks->begin() ;
       it != muonTracks->end() ; it++){

    const LHCb::MCParticle* bestMCPart = nullptr;

    // -- Associate Muon Hits to an MCParticle, get momentum components
    double truePx = 0;
    double truePy = 0;
    double truePz = 0;
    double trueP = 0;

    if(m_MC){
      bestMCPart = assocMCParticle((*it)->getHits());
      if(bestMCPart){
        truePx = bestMCPart->momentum().X();
        truePy = bestMCPart->momentum().Y();
        truePz = bestMCPart->momentum().Z();
        trueP = bestMCPart->p();
      }
    }
    // ---------------------------------------------------------------


    // -- Get the momentum of the muon track, make a new track
    //m_momentumTool->recMomentum( (*it), track, PVPos);
    std::unique_ptr<LHCb::Track> track{ new LHCb::Track() };
    sc = m_momentumTool->recMomentum( (*it), track.get());
    if(!sc) continue;
    // ---------------------------------------------------------------

    // -- Change Q/p until it points to the PV (stolen from Wouter)
    LHCb::State muonState;
    sc = iterateToPV(track.get(), muonState, PVPos, (*it)->qOverP() ); // -- This is the function that iterates
    if(!sc){
      Warning( "==> Could not iterate to PV!", StatusCode::SUCCESS, 0 ).ignore();
      continue;
    }

    LHCb::State veloState = muonState;
    sc = m_extrapolator->propagate( veloState, PVPos[2]);
    if(!sc){
      Warning( "==> Could not propagate state to VELO!", StatusCode::SUCCESS, 0 ).ignore();
      continue;
    }
    veloState.setLocation( LHCb::State::Vertex );
    // ---------------------------------------------------------------

    // -- Collect the TT hits
    PatTTHits ttHits;
    double finalChi2;
    if(m_addTTHits){
      sc = m_ttHitAddingTool->returnTTClusters( veloState, ttHits, finalChi2,0);
      if(!sc) Warning( "Could not find TT hits" ).ignore();
      if(msgLevel(MSG::DEBUG) ) debug() << "Found " << ttHits.size() << " TT hits to add" << endmsg;

      // -- Skip if not enough TT hits were found
      if( ttHits.size() < m_minNumberTTHits ){
        if(msgLevel(MSG::DEBUG)) debug() << "==> Not enough hits in TT found" << endmsg;
        continue;
      }

      // -- Add the TT hits
      for(const auto& h : ttHits) track->addToLhcbIDs( h->hit()->lhcbID() );
    }
    // ---------------------------------------------------------------

    // -- Set Pattern Reco status and track type, finally fit the track
    track->setPatRecStatus(  LHCb::Track::PatRecIDs );
    track->setType( LHCb::Track::Long ); // -- This is not so nice, as it is not really a long track
    track->setChi2PerDoF(1000); // -- Try to set a dummy value to satisfy the tupletool
    track->setNDoF(1);

    if(m_addTTHits){
      sc = m_trackFitter->fit(*track, LHCb::ParticleID(track->charge()*13) );
      if(!sc && msgLevel(MSG::DEBUG) ) debug() << "Fit failed" << endmsg;
    }else{
      track->clearStates();
      track->addToStates(veloState);
      track->addToStates(muonState);
    }
    // ---------------------------------------------------------------

    // -- Get parameters of Muon Stub, add it to the extra info field
    // -- (this is NOT a nice way, hence its off per default)
    if(m_fillMuonStubInfo){
      fillMuonStubInfo(*track, **it);

      // -- Add some mroe general information
      track->addInfo(1400, 1 ); // -- Flag it as a muon-stub-track
      track->addInfo(1401, ttHits.size() );
      track->addInfo(1402, muonTracks->size() );
    }

    // -- Add some MC info to the track
    if(m_MC){
      track->addInfo(1300, truePx);
      track->addInfo(1301, truePy);
      track->addInfo(1302, truePz);
      track->addInfo(1303, trueP);
    }

    // -- Finally, insert the track into the container!
    if (m_addTTHits){ //when performing track fit, only insert fitted tracks
	   if (sc) tracks->insert( track.release() );
    }
    else tracks->insert( track.release() );
    // ---------------------------------------------------------------

  }
  // ---------------------------------------------------------------

  if(msgLevel(MSG::DEBUG)) 
    debug() << "Filling " << tracks->size() << " tracks in " << m_outputLoc << endmsg;


  bool filterPassed = false;

  if(!tracks->empty()){
    put( tracks.release(), m_outputLoc );
    filterPassed = true;
  }

  // -- Only go on if MuonTT tracks have been reconstructed
  setFilterPassed( filterPassed );

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Fill PVs
//=============================================================================
StatusCode MuonTTTrack::fillPVs(std::vector<double>& PVPos){

  double zPVmax = 500;

  LHCb::RecVertices* PVs = getIfExists<LHCb::RecVertices>(m_pvLoc);
  if( !PVs ) return StatusCode::FAILURE;

  // -- Set the PV position (if existing)
  for(const auto& pv : *PVs) {

    if(!pv) continue;

    //-- If more than one PV, take the one closer to 0
    if( pv->position().Z() < zPVmax ){
      zPVmax = pv->position().Z();
    }
    
    PVPos[0] = pv->position().X();
    PVPos[1] = pv->position().Y();
    PVPos[2] = pv->position().Z();
    
  }
  return StatusCode::SUCCESS;
}
//=============================================================================
//  Fill Muon Stub Information
//=============================================================================
void MuonTTTrack::fillMuonStubInfo(LHCb::Track& track, const MuonTrack& muTrack) const{

  // -- Retrieve fit errors
  Gaudi::XYZVector slope;
  Gaudi::SymMatrix3x3 errmat;
  track.slopes( slope , errmat );


  // -- Get parameters of the MuonStub
  double bx_fit = track.position().X() - slope.X()*track.position().Z();
  double by_fit = track.position().Y() - slope.Y()*track.position().Z();

  double sx_fit = slope.X();
  double sy_fit = slope.Y();

  double chi2_fit = track.chi2PerDoF();

  double npad_fit = double( muTrack.getHits().size() );
  // ---------------------------------------------------------------

  double sx  = muTrack.sx() ;   // -- track slope in XZ
  double bx  = muTrack.bx() ;   // -- track intercept in XZ
  double sy  = muTrack.sy() ;   // -- track slope in YZ
  double by  = muTrack.by() ;   // -- track intercept in YZ
  double esx = muTrack.errsx(); // -- error on sx
  double ebx = muTrack.errbx(); // -- error on bx
  double esy = muTrack.errsy(); // -- error on sy
  double eby = muTrack.errby(); // -- error on by

  // ---------------------------------------------------------------
  track.addInfo(1000, muTrack.chi2x() );
  track.addInfo(1001, muTrack.chi2y() );
  track.addInfo(1002, sx);
  track.addInfo(1003, bx);
  track.addInfo(1004, sy);
  track.addInfo(1005, by);
  track.addInfo(1006, esx);
  track.addInfo(1007, ebx);
  track.addInfo(1008, esy);
  track.addInfo(1009, eby);
  track.addInfo(1010, bx_fit);
  track.addInfo(1011, by_fit);
  track.addInfo(1012, sx_fit);
  track.addInfo(1013, sy_fit);
  track.addInfo(1014, chi2_fit);
  track.addInfo(1015, npad_fit);
  track.addInfo(1016, muTrack.p());
  track.addInfo(1017, muTrack.pt());
  track.addInfo(1018, muTrack.qOverP());
  track.addInfo(1019, muTrack.covbsy());

  track.addInfo(1020, muTrack.getSpan() );
  track.addInfo(1021, muTrack.qOverP() );

  track.addInfo(1200, muTrack.momentum().X());
  track.addInfo(1201, muTrack.momentum().Y());
  track.addInfo(1202, muTrack.momentum().Z());
  // ---------------------------------------------------------------

}
//=============================================================================
//  Change the q/p till the track points to the PV (stolen from Wouter)
//=============================================================================
StatusCode MuonTTTrack::iterateToPV(LHCb::Track* track, LHCb::State& finalState, std::vector<double> PVPos, double qOverP){

  //Chrono chrono( chronoSvc(),name()+"::iterateToPV()" );


  LHCb::State muonState = track->closestState( 15000 );

  double dXdQoP = 1e7 ;
  double deltaX = 100;
  muonState.setQOverP( qOverP ) ;

  // -- Now call the extrapolator and iterate until we have the desired accuracy.
  double tolerance = 0.5 ; // [mm]
  Gaudi::TrackMatrix jacobian ;

  LHCb::State dummyState = muonState;

  for(int i=0; i<10 && std::abs(deltaX) > tolerance ; ++i) {
    dummyState = muonState ;
    StatusCode sc = m_extrapolator->propagate( dummyState, PVPos[2], &jacobian) ;
    if(!sc) return StatusCode::FAILURE;
    dXdQoP = jacobian(0,4) ;
    deltaX = -(dummyState.x() - PVPos[0]) ;
    double deltaQoP = deltaX / dXdQoP ;
    muonState.setQOverP( muonState.qOverP() + deltaQoP ) ;
  }

  finalState = muonState;
  return StatusCode::SUCCESS;

}
//=============================================================================
//  Associate MCParticle to Pad Hits
//=============================================================================
const LHCb::MCParticle* MuonTTTrack::assocMCParticle(std::vector<const MuonHit*> muonHits){

  std::map<const LHCb::MCParticle*, int> mcparts;

  // -- Get the MCParticles which are associated to the hits of the Particle in the Muon System
  // -- Put the mcparticles into a map, count how many times they appear
  for( const auto& muonHit : muonHits ) {
    const LHCb::MCParticle *p = m_muonPad2MC->Pad2MC( muonHit->centerTile() );
    if (!p) continue;
    auto iter = mcparts.find(p);
    if( iter != mcparts.end()){
      ++(iter->second);
    } else {
      mcparts.emplace( p, 1 );
    }
  }
  // ---------------------------------------------------------------

  // -- Get particle which contributes the most (i.e. appears the most)
  std::pair<const LHCb::MCParticle*, int> best{nullptr, 0};
  for(const auto& contrib : mcparts) {
    if( contrib.second > best.second ){
      best.first = contrib.first;
      best.second = contrib.second;
    }
  }
  // ---------------------------------------------------------------

  // -- Call a Particle associated if more than 70% of all MCParticles for all the hits on a track are the same
  bool isTrue = false;
  if( double(best.second) / muonHits.size() > 0.7){

    // -- Particle has to be a Muon
    if( best.first->particleID().abspid() == 13 ){
      isTrue = true;
    }
  }else{
    if(msgLevel(MSG::DEBUG)) debug() << "could not assign particle" << endmsg;
  }
  // ---------------------------------------------------------------

  return isTrue ? best.first : nullptr;

}
//=========================================================================
//  Check if a given LHCbID is on "track" of a MCParticle (not called anywhere at the moment,
//  for occasional testing needed)
//=========================================================================
bool MuonTTTrack::isIDOnTrack(LHCb::LHCbID id, LHCb::MCParticle* mcp){


  std::map<LHCb::MCParticle*, unsigned int> linkMap;
  // -- This class should link my id to the MCParticle!
  m_lhcbid2mcparticles->link(id, linkMap);

  // -- For simplicity, we only look at unambigously assigned mcparticles!
  if(linkMap.size() > 1 ) return false;

  return std::any_of( linkMap.begin(), linkMap.end(),
                      [&](const std::pair<const LHCb::MCParticle*,unsigned int>& i) {
                              return mcp==i.first;
  });
}
