// $Id: AlignTrackMonitor.cpp,v 1.7 2007-10-17 09:28:58 lnicolas Exp $
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
// Local
#include "AlignTrackMonitor.h"

#include "GaudiKernel/IMagneticFieldSvc.h"
#include "GaudiKernel/ToStream.h"

// Interfaces
#include "TrackInterfaces/ITrackCloneFinder.h"
#include "TrackInterfaces/ITrackExtrapolator.h"

// Det
#include "OTDet/DeOTDetector.h"
#include "STDet/DeSTDetector.h"
#include "STDet/DeITDetector.h"

// Boost
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>
using namespace boost::lambda;
//===========================================================================

//===========================================================================
// Declare
//===========================================================================
// Declaration of the algorithm factory
DECLARE_ALGORITHM_FACTORY ( AlignTrackMonitor );
//===========================================================================

//===========================================================================
// Constructor
//===========================================================================
AlignTrackMonitor::AlignTrackMonitor ( const std::string& name,
                                       ISvcLocator* pSvcLocator ):
  GaudiHistoAlg ( name, pSvcLocator ),
  m_otTracker ( 0 ),
  m_itTracker ( 0 ) {
  
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

  // Are we using MC or real data?
  this -> declareProperty ( "MCData", m_mcData = true );

  // 48 = 12 (layers) * 2 (ladder overlaps) * 2 (box overlaps)
  this -> declareProperty ( "IsolatedTrackNStripsTolerance",
                            m_nStripsTol = 2 );
  this -> declareProperty ( "IsolatedTrackNStrawsTolerance",
                            m_nStrawsTol = 1 );
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

  StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
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
  
  debug() << "AlignTrackMonitor initialized successfully" << endmsg;    

  return StatusCode::SUCCESS;
}
//===========================================================================


//===========================================================================
// Execute
//===========================================================================
StatusCode AlignTrackMonitor::execute ( ) {

  if ( msgLevel( MSG::DEBUG ) )     
    debug() << "AlignTrackMonitor starting execution" << endmsg;
  
  // Get the tracks
  m_tracks = get<LHCb::Tracks>( m_tracksPath );

  // Get the ST clusters
  m_itClusters = get<STLiteClusters>( m_itClustersPath );

  // Get the OT times
  m_otTimes = get<LHCb::OTTimes>( m_otTimesPath );
  
  // Get the association table 
  AsctTool associator( evtSvc(), m_tracksPath );
  m_directTable = associator.direct();

  if ( msgLevel( MSG::DEBUG ) ) {
    debug() << "Retrieved " << m_eventMultiplicity << " tracks." << endmsg;
    debug() << "Retrieved " << m_itClusters->size() << " IT clusters." << endmsg;
    debug() << "Retrieved " << m_otTimes->size() << " OT times." << endmsg;
  }

  //**********************************************************************
  // Global Variables
  //**********************************************************************
  m_eventMultiplicity = m_tracks->size();
  //**********************************************************************

  // Loop over tracks - select some and make some plots
  LHCb::Tracks::const_iterator iTracks = m_tracks->begin();
  for ( ; iTracks != m_tracks->end(); ++iTracks ) {
    LHCb::Track& aTrack = *(*iTracks);

    if ( (iTracks == m_tracks->begin()) && m_mcData )
      if ( isGhostTrack( &aTrack ) )
        m_ghostRate = (double)1/m_eventMultiplicity;
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
      if ( (iTracks == m_tracks->begin()) && m_mcData )
        if ( isGhostTrack( &tr2 ) )
          m_ghostRate += (double)1/m_eventMultiplicity;
      
    }
    if ( isAClone ) {
      if ( msgLevel( MSG::DEBUG ) )
        debug() << "Track is a clone! Skipping track!" << endmsg;
      continue;
    }

    if ( msgLevel( MSG::DEBUG ) )     
      debug() << "******************************************************" << endmsg
              << "Entering new good track" << endmsg;

    // Get the nunmber of IT and OT hits
    const std::vector<LHCb::LHCbID>& ids = aTrack.lhcbIDs();
    m_nITHits = std::count_if(ids.begin(), ids.end(),bind(&LHCb::LHCbID::isIT,_1));
    m_nOTHits = std::count_if(ids.begin(), ids.end(),bind(&LHCb::LHCbID::isOT,_1));
    
    // Only fill the histos when there are some IT or OT Hits
    if ( !m_nITHits && !m_nOTHits ) {
      if ( msgLevel( MSG::DEBUG ) )
        debug() << "There is no hit in IT nor in OT!!!" << endmsg;
      continue;
    }
    StatusCode sc;
    if ( (sc=fillVariables( &aTrack ) )
         == StatusCode::FAILURE ) return sc;
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

  return GaudiHistoAlg::finalize ( );   // Must be called after all other actions  
};
//===========================================================================


//===========================================================================
// Fill Variables
//===========================================================================
StatusCode
AlignTrackMonitor::fillVariables ( const LHCb::Track* aTrack ) {

  //**********************************************************************
  // Tracks Variables
  //**********************************************************************
  m_nSharedHits = 0;
  m_fSharedHits = defValue;
  m_nCloseHits = nNeighbouringHits( aTrack );
  m_nHoles = 0;
  m_nLadOverlaps = 0;
  m_nBoxOverlaps = 0;

  m_trackChi2PerDoF = aTrack->chi2PerDoF();
  m_trackChi2Prob = aTrack->probChi2();

  // Get momentum, transverse momentum and error in GeV
  m_trackP = aTrack->p()/Gaudi::Units::GeV;
  m_trackPt = aTrack->pt()/Gaudi::Units::GeV;
  m_trackErrP = sqrt(aTrack->firstState().errP2())/Gaudi::Units::GeV;
  m_trackMCP = defValue;
  m_trackMCPt = defValue;

  // Track Slope and Position at 7500 mm
  LHCb::State aState = aTrack->closestState(7500.);
  m_entryTX = aState.tx();
  m_entryTY = aState.ty();
  m_entryX = aState.x();
  m_entryY = aState.y();
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

    // Get MCParticle linked by highest weight to Track
    // and get its p and pt in GeV
    LHCb::MCParticle* mcPart = new LHCb::MCParticle;
    mcPart = directLink.first( aTrack );
    if ( 0 != mcPart ) {
      m_trackMCP  = mcPart->p()/Gaudi::Units::GeV;
      m_trackMCPt = mcPart->pt()/Gaudi::Units::GeV;
    }
    delete mcPart;
    //**********************************************************************
  }

  // For Box Overlap
  unsigned int overlapStation = abs(defValue);

  // Loop over the nodes to get the hits variables
  std::vector<LHCb::Node*>::const_iterator iNodes = aTrack->nodes().begin();
  for ( ; iNodes!=aTrack->nodes().end(); ++iNodes ) {

    const LHCb::Node& aNode = *(*iNodes);
    LHCb::FitNode* fNode = dynamic_cast<LHCb::FitNode*>(*iNodes);

    // Only loop on hits with measurement
    if ( !aNode.hasMeasurement() ) continue;
    if ( (aNode.measurement().type() != LHCb::Measurement::IT) &&
         (aNode.measurement().type() != LHCb::Measurement::OT) ) {
      Warning("Found measurement not in IT nor in OT!!!", StatusCode::SUCCESS, 1);
      continue;
    }

    LHCb::STChannelID theSTID;
    LHCb::OTChannelID theOTID;
    
    if ( aNode.measurement().type() == LHCb::Measurement::IT ) {
      theSTID = aNode.measurement().lhcbID().stID();
      if ( msgLevel( MSG::VERBOSE ) )
        verbose() << "IT Hit (Station,Box,Layer,Ladder,Strip) = (" 
                  << theSTID.station() << "," << theSTID.detRegion() << "," 
                  << theSTID.layer() << "," << theSTID.sector() << "," 
                  << theSTID.strip() << ")" << endmsg;
    }
    else if ( aNode.measurement().type() == LHCb::Measurement::OT ) {
      theOTID = aNode.measurement().lhcbID().otID();
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
    if ( aNode.measurement().type() == LHCb::Measurement::IT ) {
      int id = 100*theSTID.layer() + 10*theSTID.detRegion() + theSTID.sector();
      std::string resPlotID = "ITResiduals/";
      resPlotID.append( Gaudi::Utils::toString( theSTID.station() ) );
      resPlotID.append( "/" ).append( Gaudi::Utils::toString( id ) );
//       plot (aNode.residual(), resPlotID, resPlotID, -0.2, 0.2, 50);
      plot (fNode->unbiasedResidual(), resPlotID, resPlotID, -0.2, 0.2, 50);
    }
    else if ( aNode.measurement().type() == LHCb::Measurement::OT ) {
      int id = 100*theOTID.layer() + 10*theOTID.quarter() + theOTID.module();
      std::string resPlotID = "OTResiduals/";
      resPlotID.append( Gaudi::Utils::toString( theOTID.station() ) );
      resPlotID.append( "/" ).append( Gaudi::Utils::toString( id ) );
      plot (fNode->unbiasedResidual(), resPlotID, resPlotID, -1., 1., 50);
    }
    //**********************************************************************

    //**********************************************************************
    // Overlaps
    //**********************************************************************
    std::vector<LHCb::Node*>::const_iterator iNodes2 = iNodes+1;
    for ( ; iNodes2 != aTrack->nodes().end(); ++iNodes2 ) {

      const LHCb::Node& aNode2 = *(*iNodes2);
      LHCb::FitNode* fNode2 = dynamic_cast<LHCb::FitNode*>(*iNodes2);

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
          double lOResidual = 0;
//             lOResidual = aNode.residual() - aNode2.residual();
          if ( aNode.z() > aNode2.z() )
            lOResidual = fNode->unbiasedResidual() - fNode2->unbiasedResidual();
          else
            lOResidual = fNode2->unbiasedResidual() - fNode->unbiasedResidual();
//             lOResidual = aNode2.residual() - aNode.residual();
          int id = 1000*theSTID.layer() + 100*theSTID.detRegion() + 
            10*theSTID.sector() + theSTID2.sector();
          std::string lOResPlotID = "LadOverlaps/";
          lOResPlotID.append( Gaudi::Utils::toString( theSTID.station() ) );
          lOResPlotID.append( "/" ).append( Gaudi::Utils::toString( id ) );
          plot (lOResidual, lOResPlotID, lOResPlotID, -0.5, 0.5, 50);
          if ( msgLevel( MSG::DEBUG ) )
            debug() << "Found IT ladder overlap: ladOverlap = " 
                    << lOResidual << " in layer " << theSTID.uniqueDetRegion()
                    << endmsg;
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
            // Get the average of box overlap residuals
            double bOResidual = boxOverlap( aTrack, theSTID, theSTID2 );
            int id = 10*theSTID.detRegion() + theSTID2.detRegion();
            std::string bOResPlotID = "BoxOverlaps/";
            bOResPlotID.append( Gaudi::Utils::toString( theSTID.station() ) );
            bOResPlotID.append( "/" ).append( Gaudi::Utils::toString( id ) );
            plot (bOResidual, bOResPlotID, bOResPlotID, -150., 150., 50);
            if ( msgLevel(MSG::DEBUG) ) {
              debug() << "Station " << overlapStation 
                      << ": Found IT box overlap: boxOverlap = "
                      << bOResidual << endmsg;
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
  unsigned int itExpHits = (unsigned int)aTrack->info(LHCb::Track::nExpectedIT, 0);
  unsigned int otExpHits = (unsigned int)aTrack->info(LHCb::Track::nExpectedOT, 0);
  unsigned int expHits = itExpHits + otExpHits;
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

  return fillHistos( );
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
  if ( (hitStrip+clusID.strip())%(2*m_nStrips) <= 3 ) return false;

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
  if ( !(timeID.quarter()%2) && (int(timeID.module()) == m_nOTModules) ) {
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
double AlignTrackMonitor::boxOverlap ( const LHCb::Track* aTrack,
                                       LHCb::STChannelID STChanID1,
                                       LHCb::STChannelID STChanID2 ) {

  Array fitPar ( 5,defValue );

  Array bOVect;
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
    return defValue;
  }

  if ( fitPar.empty() ) {
    Warning( "Could not propagate track to defined z position!!!", StatusCode::SUCCESS, 1 );
    return defValue;
  }

  for ( ; iNodes != iNodesEnd; ++iNodes ) {

    const LHCb::Node& aNode2 = *(*iNodes);
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
    LHCb::FitNode* fNode = dynamic_cast<LHCb::FitNode*>(*iNodes);

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

    double sig2_res = gsl_pow_2(fNode->errUnbiasedResidual());
//     double sig2_res = gsl_pow_2(aNode.errResidual());

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

  // x = fitPar[0]*zÅ≤ + fitPar[1]*z + fitPar[2]
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
// Fill the histos
//===========================================================================
StatusCode AlignTrackMonitor::fillHistos ( ) {

  // Event Variables  
  plot ( m_eventMultiplicity, 104, "Multiplicity", 0., 500., 50);
  if ( m_mcData )
    plot ( m_ghostRate, 105, "GhostRate", -0.01, 1.01, 50);

  // Various numbers related to the track
  plot ( m_nITHits, 221, "NITHits", -0.5, 30.5, 31);
  plot ( m_nOTHits, 222, "NOTHits", -0.5, 40.5, 41);
  plot ( m_nSharedHits, 223, "NSharedHits", -0.5, 20.5, 21);
  plot ( m_fSharedHits, 224, "FSharedHits", -0.01, 1.01, 50);
  plot ( m_nCloseHits, 225, "NNeighbouringHits", -0.5, 50.5, 51);
  plot ( m_nHoles, 226, "NHoles", -0.5, 20.5, 21);
  plot ( m_nLadOverlaps, 227, "NLadOverlaps", -0.5, 15.5, 16);
  plot ( m_nBoxOverlaps, 228, "NBoxOverlaps", -0.5, 3.5, 4);
  
  // Track fit "quality"
  plot ( m_trackChi2PerDoF, 231, "TrackChi2PerDoF", -2., 100., 50);
  plot ( m_trackChi2Prob, 232, "TrackChi2Prob", -0.01, 1.01, 50);
  
  // Track momentum and more
  plot ( m_trackP, 241, "TrackP", -5., 205., 50);
  plot ( m_trackPt, 242, "TrackPt", -0.1, 10.1, 50);
  plot ( m_trackErrP, 243, "TrackErrP", -0.1, 10.1, 50);
  if ( m_mcData && (m_trackMCP != defValue) && (m_trackMCPt != defValue) ) {
    plot ( m_trackMCP, 244, "TrackMCP", -5., 205., 50);
    plot ( m_trackMCPt, 245, "TrackMCPt", -0.1, 10.1, 50);
  }
  // Track State closest to 7500. mm
  plot ( m_entryTX, 251, "TXAt7500", -1., 1., 50);
  plot ( m_entryTY, 252, "TYAt7500", -0.5, 0.5, 50);
  plot ( m_entryX, 253, "XAt7500", -3000., 3000., 50);
  plot ( m_entryY, 254, "YAt7500", -3000., 3000., 50);
  //**********************************************************************
  
  return StatusCode::SUCCESS;
}
//===========================================================================
