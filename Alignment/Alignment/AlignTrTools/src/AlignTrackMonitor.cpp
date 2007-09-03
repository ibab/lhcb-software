// $Id: AlignTrackMonitor.cpp,v 1.4 2007-09-03 13:23:32 jblouw Exp $
//

//-----------------------------------------------------------------------------
// Implementation file for class : AlignTrackMonitor
//
// Louis Nicolas, Adlene Hicheur (EPFL)
// Started: 06/06
///-----------------------------------------------------------------------------

//===========================================================================
// Includes
//===========================================================================
#include "GaudiKernel/ToolFactory.h"
// Local
#include "AlignTrackMonitor.h"
//===========================================================================


//===========================================================================
// Global Variables
//===========================================================================
// Declaration of the algorithm factory
DECLARE_ALGORITHM_FACTORY ( AlignTrackMonitor );
//===========================================================================

//===========================================================================
// Constructor
//===========================================================================
AlignTrackMonitor::AlignTrackMonitor ( const std::string& name,
                                       ISvcLocator* pSvcLocator )
  : GaudiTupleAlg ( name, pSvcLocator )
  , m_otTracker ( 0 )
  , m_itTracker ( 0 )
  , m_nStations ( 3 )
  , m_nLayers ( 4 )
  , m_nBoxes ( 4 )
  , m_nLadders ( 7 )
  , m_nStrips ( 384 )
  , m_nQuarters ( 4 )
  , m_nModules ( 9 )
  , m_nSubLayers ( 2 )
  , m_nStraws ( 128 )
  , m_pdgMuonID ( 13 ) {
  
  // constructor
  // Clone finder
  this -> declareProperty ( "CloneFinderTool",
                            m_cloneFinderName = "TrackCloneFinder" );

  // Location of the IT/OT geometries
  this -> declareProperty ( "OTGeometryPath",
                            m_otTrackerPath = DeOTDetectorLocation::Default );
  this -> declareProperty ( "ITGeometryPath",
                            m_itTrackerPath = DeSTDetLocation::IT );

  // Location of the different objects in use
  this -> declareProperty ( "ITClustersLocation",
                            m_itClustersPath = LHCb::STLiteClusterLocation::ITClusters );
  this -> declareProperty ( "OTTimesLocation",
                            m_otTimesPath = LHCb::OTTimeLocation::Default );
  this -> declareProperty ( "TracksLocation", m_tracksPath = "Rec/Track/Tsa" );
  this -> declareProperty ( "MuonIDLocation",
                            m_muonPIDsPath = LHCb::MuonPIDLocation::Default );

  // Are we using MC or real data?
  this -> declareProperty ( "MCData", m_mcData = true );

  // 48 = 12 (layers) * 2 (ladder overlaps) * 2 (box overlaps)
  this -> declareProperty ( "MaxNumberITHits", m_nMaxITHits = 48 );
  this -> declareProperty ( "IsolatedTrackNStripsTolerance",
                            m_nStripsTol = 2 );
  this -> declareProperty ( "IsolatedTrackNStrawsTolerance",
                            m_nStrawsTol = 1 );
  this -> declareProperty ( "TrackMuonMatchZ", m_trMuMatchZ = 12500 );
  this -> declareProperty ( "TrackMuonMatchChi2Cut", m_trMuMatchChi2 = 9 );
}
//===========================================================================


//===========================================================================
// Destructor
//===========================================================================
AlignTrackMonitor::~AlignTrackMonitor ( ) {
}
//===========================================================================


//===========================================================================
// Initialize
//===========================================================================
StatusCode AlignTrackMonitor::initialize ( ) {

  if ( msgLevel( MSG::DEBUG ) )
    debug() << "==> Initialize" << endmsg;

  StatusCode sc = GaudiTupleAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return Error( "Failed to initialize" );

  // The extrapolator
  m_extrapolator = tool<ITrackExtrapolator>( "TrackFastParabolicExtrapolator" );

  // Retrieve the clone finder tool
  m_cloneFinder = tool<ITrackCloneFinder>( m_cloneFinderName,
                                           "CloneFinderTool", this );

  // Get The Magnetic Field
  m_pIMF = svc<IMagneticFieldSvc>( "MagneticFieldSvc",true );

  // Get OT geometry
  m_otTracker = getDet<DeOTDetector>( m_otTrackerPath );

  // Get IT geometry
  m_itTracker = getDet<DeSTDetector>( m_itTrackerPath );

  // Get the IT detector unique layer numbers
  const DeSTDetector::Layers& itLayers = m_itTracker->layers();
  DeSTDetector::Layers::const_iterator iLayers = itLayers.begin();
  for ( ; iLayers != itLayers.end(); ++iLayers )
    m_itLayers.push_back(std::make_pair((*iLayers)->elementID().uniqueLayer(),
                                        (*iLayers)->elementID().station()));
  m_itLayers.sort();
  m_itLayers.unique();

  m_nEvents = 0;

  // Printing the parameters (option-choosable) being used
  info() << endmsg 
         << "===== Parameters of the Track Selection for T-Alignment =====" << endmsg
         << "Tracks will be retrieved at " << m_tracksPath << endmsg
         << "A track is isolated if no hit is found within " << m_nStripsTol
         << " strips in IT or " << m_nStrawsTol << " straws in OT" << endmsg
         << "Track-Muon matching done if chi2 < " << m_trMuMatchChi2 
         << " for position at z = " << m_trMuMatchZ << " mm" << endmsg
         << "=============================================================" << endmsg
         << endmsg;

  debug() << "AlignTrackMonitor initialized successfully" << endmsg;    

  return StatusCode::SUCCESS;
}
//===========================================================================


//===========================================================================
// Execute
//===========================================================================
StatusCode AlignTrackMonitor::execute ( ) {

  debug() << "AlignTrackMonitor starting execution" << endmsg;
  
  // Get the odin information
  odin = get<LHCb::ODIN>( LHCb::ODINLocation::Default );

  // Get the tracks
  m_tracks = get<LHCb::Tracks>( m_tracksPath );
  m_tracksMultiplicity = m_tracks->size();

  // Get the ST clusters
  if ( exist<STLiteClusters>( m_itClustersPath ) )
    m_itClusters = get<STLiteClusters>( m_itClustersPath );

  // Get the OT times
  if ( exist<LHCb::OTTimes>( m_otTimesPath ) )
    m_otTimes = get<LHCb::OTTimes>( m_otTimesPath );
  
  // Get the muon PIDs
  m_muIDs = get<LHCb::MuonPIDs>(m_muonPIDsPath);
  
  // Creating the tuple in memory
  Tuples::Tuple trackSelTuple = nTuple( "TrackSel N-tuple" );
  
  // Get the association table 
  AsctTool associator( evtSvc(), m_tracksPath );
  m_directTable = associator.direct();

  if ( msgLevel( MSG::DEBUG ) ) {
    debug() << "Retrieved " << m_tracksMultiplicity << " tracks." << endmsg;
    debug() << "Retrieved " << m_itClusters->size() << " IT clusters." << endmsg;
    debug() << "Retrieved " << m_otTimes->size() << " OT times." << endmsg;
  }

  //**********************************************************************
  // Global Variables
  //**********************************************************************
  m_runNr = odin->runNumber();
  m_evtNr = odin->eventNumber(); 
  m_nEvents++;
  //**********************************************************************

  // Loop over tracks - select some and make some plots
  LHCb::Tracks::const_iterator iTracks = m_tracks->begin();
  m_nGoodTracks = 0;
  for ( ; iTracks != m_tracks->end(); ++iTracks ) {
    LHCb::Track& aTrack = *(*iTracks);

    if ( iTracks == m_tracks->begin() )
      if ( isGhostTrack( &aTrack ) )
        m_ghostRate = (double)1/m_tracksMultiplicity;
      else
        m_ghostRate = 0;

    bool isAClone = false;
    // Do not run code on clones
    LHCb::Tracks::const_iterator iTracks2 = iTracks+1;
    for ( ; iTracks2 != m_tracks->end(); ++iTracks2 ) {
      LHCb::Track& tr2 = *(*iTracks2);
      if ( m_cloneFinder->areClones( aTrack, tr2 ) ) {
        isAClone = true;
        break;
      }
      
      // Compute ghost rate
      if ( iTracks == m_tracks->begin() )
        if ( isGhostTrack( &tr2 ) )
          m_ghostRate += (double)1/m_tracksMultiplicity;

    }
    if ( isAClone ) {
      if ( msgLevel( MSG::DEBUG ) )
        debug() << "Track is a clone! Skipping track!" << endmsg;
      continue;
    }

    if ( msgLevel( MSG::DEBUG ) )     
      debug() << "******************************************************" << endmsg
              << "Entering new good track" << endmsg;

    // Initialize counters for each new track
    m_nITHits = 0;
    m_nOTHits = 0;
    
    // Get the nodes for this track
    std::vector<LHCb::Node*>::const_iterator iNodes = aTrack.nodes().begin();
    for ( ; iNodes != aTrack.nodes().end(); ++iNodes ) {
      const LHCb::Node& aNode = *(*iNodes);
      if ( aNode.hasMeasurement() )
        if ( aNode.measurement().type() == LHCb::Measurement::IT )
          ++m_nITHits;
        else if ( aNode.measurement().type() == LHCb::Measurement::OT )
          ++m_nOTHits;
    }
    
    // Only fill NTuple when there are some IT or OT Hits
    if ( !m_nITHits && !m_nOTHits ) {
      if ( msgLevel( MSG::DEBUG ) )
        debug() << "There is no hit in IT nor in OT!!!" << endmsg;
      continue;
    }
    StatusCode sc;
    if ( (sc=fillVariables( &aTrack, trackSelTuple ) )
         == StatusCode::FAILURE ) return sc;
    ++m_nGoodTracks;
  }
  
  return StatusCode::SUCCESS;  
}
//===========================================================================

  
//===========================================================================
// Finalize
//===========================================================================
StatusCode AlignTrackMonitor::finalize ( ) {
  
  if ( msgLevel( MSG::DEBUG ) )
    debug() << "==> Finalize" << endmsg;

  return GaudiTupleAlg::finalize ( );   // Must be called after all other actions  
};
//===========================================================================


//===========================================================================
// Fill Variables
//===========================================================================
StatusCode
AlignTrackMonitor::fillVariables ( const LHCb::Track* aTrack,
                                   Tuples::Tuple trackSelTuple ) {

  //**********************************************************************
  // Tracks Variables
  //**********************************************************************
  m_isGhostTrack = isGhostTrack( aTrack );
  m_isMuonTrack = false;

  m_nITDoubleHits = 0;
  m_nOTDoubleHits = 0;
  m_nSharedHits = 0;
  m_fSharedHits = defValue;
  m_nCloseHits = nNeighbouringHits( aTrack );
  m_nHoles = 0;

  m_trackChi2PerDoF = aTrack->chi2PerDoF();
  m_trackChi2Prob = gsl_cdf_chisq_Q(aTrack->chi2(),aTrack->nDoF());

  m_trackP = aTrack->p();
  m_trackPt = aTrack->pt();
  m_trackErrP = sqrt(aTrack->firstState().errP2());
  if ( m_mcData ) {
    m_trackMCP = defValue;
    m_trackMCPt = defValue;
  }
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

  m_hitLocalX.clear();
  m_hitLocalY.clear();
  m_hitLocalZ.clear();

  m_tx.clear();
  m_ty.clear();

  m_nLadOverlaps = 0;
  m_ladOverlaps.clear();
  m_ladOverlapsZ.clear();

  m_nBoxOverlaps = 0;
  m_boxOverlaps = Matrix(m_nStations,Array(m_nLayers, defValue));
  //**********************************************************************

  //**********************************************************************
  // Track - Muon matching
  //**********************************************************************
  m_matchedMuon = isMuonTrack( aTrack );
  m_isMuonTrack = (0 != m_matchedMuon);
  //**********************************************************************

  LHCb::MCParticle* mcPart = new LHCb::MCParticle;
  if ( m_mcData ) {
    //**********************************************************************
    // MCP and MCPt (get MCParticle linked to the track)
    //**********************************************************************
    // Retrieve the Linker table
    LinkedTo<LHCb::MCParticle,LHCb::Track>
      directLink( evtSvc(), msgSvc(), m_tracksPath );
    if ( directLink.notFound() )
      Error("Linker table not found", StatusCode::SUCCESS, 1);
    
    // Get MCParticle linked by highest weight to Track and get its p and pt
    mcPart = directLink.first( aTrack );
    if ( 0 != mcPart ) {
      m_trackMCP  = mcPart->p();
      m_trackMCPt = mcPart->pt();
    }
    //**********************************************************************
  }

  unsigned int overlapStation = abs(defValue);
  unsigned int itStationHit = abs(defValue);
  int nITStationsHit = 0;
  unsigned int otStationHit = abs(defValue);
  int nOTStationsHit = 0;
  unsigned int tStationHit = abs(defValue);
  int nTStationsHit = 0;

  // Loop over the nodes to get the hits variables
  std::vector<LHCb::Node*>::const_iterator iNodes = aTrack->nodes().begin();
  for ( ; iNodes!=aTrack->nodes().end(); ++iNodes ) {

    const LHCb::Node& aNode = *(*iNodes);

    // Only loop on hits with measurement
    if ( !aNode.hasMeasurement() ) continue;
    if ( (aNode.measurement().type() != LHCb::Measurement::IT) &&
         (aNode.measurement().type() != LHCb::Measurement::OT) )
      Warning("Found measurement not in IT nor in OT!!!", StatusCode::SUCCESS, 1);

    LHCb::STChannelID theSTID;
    LHCb::OTChannelID theOTID;
    
    if ( aNode.measurement().type() == LHCb::Measurement::IT ) {
      theSTID = aNode.measurement().lhcbID().stID();
      if ( theSTID.station() != itStationHit ) {
        itStationHit = theSTID.station();
        ++nITStationsHit;
      }
      if ( theSTID.station() != tStationHit ) {
        tStationHit = theSTID.station();
        ++nTStationsHit;
      }
      if ( msgLevel( MSG::VERBOSE ) )
        verbose() << "IT Hit (Station,Box,Layer,Ladder,Strip) = (" 
                  << theSTID.station() << "," << theSTID.detRegion() << "," 
                  << theSTID.layer() << "," << theSTID.sector() << "," 
                  << theSTID.strip() << ")" << endmsg;
    }
    else if ( aNode.measurement().type() == LHCb::Measurement::OT ) {
      theOTID = aNode.measurement().lhcbID().otID();
      if ( theOTID.station() != otStationHit ) {
        otStationHit = theOTID.station();
        ++nOTStationsHit;
      }
      if ( theOTID.station() != tStationHit ) {
        tStationHit = theOTID.station();
        ++nTStationsHit;
      }
      if ( msgLevel( MSG::VERBOSE ) )
        verbose() << "OT Hit (Station,Layer,Quarter,Module,Straw) = (" 
                  << theOTID.station() << "," << theOTID.layer() << "," 
                  << theOTID.quarter() << "," << theOTID.module() << "," 
                  << theOTID.straw() << ")" << endmsg;
    }

    //**********************************************************************
    // Number of shared hits in the track
    //**********************************************************************
    if ( isSharedHit( aTrack, &aNode ) )
      ++m_nSharedHits;
    //**********************************************************************

    //**********************************************************************
    // Residuals and Residual Errors
    //**********************************************************************
    ++m_nRes;
    m_res.push_back(aNode.residual());
    m_errRes.push_back(aNode.errResidual());
    //**********************************************************************

    //**********************************************************************    
    // Hit Position
    //**********************************************************************
    Gaudi::XYZPoint hitPos = aNode.state().position();
    m_hitX.push_back(hitPos.X());
    m_hitY.push_back(hitPos.Y());
    m_hitZ.push_back(hitPos.Z());

    Gaudi::XYZPoint hitLocalPos(defValue,defValue,defValue);
    if ( aNode.measurement().type() == LHCb::Measurement::IT )
      if( 0 != m_itTracker->findSector(theSTID) )
        hitLocalPos = m_itTracker->findSector(theSTID)->toLocal(hitPos);
      else if ( aNode.measurement().type() == LHCb::Measurement::OT )
        if( 0 != m_otTracker->findModule(theOTID) )
          hitLocalPos = m_otTracker->findModule(theOTID)->toLocal(hitPos);

    m_hitLocalX.push_back(hitLocalPos.X());
    m_hitLocalY.push_back(hitLocalPos.Y());
    m_hitLocalZ.push_back(hitLocalPos.Z());
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
    std::vector<LHCb::Node*>::const_iterator iNodes2 = iNodes+1;
    for ( ; iNodes2 != aTrack->nodes().end(); ++iNodes2 ) {

      const LHCb::Node& aNode2 = *(*iNodes2);

      // Only loop on hits with measurement
      if ( !aNode2.hasMeasurement() ) continue;

      LHCb::STChannelID theSTID2;
      LHCb::OTChannelID theOTID2;
      if ( aNode2.measurement().type() == LHCb::Measurement::IT )
        theSTID2 = aNode2.measurement().lhcbID().stID();
      else if ( aNode2.measurement().type() == LHCb::Measurement::OT )
        theOTID2 = aNode2.measurement().lhcbID().otID();
      
      if ( (aNode.measurement().type() == LHCb::Measurement::IT) &&
           (aNode2.measurement().type() == LHCb::Measurement::IT) ) {
        //**********************************************************************
        // Ladder overlap residuals
        //**********************************************************************
        // Two hits in same layer of same box but not same ladder
        if ( (theSTID.uniqueDetRegion() == theSTID2.uniqueDetRegion()) &&
             (theSTID.sector() != theSTID2.sector()) ) {
          // Convention: define overlap residual as
          // residual(at bigger z) - residual (at smaller z)
          if ( aNode.z()>aNode2.z() )
            m_ladOverlaps.push_back(aNode.residual()-aNode2.residual());
          else
            m_ladOverlaps.push_back(aNode2.residual()-aNode.residual());
          m_ladOverlapsZ.push_back((aNode.z()+aNode2.z())/2);
          if ( msgLevel( MSG::DEBUG ) )
            debug() << "Found IT ladder overlap: ladOverlap = " 
                    << m_ladOverlaps.back() << " at <z> = "
                    << m_ladOverlapsZ.back() << endmsg;
          ++m_nLadOverlaps;
        }
        //**********************************************************************
        
        //**********************************************************************
        // Doubled hits for IT (splitted clusters at beetles boundaries)
        //**********************************************************************
        // Two hits in same ladder
        if ( theSTID.uniqueSector() == theSTID2.uniqueSector() ) {
          if ( msgLevel( MSG::DEBUG ) )
            debug() << "Found IT double hit" << endmsg;
          ++m_nITDoubleHits;
        }
        //**********************************************************************
        
        //**********************************************************************
        // Box overlap residuals
        //**********************************************************************
        // Different hits in different box of same station
        if ( (theSTID.station() == theSTID2.station())
             && (theSTID.detRegion() != theSTID2.detRegion()) ) {
          
          // Save only one overlap per station (up to three box overlaps per track)
          if ( theSTID.station() != overlapStation ) {
            overlapStation = theSTID.station();
            // Get a vector of box overlap residuals
            if ( int(overlapStation-1) < m_nStations ) {
              m_boxOverlaps[overlapStation-1] =
                boxOverlap( aTrack, theSTID, theSTID2 );
              if ( msgLevel(MSG::DEBUG) ) {
                debug() << "Station " << overlapStation 
                        << ": Found IT box overlap: boxOverlap = " << endmsg;
                for ( int i=0; i<m_nLayers; ++i )
                  debug() << "layer " << i << " overlap = " 
                          << m_boxOverlaps[overlapStation-1][i] << endmsg;
              }
              ++m_nBoxOverlaps;
            }
            else
              Warning( "#Station > max number of stations", StatusCode::SUCCESS, 1 );
          }
        }
        //**********************************************************************
      }
      else if ( (aNode.measurement().type() == LHCb::Measurement::OT) &&
                (aNode2.measurement().type() == LHCb::Measurement::OT) )
        //**********************************************************************
        // Doubled hits for OT
        //**********************************************************************
        // Two hits in same "sub-layer" of same module
        if ( (theOTID.uniqueModule() == theOTID2.uniqueModule()) &&
             (abs(int(theOTID.straw()-theOTID2.straw()))<5) ) {
          if ( msgLevel( MSG::DEBUG ) )
            debug() << "Found OT double hit" << endmsg;
          ++m_nOTDoubleHits;
        }
      //**********************************************************************
    }
    //**********************************************************************
  }

  //**********************************************************************
  // Calculate number of holes using the available information
  //**********************************************************************
  // In case no hit in OT
  if ( m_nITHits && !m_nOTHits )
    m_nHoles = (m_nStations + m_nBoxOverlaps)*m_nLayers + m_nLadOverlaps + 
      m_nITDoubleHits - m_nITHits;
  // In case no hit in IT
  else if ( !m_nITHits && m_nOTHits )
    m_nHoles = m_nStations*m_nSubLayers*m_nLayers + m_nOTDoubleHits - m_nOTHits;
  // In case both in IT and OT
  else if ( m_nITHits && m_nOTHits )
    m_nHoles = (nITStationsHit + m_nBoxOverlaps)*m_nLayers + m_nLadOverlaps + 
      m_nITDoubleHits - m_nITHits +
      nOTStationsHit*m_nSubLayers*m_nLayers + m_nOTDoubleHits - m_nOTHits +
      (m_nStations-nTStationsHit>0)*(m_nStations-nTStationsHit)*
      int(m_nLayers*(1+m_nSubLayers)/2);
  //**********************************************************************

  //**********************************************************************
  // Fraction of shared hits in the track
  //**********************************************************************
  m_fSharedHits = 100*double(m_nSharedHits)/(m_nITHits+m_nOTHits);
  //**********************************************************************

  // Check that all IT and OT Hits have given a residual
  if ( (m_nITHits+m_nOTHits) != m_nRes )
    Warning( "Number of saved residuals != Number of IT+OT Hits !!!",
             StatusCode::SUCCESS, 1 );

  //**********************************************************************
  // Printing some informations in debug mode:
  //**********************************************************************
  if ( msgLevel( MSG::DEBUG ) )
    debug() << "This track has " << m_nITHits << " IT hits" << endmsg
            << "               " << m_nITDoubleHits << " IT double hits" << endmsg
            << "               " << m_nLadOverlaps << " IT ladder overlaps" << endmsg
            << "               " << m_nBoxOverlaps << " IT box overlaps" << endmsg
            << "               " << m_nOTHits << " OT hits" << endmsg
            << "               " << m_nOTDoubleHits << " OT double hits" << endmsg
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
bool AlignTrackMonitor::isGhostTrack ( const LHCb::Track* aTrack ) {

  DirectRange range = m_directTable->relations(aTrack);

  return ( range.empty() ? true:false );
}
//===========================================================================


//===========================================================================
// Match Tracks with Muons
//===========================================================================
LHCb::MuonPID* AlignTrackMonitor::isMuonTrack (const LHCb::Track* aTrack) {

  std::list<std::pair<double,LHCb::MuonPID*> > matchedMuList;

  LHCb::MCParticle* muonPart = new LHCb::MCParticle;
  if ( m_mcData ) {
    //**********************************************************************
    // Get MCParticle linked by highest weight to Track, get its ID and
    // check that it is a muon (only for testing)
    LinkedTo<LHCb::MCParticle,LHCb::Track>
      directLink( evtSvc(), msgSvc(), m_tracksPath );
    if ( directLink.notFound() )
      Error("Linker table not found", StatusCode::SUCCESS, 1);
    
    muonPart = directLink.first( aTrack );
    //**********************************************************************
  }
  
  LHCb::MuonPIDs::const_iterator iMuPIDs = m_muIDs->begin();
  for ( ; iMuPIDs != m_muIDs->end(); ++iMuPIDs ) {

    // Try to match track to muon only if there is a muon found
    if ( !(*iMuPIDs)->IsMuon() ) continue;

    LHCb::State trState;
    LHCb::State muState;
    if ( (m_extrapolator->propagate( *aTrack, m_trMuMatchZ, trState ) 
          == StatusCode::FAILURE) ||
         (m_extrapolator->propagate( *((*iMuPIDs)->idTrack()), m_trMuMatchZ, muState )
          == StatusCode::FAILURE) ) {
      std::ostringstream mess;
      mess << "Could not propagate track to defined z position (" << m_trMuMatchZ << ")!!!";
      Warning( mess.str(), StatusCode::SUCCESS, 1 );
      return 0;
    }

    double chi2 =
      gsl_pow_2(trState.x()-muState.x())/(trState.errX2()+muState.errX2()) +
      gsl_pow_2(trState.y()-muState.y())/(trState.errY2()+muState.errY2());

    if (chi2<m_trMuMatchChi2) {
      if ( msgLevel( MSG::DEBUG ) )
        debug() << "Track matched to a muon" << endmsg;
      if ( m_mcData )
        if ( 0 != muonPart )
          if ( muonPart->particleID().abspid() != m_pdgMuonID ) {
            std::ostringstream mess;
            mess << "Track should not be matched to a muon! (MCParticle ID = " 
                 << muonPart->particleID().abspid() << ")";
            Warning( mess.str(), StatusCode::SUCCESS, 1 );
          }
      matchedMuList.push_back(std::make_pair(chi2,(*iMuPIDs)));
    }
  }

  if ( !matchedMuList.empty() ) {
    matchedMuList.sort();
    return matchedMuList.front().second;
  }

  if ( msgLevel( MSG::DEBUG ) )
    debug() << "Track not matched to a muon" << endmsg;
  if ( m_mcData )
    if ( 0 != muonPart )
      if ( (muonPart->particleID().abspid() == m_pdgMuonID) &&
           (muonPart->p() > 5*Gaudi::Units::GeV) ) {
        std::ostringstream mess;
        mess << "Track could be matched to a muon! (MCParticle ID = " 
             << m_pdgMuonID << " and P > 5 GeV)";
        Warning( mess.str(), StatusCode::SUCCESS, 1 );
      }
  
  delete muonPart;

  return 0;
}
//===========================================================================


//===========================================================================
// Is track isolated or not?
//===========================================================================
int AlignTrackMonitor::nNeighbouringHits ( const LHCb::Track* aTrack ) {

  int nNeighbouringHits = 0;

  //**********************************************************************
  // For each IT/OT hit check if there are neighbouring clusters/times
  //**********************************************************************
  STLiteClusters::const_iterator iClus = m_itClusters->begin();
  for ( ; iClus != m_itClusters->end(); ++iClus ) {
    // Not already on the track
    if ( aTrack->isOnTrack( (*iClus).channelID() ) ) continue;

    std::vector<LHCb::Node*>::const_iterator iNodes = aTrack->nodes().begin();  
    for ( ; iNodes != aTrack->nodes().end(); ++iNodes ) {
      if ( (*iNodes)->hasMeasurement() ) {
        if ( (*iNodes)->measurement().type() == LHCb::Measurement::IT ) {
          LHCb::STChannelID nodeSTID;
          nodeSTID = (*iNodes)->measurement().lhcbID().stID();
          
          // For each IT hit, check that no other cluster is found close to it          
          if ( isNeighbouringHit((*iClus).channelID(),
                                 nodeSTID, nodeSTID.strip()) ) {
            ++nNeighbouringHits;
            break;
          }
        }
      }
    }
  }
  
  LHCb::OTTimes::const_iterator iTimes = m_otTimes->begin();
  for ( ; iTimes != m_otTimes->end(); ++iTimes ) {
    // Not already on the track
    if ( aTrack->isOnTrack( (*iTimes)->channel() ) ) continue;

    std::vector<LHCb::Node*>::const_iterator iNodes = aTrack->nodes().begin();  
    for ( ; iNodes != aTrack->nodes().end(); ++iNodes ) {    
      if ( (*iNodes)->hasMeasurement() ) {
        if ( (*iNodes)->measurement().type() == LHCb::Measurement::OT ) {        
          LHCb::OTChannelID nodeOTID;
          nodeOTID = (*iNodes)->measurement().lhcbID().otID();
          
          // For each OT hit, check that no other time is found close to it
          if ( isNeighbouringHit((*iTimes)->channel(),
                                 nodeOTID, nodeOTID.straw()) ) {
            ++nNeighbouringHits;
            break;
          }
        }
      }
    }
  }
  //**********************************************************************

  return nNeighbouringHits;
}
//===========================================================================


//===========================================================================
// Check if cluster is neighbouring the IT hit
//===========================================================================
bool AlignTrackMonitor::isNeighbouringHit ( LHCb::STChannelID clusID,
                                            LHCb::STChannelID hitID,
                                            unsigned int hitStrip ) {
  
  // Not a TELL1 artefact at beetles boundaries
  if ( (hitStrip+clusID.strip())%256 <= 3 ) return false;

  // Same ladder within 2 strips aside
  if ( (hitID.uniqueSector() == clusID.uniqueSector()) &&
       (abs(int(hitStrip-clusID.strip()))<=m_nStripsTol) ) return true;
  
  return false;
}
//===========================================================================


//===========================================================================
// Check if time is neighbouring the OT hit
//===========================================================================
bool AlignTrackMonitor::isNeighbouringHit ( LHCb::OTChannelID timeID,
                                            LHCb::OTChannelID hitID,
                                            unsigned int hitStraw ) {
       
  // Same module?
  if ( hitID.uniqueModule() != timeID.uniqueModule() ) return false;
  
  // In module 9 of quarters 0 and 2, only 2*32 straws
  if ( !(timeID.quarter()%2) && (int(timeID.module()) == m_nModules) ) {
    if ( (abs(int((hitStraw%int(m_nStraws/4))
              -(timeID.straw()%int(m_nStraws/4))))<=m_nStrawsTol) ) return true;
  }
  else if ( (abs(int((hitStraw%int(m_nStraws/2))
                 -(timeID.straw()%int(m_nStraws/2))))<=m_nStrawsTol) ) return true;

  return false;
}
//===========================================================================


//===========================================================================
// Check if this hit is shared with (at least) one other track
//===========================================================================
bool AlignTrackMonitor::isSharedHit ( const LHCb::Track* iTrack,
                                      const LHCb::Node* aNode ) {
  
  LHCb::LHCbID measurementID;
  if ( aNode->hasMeasurement() )
    measurementID = aNode->measurement().lhcbID();
  else
    return false;

  LHCb::Tracks::const_iterator iTracks = m_tracks->begin();
  for ( ; iTracks != m_tracks->end(); ++iTracks ) {
    const LHCb::Track& aTrack = *(*iTracks);
    if ( &aTrack != iTrack )
      if ( aTrack.isMeasurementOnTrack(measurementID) ) {
        if ( msgLevel( MSG::DEBUG ) )
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
AlignTrackMonitor::boxOverlap ( const LHCb::Track* aTrack,
                                LHCb::STChannelID STChanID1,
                                LHCb::STChannelID STChanID2 ) {

  Array fitPar ( 5,defValue );

  Array boxOverlap ( m_nLayers, defValue );
  std::vector<LHCb::Node*> hitsOverlapBox1;
  std::vector<LHCb::Node*> hitsOverlapBox2;
  std::vector<LHCb::Node*>::const_iterator iNodes = aTrack->nodes().begin();
  std::vector<LHCb::Node*>::const_iterator iNodesEnd = aTrack->nodes().end();

  for ( ; iNodes!=iNodesEnd; ++iNodes ) {
    LHCb::Node& aNode = *(*iNodes);
    LHCb::STChannelID aSTID;
    if ( aNode.hasMeasurement() )
      aSTID = aNode.measurement().lhcbID().stID();
    else continue;
    // Save hits of overlapping box 1
    if ( (aSTID.station() == STChanID1.station()) &&
         (aSTID.detRegion() == STChanID1.detRegion()) )
      hitsOverlapBox1.push_back( &aNode );
    // Save hits of overlapping box 2
    if ( (aSTID.station() == STChanID2.station()) &&
         (aSTID.detRegion() == STChanID2.detRegion()) )
      hitsOverlapBox2.push_back( &aNode );
  }  

  if ( (int(hitsOverlapBox1.size()) >= m_nLayers) ||
       (int(hitsOverlapBox2.size()) >= m_nLayers) )
    if ( hitsOverlapBox1.size() >= hitsOverlapBox2.size() ) {
      fitPar = fitTrackPiece(aTrack, hitsOverlapBox1);
      iNodes = hitsOverlapBox2.begin();
      iNodesEnd = hitsOverlapBox2.end();
    }
    else {
      fitPar = fitTrackPiece(aTrack, hitsOverlapBox2);
      iNodes = hitsOverlapBox1.begin();
      iNodesEnd = hitsOverlapBox1.end();
    }
  else {
    Warning( "Not enough hits to fit piece of track!!!", StatusCode::SUCCESS, 1 );
    return boxOverlap;
  }

  if ( fitPar.empty() ) {
    Warning( "Could not propagate track to defined z position!!!", StatusCode::SUCCESS, 1 );
    return boxOverlap;
  }

  for ( int hitCnt=0; iNodes != iNodesEnd; ++iNodes, ++hitCnt ) {
    if ( hitCnt>=m_nLayers ) {
      std::ostringstream mess;
      mess << "Too many hits (" << hitCnt+1 
           << ") to save in box overlap!!! Keeping only " << m_nLayers << "!!!";
      Warning( mess.str(), StatusCode::SUCCESS, 1 );
      break;
    }

    const LHCb::Node& aNode2 = *(*iNodes);
    if ( !aNode2.hasMeasurement() ) {
      --hitCnt;
      continue;
    }

    double xTrack = fitPar[0]*aNode2.z()*aNode2.z() + fitPar[1]*aNode2.z() + fitPar[2];
    double yTrack = fitPar[3]*aNode2.z() + fitPar[4];
    
    double b = -(aNode2.measurement().trajectory().beginPoint().X()-
                 aNode2.measurement().trajectory().endPoint().X())/
      (aNode2.measurement().trajectory().beginPoint().Y()-
       aNode2.measurement().trajectory().endPoint().Y());

    if ( hitCnt < m_nLayers )
      boxOverlap[hitCnt] =
        (xTrack +
         b*(yTrack-aNode2.measurement().trajectory().endPoint().Y()) -
         aNode2.measurement().trajectory().endPoint().X())/
        sqrt(1+b*b);
    else {
      Warning( "#Layer > max number of layers", StatusCode::SUCCESS, 1 );
      return boxOverlap;
    }
  }

  return boxOverlap;
}
//===========================================================================

 
//===========================================================================
// Fit piece of track with straight line and return fit parameters
//===========================================================================
std::vector<double>
AlignTrackMonitor::fitTrackPiece ( const LHCb::Track* aTrack,
                                   std::vector<LHCb::Node*> hitsOverlapBox ) {

  int nHitsToFit = hitsOverlapBox.size();

  StatusCode sc;
  Array fitPar (5, defValue );

  // If 4+ hits in box, fit piece of track with a straight line
  // and using minimisation of chi2
  Array xPar(8,0);
  Array yPar(5,0);
  Gaudi::XYZPoint bFieldPoint;
  std::vector<LHCb::Node*>::const_iterator iNodes = hitsOverlapBox.begin();
  for ( ; iNodes != hitsOverlapBox.end(); ++iNodes ) {
    const LHCb::Node& aNode = *(*iNodes);

    LHCb::State hitState;
    sc = m_extrapolator->propagate( *aTrack, aNode.z(), hitState );
    if ( sc.isFailure() ) continue;
    
    if ( !aNode.hasMeasurement() ) {
      --nHitsToFit;
      if ( nHitsToFit < m_nLayers ) {
        fitPar.clear();
        return fitPar;
      } 
      continue;
    }

    double xHit =
      aNode.measurement().trajectory().beginPoint().X() +
      hitState.y()*
      (aNode.measurement().trajectory().endPoint().X() -
       aNode.measurement().trajectory().beginPoint().X())/
      (aNode.measurement().trajectory().endPoint().Y() -
       aNode.measurement().trajectory().beginPoint().Y());

    bFieldPoint = hitState.position();

    double sig2_res = gsl_pow_2(aNode.errResidual());

//     xPar[0] += gsl_pow_4(aNode.z())/sig2_res;
    xPar[1] += gsl_pow_3(aNode.z())/sig2_res;
    xPar[2] += gsl_pow_2(aNode.z())/sig2_res;
//     xPar[3] += gsl_pow_2(aNode.z())*xHit/sig2_res;
    xPar[4] += aNode.z()/sig2_res;
    xPar[5] += aNode.z()*xHit/sig2_res;
    xPar[6] += xHit/sig2_res;
    xPar[7] += 1/sig2_res;

    yPar[0] += gsl_pow_2(aNode.z())/hitState.errY2();
    yPar[1] += aNode.z()/hitState.errY2();
    yPar[2] += aNode.z()*hitState.y()/hitState.errY2();
    yPar[3] += hitState.y()/hitState.errY2();
    yPar[4] += 1/hitState.errY2();
  }

  Gaudi::XYZVector bfield;
  m_pIMF -> fieldVector ( bFieldPoint, bfield );
  double pT = 0;
  m_extrapolator -> pt ( *aTrack, bFieldPoint.z(), pT );

  // x = fitPar[0]*z² + fitPar[1]*z + fitPar[2]
  // y = fitPar[3]*z + fitPar[4]
  if ( !sc.isFailure() ) {

    // Using pT constrained fit
    double D = gsl_pow_2(xPar[4])-xPar[2]*xPar[7];
    
    fitPar[0] = 2 * bfield.y() / pT;
    
    fitPar[1] = (xPar[5]*xPar[7]-xPar[4]*xPar[6]+
                 (xPar[2]*xPar[4]-xPar[1]*xPar[7])*fitPar[0])/(-D);

    fitPar[2] = ((xPar[4]*xPar[5]-xPar[2]*xPar[6]) +
                 (gsl_pow_2(xPar[2])-xPar[1]*xPar[4])*fitPar[0])/D;

    fitPar[4] = (yPar[1]*yPar[2]-yPar[0]*yPar[3])/(gsl_pow_2(yPar[1])-yPar[0]*yPar[4]);

    fitPar[3] = (yPar[2]-yPar[1]*fitPar[4])/yPar[0];

  }
  else 
    fitPar.clear();
  
  return fitPar;
}
//===========================================================================


//===========================================================================
// Write NTuple
//===========================================================================
StatusCode AlignTrackMonitor::writeNTuple ( Tuples::Tuple trackSelTuple ) {

  //**********************************************************************
  // Fill the NTuple columns
  //**********************************************************************
  //     plot(variable, tag, "variable", min, max, nbins);
  // Global Variables
  trackSelTuple -> column ( "NEvents", m_nEvents );
  trackSelTuple -> column ( "EvtNr", m_evtNr );
  trackSelTuple -> column ( "RunNr", m_runNr );
  trackSelTuple -> column ( "Multiplicity", m_tracksMultiplicity );
  trackSelTuple -> column ( "GhostRate", m_ghostRate );

  // Track Variables                     
  trackSelTuple -> column ( "NTrack", m_nGoodTracks );
  
  trackSelTuple -> column ( "IsAGhost", m_isGhostTrack );
  trackSelTuple -> column ( "IsAMuon", m_isMuonTrack );
  
  trackSelTuple -> column ( "NITHits", m_nITHits );
  trackSelTuple -> column ( "NOTHits", m_nOTHits );
  trackSelTuple -> column ( "NSharedHits", m_nSharedHits );
  trackSelTuple -> column ( "FSharedHits", m_fSharedHits );
  trackSelTuple -> column ( "NNeighbouringHits", m_nCloseHits );
  trackSelTuple -> column ( "NHoles", m_nHoles );
  
  trackSelTuple -> column ( "TrackChi2PerDoF", m_trackChi2PerDoF );
  trackSelTuple -> column ( "TrackChi2Prob", m_trackChi2Prob );
  
  trackSelTuple -> column ( "TrackP", m_trackP );
  trackSelTuple -> column ( "TrackPt", m_trackPt );
  trackSelTuple -> column ( "TrackErrP", m_trackErrP );
  if ( m_mcData ) {
    trackSelTuple -> column ( "TrackMCP", m_trackMCP );
    trackSelTuple -> column ( "TrackMCPt", m_trackMCPt );
  }
  
  // Hits Variables
  trackSelTuple -> farray ( "Residuals", m_res.begin(),
                            m_res.end(), "NResiduals", m_nMaxITHits );
  trackSelTuple -> farray ( "ErrResiduals", m_errRes.begin(),
                            m_errRes.end(), "NResiduals", m_nMaxITHits );
  
  trackSelTuple -> farray ( "HitX", m_hitX.begin(),
                            m_hitX.end(), "NResiduals", m_nMaxITHits );
  trackSelTuple -> farray ( "HitY", m_hitY.begin(),
                            m_hitY.end(), "NResiduals", m_nMaxITHits );
  trackSelTuple -> farray ( "HitZ", m_hitZ.begin(),
                            m_hitZ.end(), "NResiduals", m_nMaxITHits );
  
  trackSelTuple -> farray ( "HitLocalX", m_hitLocalX.begin(),
                            m_hitLocalX.end(), "NResiduals", m_nMaxITHits );
  trackSelTuple -> farray ( "HitLocalY", m_hitLocalY.begin(),
                            m_hitLocalY.end(), "NResiduals", m_nMaxITHits );
  trackSelTuple -> farray ( "HitLocalZ", m_hitLocalZ.begin(),
                            m_hitLocalZ.end(), "NResiduals", m_nMaxITHits );
  
  trackSelTuple -> farray ( "TXAtHit", m_tx.begin(),
                            m_tx.end(), "NResiduals", m_nMaxITHits );
  trackSelTuple -> farray ( "TYAtHit", m_ty.begin(),
                            m_ty.end(), "NResiduals", m_nMaxITHits );
  
  trackSelTuple -> farray ( "LadOverlaps", m_ladOverlaps.begin(),
                            m_ladOverlaps.end(),  "NLadOverlaps", m_nMaxITHits );
  trackSelTuple -> farray ( "LadOverlapsZ", m_ladOverlapsZ.begin(),
                            m_ladOverlapsZ.end(), "NLadOverlaps", m_nMaxITHits );
  
  trackSelTuple -> column ( "NBoxOverlaps", m_nBoxOverlaps );
  trackSelTuple -> matrix ( "BoxOverlaps", m_boxOverlaps,
                            m_nStations, m_nLayers );
  //**********************************************************************
  
  return trackSelTuple->write();  
}
//===========================================================================
