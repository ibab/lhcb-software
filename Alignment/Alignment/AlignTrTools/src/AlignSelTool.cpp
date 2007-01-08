// $Id: AlignSelTool.cpp,v 1.1 2007-01-08 15:51:39 lnicolas Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// local
#include "AlignSelTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : AlignSelTool
//
// 2006-12-01 : Louis Nicolas
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY ( AlignSelTool );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
AlignSelTool::AlignSelTool ( const std::string& type,
                             const std::string& name,
                             const IInterface* parent )
  : GaudiTool ( type, name , parent )
  , m_nStations ( 3 )
  , m_nLayers ( 4 )
  , m_nStrips ( 384 )
  , m_nSubLayers ( 2 )
  , m_nModules ( 9 )
  , m_nStraws ( 128 ) {
  
  declareInterface<IAlignSelTool>(this);

//   declareProperty( "ITGeometryPath",
//                    m_itTrackerPath = DeSTDetLocation::location("IT") );
//   declareProperty ( "OTGeometryPath",
//                     m_otTrackerPath = DeOTDetectorLocation::Default );

  declareProperty( "TracksLocation", m_tracksPath = "Rec/Track/Tsa" );
  declareProperty( "ITClustersLocation",
                   m_itClustersPath = LHCb::STLiteClusterLocation::ITClusters );
  declareProperty ( "OTTimesLocation",
                    m_otTimesPath = LHCb::OTTimeLocation::Default );

  declareProperty( "BFieldStatus",                   m_fieldOn        = true          );
  declareProperty ( "IsolatedTrackOTTolerance",
                    m_otCloseHitTol = 2*Gaudi::Units::mm );
  declareProperty ( "IsolatedTrackNStrawsTolerance", m_nStrawsTol = 1 );
  declareProperty ( "IsolatedTrackITTolerance",
                    m_itCloseHitTol = 0.5*Gaudi::Units::mm );
  declareProperty ( "IsolatedTrackNStripsTolerance", m_nStripsTol = 2 );

  declareProperty( "MinMomentumCut",                 c_minP           = defValue      );
  declareProperty( "Chi2PerDoFMaxCut",               c_maxChi2PerDoF  = abs(defValue) );
  declareProperty( "Chi2ProbMinCut",                 c_minChi2Prob    = defValue      );
  declareProperty( "NHolesMaxCut",                   c_maxNHoles      = abs(defValue) );
  declareProperty( "NSharedHitsMaxCut",              c_maxNSharedHits = abs(defValue) );
  declareProperty( "NCloseHitsMaxCut",               c_maxNCloseHits  = abs(defValue) );
}
//=============================================================================


//=============================================================================
// Destructor
//=============================================================================
AlignSelTool::~AlignSelTool ( ) { }
//=============================================================================


//=============================================================================
// Initialization
//=============================================================================
StatusCode AlignSelTool::initialize ( ) {

  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;  // error already reported by base class

//   m_otTracker = getDet<DeOTDetector>( m_otTrackerPath );
//   m_itTracker = getDet<DeSTDetector>( m_itTrackerPath );

  m_extrapolator = tool<ITrackExtrapolator>( "TrackFastParabolicExtrapolator" );
  
  return StatusCode::SUCCESS;
}
//=============================================================================


//=============================================================================
// Cutting on all the variables
//=============================================================================
bool AlignSelTool::selectTrack ( LHCb::Track* aTrack ) {

  if ( c_maxNCloseHits < abs(defValue) ) {
    // Get The IT Clusters
    if ( exist<STLiteClusters>( m_itClustersPath ) )
      m_itClusters = get<STLiteClusters>( m_itClustersPath );
    
    // get The OT Times
    if ( exist<LHCb::OTTimes>( m_otTimesPath ) )
      m_otTimes = get<LHCb::OTTimes>( m_otTimesPath );
  }

  if ( getAllVariables ( aTrack ) == defValue )
    return false;

  if ( cutTrackP() || cutNHoles() ||
       cutNSharedHits() || cutNCloseHits() ||
       cutTrackChi2Prob() ) return false;

  return true;
}
//=============================================================================


//=============================================================================
// Getting all the variables
//=============================================================================
int AlignSelTool::getAllVariables ( LHCb::Track* aTrack ) {

  // Get the nodes for this track
  int nITHits = 0;
  int nOTHits = 0;
  // Do we want to fill the nodes vector even if we don't cut on anything?
  if ( (c_maxNHoles < abs(defValue)) ||
       (c_maxNSharedHits < abs(defValue)) ||
       (c_maxNCloseHits < abs(defValue)) ) {
    std::vector<LHCb::Node*>::const_iterator iAllNodes = aTrack->nodes().begin();
    for ( ; iAllNodes != aTrack->nodes().end(); ++iAllNodes ) {
      const LHCb::Node& aNode = *(*iAllNodes);
      if ( aNode.hasMeasurement() )
        if ( aNode.measurement().type() == LHCb::Measurement::IT )
          ++nITHits;
        else if ( aNode.measurement().type() == LHCb::Measurement::OT )
          ++nOTHits;
    }
  }
  
  m_trP = 0;
  m_trChi2PerDoF = 0;
  m_trChi2Prob = 0;
  m_nHoles = 0;
  m_nSharedHits = 0;
  m_nCloseHits = 0;

  m_nOTDoubleHits = 0;
  m_nITDoubleHits = 0;
  m_nLadOverlaps = 0;
  m_nBoxOverlaps = 0;

  if ( m_fieldOn && (c_minP > defValue) )
    m_trP = aTrack->p();
  if ( c_maxChi2PerDoF < abs(defValue) )
    m_trChi2PerDoF = aTrack->chi2PerDoF();
  if ( c_minChi2Prob > defValue )
    m_trChi2Prob = gsl_cdf_chisq_Q(aTrack->chi2(),aTrack->nDoF());

  unsigned int overlapStation = abs(defValue);
  unsigned int itStationHit = abs(defValue);
  int nITStationsHit = 0;
  unsigned int otStationHit = abs(defValue);
  int nOTStationsHit = 0;
  unsigned int tStationHit = abs(defValue);
  int nTStationsHit = 0;

  std::vector<LHCb::Node*>::const_iterator iNodes = aTrack->nodes().begin();
  for ( ; (iNodes != aTrack->nodes().end()) &&
          ((c_maxNHoles < abs(defValue)) ||
           (c_maxNSharedHits < abs(defValue)))
           ; ++iNodes ) {

    const LHCb::Node& aNode = *(*iNodes);

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
    }

    std::vector<LHCb::Node*>::const_iterator iNodes2 = iNodes+1;
    for ( ; (iNodes2 != aTrack->nodes().end()) &&
            (c_maxNHoles < abs(defValue)); ++iNodes2 ) {

      const LHCb::Node& aNode2 = *(*iNodes2);

      if ( !aNode2.hasMeasurement() ) continue;

      LHCb::STChannelID theSTID2;
      LHCb::OTChannelID theOTID2;
      if ( aNode2.measurement().type() == LHCb::Measurement::IT )
        theSTID2 = aNode2.measurement().lhcbID().stID();
      else if ( aNode2.measurement().type() == LHCb::Measurement::OT )
        theOTID2 = aNode2.measurement().lhcbID().otID();
      
      if ( (aNode.measurement().type() == LHCb::Measurement::IT) &&
           (aNode2.measurement().type() == LHCb::Measurement::IT) ) {
        // Double Hits
        if ( theSTID.uniqueSector() == theSTID2.uniqueSector() ) {
          if ( msgLevel( MSG::DEBUG ) )
            debug() << "Found double hit" << endmsg;
          m_nITDoubleHits++;
        }
        
        // Ladder Overlaps
        // Two hits in same layer of same box but not same ladder
        if ( (theSTID.uniqueDetRegion() == theSTID2.uniqueDetRegion()) &&
             (theSTID.sector() != theSTID2.sector()) )
          m_nLadOverlaps++;
        
        // Box Overlaps
        // Different hits in different box of same station
        if ( (theSTID.station() == theSTID2.station())
             && (theSTID.detRegion() != theSTID2.detRegion()) )
          // Save only one overlap per station (up to three box overlaps per track)
          if ( theSTID.station() != overlapStation ) {
            overlapStation = theSTID.station();
            m_nBoxOverlaps++;
          }
      }
      else if ( (aNode.measurement().type() == LHCb::Measurement::OT) &&
                (aNode2.measurement().type() == LHCb::Measurement::OT) )
        //**********************************************************************
        // Doubled hits for OT
        //**********************************************************************
        // Two hits in same "sub-layer" of same module
        if ( (theOTID.uniqueModule() == theOTID2.uniqueModule()) &&
             (abs(theOTID.straw()-theOTID2.straw())<5) ) {
          if ( msgLevel( MSG::DEBUG ) )
            debug() << "Found OT double hit" << endmsg;
          ++m_nOTDoubleHits;
        }
      //**********************************************************************
    }
    
    // Shared Hits
    if ( c_maxNSharedHits < abs(defValue) )
      if ( isSharedHit( aTrack, &aNode ) )
        m_nSharedHits++; 
  }
  
  // Holes
  // In case no hit in OT
  if ( c_maxNHoles < abs(defValue) )
    if ( nITHits && !nOTHits )
      m_nHoles = (m_nStations + m_nBoxOverlaps)*m_nLayers + m_nLadOverlaps + 
        m_nITDoubleHits - nITHits;
  // In case no hit in IT
    else if ( !nITHits && nOTHits )
      m_nHoles = m_nStations*m_nSubLayers*m_nLayers + m_nOTDoubleHits - nOTHits;
  // In case hits both in IT and OT
    else if ( nITHits && nOTHits )
      m_nHoles = (nITStationsHit + m_nBoxOverlaps)*m_nLayers + m_nLadOverlaps + 
        m_nITDoubleHits - nITHits +
        nOTStationsHit*m_nSubLayers*m_nLayers + m_nOTDoubleHits - nOTHits +
        (m_nStations-nTStationsHit>0)*(m_nStations-nTStationsHit)*
        int(m_nLayers*(1+m_nSubLayers)/2);

  // Close Hits
  if ( c_maxNCloseHits < abs(defValue) )
    m_nCloseHits = nNeighbouringHits( aTrack );

  return aTrack->nodes().size();
}
//=============================================================================


//=============================================================================
// Cutting on the track momentum at first state
//=============================================================================
bool AlignSelTool::cutTrackP ( ) {
  if ( m_fieldOn && (m_trP < c_minP) ) return true;
  return false;
}
//=============================================================================


//=============================================================================
// Cutting on the track chi2 per DoF
//=============================================================================
bool AlignSelTool::cutTrackChi2PerDoF ( ) {
  if ( m_trChi2PerDoF > c_maxChi2PerDoF ) return true;
  return false;
}
//=============================================================================


//=============================================================================
// Cutting on the track chi2 probability
//=============================================================================
bool AlignSelTool::cutTrackChi2Prob ( ) {
  if ( m_trChi2Prob < c_minChi2Prob ) return true;
  return false;
}
//=============================================================================


//=============================================================================
// Cutting on the number of holes
//=============================================================================
bool AlignSelTool::cutNHoles ( ) {
  if ( m_nHoles > c_maxNHoles ) return true;
  return false;
}
//=============================================================================


//=============================================================================
// Cutting on the number of shared hits
//=============================================================================
bool AlignSelTool::cutNSharedHits ( ) {
  if ( m_nSharedHits > c_maxNSharedHits ) return true;
  return false;
}
//=============================================================================


//=============================================================================
// Cutting on the number of neighbouring hits
//=============================================================================
bool AlignSelTool::cutNCloseHits ( ) {
  if ( m_nCloseHits > c_maxNCloseHits ) return true;
  return false;
}
//=============================================================================


//===========================================================================
// Check if this hit is shared with (at least) one other track
//===========================================================================
bool AlignSelTool::isSharedHit ( const LHCb::Track* iTrack,
                                 const LHCb::Node* aNode ) {

  LHCb::LHCbID measurementID;
  if ( aNode->hasMeasurement() )
    measurementID = aNode->measurement().lhcbID();
  else
    return false;

  const LHCb::Tracks* m_tracks;
  m_tracks = get<LHCb::Tracks>( m_tracksPath );
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
// Is track isolated or not?
//===========================================================================
int AlignSelTool::nNeighbouringHits ( const LHCb::Track* aTrack ) {

  int nNeighbouringHits = 0;
  
  bool isClose = false;
  
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
          LHCb::STChannelID nodeSTID = (*iNodes)->measurement().lhcbID().stID();
          LHCb::STChannelID clusSTID = (*iClus).channelID();

          // For each IT hit, check that no other cluster is found close to it          
          // Not a TELL1 artefact at beetles boundaries
          if ( (nodeSTID.strip()+clusSTID.strip())%256 > 3 )
            
            // Same ladder within 2 strips aside
            if ( (nodeSTID.uniqueSector() == clusSTID.uniqueSector()) &&
                 (abs(nodeSTID.strip()-clusSTID.strip())<=m_nStripsTol) ) {
              ++nNeighbouringHits;
              isClose = true;
              break;
            }
        }
      }
    }
    if ( isClose ) {
      isClose = false;
      break;
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
            isClose = true;
            break;
          }
        }
      }
    }
    if ( isClose ) {
      isClose = false;
      break;
    }
  }
  //**********************************************************************
  
  return nNeighbouringHits;
}
//===========================================================================


//===========================================================================
// Check if time is neighbouring the OT hit
//===========================================================================
bool AlignSelTool::isNeighbouringHit ( LHCb::OTChannelID timeID,
                                       LHCb::OTChannelID hitID,
                                       unsigned int hitStraw ) {
       
  // Same module?
  if ( hitID.uniqueModule() != timeID.uniqueModule() ) return false;
  
  // In module 9 of quarters 0 and 2, only 2*32 straws
  if ( !(timeID.quarter()%2) && (int(timeID.module()) == m_nModules) ) {
    if ( (abs((hitStraw%int(m_nStraws/4))
              -(timeID.straw()%int(m_nStraws/4)))<=m_nStrawsTol) ) return true;
  }
  else if ( (abs((hitStraw%int(m_nStraws/2))
                 -(timeID.straw()%int(m_nStraws/2)))<=m_nStrawsTol) ) return true;

  return false;
}
//===========================================================================
