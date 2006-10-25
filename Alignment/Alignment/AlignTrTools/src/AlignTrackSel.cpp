// $Id: AlignTrackSel.cpp,v 1.2 2006-10-25 09:43:48 lnicolas Exp $
//

//-----------------------------------------------------------------------------
// Implementation file for class : AlignTrackSel
//
// Louis Nicolas, Adlene Hicheur (EPFL)
// Started: 06/06
///-----------------------------------------------------------------------------

//===========================================================================
// Includes
//===========================================================================
// Local
#include "AlignTrackSel.h"
//===========================================================================


//===========================================================================
// Global Variables
//===========================================================================
// Needed for the creation of TsaEff objects.
static const AlgFactory<AlignTrackSel> s_factory;
const IAlgFactory& AlignTrackSelFactory = s_factory;
//===========================================================================


//===========================================================================
// Constructor
//===========================================================================
AlignTrackSel::AlignTrackSel( const std::string& name,
                              ISvcLocator* pSvcLocator )
  : GaudiTupleAlg( name, pSvcLocator )
    , m_itTracker( 0 )
    , m_nStations( 3 )
    , m_nBoxes( 4 )
    , m_nLayers( 4 )
    , m_nLadders( 7 )
    , m_nStrips( 384 )
    , m_pdgMuonID ( 13 ) {

  // constructor
  this -> declareProperty( "ITGeometryPath",
                           m_itTrackerPath = DeSTDetLocation::location("IT") );
  this -> declareProperty( "ClustersLocation",
                           m_clustersPath = LHCb::STClusterLocation::ITClusters );
  this -> declareProperty( "TracksLocation", m_tracksPath = "Rec/Track/Tsa" );
  //  this -> declareProperty( "selectorName", m_selectorName = "TrackSelector" );
   this -> declareProperty( "MuonIDLocation",
                            m_muonPIDsPath = LHCb::MuonPIDLocation::Default );
  // 48 = 12 (layers) * 2 (ladder overlaps) * 2 (box overlaps)
  this -> declareProperty( "MaxNumberITHits", m_nMaxITHits = 48 );
  this -> declareProperty( "IsolatedTrackNStripsTolerance", m_nStripsTol = 2 );
  this -> declareProperty( "TrackMuonMatchZ", m_trMuMatchZ = 12500 );
  this -> declareProperty( "TrackMuonMatchChi2Cut", m_trMuMatchChi2 = 9 );
}
//===========================================================================


//===========================================================================
// Destructor
//===========================================================================
AlignTrackSel::~AlignTrackSel( ) {}
//===========================================================================


//===========================================================================
// Initialize
//===========================================================================
StatusCode AlignTrackSel::initialize( ) {

  debug() << "==> Initialize" << endmsg;

  // Initializes TsaInitialization at the begin of program execution.
  StatusCode sc = GaudiTupleAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) {
    return Error( "Failed to initialize" );
  }

  //   // The selector
  //   m_selector = tool<ITrackSelector>( m_selectorName );

  // The extrapolator
  m_extrapolator = tool<ITrackExtrapolator>( "TrackFastParabolicExtrapolator" );

  // Get IT silicon tracker geometry
  m_itTracker = new DeITDetector;
  m_itTracker = getDet<DeITDetector>( m_itTrackerPath );

  // Get the IT detector unique layer numbers
  const DeSTDetector::Layers& itLayers = m_itTracker->layers();
  DeSTDetector::Layers::const_iterator iLayers = itLayers.begin();
  for( ; iLayers != itLayers.end(); ++iLayers )
    m_itLayers.push_back(std::make_pair((*iLayers)->elementID().uniqueLayer(),
                                        (*iLayers)->elementID().station()));
  m_itLayers.sort();
  m_itLayers.unique();

  // Printing the parameters (option-choosable) being used
  info() << endmsg 
         << "===== Parameters of the Track Selection for T-Alignment =====" << endmsg
         << "Tracks will be retrieved at " << m_tracksPath << endmsg
         << "A track is isolated if no hit is found within " << m_nStripsTol
         << " strips on each side" << endmsg
         << "Track-Muon matching done if chi2 < " << m_trMuMatchChi2 
         << " for position at z = " << m_trMuMatchZ << endmsg
         << "=============================================================" << endmsg
         << endmsg;
    

  return StatusCode::SUCCESS;
}
//===========================================================================


//===========================================================================
// Execute
//===========================================================================
StatusCode AlignTrackSel::execute( ) {

  // Get the event header
  m_evtHeader = get<LHCb::MCHeader>( LHCb::MCHeaderLocation::Default );

  // Get the tracks
  m_tracks = get<LHCb::Tracks>( m_tracksPath );
  debug() << "Retrieved " << m_tracks->size() << " tracks." << endmsg;

  // Get the clusters
  m_clusters = get<LHCb::STClusters>( m_clustersPath );
  debug() << "Retrieved " << m_clusters->size() << " clusters." << endmsg;

  // Get the muon PIDs
  m_muIDs = get<LHCb::MuonPIDs>(m_muonPIDsPath);

  // Creating the tuple in memory
  Tuples::Tuple trackSelTuple = nTuple( "TrackSel N-tuple" );

  // Get the association table 
  AsctTool associator( evtSvc(), m_tracksPath );
  m_directTable = associator.direct();

  // Loop over tracks - select some and make some plots
  LHCb::Tracks::const_iterator iTracks = m_tracks->begin();
  m_nGoodTracks = 0;
  for( ; iTracks != m_tracks->end(); ++iTracks ) {
    const LHCb::Track& aTrack = *(*iTracks);
    
    debug() << "******************************************************" << endmsg
            << "Entering new good track" << endmsg;
    
    // Get the nodes for this track
    int nOTHits = 0;
    int nITHits = 0;
    std::vector<LHCb::Node*>::const_iterator iNodes = aTrack.nodes().begin();
    std::vector<const LHCb::Node*> theNodes;
    for( ; iNodes != aTrack.nodes().end(); ++iNodes ) {
      const LHCb::Node& aNode = *(*iNodes);
      if( aNode.hasMeasurement() &&
          ( aNode.measurement().type()==LHCb::Measurement::IT ) ) {
        nITHits++;
        theNodes.push_back( &aNode );
      }
      else if( aNode.hasMeasurement() &&
               (aNode.measurement().type()==LHCb::Measurement::OT) )
        nOTHits++;
    }      

    // Only fill NTuple when there are some IT Hits
    if( theNodes.size()!=0 ) {
      StatusCode sc;
      if( (sc=fillVariables( &aTrack, theNodes, trackSelTuple ) )
          == StatusCode::FAILURE )
        return sc;
      m_nGoodTracks++;
    }
    debug() << "nITHits = " << nITHits << endmsg
            << "nOTHits = " << nOTHits << endmsg;
  }  

  return StatusCode::SUCCESS;  
}
//===========================================================================

  
//===========================================================================
// Finalize
//===========================================================================
StatusCode AlignTrackSel::finalize( ) {
  
  debug() << "==> Finalize" << endmsg;

  return GaudiTupleAlg::finalize();   // Must be called after all other actions  
};
//===========================================================================


//===========================================================================
// Fill Variables
//===========================================================================
StatusCode
AlignTrackSel::fillVariables( const LHCb::Track* aTrack,
                              std::vector<const LHCb::Node*> theNodes,
                              Tuples::Tuple trackSelTuple ) {

  StatusCode sc;

  //**********************************************************************
  // Global Variables
  //**********************************************************************
  m_nEvents = m_evtHeader->evtNumber();
  //**********************************************************************

  //**********************************************************************
  // Tracks Variables
  //**********************************************************************
  m_isAGhost = isGhostTrack( aTrack );
  m_isAMuon = false;

  m_nITHits = theNodes.size();
  m_nDoubleHits = 0;
  m_nSharedHits = 0;
  m_fSharedHits = -999.0;
  m_nCloseHits = nNeighbouringHits( aTrack, theNodes );
  m_nHoles = 0;

  m_trackChi2PerDoF = aTrack->chi2PerDoF();
  m_trackChi2Prob = gsl_cdf_chisq_Q(aTrack->chi2(),aTrack->nDoF());

  m_trackP = aTrack->p();
  m_trackPt = aTrack->pt();
  m_trackMCP = -999.0;
  m_trackMCPt = -999.0;
  m_trackErrP = sqrt(aTrack->firstState().errP2());
  //**********************************************************************

  //**********************************************************************
  // Hits Variables
  //**********************************************************************
  m_nRes = 0;
  m_res.clear();
  m_errRes.clear();

  m_hitX.clear();
  m_hitY.clear();
  m_hitZ.clear();

  m_tx.clear();
  m_ty.clear();

  m_lToHit.clear();
  m_lToHit.push_back(0);

  m_nLadOverlaps = 0;
  m_ladOverlaps.clear();
  m_ladOverlapsZ.clear();

  m_nBoxOverlaps = 0;
  m_boxOverlaps = Matrix(m_nStations,Array(m_nLayers, -999.0));
  //**********************************************************************

  //**********************************************************************
  // Track - Muon matching
  //**********************************************************************
  m_matchedMuon = isMuonTrack( aTrack );
  m_isAMuon = (m_matchedMuon != NULL);
  //**********************************************************************

  //**********************************************************************
  // MCP and MCPt (get MCParticle linked to the track)
  //**********************************************************************
  // Retrieve the Linker table
  LinkedTo<LHCb::MCParticle,LHCb::Track>
    directLink( evtSvc(), msgSvc(), m_tracksPath );
  if( directLink.notFound() ) {
    error() << "Linker table not found" << endmsg;
    return StatusCode::FAILURE;
  }

  // Get MCParticle linked by highest weight to Track and get its p and pt
  LHCb::MCParticle* mcPart = new LHCb::MCParticle;
  mcPart = directLink.first( aTrack );
  if( NULL != mcPart ) {
    m_trackMCP  = mcPart->p();
    m_trackMCPt = mcPart->pt();
  }
  //**********************************************************************

  // Loop over the nodes to get the hits variables
  unsigned int overlapStation = 999;
  std::vector<const LHCb::Node*>::const_iterator iNodes = theNodes.begin();
  for( ; iNodes!=theNodes.end(); ++iNodes,m_nRes++ ) {
    const LHCb::Node& aNode = *(*iNodes);
    const LHCb::STChannelID theSTID = aNode.measurement().lhcbID().stID();    

    //**********************************************************************
    // Printing some info on the hits
    //**********************************************************************
    verbose() << "Hits (Station,Box,Layer,Ladder,Strip) = (" 
              << theSTID.station() << "," << theSTID.detRegion() << "," 
              << theSTID.layer() << "," << theSTID.sector() << "," 
              << theSTID.strip() << ")" << endmsg;
    //**********************************************************************

    //**********************************************************************
    // Number of shared hits in the track
    //**********************************************************************
    if( isSharedHit( aTrack, &aNode ) )
      m_nSharedHits++;
    //**********************************************************************
    
    //**********************************************************************
    // Residuals and Residual Errors
    //**********************************************************************
    m_res.push_back(aNode.residual());
    m_errRes.push_back(aNode.errResidual());
    //**********************************************************************

    //**********************************************************************    
    // Hit Position
    //**********************************************************************
    Gaudi::XYZPoint hitPos(aNode.state().x(),
                           aNode.state().y(),
                           aNode.state().z());
    m_hitX.push_back(hitPos.X());
    m_hitY.push_back(hitPos.Y());
    m_hitZ.push_back(hitPos.Z());
    //**********************************************************************
    
    //**********************************************************************    
    // Track Slope at Hit
    //**********************************************************************
    m_tx.push_back(aNode.state().tx());
    m_ty.push_back(aNode.state().ty());
    //**********************************************************************

    //**********************************************************************
    // Overlaps
    //**********************************************************************
    bool getLength = true;
    std::vector<const LHCb::Node*>::const_iterator iNodes2 = iNodes+1;
    for( ; iNodes2!=theNodes.end(); ++iNodes2 ) {
      const LHCb::Node& aNode2 = *(*iNodes2);
      const LHCb::STChannelID theSTID2 = aNode2.measurement().lhcbID().stID();

      //**********************************************************************
      // Length between this hit (aNode) and next one (1st aNode2 in the loop)
      //**********************************************************************
      if( getLength ) {
        Gaudi::XYZPoint hitPos2;
        sc = m_extrapolator->position( *aTrack, aNode2.z(), hitPos2 );
        if( !sc.isFailure() ) {
          Gaudi::XYZVector dVec = hitPos2 - hitPos;
          m_lToHit.push_back(dVec.R());
          debug() << "Track Length between Hits: L = " << m_lToHit.back() << endmsg;
          getLength = false;
        }
      }
      //**********************************************************************

      //**********************************************************************
      // Ladder overlap residuals
      //**********************************************************************
      // Two hits in same layer of same box but not same ladder
      if( (theSTID.uniqueDetRegion() == theSTID2.uniqueDetRegion()) &&
          (theSTID.sector() != theSTID2.sector()) ) {
        // Convention: define overlap residual as
        // residual(at bigger z) - residual (at smaller z)
        if( aNode.z()>aNode2.z() )
          m_ladOverlaps.push_back(aNode.residual()-aNode2.residual());
        else
          m_ladOverlaps.push_back(aNode2.residual()-aNode.residual());
        m_ladOverlapsZ.push_back((aNode.z()+aNode2.z())/2);
        debug() << "Found ladder overlap: ladOverlap = " 
                << m_ladOverlaps.back() << " at <z> = "
                << m_ladOverlapsZ.back() << endmsg;
        m_nLadOverlaps++;
      }
      //**********************************************************************

      //**********************************************************************
      // Doubled hits (splitted clusters at beetles boundaries)
      //**********************************************************************
      // Two hits in same ladder
      if( theSTID.uniqueSector() == theSTID2.uniqueSector() ){
        debug() << "Found double hit" << endmsg;
        m_nDoubleHits++;
      }
      //**********************************************************************

      //**********************************************************************
      // Box overlap residuals
      //**********************************************************************
      // Different hits in different box of same station
      if( (theSTID.station() == theSTID2.station())
          && (theSTID.detRegion() != theSTID2.detRegion()) ) {

        // Save only one overlap per station (up to three box overlaps per track)
        if( theSTID.station() != overlapStation ) {
          overlapStation = theSTID.station();
          // Get a vector of box overlap residuals
          m_boxOverlaps[overlapStation-1] =
            boxOverlap( aTrack, theNodes, theSTID, theSTID2 );
          if( msgLevel(MSG::DEBUG) ) {
            debug() << "Station " << overlapStation 
                    << ": Found box overlap: boxOverlap = " << endmsg;
            for(int i=0; i<m_nLayers; i++)
              debug() << "layer " << i << " overlap = " 
                      << m_boxOverlaps[overlapStation-1][i] << endmsg;
          }
          m_nBoxOverlaps++;
        }
      }
      //**********************************************************************
    }
    //**********************************************************************
  }

  //**********************************************************************
  // Calculate number of holes using the available information
  //**********************************************************************
  m_nHoles = (m_nStations*m_nLayers) +
    (m_nBoxOverlaps*m_nLayers) + (m_nLadOverlaps) + 
    m_nDoubleHits - m_nITHits;
  //**********************************************************************

  //**********************************************************************
  // Fraction of shared hits in the track
  //**********************************************************************
  m_fSharedHits = 100*double(m_nSharedHits)/m_nITHits;
  //**********************************************************************

  // Check that all IT Hits have given a residual
  if( m_nITHits != m_nRes )
    Warning( "Number of saved residuals != Number of IT Hits !!!", StatusCode::SUCCESS, 1 );

  //**********************************************************************
  // Printing some informations in debug mode:
  //**********************************************************************
  debug() << "This track has " << m_nITHits << " hits" << endmsg
          << "               " << m_nDoubleHits << " double hits" << endmsg
          << "               " << m_nLadOverlaps << " ladder overlaps" << endmsg
          << "               " << m_nBoxOverlaps << " box overlaps" << endmsg
          << "               " << m_nHoles << " holes" << endmsg
          << "               " << m_nSharedHits << " shared hits"
          << " (= " << m_fSharedHits << " %)" << endmsg
          << "               " << m_nCloseHits << " neighbouring hits" << endmsg;
  //**********************************************************************

  delete mcPart;

  return writeNTuple( trackSelTuple );
}
//===========================================================================


//===========================================================================
// Is track ghost or not?
//===========================================================================
bool AlignTrackSel::isGhostTrack( const LHCb::Track* aTrack ) {

  DirectRange range = m_directTable->relations(aTrack);

  return ( range.empty() ? true:false );
}
//===========================================================================


//===========================================================================
// Match Tracks with Muons
//===========================================================================
LHCb::MuonPID* AlignTrackSel::isMuonTrack(const LHCb::Track* aTrack) {

  std::list<std::pair<double,LHCb::MuonPID*> > matchedMuList;

  //**********************************************************************
  // Get MCParticle linked by highest weight to Track, get its ID and
  // check that it is a muon (only for testing)
  LinkedTo<LHCb::MCParticle,LHCb::Track>
    directLink( evtSvc(), msgSvc(), m_tracksPath );
  if( directLink.notFound() ) {
    error() << "Linker table not found" << endmsg;
    return NULL;
  }
  LHCb::MCParticle* muonPart = new LHCb::MCParticle;
  muonPart = directLink.first( aTrack );
  //**********************************************************************
  
  LHCb::MuonPIDs::const_iterator iMuPIDs = m_muIDs->begin();
  for( ; iMuPIDs != m_muIDs->end(); ++iMuPIDs ) {

    // Try to match track to muon only if there is a muon found
    if( !(*iMuPIDs)->IsMuon() ) continue;

    LHCb::State trState;
    LHCb::State muState;
    if( (m_extrapolator->propagate( *aTrack, m_trMuMatchZ, trState ) 
        == StatusCode::FAILURE) ||
        (m_extrapolator->propagate( *((*iMuPIDs)->idTrack()), m_trMuMatchZ, muState )
         == StatusCode::FAILURE) ) {
      std::ostringstream mess;
      mess << "Could not propagate track to defined z position (" << m_trMuMatchZ << ")!!!";
      Warning( mess.str(), StatusCode::SUCCESS, 1 );
      return NULL;
    }

    double chi2 =
      pow(trState.x()-muState.x(),2)/(trState.errX2()+muState.errX2()) +
      pow(trState.y()-muState.y(),2)/(trState.errY2()+muState.errY2());

    plot(chi2, 701, "chi2", 0, 50, 100);

    if(chi2<m_trMuMatchChi2){
      debug() << "Track matched to a muon" << endmsg;
      if( NULL != muonPart )
        if( muonPart->particleID().abspid() != m_pdgMuonID ) {
          std::ostringstream mess;
          mess << "Track should not be matched to a muon! (MCParticle ID = " 
               << muonPart->particleID().abspid() << ")";
          Warning( mess.str(), StatusCode::SUCCESS, 1 );
        }
      matchedMuList.push_back(std::make_pair(chi2,(*iMuPIDs)));
    }
  }

  if( !matchedMuList.empty() ) {
    matchedMuList.sort();
    return matchedMuList.front().second;
  }

  debug() << "Track not matched to a muon" << endmsg;
  if( NULL != muonPart )
    if( muonPart->particleID().abspid() == m_pdgMuonID )
      Warning( "Track should be matched to a muon", StatusCode::SUCCESS, 1 );

  delete muonPart;

  return NULL;
}
//===========================================================================


//===========================================================================
// Is track isolated or not?
//===========================================================================
int AlignTrackSel::nNeighbouringHits( const LHCb::Track* aTrack,
                                      std::vector<const LHCb::Node*> theNodes ) {

  int nNeighbouringHits = 0;

  //**********************************************************************
  // Get a vector of hits and check for each hit if there are neighbouring
  // clusters
  //**********************************************************************
  std::vector<LHCb::STChannelID> theHitsIDs;
  unsigned int numDetRegion = 0;
  
  std::vector<const LHCb::Node*>::reverse_iterator iNodes = theNodes.rbegin();  
  for( ; iNodes!=theNodes.rend(); ++iNodes ) {
    LHCb::STChannelID nodeSTID = (*iNodes)->measurement().lhcbID().stID();
    
    // Fill vector of hits STID, only one hit per layer per box
    if( nodeSTID.uniqueDetRegion() != numDetRegion ){
      numDetRegion = nodeSTID.uniqueDetRegion();
      theHitsIDs.push_back(nodeSTID);
    }

    // For each hit, check that no other hit is found close to it
    LHCb::STClusters::const_iterator iClus = m_clusters->begin();
    for( ; iClus != m_clusters->end(); ++iClus )
      if( isNeighbouringHit(aTrack, (*iClus)->channelID(),
                            nodeSTID, nodeSTID.strip()) )
        nNeighbouringHits++;
  }
  //**********************************************************************

  //**********************************************************************
  // Get the holes uniqueLayer and detRegion IDs
  //**********************************************************************
  unsigned int numBox = 0;
  unsigned int numStation = 0;
  bool newStation = false;
  bool boxOverlap = false;
  std::list<std::pair<unsigned int,unsigned int> >::const_iterator
    iDetLay = m_itLayers.begin();
  std::list<std::pair<unsigned int,unsigned int> > holes;

  for( ; iDetLay != m_itLayers.end(); ++iDetLay ){
    bool hasHit = false;
    // If station number changes, we're in a new station (see later)
    if( (*iDetLay).second != numStation )
      newStation = true;
    numStation = (*iDetLay).second;
    std::vector<LHCb::STChannelID>::const_iterator iHits = theHitsIDs.begin();
    for( ; (iHits != theHitsIDs.end()) && 
           ((*iHits).station() <= numStation); ++iHits ){
      
      // Only look at hits in the right station
      if( (*iHits).station() != numStation )
        continue;

      // For first hit of new station, update box number
      // (important to deal with box overlaps)
      if( newStation ){
        numBox = (*iHits).detRegion();
        boxOverlap = false;
        newStation = false;
      }

      // Check if there is a hit in this layer
      if( ((*iHits).uniqueLayer() == (*iDetLay).first) &&
          ((*iHits).detRegion() == numBox) )
        hasHit = true;

      // In case we have a box overlap (same uniqueLayer, different box)
      // we have to look at all layers in the new box
      else if ( !boxOverlap && ((*iHits).detRegion() != numBox) ){
        boxOverlap = true;
        std::list<std::pair<unsigned int,unsigned int> >::const_iterator
          iDetLay2 = m_itLayers.begin();
        for( ; (iDetLay2 != m_itLayers.end()) &&
               ((*iDetLay2).second <= numStation); ++iDetLay2 ){
          bool hasHit2 = false;
          // Only look at layers in the right station
          if( (*iDetLay2).second != numStation )
            continue;

          std::vector<LHCb::STChannelID>::const_iterator iHits2 = iHits;
          for( ; (iHits2 != theHitsIDs.end()) && 
                 ((*iHits2).station() <= numStation); ++iHits2 ){

            // Only look at hits in the right station
            if( (*iHits2).station() != numStation )
              continue;

            // Check if there is a hit in this layer
            if( ((*iHits2).uniqueLayer() == (*iDetLay2).first) &&
                ((*iHits2).detRegion() == (*iHits).detRegion()) )
              hasHit2 = true;
          }

          // If no hit has been found in the layer of box, save hole
          if( !hasHit2 )
            holes.push_back(std::make_pair((*iDetLay2).first,(*iHits).detRegion()));
        }
      }
    }
    // If no hit has been found in the layer of box, save hole
    if( !hasHit ){
      if(numBox==0)
        numBox = theHitsIDs.front().detRegion();
      holes.push_back(std::make_pair((*iDetLay).first,numBox));    
    }
  }
  //**********************************************************************

  //**********************************************************************
  // For the layers with no hit, check that no other hits is found close
  // to the track
  //**********************************************************************
  // Get the z-position of the holes layers
  std::list<std::pair<unsigned int,unsigned int> >::const_iterator iHoles 
    = holes.begin();
  for( ; iHoles != holes.end(); ++iHoles ){

    const DeSTDetector::Sectors& itSectors = m_itTracker->sectors();
    DeSTDetector::Sectors::const_iterator iSectors = itSectors.begin();
    for( ; iSectors != itSectors.end(); ++iSectors ){
      // Same layer of same box
      if( ((*iHoles).first == (*iSectors)->elementID().uniqueLayer()) &&
          ((*iHoles).second == (*iSectors)->elementID().detRegion()) ){
        double holeZ = ((*iSectors)->globalCentre()).z();
        // Get an extrapolated state at position of hole
        Gaudi::XYZPoint holePos;
        StatusCode 
          sc = m_extrapolator->position( *aTrack, holeZ, holePos );
        if(!sc.isFailure()){
          // Convert global position holePos to local sector u
          // and then convert to strip number
          unsigned int holeStrip = 
            (*iSectors)->localUToStrip((*iSectors)->toLocal(holePos).x());
          
          // If holePos not in this sector, will get invalid local u ==> strip==0
          if( !holeStrip ) continue;
          
          LHCb::STClusters::const_iterator iClus = m_clusters->begin();
          for( ; iClus != m_clusters->end(); ++iClus )
            // Check that no other hit is found "close to the holes"            
            if( isNeighbouringHit(aTrack, (*iClus)->channelID(), 
                                  (*iSectors)->elementID(), holeStrip) )
              nNeighbouringHits++;
        }
      }
    }
  }
  //**********************************************************************

  return nNeighbouringHits;
}
//===========================================================================


//===========================================================================
// Check if cluster is neighbouring the track in the given sector
//===========================================================================
bool AlignTrackSel::isNeighbouringHit( const LHCb::Track* aTrack,
                                       LHCb::STChannelID clusSTID,
                                       LHCb::STChannelID sectSTID,
                                       unsigned int hitStrip) {
  // Analyze only the IT clusters 
  if( !(clusSTID.isIT()) ) return false;
  
  // Not already on the track
  if( aTrack->isOnTrack(clusSTID) ) return false;

  // Not a TELL1 artefact at beetles boundaries
  if( (hitStrip+clusSTID.strip())%256 <= 3 ) return false;
  
  // Same ladder within 2 strips aside
  if( (sectSTID.uniqueSector() == clusSTID.uniqueSector()) &&
      (abs(hitStrip-clusSTID.strip())<=m_nStripsTol) )
    return true;
  else if( ( sectSTID.uniqueDetRegion() == clusSTID.uniqueDetRegion() ) &&
           (abs(sectSTID.sector()-clusSTID.sector())==1) )
    // or next ladder with bordering strips
    if( ((sectSTID.sector()>clusSTID.sector()) && 
         (m_nStrips-int(clusSTID.strip()-hitStrip)<=m_nStripsTol)) ||
        ((sectSTID.sector()<clusSTID.sector()) && 
         (m_nStrips-int(hitStrip-clusSTID.strip())<=m_nStripsTol)) )
      return true;

  return false;
}
//===========================================================================


//===========================================================================
// Check if this hit is shared with (at least) one other track
//===========================================================================
bool AlignTrackSel::isSharedHit( const LHCb::Track* iTrack,
                                 const LHCb::Node* aNode ) {

  const LHCb::LHCbID& measurementID = aNode->measurement().lhcbID();

  LHCb::Tracks::const_iterator iTracks = m_tracks->begin();
  for(; iTracks != m_tracks->end(); ++iTracks) {
    const LHCb::Track& aTrack = *(*iTracks);
    if( &aTrack != iTrack )
      if(aTrack.isMeasurementOnTrack(measurementID)) {
        debug() << "This hit is being shared" << endmsg;
        return true;
      }
  }
  
  return false;
}
//===========================================================================


//===========================================================================
// Get box overlaps
//===========================================================================
std::vector<double>
AlignTrackSel::boxOverlap(const LHCb::Track* aTrack,
                          std::vector<const LHCb::Node*> theNodes,
                          LHCb::STChannelID STChanID1,
                          LHCb::STChannelID STChanID2) {

  Array fitPar(4,-999.0);

  Array boxOverlap(m_nLayers,-999.0);
  std::vector<const LHCb::Node*> hitsOverlapBox1;
  std::vector<const LHCb::Node*> hitsOverlapBox2;
  std::vector<const LHCb::Node*>::const_iterator iNodes = theNodes.begin();

  for( ; iNodes!=theNodes.end(); ++iNodes ) {
    const LHCb::Node& aNode = *(*iNodes);
    const LHCb::STChannelID aSTID = aNode.measurement().lhcbID().stID();
    // Save hits of overlapping box 1
    if( (aSTID.station() == STChanID1.station()) &&
        (aSTID.detRegion() == STChanID1.detRegion()) )
      hitsOverlapBox1.push_back( &aNode );
    // Save hits of overlapping box 2
    if( (aSTID.station() == STChanID2.station()) &&
        (aSTID.detRegion() == STChanID2.detRegion()) )
      hitsOverlapBox2.push_back( &aNode );
  }  

  if( (int(hitsOverlapBox1.size()) >= m_nLayers) ||
      (int(hitsOverlapBox2.size()) >= m_nLayers) )
    if( hitsOverlapBox1.size() >= hitsOverlapBox2.size() ) {
      fitPar = fitTrackPiece(aTrack, hitsOverlapBox1);
      iNodes = hitsOverlapBox2.begin();
    }
    else {
      fitPar = fitTrackPiece(aTrack, hitsOverlapBox2);
      iNodes = hitsOverlapBox1.begin();
    }
  else {
    Warning( "Not enough hits to fit piece of track!!!", StatusCode::SUCCESS, 1 );
    return boxOverlap;
  }

  if( fitPar.empty() ) {
    Warning( "Could not propagate track to defined z position!!!", StatusCode::SUCCESS, 1 );
    return boxOverlap;
  }

  double xTrack = 0;
  double yTrack = 0;

  for( int hitCnt=0; (iNodes!=hitsOverlapBox1.end()) && 
         (iNodes!=hitsOverlapBox2.end()); ++iNodes, ++hitCnt ) {
    const LHCb::Node& aNode2 = *(*iNodes);

    xTrack = fitPar[0]*aNode2.z() + fitPar[1];
    yTrack = fitPar[2]*aNode2.z() + fitPar[3];
    
    double b = -(aNode2.measurement().trajectory().beginPoint().X()-
                 aNode2.measurement().trajectory().endPoint().X())/
      (aNode2.measurement().trajectory().beginPoint().Y()-
       aNode2.measurement().trajectory().endPoint().Y());
    
    boxOverlap[hitCnt] =
      (xTrack +
       b*(yTrack-aNode2.measurement().trajectory().endPoint().Y()) -
       aNode2.measurement().trajectory().endPoint().X())/
      sqrt(1+b*b);
    
    if(hitCnt>=m_nLayers){
      std::ostringstream mess;
      mess << "Too many hits (" << hitCnt << ") to calculate chi2 of box overlap!!!";
      Warning( mess.str(), StatusCode::SUCCESS, 1 );
    }
  }

  return boxOverlap;
}
//===========================================================================

 
//===========================================================================
// Fit piece of track with straight line and return fit parameters
//===========================================================================
std::vector<double>
AlignTrackSel::fitTrackPiece( const LHCb::Track* aTrack,
                              std::vector<const LHCb::Node*> hitsOverlapBox ) {

  StatusCode sc;
  Array fitPar(4,-999.0);
  Array fitParSigma(4,-999.0);

  // If 4+ hits in box, fit piece of track with a straight line
  // and using minimisation of chi2
  Array xPar(5,0);
  Array yPar(5,0);
  std::vector<const LHCb::Node*>::const_iterator iNodes = hitsOverlapBox.begin();
  for( ; iNodes!=hitsOverlapBox.end(); ++iNodes ) {
    const LHCb::Node& aNode = *(*iNodes);

    LHCb::State hitState;
    sc = m_extrapolator->propagate( *aTrack, aNode.z(), hitState );
    if( sc.isFailure() )
      continue;

    double xHit = 0;
    if( aNode.measurement().trajectory().beginPoint().X() ==
        aNode.measurement().trajectory().endPoint().X() )
      xHit = aNode.measurement().trajectory().beginPoint().X();
    else
      xHit = aNode.measurement().trajectory().beginPoint().X() +
        hitState.y()*
        (aNode.measurement().trajectory().endPoint().X() -
         aNode.measurement().trajectory().beginPoint().X())/
        (aNode.measurement().trajectory().endPoint().Y() -
         aNode.measurement().trajectory().beginPoint().Y());

    xPar[0] += 1/pow(aNode.errResidual(),2);
    xPar[1] += aNode.z()/pow(aNode.errResidual(),2);
    xPar[2] += pow(aNode.z(),2)/pow(aNode.errResidual(),2);
    xPar[3] += (aNode.z()*xHit)/pow(aNode.errResidual(),2);
    xPar[4] += xHit/pow(aNode.errResidual(),2);
    yPar[0] += 1/hitState.errY2();
    yPar[1] += aNode.z()/hitState.errY2();
    yPar[2] += pow(aNode.z(),2)/hitState.errY2();
    yPar[3] += (aNode.z()*hitState.y())/hitState.errY2();
    yPar[4] += hitState.y()/hitState.errY2();
  }

  // x = fitPar[0]*z + fitPar[1]
  // y = fitPar[2]*z + fitPar[3]
  if( !sc.isFailure() ) {
    fitPar[0] = (xPar[0]*xPar[3] - xPar[1]*xPar[4])/
      (xPar[0]*xPar[2] - xPar[1]*xPar[1]);
    fitPar[1] = (xPar[4] - fitPar[0]*xPar[1])/xPar[0];
    fitPar[2] = (yPar[0]*yPar[3] - yPar[1]*yPar[4])/
      (yPar[0]*yPar[2] - yPar[1]*yPar[1]);
    fitPar[3] = (yPar[4] - fitPar[2]*yPar[1])/yPar[0];

    fitParSigma[0] = 1/sqrt(xPar[2]);
    fitParSigma[1] = 1/sqrt(xPar[0]);
    fitParSigma[2] = 1/sqrt(yPar[2]);
    fitParSigma[3] = 1/sqrt(yPar[0]);
  }
  else {
    fitPar.clear();
    fitParSigma.clear();
  }

  // ======================================================================
  // REMOVE WHEN TESTS FINISHED!!!
  plot(fitPar[0], 41, "a", -1, 2, 100);
  plot(fitPar[1], 42, "b", -15000, 10000, 100);
  plot(fitPar[2], 43, "c", -0.05, 0.05, 100);
  plot(fitPar[3], 44, "d", -100, 100, 100);

  plot(fitParSigma[0], 51, "sig_a", 0, 5E-6, 100);
  plot(fitParSigma[1], 52, "sig_b", 0, 0.05, 100);
  plot(fitParSigma[2], 53, "sig_c", 0, 1E-4, 100);
  plot(fitParSigma[3], 54, "sig_d", 0, 1, 100);

  for( iNodes = hitsOverlapBox.begin(); iNodes!=hitsOverlapBox.end(); ++iNodes ) {
    const LHCb::Node& aNode = *(*iNodes);
    LHCb::State hitState;
    sc = m_extrapolator->propagate( *aTrack, aNode.z(), hitState );
    if( sc.isFailure() )
      continue;
    plot(hitState.tx()-fitPar[0], 61, "Diff tx", -2, 2, 100);
    plot(hitState.ty()-fitPar[2], 62, "Diff ty", -1E-4, 1E-4, 100);
    plot(hitState.x()-(fitPar[0]*aNode.z()+fitPar[1]), 63, "Diff x", -50, 50, 100);
    plot(hitState.y()-(fitPar[2]*aNode.z()+fitPar[3]), 64, "Diff y", -5E-5, 5E-5, 100);
  }
  // ======================================================================
  
  return fitPar;
}
//===========================================================================


//===========================================================================
// Write NTuple
//===========================================================================
StatusCode AlignTrackSel::writeNTuple( Tuples::Tuple trackSelTuple ) {

  //**********************************************************************
  // Fill the NTuple columns
  //**********************************************************************
  // Global Variables
  trackSelTuple -> column( "NEvents", m_nEvents );
  
  // Track Variables                     
  trackSelTuple -> column( "NTrack", m_nGoodTracks );
  
  trackSelTuple -> column( "IsAGhost", m_isAGhost );
  trackSelTuple -> column( "IsAMuon", m_isAMuon );
  
  trackSelTuple -> column( "NITHits", m_nITHits );
  trackSelTuple -> column( "NSharedHits", m_nSharedHits );
  trackSelTuple -> column( "FSharedHits", m_fSharedHits );
  trackSelTuple -> column( "NNeighbouringHits", m_nCloseHits );
  trackSelTuple -> column( "NHoles", m_nHoles );
  
  trackSelTuple -> column( "TrackChi2PerDoF", m_trackChi2PerDoF );
  trackSelTuple -> column( "TrackChi2Prob", m_trackChi2Prob );
  
  trackSelTuple -> column( "TrackP", m_trackP );
  trackSelTuple -> column( "TrackPt", m_trackPt );
  trackSelTuple -> column( "TrackMCP", m_trackMCP );
  trackSelTuple -> column( "TrackMCPt", m_trackMCPt );
  trackSelTuple -> column( "TrackErrP", m_trackErrP );
  
  // Hits Variables
  trackSelTuple -> farray( "Residuals", m_res.begin(),
                           m_res.end(), "NResiduals", m_nMaxITHits );
  trackSelTuple -> farray( "ErrResiduals", m_errRes.begin(),
                           m_errRes.end(), "NResiduals", m_nMaxITHits );
  
  trackSelTuple -> farray( "HitX", m_hitX.begin(),
                           m_hitX.end(), "NResiduals", m_nMaxITHits );
  trackSelTuple -> farray( "HitY", m_hitY.begin(),
                           m_hitY.end(), "NResiduals", m_nMaxITHits );
  trackSelTuple -> farray( "HitZ", m_hitZ.begin(),
                           m_hitZ.end(), "NResiduals", m_nMaxITHits );
  
  trackSelTuple -> farray( "TXAtHit", m_tx.begin(),
                           m_tx.end(), "NResiduals", m_nMaxITHits );
  trackSelTuple -> farray( "TYAtHit", m_ty.begin(),
                           m_ty.end(), "NResiduals", m_nMaxITHits );
  
  trackSelTuple -> farray( "LengthToHit", m_lToHit.begin(),
                           m_lToHit.end(), "NResiduals", m_nMaxITHits );

  trackSelTuple -> farray( "LadOverlaps", m_ladOverlaps.begin(),
                           m_ladOverlaps.end(),  "NLadOverlaps", m_nMaxITHits );
  trackSelTuple -> farray( "LadOverlapsZ", m_ladOverlapsZ.begin(),
                           m_ladOverlapsZ.end(), "NLadOverlaps", m_nMaxITHits );
  
  trackSelTuple -> column( "NBoxOverlaps", m_nBoxOverlaps );

  trackSelTuple -> matrix( "BoxOverlaps", m_boxOverlaps,
                           m_nStations, m_nLayers );
  //**********************************************************************
  
  return trackSelTuple->write();  
}
//===========================================================================


// Test Geometry informations
// channelID: ???
// station: one of 3 stations
// detRegion: one of 4 boxes in one station
// layer: one of 4 layers in one box
// sector: one of 7 ladders in one layer

// uniqueDetRegion=detRegion+8*uniqueLayer:
// 48 values == 3 stations * 4 boxes/station * 4 layers/box

// uniqueLayer=layer+8*station:
// 12 values (layers) == 3 stations * 4 layers/station

// uniqueSector=sector+32*uniqueDetRegion:
// 336 values (ladders) == 3 stations * 4 boxes/station * 4 layers/box * 7 ladders/layer
