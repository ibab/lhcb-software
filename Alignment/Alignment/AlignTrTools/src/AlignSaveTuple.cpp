// $Id: AlignSaveTuple.cpp,v 1.11 2009-10-09 12:00:00 wouter Exp $
//

//-----------------------------------------------------------------------------
// Implementation file for class : AlignSaveTuple
//
// Louis Nicolas, Adlene Hicheur (EPFL)
// Started: 06/06
///-----------------------------------------------------------------------------

//===========================================================================
// Includes
//===========================================================================
// Local
#include "AlignSaveTuple.h"

#include "GaudiKernel/IMagneticFieldSvc.h"

// Interfaces
#include "TrackInterfaces/ITrackExtrapolator.h"
#include "MCInterfaces/ITrackGhostClassification.h"

// Det
#include "OTDet/DeOTDetector.h"
#include "STDet/DeSTDetector.h"
#include "STDet/DeITDetector.h"

// Event
#include "Event/GhostTrackInfo.h"

//===========================================================================

//===========================================================================
// Declare
//===========================================================================
// Declaration of the algorithm factory
DECLARE_ALGORITHM_FACTORY ( AlignSaveTuple )
//===========================================================================

//===========================================================================
// Constructor
//===========================================================================
AlignSaveTuple::AlignSaveTuple ( const std::string& name,
                                       ISvcLocator* pSvcLocator ):
  GaudiTupleAlg ( name, pSvcLocator ),
  m_otTracker ( 0 ),
  m_itTracker ( 0 ),
  m_extrapolator("TrackParabolicExtrapolator"),
  m_itHitExpectation("ITHitExpectation"),
  m_otHitExpectation("OTHitExpectation")
 {

  // Reserving space for some vectors
  m_closeHits.reserve  ( 1000 );
  m_sharedHits.reserve ( 1000 );

  // Ghost classification
  this -> declareProperty( "GhostClassification",
                           m_ghostToolName = "TTrackGhostClassification" );

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
  this -> declareProperty ( "TracksLocation", m_tracksPath = "Rec/Track/Best" );

  // Are we using MC or real data?
  this -> declareProperty ( "MCData", m_mcData = true );

  // 48 = 12 (layers) * 2 (ladder overlaps) * 2 (box overlaps)
  this -> declareProperty ( "IsolatedTrackNStripsTolerance",
                            m_nStripsTol = 2 );
  this -> declareProperty ( "IsolatedTrackNStrawsTolerance",
                            m_nStrawsTol = 1 );

  this -> declareProperty ( "MaxNHits", m_maxNHits = 40 );
}
//===========================================================================


//===========================================================================
// Destructor
//===========================================================================
AlignSaveTuple::~AlignSaveTuple ( ) {
}
//===========================================================================


//===========================================================================
// Initialize
//===========================================================================
StatusCode AlignSaveTuple::initialize ( ) {

  if ( msgLevel( MSG::DEBUG ) )
    debug() << "==> Initialize" << endmsg;

  StatusCode sc = GaudiTupleAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return Error( "Failed to initialize" );

  // The extrapolator
  m_extrapolator.retrieve().ignore() ;
  m_itHitExpectation.retrieve().ignore() ;
  m_otHitExpectation.retrieve().ignore() ;

  // Retrieve the ghost classification tool
  m_ghostClassification = tool<ITrackGhostClassification>( m_ghostToolName,
                                                           "GhostTool", this );

  // Get The Magnetic Field
  m_pIMF = svc<IMagneticFieldSvc>( "MagneticFieldSvc",true );

  // Get OT geometry
  m_otTracker = getDet<DeOTDetector>( m_otTrackerPath );
  m_nStations = m_otTracker->nStation();
  m_nStraws = m_otTracker->nMaxChanInModule();
  m_nOTModules = m_otTracker->modules().size() / m_otTracker->quarters().size();

  // Get IT geometry
  m_itTracker = getDet<DeITDetector>( m_itTrackerPath );
  if ( int(m_itTracker->nStation()) != m_nStations )
    warning() << "Number of IT stations is not equal to number of OT stations!!!"
              << endmsg;
  m_nLayers = m_itTracker->nLayerPerBox();
  m_nStrips = m_itTracker->nStrip()/m_itTracker->nReadoutSector()/3;

  // Max possible number of hits:
  // In OT: = nLayers * 2 (two layers of straws per layer)
  // In IT: counted wrong: nLayers = 3 stations * 4 boxes/station * 4 layers/box
  //        shouldn't count 4 boxes
  //        but compensates possible box (*2) and ladder (*2) overlaps
//   m_maxNHits = m_otTracker->layers().size()*2 + m_itTracker->nLayer();

  m_iEvent = 0;

  debug() << "AlignSaveTuple initialized successfully" << endmsg;

  return StatusCode::SUCCESS;
}
//===========================================================================


//===========================================================================
// Execute
//===========================================================================
StatusCode AlignSaveTuple::execute ( ) {

  if ( msgLevel( MSG::DEBUG ) )
    debug() << "AlignSaveTuple starting execution" << endmsg;

  // Get the odin information
  odin = get<LHCb::ODIN>( LHCb::ODINLocation::Default );

  // Get the tracks
  m_tracks = get<LHCb::Tracks>( m_tracksPath );

  // Get the ST clusters
  m_itClusters = get<STLiteClusters>( m_itClustersPath );

  // Get the OT times
  m_otTimes = get<LHCb::OTTimes>( m_otTimesPath );

  // Get the hits close to any other
  m_closeHits.clear();
  getCloseHits ( );

  // Get the hits shared by more than one track
  m_sharedHits.clear();
  getSharedHits();

  // Creating the tuple in memory
  Tuples::Tuple trackSelTuple = nTuple( "AllVariables" );

  // Get the association table
  AsctTool associator( evtSvc(), m_tracksPath );
  m_directTable = associator.direct();

  //**********************************************************************
  // Global Variables
  //**********************************************************************
  m_runNr = odin->runNumber();
  m_evtNr = odin->eventNumber();
  ++m_iEvent;
  m_eventMultiplicity = m_tracks->size();
  m_nITClusters = m_itClusters->size();
  const LHCb::VeloClusters* veloClusters = get<LHCb::VeloClusters>( LHCb::VeloClusterLocation::Default );
  m_nVeLoClusters = veloClusters->size();
  //**********************************************************************

  if ( msgLevel( MSG::DEBUG ) ) {
    debug() << "Retrieved " << m_eventMultiplicity << " tracks." << endmsg;
    debug() << "Retrieved " << m_itClusters->size() << " IT clusters." << endmsg;
    debug() << "Retrieved " << m_otTimes->size() << " OT times." << endmsg;
  }

  // Loop over tracks - select some and make some plots
  LHCb::Tracks::const_iterator iTracks = m_tracks->begin();
  m_iGoodTrack = 0;
  for ( ; iTracks != m_tracks->end(); ++iTracks ) {
    LHCb::Track& aTrack = **iTracks;

    if ( m_mcData ) {
      if ( iTracks == m_tracks->begin() ) {
        if ( isGhostTrack( &aTrack ) )
          m_ghostRate = (double)1/m_eventMultiplicity;
        else
          m_ghostRate = 0;
      }


      LHCb::Tracks::const_iterator iTracks2 = iTracks+1;
      for ( ; iTracks2 != m_tracks->end(); ++iTracks2 ) {
	LHCb::Track& tr2 = **iTracks2;
	// Compute ghost rate
	if ( iTracks == m_tracks->begin() )
	  if ( isGhostTrack( &tr2 ) )
	    m_ghostRate += (double)1/m_eventMultiplicity;
      }
    }

    if ( msgLevel( MSG::DEBUG ) )
      debug() << "******************************************************" << endmsg
              << "Entering new good track" << endmsg;

    // Get the number of IT and OT hits
    const std::vector<LHCb::LHCbID>& ids = aTrack.lhcbIDs();
    m_nITHits = m_nOTHits =  0 ;
    for( std::vector<LHCb::LHCbID>::const_iterator id = ids.begin() ; id != ids.end(); ++id)
      if(      id->isIT() ) ++m_nITHits ;
      else if( id->isOT() ) ++m_nOTHits ;

    // Only fill NTuple when there are some IT or OT Hits
    if ( !m_nITHits && !m_nOTHits ) {
      if ( msgLevel( MSG::DEBUG ) )
        debug() << "There is no hit in IT nor in OT!!!" << endmsg;
      continue;
    }
    StatusCode sc;
    if ( (sc=fillVariables( &aTrack, trackSelTuple ) )
         == StatusCode::FAILURE ) return sc;
    ++m_iGoodTrack;
  }

  return StatusCode::SUCCESS;
}
//===========================================================================


//===========================================================================
// Finalize
//===========================================================================
StatusCode AlignSaveTuple::finalize ( ) {

  if ( msgLevel( MSG::DEBUG ) )
    debug() << "==> Finalize" << endmsg;

  return GaudiTupleAlg::finalize ( );   // Must be called after all other actions
}
//===========================================================================


//===========================================================================
// Fill Variables
//===========================================================================
StatusCode
AlignSaveTuple::fillVariables ( const LHCb::Track* aTrack,
                                   Tuples::Tuple trackSelTuple ) {

  //**********************************************************************
  // Tracks Variables
  //**********************************************************************
  // Ghost? ==> ghost classification
  if ( m_mcData ) {
    m_isGhostTrack = isGhostTrack( aTrack );
    m_ghostTrackClassification = defValue;
    LHCb::GhostTrackInfo ghostClassInfo;
    if ( m_isGhostTrack ) {
      m_ghostClassification->info( *aTrack , ghostClassInfo );
      m_ghostTrackClassification = ghostClassInfo.classification();
      if ( msgLevel( MSG::DEBUG ) )
        debug() << "Ghost classification = "
                << ghostClassInfo.classification() << endmsg;
    }
  }
  // Ghost Track Classification:
  // 0  = None
  // 1  = Real
  // 10 = Conversion
  // 11 = EM
  // 12 = FromPhi
  // 13 = HadronicInteraction
  // 14 = DecayInFlight
  // 20 = SpilloverAndNoise
  // 21 = GhostParent
  // 22 = InconsistentParts
  // 23 = Combinatoric

  m_nSharedHits = 0;
  m_fSharedHits = 0;
  m_nCloseHits = 0;
  m_nHoles = 0;

  m_trackChi2PerDoF = aTrack->chi2PerDoF();
  m_trackChi2Prob = aTrack->probChi2();
  m_trackFitMatchChi2 = aTrack->info(LHCb::Track::FitMatchChi2, 999999);

  m_trackEta = aTrack->pseudoRapidity();

  m_trackP = aTrack->p()/Gaudi::Units::GeV;
  m_trackPt = aTrack->pt()/Gaudi::Units::GeV;
  m_trackErrP = sqrt(aTrack->firstState().errP2())/Gaudi::Units::GeV;
  if ( m_mcData ) {
    m_trackMCP = defValue;
    m_trackMCPt = defValue;
  }

  // Track Slope and Position at 7500 mm
  LHCb::State aState = aTrack->closestState(7500.);
  m_entryTX = aState.tx();
  m_entryTY = aState.ty();
  m_entryX = aState.x();
  m_entryY = aState.y();
  //**********************************************************************


  //**********************************************************************
  // Hits Variables
  //**********************************************************************
  m_res.clear();
  m_errRes.clear();

  m_hitX.clear();
  m_hitY.clear();
  m_hitZ.clear();

  m_nLadOverlaps = 0;
  m_ladOvlapRes.clear();
  m_ladOvlapLay.clear();

  m_nBoxOverlaps = 0;
  m_boxOvlapRes.clear();
  m_boxOvlapSta.clear();
  //**********************************************************************

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
    LHCb::MCParticle* mcPart = new LHCb::MCParticle;
    mcPart = directLink.first( aTrack );
    if ( 0 != mcPart ) {
      m_trackMCP  = mcPart->p()/Gaudi::Units::GeV;
      m_trackMCPt = mcPart->pt()/Gaudi::Units::GeV;
    }
    delete mcPart;
    //**********************************************************************
  }

  std::vector<LHCb::LHCbID>::const_iterator iIDs = aTrack->lhcbIDs().begin();
  for ( ; iIDs != aTrack->lhcbIDs().end(); ++iIDs ) {
    const LHCb::LHCbID& aID = *iIDs;
    //**********************************************************************
    // Number of shared hits in the track
    //**********************************************************************
    if ( std::binary_search( m_sharedHits.begin(),
			     m_sharedHits.end(), aID, lessByID() ) )
      ++m_nSharedHits;
    //**********************************************************************

    //**********************************************************************
    // Number of hits close to the track
    //**********************************************************************
    if ( std::binary_search( m_closeHits.begin(),
			     m_closeHits.end(), aID, lessByID() ) )
      ++m_nCloseHits;
    //**********************************************************************
  }

  // For Box Overlap
  unsigned int overlapStation = abs(defValue);

  // Loop over the nodes to get the hits variables
  LHCb::Track::ConstNodeRange nodes = aTrack->nodes() ;
  LHCb::Track::ConstNodeRange::const_iterator iNodes = nodes.begin();
  for ( ; iNodes != nodes.end(); ++iNodes ) {

    const LHCb::Node& aNode = **iNodes;
    const LHCb::FitNode* fNode = dynamic_cast<const LHCb::FitNode*>(*iNodes);

    // Only loop on hits with measurement
    if ( !aNode.hasMeasurement() ) continue;

    LHCb::LHCbID nodeID = aNode.measurement().lhcbID();
    LHCb::STChannelID theSTID;
    LHCb::OTChannelID theOTID;

    if ( aNode.measurement().type() == LHCb::Measurement::IT ) {
      theSTID = nodeID.stID();
      if ( msgLevel( MSG::VERBOSE ) )
        verbose() << "IT Hit (Station,Box,Layer,Ladder,Strip) = ("
                  << theSTID.station() << "," << theSTID.detRegion() << ","
                  << theSTID.layer() << "," << theSTID.sector() << ","
                  << theSTID.strip() << ")" << endmsg;
    }
    else if ( aNode.measurement().type() == LHCb::Measurement::OT ) {
      theOTID = nodeID.otID();
      if ( msgLevel( MSG::VERBOSE ) )
        verbose() << "OT Hit (Station,Layer,Quarter,Module,Straw) = ("
                  << theOTID.station() << "," << theOTID.layer() << ","
                  << theOTID.quarter() << "," << theOTID.module() << ","
                  << theOTID.straw() << ")" << endmsg;
    }
    else {
      Warning("Found measurement not in IT nor in OT!!!", StatusCode::SUCCESS, 1);
      continue;
    }

    //**********************************************************************
    // Residuals and Residual Errors
    //**********************************************************************
//     m_res.push_back(aNode.residual());
//     m_errRes.push_back(aNode.errResidual());
    m_res.push_back(fNode->unbiasedResidual());
    m_errRes.push_back(fNode->errUnbiasedResidual());
    //**********************************************************************

    //**********************************************************************
    // Hit Position
    //**********************************************************************
     Gaudi::XYZPoint hitPos = aNode.state().position();
     m_hitX.push_back(hitPos.X());
     m_hitY.push_back(hitPos.Y());
     m_hitZ.push_back(hitPos.Z());
    //**********************************************************************

    //**********************************************************************
    // Overlaps
    //**********************************************************************
    LHCb::Track::ConstNodeRange::const_iterator iNodes2 = iNodes+1;
    for ( ; iNodes2 != nodes.end(); ++iNodes2 ) {

      const LHCb::Node& aNode2 = **iNodes2;
      const LHCb::FitNode* fNode2 = dynamic_cast<const LHCb::FitNode*>(*iNodes2);

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
//             m_ladOvlapRes.push_back( aNode.residual()-aNode2.residual() );
          if ( aNode.z()>aNode2.z() )
            m_ladOvlapRes.push_back( fNode->unbiasedResidual()-fNode2->unbiasedResidual() );
          else
            m_ladOvlapRes.push_back( fNode2->unbiasedResidual()-fNode->unbiasedResidual() );
//             m_ladOvlapRes.push_back( aNode2.residual()-aNode.residual() );
          m_ladOvlapLay.push_back( theSTID.uniqueDetRegion() );
          if ( msgLevel( MSG::DEBUG ) )
            debug() << "Found IT ladder overlap: ladOverlap = "
                    << m_ladOvlapRes.back() << " in layer "
                    << m_ladOvlapLay.back() << endmsg;
          ++m_nLadOverlaps;
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
            m_boxOvlapRes.push_back( boxOverlap( aTrack, theSTID, theSTID2 ) );
            // Remove entry of a problem occured in boxOverlap function
            if ( m_boxOvlapRes.back() == defValue )
              m_boxOvlapRes.pop_back();
            else
              m_boxOvlapSta.push_back( overlapStation );
            if ( msgLevel(MSG::DEBUG) ) {
              debug() << "Station " << overlapStation
                      << ": Found IT box overlap: boxOverlap = "
                      << m_boxOvlapRes.back() << endmsg;
            }
            ++m_nBoxOverlaps;
          }
        }
        //**********************************************************************
      }
    }
    //**********************************************************************
  }

  //**********************************************************************
  // Number of Holes
  //**********************************************************************
  int itExpHits = m_itHitExpectation->nExpected(*aTrack);
  int otExpHits = m_otHitExpectation->nExpected(*aTrack);
  int expHits = itExpHits + otExpHits;
  if ( expHits < m_nITHits + m_nOTHits ) expHits = m_nITHits + m_nOTHits;
  m_nHoles = expHits - (m_nITHits + m_nOTHits);
  //**********************************************************************

  //**********************************************************************
  // Fraction of shared hits in the track
  //**********************************************************************
  m_fSharedHits = double(m_nSharedHits)/(m_nITHits+m_nOTHits);
  //**********************************************************************

  //**********************************************************************
  // Printing some informations in debug mode:
  //**********************************************************************
  if ( msgLevel( MSG::DEBUG ) )
    debug() << "This track has " << m_nITHits << " IT hits" << endmsg
            << "               " << itExpHits << " IT expected hits" << endmsg
            << "               " << m_nLadOverlaps << " IT ladder overlaps" << endmsg
            << "               " << m_nBoxOverlaps << " IT box overlaps" << endmsg
            << "               " << m_nOTHits << " OT hits" << endmsg
            << "               " << otExpHits << " OT expected hits" << endmsg
            << "               " << m_nHoles << " holes" << endmsg
            << "               " << m_nSharedHits << " shared hits" << " (= "
            << 100*m_fSharedHits << " %)" << endmsg
            << "               " << m_nCloseHits << " neighbouring hits" << endmsg;
  //**********************************************************************

  return writeNTuple( trackSelTuple );
}
//===========================================================================


//===========================================================================
// Is track ghost or not?
//===========================================================================
bool AlignSaveTuple::isGhostTrack ( const LHCb::Track* aTrack ) {

  DirectRange range;
  if ( !m_directTable ) {
    Error( "Track MCParticle table missing. Are you running TrackAssociator before?" );
    return false;
  }
  range = m_directTable->relations(aTrack);

  return ( range.empty() ? true:false );
}
//===========================================================================


//===========================================================================
// Get the IT clusters and OT times that are close to anything else
//===========================================================================
void AlignSaveTuple::getCloseHits ( ) {

  const STLiteClusters* itClusters = get<STLiteClusters>( m_itClustersPath );
  const LHCb::OTTimes* otTimes = get<LHCb::OTTimes>( m_otTimesPath );
  STLiteClusters::const_iterator iClus = itClusters->begin();
  LHCb::OTTimes::const_iterator iTimes = otTimes->begin();

  // Loop over all the IT clusters
  if ( itClusters->size() != 0 )
    for ( ; iClus+1 != itClusters->end(); ++iClus )
      if ( isNeighbouringHit( (*iClus).channelID(), (*(iClus+1)).channelID() ) ) {
        m_closeHits.push_back( (*iClus).channelID() );
        m_closeHits.push_back( (*(iClus+1)).channelID() );
      }

  // Loop over all the OT times
  if ( otTimes->size() != 0 )
    for ( ; iTimes+1 != otTimes->end(); ++iTimes )
      if ( isNeighbouringHit( (**iTimes).channel(), (**(iTimes+1)).channel()) ) {
        m_closeHits.push_back( (**iTimes).channel() );
        m_closeHits.push_back( (**(iTimes+1)).channel() );
      }

  // sorting and stripping out duplicates
  std::sort( m_closeHits.begin(), m_closeHits.end(), lessByID() );
  m_closeHits.erase( std::unique( m_closeHits.begin(), m_closeHits.end() ), m_closeHits.end() );
}
//===========================================================================


//===========================================================================
// Check if cluster is neighbouring the IT hit
//===========================================================================
bool AlignSaveTuple::isNeighbouringHit ( LHCb::STChannelID clusID,
                                         LHCb::STChannelID hitID ) {

  // Not a TELL1 artefact at beetles boundaries
  if ( (hitID.strip()+clusID.strip())%(2*m_nStrips) <= 3 ) return false;

  // Same ladder within N strips aside
  if ( (hitID.uniqueSector() == clusID.uniqueSector()) &&
       (abs(int(hitID.strip()-clusID.strip())) <= m_nStripsTol) ) return true;

  return false;
}
//===========================================================================


//===========================================================================
// Check if time is neighbouring the OT hit
//===========================================================================
bool AlignSaveTuple::isNeighbouringHit ( LHCb::OTChannelID timeID,
                                         LHCb::OTChannelID hitID ) {

  // Same module?
  if ( hitID.uniqueModule() != timeID.uniqueModule() ) return false;

  // In module 9 of quarters 0 and 2, only 2*32 straws
  if ( !(timeID.quarter()%2) && (int(timeID.module()) == m_nOTModules) ) {
    if ( (abs(int((hitID.straw()%int(m_nStraws/4))
                  -(timeID.straw()%int(m_nStraws/4)))) <= m_nStrawsTol) ) return true;
  }
  else if ( (abs(int((hitID.straw()%int(m_nStraws/2))
                     -(timeID.straw()%int(m_nStraws/2)))) <= m_nStrawsTol) ) return true;

  return false;
}
//===========================================================================


//===========================================================================
// Check if this hit is shared with (at least) one other track
//===========================================================================
void AlignSaveTuple::getSharedHits ( ) {

  LHCb::Tracks::const_iterator iTracks = m_tracks->begin();
  for ( ; iTracks != m_tracks->end(); ++iTracks ) {
    const LHCb::Track* aTrack = *iTracks;
    std::vector<LHCb::LHCbID>::const_iterator iIDs = aTrack->lhcbIDs().begin();
    for ( ; iIDs != aTrack->lhcbIDs().end(); ++iIDs ) {
      const LHCb::LHCbID aID = *iIDs;
      if ( isSharedHit ( aTrack, aID ) )
        m_sharedHits.push_back ( aID );
    }
  }

  // sorting and stripping out duplicates
  std::sort( m_sharedHits.begin(), m_sharedHits.end(), lessByID() );
  m_sharedHits.erase( std::unique( m_sharedHits.begin(), m_sharedHits.end() ), m_sharedHits.end() );
}
//===========================================================================


//===========================================================================
// Check if this hit is shared with (at least) one other track
//===========================================================================
bool AlignSaveTuple::isSharedHit ( const LHCb::Track* theTrack,
                                   const LHCb::LHCbID aID ) {

  LHCb::Tracks::const_iterator iTracks = m_tracks->begin();
  for ( ; iTracks != m_tracks->end(); ++iTracks ) {
    const LHCb::Track* aTrack = *iTracks;
    if ( aTrack != theTrack )
      if ( aTrack->isOnTrack( aID ) ) {
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
double AlignSaveTuple::boxOverlap ( const LHCb::Track* aTrack,
                                    LHCb::STChannelID STChanID1,
                                    LHCb::STChannelID STChanID2 ) {

  Array fitPar ( 5,defValue );

  Array bOVect;
  std::vector<const LHCb::Node*> hitsOverlapBox1;
  std::vector<const LHCb::Node*> hitsOverlapBox2;
  LHCb::Track::ConstNodeRange nodes = aTrack->nodes() ;
  LHCb::Track::ConstNodeRange::const_iterator iNodes = nodes.begin();
  LHCb::Track::ConstNodeRange::const_iterator iNodesEnd = nodes.end();
  for ( ; iNodes!=iNodesEnd; ++iNodes ) {
    const LHCb::Node& aNode = **iNodes;
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
    return defValue;
  }

  if ( fitPar.empty() ) {
    Warning( "Could not propagate track to defined z position!!!", StatusCode::SUCCESS, 1 );
    return defValue;
  }

  for ( ; iNodes != iNodesEnd; ++iNodes ) {

    const LHCb::Node& aNode2 = **iNodes;
    if ( !aNode2.hasMeasurement() )
      continue;

    double xTrack = fitPar[0]*aNode2.z()*aNode2.z() + fitPar[1]*aNode2.z() + fitPar[2];
    double yTrack = fitPar[3]*aNode2.z() + fitPar[4];

    double b = -(aNode2.measurement().trajectory().beginPoint().X()-
                 aNode2.measurement().trajectory().endPoint().X())/
      (aNode2.measurement().trajectory().beginPoint().Y()-
       aNode2.measurement().trajectory().endPoint().Y());

    bOVect.push_back( (xTrack +
                       b*(yTrack-aNode2.measurement().trajectory().endPoint().Y()) -
                       aNode2.measurement().trajectory().endPoint().X())/
                      sqrt(1+b*b) );
  }

  // Averaging the overlap residual values of hits in second box
  double bOResidual = 0;
  std::vector<double>::const_iterator iBOVect = bOVect.begin();
  for ( ; iBOVect != bOVect.end(); ++iBOVect )
    bOResidual += *iBOVect;

  return bOResidual/bOVect.size();
}
//===========================================================================


//===========================================================================
// Fit piece of track with straight line and return fit parameters
//===========================================================================
std::vector<double>
AlignSaveTuple::fitTrackPiece ( const LHCb::Track* aTrack,
				std::vector<const LHCb::Node*> hitsOverlapBox ) {

  int nHitsToFit = hitsOverlapBox.size();

  StatusCode sc;
  Array fitPar (5, defValue );

  // If 4+ hits in box, fit piece of track with a straight line
  // and using minimisation of chi2
  Array xPar(8,0);
  Array yPar(5,0);
  Gaudi::XYZPoint bFieldPoint;
  std::vector<const LHCb::Node*>::const_iterator iNodes = hitsOverlapBox.begin();
  for ( ; iNodes != hitsOverlapBox.end(); ++iNodes ) {
    const LHCb::Node& aNode = **iNodes;
    const LHCb::FitNode* fNode = dynamic_cast<const LHCb::FitNode*>(*iNodes);

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

    double sig2_res = std::pow(fNode->errUnbiasedResidual(),2);
//     double sig2_res = gsl_pow_2(aNode.errResidual());

//     xPar[0] += gsl_pow_4(aNode.z())/sig2_res;
    xPar[1] += std::pow(aNode.z(),3)/sig2_res;
    xPar[2] += std::pow(aNode.z(),2)/sig2_res;
//     xPar[3] += gsl_pow_2(aNode.z())*xHit/sig2_res;
    xPar[4] += aNode.z()/sig2_res;
    xPar[5] += aNode.z()*xHit/sig2_res;
    xPar[6] += xHit/sig2_res;
    xPar[7] += 1/sig2_res;

    yPar[0] += std::pow(aNode.z(),2)/hitState.errY2();
    yPar[1] += aNode.z()/hitState.errY2();
    yPar[2] += aNode.z()*hitState.y()/hitState.errY2();
    yPar[3] += hitState.y()/hitState.errY2();
    yPar[4] += 1/hitState.errY2();
  }

  Gaudi::XYZVector bfield;
  m_pIMF -> fieldVector ( bFieldPoint, bfield );
  double pT = 0;
  m_extrapolator -> pt ( *aTrack, bFieldPoint.z(), pT );

  // x = fitPar[0]*z�� + fitPar[1]*z + fitPar[2]
  // y = fitPar[3]*z + fitPar[4]
  if ( !sc.isFailure() ) {

    // Using pT constrained fit
    double D = std::pow(xPar[4],2)-xPar[2]*xPar[7];

    fitPar[0] = 2 * bfield.y() / pT;

    fitPar[1] = (xPar[5]*xPar[7]-xPar[4]*xPar[6]+
                 (xPar[2]*xPar[4]-xPar[1]*xPar[7])*fitPar[0])/(-D);

    fitPar[2] = ((xPar[4]*xPar[5]-xPar[2]*xPar[6]) +
                 (std::pow(xPar[2],2)-xPar[1]*xPar[4])*fitPar[0])/D;

    fitPar[4] = (yPar[1]*yPar[2]-yPar[0]*yPar[3])/(std::pow(yPar[1],2)-yPar[0]*yPar[4]);

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
StatusCode AlignSaveTuple::writeNTuple ( Tuples::Tuple trackSelTuple ) {

  //**********************************************************************
  // Fill the NTuple columns
  //**********************************************************************
  //     plot(variable, tag, "variable", min, max, nbins);
  // Global Variables
  trackSelTuple -> column ( "IEvent", m_iEvent );
  trackSelTuple -> column ( "EvtNr", m_evtNr );
  trackSelTuple -> column ( "RunNr", m_runNr );
  trackSelTuple -> column ( "Multiplicity", m_eventMultiplicity );
  if ( m_mcData )
    trackSelTuple -> column ( "GhostRate", m_ghostRate );

  trackSelTuple -> column ( "NVeLoClusters", m_nVeLoClusters );
  trackSelTuple -> column ( "NITClusters", m_nITClusters );

  // Track Variables
  trackSelTuple -> column ( "ITrack", m_iGoodTrack );

  if ( m_mcData ) {
    trackSelTuple -> column ( "IsAGhost", m_isGhostTrack );
    trackSelTuple -> column ( "GhostTrackClassification", m_ghostTrackClassification );
  }

  trackSelTuple -> column ( "NITHits", m_nITHits );
  trackSelTuple -> column ( "NOTHits", m_nOTHits );
  trackSelTuple -> column ( "NSharedHits", m_nSharedHits );
  trackSelTuple -> column ( "FSharedHits", m_fSharedHits );
  trackSelTuple -> column ( "NNeighbouringHits", m_nCloseHits );
  trackSelTuple -> column ( "NHoles", m_nHoles );

  trackSelTuple -> column ( "TrackChi2PerDoF", m_trackChi2PerDoF );
  trackSelTuple -> column ( "TrackChi2Prob", m_trackChi2Prob );
  trackSelTuple -> column ( "TrackFitMatchChi2", m_trackFitMatchChi2 );

  trackSelTuple -> column ( "TrackEta", m_trackEta );

  trackSelTuple -> column ( "TrackP", m_trackP );
  trackSelTuple -> column ( "TrackPt", m_trackPt );
  trackSelTuple -> column ( "TrackErrP", m_trackErrP );
  if ( m_mcData ) {
    trackSelTuple -> column ( "TrackMCP", m_trackMCP );
    trackSelTuple -> column ( "TrackMCPt", m_trackMCPt );
  }
  // Track State closest to 7500. mm
  trackSelTuple -> column ( "TXAt7500", m_entryTX );
  trackSelTuple -> column ( "TYAt7500", m_entryTY );
  trackSelTuple -> column ( "XAt7500", m_entryX );
  trackSelTuple -> column ( "YAt7500", m_entryY );

  // Overlaps
  trackSelTuple -> farray ( "LadOvlapRes", m_ladOvlapRes.begin(),
                            m_ladOvlapRes.end(),  "NLadOverlaps", int(m_maxNHits/2) );
  trackSelTuple -> farray ( "LadOvlapLay", m_ladOvlapLay.begin(),
                            m_ladOvlapLay.end(), "NLadOverlaps", int(m_maxNHits/2) );

  trackSelTuple -> farray ( "BoxOvlapRes", m_boxOvlapRes.begin(),
                            m_boxOvlapRes.end(), "NBoxOverlaps", m_nStations );
  trackSelTuple -> farray ( "BoxOvlapStation", m_boxOvlapSta.begin(),
                            m_boxOvlapSta.end(), "NBoxOverlaps", m_nStations );

  // Hits Variables
  trackSelTuple -> farray ( "Residuals", m_res.begin(),
                            m_res.end(), "NResiduals", m_maxNHits );
  trackSelTuple -> farray ( "ErrResiduals", m_errRes.begin(),
                            m_errRes.end(), "NResiduals", m_maxNHits );

  trackSelTuple -> farray ( "HitX", m_hitX.begin(),
			    m_hitX.end(), "NResiduals", m_maxNHits );
  trackSelTuple -> farray ( "HitY", m_hitY.begin(),
			    m_hitY.end(), "NResiduals", m_maxNHits );
  trackSelTuple -> farray ( "HitZ", m_hitZ.begin(),
			    m_hitZ.end(), "NResiduals", m_maxNHits );
  //**********************************************************************

  return trackSelTuple->write();
}
//===========================================================================
