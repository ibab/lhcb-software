// $Id: AlignTrackSel.cpp,v 1.1 2006-10-02 15:20:37 lnicolas Exp $
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
  //     , m_itTracker(0)
  , m_nStations( 3 )
  , m_nBoxes( 4 )
  , m_nLayers( 4 )
  , m_nLadders( 7 )
  , m_nStrips( 384 )
{

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

  debug() << "==> Initialize" << endreq;

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
  debug() << "Retrieved " << m_tracks->size() << " tracks." << endreq;

  // Get the clusters
  m_clusters = get<LHCb::STClusters>( m_clustersPath );
  debug() << "Retrieved " << m_clusters->size() << " clusters." << endreq;

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
    //     if(m_selector->accept( aTrack ) == true ) {
    
    debug() << "***********************************" 
            << "***********************************" << endreq;
    debug() << "Entering new good track" << endreq;
    
    // Get the nodes for this track
    int nOTHits = 0;
    int nITHits = 0;
    std::vector<LHCb::Node*>::const_iterator iNodes = aTrack.nodes().begin();
    std::vector<const LHCb::Node*> theNodes;
    theNodes.clear();
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
    debug() << "nITHits = " << nITHits << endreq;
    debug() << "nOTHits = " << nOTHits << endreq;
    //   }
  }  

  return StatusCode::SUCCESS;
  
}
//===========================================================================

  
//===========================================================================
// Finalize
//===========================================================================
StatusCode AlignTrackSel::finalize( ) {
  
  debug() << "==> Finalize" << endreq;

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
  m_isAMuon = isMuonTrack( aTrack );

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
  // MCP and MCPt (get MCParticle linked to the track)
  //**********************************************************************
  // Retrieve the Linker table
  LinkedTo<LHCb::MCParticle,LHCb::Track>
    directLink( evtSvc(), msgSvc(), m_tracksPath );
  if( directLink.notFound() ) {
    error() << "Linker table not found" << endreq;
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
    // Number of shared hits in the track
    //**********************************************************************
    if( isSharedHit( aTrack, &aNode ) )
      m_nSharedHits++;
    
    //**********************************************************************
    // Residuals and Residual Errors
    //**********************************************************************
    m_res.push_back(aNode.residual());
    m_errRes.push_back(aNode.errResidual());

    // Plot residual pull
    if(m_isAGhost)
      plot(m_res.back()/m_errRes.back(), 21, "ghost pull", -10, 10, 100);
    else
      plot(m_res.back()/m_errRes.back(), 22, "not ghost pull", -10, 10, 100);
    //**********************************************************************

    //**********************************************************************    
    // Hit Position
    //**********************************************************************
    LHCb::State hitState;
    sc = m_extrapolator->propagate( *aTrack, aNode.z(), hitState );
    if( !sc.isFailure() ) {
      m_hitX.push_back(hitState.x());
      m_hitY.push_back(hitState.y());
      m_hitZ.push_back(hitState.z());
    }
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
        LHCb::State hitState2;
        sc = m_extrapolator->propagate( *aTrack, aNode2.z(), hitState2 );
        if( !sc.isFailure() ) {
          Gaudi::XYZVector dVec = hitState2.position() - hitState.position();
          m_lToHit.push_back(dVec.R());
          debug() << "Track Length between Hits: L = " << m_lToHit.back() << endreq;
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
                << m_ladOverlapsZ.back() << endreq;
        m_nLadOverlaps++;
      }
      //**********************************************************************

      //**********************************************************************
      // Doubled hits (splitted clusters at beetles boundaries)
      //**********************************************************************
      // Two hits in same ladder
      if( theSTID.uniqueSector() == theSTID2.uniqueSector() ){
        debug() << "Found double hit" << endreq;
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
          // Get a vector of box overlap values
          m_boxOverlaps[overlapStation-1] =
            boxOverlap( aTrack, theNodes, theSTID, theSTID2 );
          if( !m_boxOverlaps[overlapStation-1].empty() ) {
            if( msgLevel(MSG::DEBUG) ) {
              debug() << "Station " << overlapStation 
                      << ": Found box overlap: boxOverlap = " << endreq;
              for(int i=0; i<m_nLayers; i++)
                debug() << "layer " << i << " overlap = " 
                        << m_boxOverlaps[overlapStation-1][i] << endreq;
            }
            m_nBoxOverlaps++;
          }
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
    (m_nLadOverlaps) + (m_nBoxOverlaps*m_nLayers) +
    m_nDoubleHits - m_nITHits;
  //**********************************************************************

  //**********************************************************************
  // Fraction of shared hits in the track
  //**********************************************************************
  m_fSharedHits = 100*double(m_nSharedHits)/m_nITHits;
  //**********************************************************************

  // Check that all IT Hits have given a residual
  if( m_nITHits != m_nRes )
    warning() << "Number of saved residuals != Number of IT Hits !!!" << endreq;

  //**********************************************************************
  // Printing some informations in debug mode:
  //**********************************************************************
  debug() << "This track has " << m_nITHits << " hits" << endreq;
  debug() << "               " << m_nDoubleHits << " double hits" << endreq;
  debug() << "               " << m_nLadOverlaps << " ladder overlaps" << endreq;
  debug() << "               " << m_nBoxOverlaps << " box overlaps" << endreq;      
  debug() << "               " << m_nHoles << " holes" << endreq;  
  debug() << "               " << m_nSharedHits << " shared hits"
          << " (= " << m_fSharedHits << "%)" << endreq;
  debug() << "               " << m_nCloseHits << " neighbouring hits" << endreq;
  //**********************************************************************

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
bool AlignTrackSel::isMuonTrack(const LHCb::Track* aTrack) {
  
  LHCb::MuonPIDs::const_iterator iMuPIDs = m_muIDs->begin();
  for( ; iMuPIDs != m_muIDs->end(); ++iMuPIDs ) {

    const LHCb::MuonPID& aMuPID = *(*iMuPIDs);    

    // Try to match track to muon only if there is a muon found
    info() << "Is a muon? " << aMuPID.IsMuon() << endreq;
    if( !aMuPID.IsMuon() ) continue;
    
    debug() << "Track matched to a muon" << endreq;
    return true;
  }

  debug() << "Track not matched to a muon" << endreq;
  return false;
}
//===========================================================================


//===========================================================================
// Is track isolated or not?
//===========================================================================
int AlignTrackSel::nNeighbouringHits( const LHCb::Track* aTrack,
                                      std::vector<const LHCb::Node*> theNodes ) {

  int nNeighbouringHits=0;

  // Get the Layers being hit
  std::vector<LHCb::STChannelID> theHitsIDs;
  theHitsIDs.clear();
  unsigned int numDetRegion = 0;
  std::vector<const LHCb::Node*>::const_iterator iNodes = theNodes.begin();  
  for( ; iNodes!=theNodes.end(); ++iNodes ) {
    LHCb::STChannelID nodeSTID = (*iNodes)->measurement().lhcbID().stID();
    if(nodeSTID.uniqueDetRegion() != numDetRegion){
      numDetRegion = nodeSTID.uniqueDetRegion();
      theHitsIDs.push_back(nodeSTID);
    }

    // For the layers with hits, check that no other hit is found close to it
    LHCb::STClusters::const_iterator iClus = m_clusters->begin();
    for( ; iClus != m_clusters->end(); ++iClus ){
      // Analyze only the IT clusters
      if( !((*iClus)->isIT()) ) continue;

      // Get the cluster's STChannelID
      LHCb::STChannelID itClusSTID = (*(*iClus)).channelID();

      if( !(nodeSTID == itClusSTID) ) // Not the same hit
        // Same ladder within 2 strips aside
        if( (nodeSTID.uniqueSector() == itClusSTID.uniqueSector()) &&
            (abs(nodeSTID.strip()-itClusSTID.strip())<=m_nStripsTol) ){
          // Not a Tell1 artefact at beetles boundaries
          if( (nodeSTID.strip()+itClusSTID.strip())%256 > 3 )
            nNeighbouringHits++;
        }
        else if( (nodeSTID.uniqueDetRegion() == itClusSTID.uniqueDetRegion()) && 
                 (abs(nodeSTID.sector()-itClusSTID.sector())==1) )
          // or next ladder with bordering strips
          if( ((nodeSTID.sector()>itClusSTID.sector()) && 
               (m_nStrips-int(itClusSTID.strip()-nodeSTID.strip())<=m_nStripsTol)) ||
              ((nodeSTID.sector()<itClusSTID.sector()) && 
               (m_nStrips-int(nodeSTID.strip()-itClusSTID.strip())<=m_nStripsTol)) )
            // Not a Tell1 artefact at beetles boundaries
            if( (nodeSTID.strip()+itClusSTID.strip())%256 < 3 )
              nNeighbouringHits++;
    }
  }

  // Get the IT detector unique layer numbers
  const DeSTDetector::Layers& itLayers = m_itTracker->layers();
  DeSTDetector::Layers::const_iterator iLayers = itLayers.begin();
  std::list<std::pair<unsigned int,unsigned int> > detLayers;
  for( ; iLayers != itLayers.end(); ++iLayers ){
    detLayers.push_back(std::make_pair((*iLayers)->elementID().uniqueLayer(),
                                       (*iLayers)->elementID().station()));
  }
  detLayers.sort();
  detLayers.unique();

  // Get the holes uniqueLayer and detRegion IDs
  unsigned int numBox = 0;
  unsigned int numStation = 0;
  bool newStation = false;
  std::list<std::pair<unsigned int,unsigned int> >::const_iterator
    iDetLay = detLayers.begin();
  std::list<std::pair<unsigned int,unsigned int> > holes;
  holes.clear();

  for( ; iDetLay != detLayers.end(); ++iDetLay ){
    // If station number changes, we're in a new station (...)
    if( (*iDetLay).second != numStation )
      newStation = true;
    numStation = (*iDetLay).second;
    std::vector<LHCb::STChannelID>::const_iterator iHits = theHitsIDs.begin();
    for( ; iHits != theHitsIDs.end(); ++iHits ){
      
      // Only look at hits in the right station
      if( (*iHits).station() != numStation ) continue;

      // For first hit of new station, update box number
      // (important to deal with box overlaps)
      if( newStation ){
        numBox = (*iHits).detRegion();
        newStation = false;
      }

      // Check if there is a hit in this layer
      if( ((*iHits).uniqueLayer() == (*iDetLay).first) &&
          ((*iHits).detRegion() == numBox) ) break;

      // In case we have a box overlap (same uniqueLayer, different box)
      // we have to look at all layers in the new box
      else if ( (*iHits).detRegion() != numBox ){
        info() << "Are you sure there is a box overlap???" << endreq;
        std::list<std::pair<unsigned int,unsigned int> >::const_iterator
          iDetLay2 = detLayers.begin();
        for( ; iDetLay2 != detLayers.end(); ++iDetLay2 ){
          // Only look at layers in the right station
          if( (*iDetLay2).second != numStation ) continue;

          std::vector<LHCb::STChannelID>::const_iterator iHits2 = theHitsIDs.begin();
          for( ; iHits2 != theHitsIDs.end(); ++iHits2 ){
            // Only look at hits in the right station
            if( (*iHits2).station() != numStation ) continue;
            // Check if there is a hit in this layer
            if( ((*iHits2).uniqueLayer() == (*iDetLay2).first) &&
                ((*iHits2).detRegion() == (*iHits).detRegion()) ) break;
          }
          // If no hit has been found in the layer of box, save hole
          if( iHits2 == theHitsIDs.end() )
            holes.push_back(std::make_pair((*iDetLay2).first,(*iHits).detRegion()));
        }
      }
    }
    // If no hit has been found in the layer of box, save hole
    if( iHits == theHitsIDs.end() ){
      if(numBox==0)
        numBox = theHitsIDs.front().detRegion();
      holes.push_back(std::make_pair((*iDetLay).first,numBox));    
    }
  }

  // For the layers with no hit, extrapolate track to z of layer and
  // check that no other hits is found close to it
  //
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
        LHCb::State holeState;
        StatusCode 
          sc = m_extrapolator->propagate( *aTrack, holeZ, holeState );
        Gaudi::XYZPoint statePos;
        if(!sc.isFailure()){
          statePos = holeState.position();
          // convert global position posState to local sector u;
          double stateU = (*iSectors)->toLocal(statePos).x();
          int holeStrip = (*iSectors)->localUToStrip(stateU);

          // Check that no other hit is found "close to the holes"
          LHCb::STClusters::const_iterator iClus = m_clusters->begin();
          for( ; iClus != m_clusters->end(); ++iClus ){
            // Analyze only the IT clusters
            if( !((*iClus)->isIT()) ) continue;

            // Get the cluster's STChannelID
            LHCb::STChannelID itClusSTID = (*(*iClus)).channelID();
            
            // Same ladder within 2 strips aside
            if( ((*iSectors)->elementID().uniqueSector() == itClusSTID.uniqueSector()) &&
                (abs(holeStrip-itClusSTID.strip())<=m_nStripsTol) ){
              // Not a Tell1 artefact at beetles boundaries
              if( (holeStrip+itClusSTID.strip())%256 < 3 )
                nNeighbouringHits++;
            }
            else if( ( (*iSectors)->elementID().uniqueDetRegion() == 
                       itClusSTID.uniqueDetRegion() ) &&
                     (abs((*iSectors)->id()-itClusSTID.sector())==1) )
              // or next ladder with bordering strips
              if( (((*iSectors)->id()>itClusSTID.sector()) && 
                   (m_nStrips-int(itClusSTID.strip()-holeStrip)<=m_nStripsTol)) ||
                  (((*iSectors)->id()<itClusSTID.sector()) && 
                   (m_nStrips-int(holeStrip-itClusSTID.strip())<=m_nStripsTol)) )
                // Not a Tell1 artefact at beetles boundaries
                if( (holeStrip+itClusSTID.strip())%256 < 3 )
                  nNeighbouringHits++;
          }          
        }
      }
    }
  }

  return nNeighbouringHits;
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
        debug() << "This hit is being shared" << endreq;
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
  fitPar.clear();

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
    warning() << "Not enough hits to fit piece of track!" << endreq;
    boxOverlap.clear();
    return boxOverlap;
  }

  double xTrack = 0;
  double yTrack = 0;
  StatusCode sc;

  for( int hitCnt=0; (iNodes!=hitsOverlapBox1.end()) && 
         (iNodes!=hitsOverlapBox2.end()); ++iNodes, ++hitCnt ) {
    const LHCb::Node& aNode2 = *(*iNodes);

    LHCb::State hitState;
    sc = m_extrapolator->propagate( *aTrack, aNode2.z(), hitState );
    if( sc.isFailure() )
      break;

    if( !fitPar.empty() ) {
      xTrack = fitPar[0]*aNode2.z() + fitPar[1];
      yTrack = fitPar[2]*aNode2.z() + fitPar[3];
    }

    double b = -(aNode2.measurement().trajectory().beginPoint().X()-
                 aNode2.measurement().trajectory().endPoint().X())/
      (aNode2.measurement().trajectory().beginPoint().Y()-
       aNode2.measurement().trajectory().endPoint().Y());
    
    boxOverlap[hitCnt] =
      (xTrack +
       b*(yTrack-aNode2.measurement().trajectory().endPoint().Y()) -
       aNode2.measurement().trajectory().endPoint().X())/
      sqrt(1+b*b);

    if(hitCnt>=m_nLayers)
      error() << "hitCnt = " << hitCnt << endreq;
  }

  if( fitPar.empty() || sc.isFailure() ) {
    warning() << "Could not propagate track to defined z position!" << endreq;
    boxOverlap.clear();
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
  trackSelTuple -> column( "NResiduals", m_nRes ); // should be = m_nITHits!!!
  trackSelTuple -> farray( "Residuals", m_res.begin(),
                           m_res.end(), "LengthH", m_nMaxITHits );
  trackSelTuple -> farray( "ErrResiduals", m_errRes.begin(),
                           m_errRes.end(), "LengthH", m_nMaxITHits );
  
  trackSelTuple -> farray( "HitX", m_hitX.begin(),
                           m_hitX.end(), "LengthH", m_nMaxITHits );
  trackSelTuple -> farray( "HitY", m_hitY.begin(),
                           m_hitY.end(), "LengthH", m_nMaxITHits );
  trackSelTuple -> farray( "HitZ", m_hitZ.begin(),
                           m_hitZ.end(), "LengthH", m_nMaxITHits );
  
  trackSelTuple -> farray( "TXAtHit", m_tx.begin(),
                           m_tx.end(), "LengthH", m_nMaxITHits );
  trackSelTuple -> farray( "TYAtHit", m_ty.begin(),
                           m_ty.end(), "LengthH", m_nMaxITHits );
  
  trackSelTuple -> farray( "LengthToHit", m_lToHit.begin(),
                           m_lToHit.end(), "LengthH", m_nMaxITHits );
  
  trackSelTuple -> column( "NLadOverlaps", m_nLadOverlaps );
  trackSelTuple -> farray( "LadOverlaps", m_ladOverlaps.begin(),
                           m_ladOverlaps.end(),  "LengthLO", m_nMaxITHits );
  trackSelTuple -> farray( "LadOverlapsZ", m_ladOverlapsZ.begin(),
                           m_ladOverlapsZ.end(), "LengthLO", m_nMaxITHits );
  
  trackSelTuple -> column( "NBoxOverlaps", m_nBoxOverlaps );

  trackSelTuple -> matrix( "BoxOverlaps", m_boxOverlaps,
                           m_nStations, m_nLayers ); // To be finished
  //**********************************************************************
  
  return trackSelTuple->write();
  
}
//===========================================================================


// Test Geometry informations
// channelID: ???
// sector: one of 7 ladders in one layer of one box
// detRegion: one of 4 boxes in one station
// layer: one of 4 layers in one box

// uniqueLayer=layer+8*station:
// 12 values (layers) == 3 stations * 4 layers/station

// uniqueDetRegion=detRegion+8*uniqueLayer:
// 48 values == 3 stations * 4 boxes/station * 4 layers/box

// uniqueSector=sector+32*uniqueDetRegion:
// 336 values (ladders) == 3 stations * 4 boxes/station * 4 layers/box * 7 ladders/layer
