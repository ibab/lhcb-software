// $Id: AlignSelTool.cpp,v 1.7 2007-10-25 09:59:49 lnicolas Exp $
// Include files 

// local
#include "AlignSelTool.h"
#include "GaudiKernel/IIncidentSvc.h"

// Det
#include "OTDet/DeOTDetector.h"
#include "STDet/DeSTDetector.h"
#include "STDet/DeITDetector.h"

// Boost
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>
using namespace boost::lambda;

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
                             const IInterface* parent ):
  GaudiTool ( type, name , parent ),
  m_configured ( false ) {
  
  declareInterface<ITrackSelector>(this);

  declareProperty ( "TracksLocation", m_tracksPath = "Rec/Track/Tsa" );
  declareProperty ( "ITClustersLocation",
                    m_itClustersPath = LHCb::STLiteClusterLocation::ITClusters );
  declareProperty ( "OTTimesLocation",
                    m_otTimesPath = LHCb::OTTimeLocation::Default );

  declareProperty ( "BFieldStatus",                  m_fieldOn    = true    );
  declareProperty ( "IsolatedTrackNStrawsTolerance", m_nStrawsTol = 1       );
  declareProperty ( "IsolatedTrackNStripsTolerance", m_nStripsTol = 2       );

  declareProperty ( "TrackType",         c_trackType      = "ALL"           );
  declareProperty ( "ModulesToAlign",    c_modulesToAlign = defValue        );
  declareProperty ( "FlatIllumination",  c_flatIllum      = false           );

  declareProperty ( "MultiplicityCut",   c_maxMulti       = abs(defValue)   );
  declareProperty ( "MinMomentumCut",    c_minP           = defValue        );
  declareProperty ( "Chi2PerDoFMaxCut",  c_maxChi2PerDoF  = abs(defValue)   );
  declareProperty ( "Chi2ProbMinCut",    c_minChi2Prob    = defValue        );
  declareProperty ( "NHolesMaxCut",      c_maxNHoles      = abs(defValue)   );
  declareProperty ( "NSharedHitsMaxCut", c_maxNSharedHits = abs(defValue)   );
  declareProperty ( "NCloseHitsMaxCut",  c_maxNCloseHits  = abs(defValue)   );
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

  StatusCode sc = GaudiTool::initialize( );
  if ( sc.isFailure( ) ) return sc;  // error already reported by base class

  // Get random numbers generator (flat)
  IRndmGenSvc * randSvc = svc<IRndmGenSvc>( "RndmGenSvc", true );
  if ( !m_uniformDist.initialize( randSvc, Rndm::Flat(0.,0.5) ) )
    return Error ( "Failed to initialize Flat Random distribution!" );

  // Get parabolic extrapolator
  m_extrapolator = tool<ITrackExtrapolator>( "TrackFastParabolicExtrapolator" );

  // Get OT geometry
  DeOTDetector* otTracker = getDet<DeOTDetector>( DeOTDetectorLocation::Default );
  m_nStations = otTracker->nStation();
  m_nQuarters = otTracker->quarters().size() / otTracker->layers().size();
  m_nModules = otTracker->modules().size() / otTracker->quarters().size();
  m_nStraws = otTracker->nMaxChanInModule();

  // Get IT geometry
  DeITDetector* itTracker = getDet<DeITDetector>( DeSTDetLocation::IT );
  if ( int(itTracker->nStation()) != m_nStations )
    warning() << "Number of IT stations is not equal to number of OT stations!!!"
              << endmsg;
  m_nBoxes = itTracker->nBoxPerStation();
  m_nLadders = itTracker->nReadoutSector()/itTracker->nLayer();
  // What I actually want is the number of strips per Beetle, not per Ladder
  m_nStrips = itTracker->nStrip()/itTracker->nReadoutSector()/3;

  alignSelectedModules = defValue;

  info() << endmsg
         << " ================== Track Selection criteria =================" << endmsg;
  if ( !c_trackType.compare( "ALL" ) )
    info() << "          Will select all types of tracks" << endmsg;
  // Tracks going only through IT
  else if ( !c_trackType.compare( "IT" ) ) {
    if ( c_modulesToAlign == defValue )
      info() << "          Will select tracks with hits only in IT (no box defined)"
             << endmsg;
    else if ( (c_modulesToAlign/10 > 0) && (c_modulesToAlign/10 <= m_nBoxes) &&
              (c_modulesToAlign%10 >= 0) && (c_modulesToAlign%10 <= m_nLadders) ) {
      info() << "          Will select tracks that go through ladder ";
      if ( c_modulesToAlign == 0 ) info() << "'any'";
      else info() << c_modulesToAlign%10;
      info() << " of box " << c_modulesToAlign/10 << endmsg 
             << "          in all stations of the Inner Tracker" << endmsg;
      alignSelectedModules = 1;
    }
    else warning() << "          Box number not defined:"
                   << "              will select tracks going only through IT" << endmsg;
  }
  // Tracks going only through OT
  else if ( !c_trackType.compare( "OT" ) ) {
    if ( c_modulesToAlign == defValue )
      info() << "          Will select tracks with hits only in OT (no module defined)"
             << endmsg;
    else if ( (c_modulesToAlign/10 > 0) && (c_modulesToAlign/10 <= m_nQuarters) &&
              (c_modulesToAlign%10 >= 0) && (c_modulesToAlign%10 <= m_nModules) ) {
      info() << "          Will select tracks that go through module ";
      if ( c_modulesToAlign%10 == 0 ) info() << "'any'";
      else info() << c_modulesToAlign%10;
      info() << " of quarter " << c_modulesToAlign/10 << endmsg
             << "          in all stations of the Outer Tracker" << endmsg;
      alignSelectedModules = 2;
    }
    else warning() << "          Module number not defined:"
                   << "              will select tracks going only through OT" << endmsg;
  }
  // Tracks going through IT Box Overlaps
  else if ( !c_trackType.compare( "BoxOverlaps" ) ) {
    if ( c_modulesToAlign == defValue )
      info() << "          Will select tracks going through any box overlap"
             << "          (no box overlap defined)" << endmsg;
    else if ( (c_modulesToAlign/10 > 0) && (c_modulesToAlign/10 <= m_nBoxes) &&
              (c_modulesToAlign%10 > 0) && (c_modulesToAlign%10 <= m_nBoxes) &&
              (c_modulesToAlign/10 != c_modulesToAlign%10) ) {
      info() << "          Will select tracks that go through overlap between boxes "
             << c_modulesToAlign/10 << " and " << c_modulesToAlign%10 << endmsg
             << "          in any station of the Inner Tracker" << endmsg;
      alignSelectedModules = 3;
    }
    else warning() << "          Box overlap number not defined:"
                   << "              will select any box overlap" << endmsg;
  }
  // Tracks going through both IT and OT
  else if ( !c_trackType.compare( "BOTH" ) )
    info() << "          Will select tracks with hits both in IT and in OT" << endmsg;
  // Problem with option in option file
  else 
    info() << "          Track type not defined: will select all types of tracks"
           << endmsg;

  // Printing value of defined cuts
  if ( c_flatIllum )
    info() << "          Will flatten out illumination" << endmsg;
  if ( c_maxMulti != abs(defValue) )
    info() << "          Max Multiplicity cut = " << c_maxMulti << endmsg;
  if ( c_minP != defValue )
    info() << "          Min P cut = " << c_minP << endmsg;
  if ( c_maxChi2PerDoF != abs(defValue) )
    info() << "          Max chi2/DoF cut = " << c_maxChi2PerDoF << endmsg;
  if ( c_minChi2Prob != defValue )
    info() << "          Min chi2 prob. cut = " << c_minChi2Prob << endmsg;
  if ( c_maxNHoles != abs(defValue) )
    info() << "          Max # holes cut = " << c_maxNHoles << endmsg;
  if ( c_maxNSharedHits != abs(defValue) )
    info() << "          Max # shared hits cut = " << c_maxNSharedHits << endmsg;
  if ( c_maxNCloseHits != abs(defValue) )
    info() << "          Max # neighbouring hits cut = " << c_maxNCloseHits << endmsg;

  info() << " =============================================================" << endmsg
         << endmsg;

  // Use IncidentListener so we can fill the
  // tracks / times /clusters containers only once
  incSvc()->addListener( this, IncidentType::BeginEvent );

  debug() << "AlignSelTool initialized successfully" << endmsg;
  
  return StatusCode::SUCCESS;
}
//=============================================================================


//=============================================================================
// Finalization
//=============================================================================
StatusCode AlignSelTool::finalize ( ) {
  
  m_uniformDist.finalize( );

  GaudiTool::finalize( );

  debug() << "AlignSelTool finalized successfully" << endmsg;
  
  return StatusCode::SUCCESS;
}
//=============================================================================


//=============================================================================
// Incident Listener ==> Get the containers only once per event
//=============================================================================
void AlignSelTool::handle ( const Incident& incident ) {
  if ( IncidentType::BeginEvent == incident.type() ) { m_configured = false; }
}
 
void AlignSelTool::initEvent ( ) const {
 m_configured = true;
 m_itClusters = get<STLiteClusters>( m_itClustersPath );
 m_otTimes = get<LHCb::OTTimes>( m_otTimesPath );
 m_tracks = get<LHCb::Tracks>( m_tracksPath );
}
//=============================================================================


//=============================================================================
// Cutting on all the variables
//=============================================================================
bool AlignSelTool::accept ( const LHCb::Track& aTrack ) const {

  debug() << "AlignSelTool starting 'accept(track)'" << endmsg;

  // Get the containers for first track of event
  if (!m_configured) initEvent();

  // Get all the variables
  if ( getAllVariables ( aTrack ) == defValue )
    return false;

  // Cut on some variables
  if ( cutMultiplicity( ) || cutTrackP( ) || cutNHoles( ) ||
       cutNSharedHits( ) || cutNCloseHits( ) ||
       cutTrackChi2Prob( ) ) return false;

  // Cut tracks "randomly" to get "flat" illumination of detector
  if ( c_flatIllum == true )
    if ( cutFlatIllum( ) ) return false;

  return true;
}
//=============================================================================


//=============================================================================
// Getting all the variables
//=============================================================================
int AlignSelTool::getAllVariables ( const LHCb::Track& aTrack ) const {

  if ( c_maxMulti < abs(defValue) )
    m_multiplicity = m_tracks->size();

  // Get the number of IT and OT hits
  int nITHits = 0;
  int nOTHits = 0;
  // Do we want to fill the nodes vector even if we don't cut on anything?
  if ( (c_maxNHoles < abs(defValue)) ||
       (c_maxNSharedHits < abs(defValue)) ||
       (c_maxNCloseHits < abs(defValue)) ||
       c_trackType.compare( "ALL" ) ) {
    const std::vector<LHCb::LHCbID>& ids = aTrack.lhcbIDs();
    nITHits = std::count_if(ids.begin(), ids.end(),bind(&LHCb::LHCbID::isIT,_1));
    nOTHits = std::count_if(ids.begin(), ids.end(),bind(&LHCb::LHCbID::isOT,_1));
  }

  // Selecting only tracks of defined track type
  if ( alignSelectedModules != defValue ) {
    if ( alignSelectedModules == 1 ) {
      if ( (nITHits == 0) || !selectDefinedModules( aTrack ) )
        return defValue;
    }
    else if ( alignSelectedModules == 2 ) {
      if ( (nOTHits == 0) || !selectDefinedModules( aTrack ) ) {
        return defValue;
      }
    }
    else if ( alignSelectedModules == 3 ) {
      if ( (nITHits == 0) || !selectBoxOverlaps( aTrack ) )
        return defValue;
    }
  }
  else if ( c_trackType.compare( "ALL" ) )
    if ( !c_trackType.compare( "IT" ) ) {
      if ( (nOTHits != 0) || (nITHits == 0) ) return defValue;
    }
    else if ( !c_trackType.compare( "BoxOverlaps" ) ) {
      if ( (nITHits == 0) || !selectBoxOverlaps( aTrack ) ) return defValue;
    }
    else if ( !c_trackType.compare( "OT" ) ) {
      if ( (nOTHits == 0) || (nITHits != 0) ) return defValue;
    }
    else if ( !c_trackType.compare( "BOTH" ) ) {
      if ( (nOTHits == 0) || (nITHits == 0) ) return defValue;
    }

  // Initialization of tracks variables
  m_trP = 0;
  m_entryTX = aTrack.closestState( 7500. ).tx();
  m_entryTY = aTrack.closestState( 7500. ).ty();
  m_trChi2PerDoF = 0;
  m_trChi2Prob = 0;
  m_nHoles = 0;
  m_nSharedHits = 0;
  m_nCloseHits = 0;

  if ( m_fieldOn && (c_minP > defValue) )
    m_trP = aTrack.p();
  if ( c_maxChi2PerDoF < abs(defValue) )
    m_trChi2PerDoF = aTrack.chi2PerDoF();
  if ( c_minChi2Prob > defValue )
    m_trChi2Prob = aTrack.probChi2();

  std::vector<LHCb::Node*>::const_iterator iNodes = aTrack.nodes().begin();
  for ( ; (iNodes != aTrack.nodes().end()) &&
          ((c_maxNHoles < abs(defValue)) ||
           (c_maxNSharedHits < abs(defValue)))
           ; ++iNodes ) {

    const LHCb::Node& aNode = *(*iNodes);

    if ( !aNode.hasMeasurement() ) continue;
    if ( (aNode.measurement().type() != LHCb::Measurement::IT) &&
         (aNode.measurement().type() != LHCb::Measurement::OT) ) {
      Warning("Found measurement not in IT nor in OT!!!", StatusCode::SUCCESS, 1);
      continue;
    }

    // Shared Hits
    if ( c_maxNSharedHits < abs(defValue) )
      if ( isSharedHit( aTrack, &aNode ) )
        m_nSharedHits++; 
  }
  
  // Holes
  int itExpHits = (int)aTrack.info(LHCb::Track::nExpectedIT, 0);
  int otExpHits = (int)aTrack.info(LHCb::Track::nExpectedOT, 0);
  int expHits = itExpHits + otExpHits;
  if ( expHits < nITHits + nOTHits ) expHits = nITHits + nOTHits;
  m_nHoles = expHits - (nITHits + nOTHits);

  // Close Hits
  if ( c_maxNCloseHits < abs(defValue) )
    m_nCloseHits = nNeighbouringHits( aTrack );

  return aTrack.nodes().size();
}
//=============================================================================

//=============================================================================
// Cutting on the event multiplicity
//=============================================================================
bool AlignSelTool::cutFlatIllum ( ) const {
  double flatIllum_var = sqrt( gsl_pow_2( m_entryTX ) + gsl_pow_2( m_entryTY ) );

  // If variable larger than 0.5, keep track anyways
  if ( flatIllum_var > 0.5 ) return false;

  // If prob of track larger than prob of random number, kill track
  if ( flatIllum_distri ( flatIllum_var ) >
       flatIllum_distri ( m_uniformDist( ) ) ) return true;

  return false;
}
//=============================================================================

//=============================================================================
// Cutting on the event multiplicity
//=============================================================================
bool AlignSelTool::cutMultiplicity ( ) const {
  if ( m_multiplicity > c_maxMulti ) return true;
  return false;
}
//=============================================================================


//=============================================================================
// Cutting on the track momentum at first state
//=============================================================================
bool AlignSelTool::cutTrackP ( ) const {
  if ( m_fieldOn && (m_trP < c_minP) ) return true;
  return false;
}
//=============================================================================


//=============================================================================
// Cutting on the track chi2 per DoF
//=============================================================================
bool AlignSelTool::cutTrackChi2PerDoF ( ) const {
  if ( m_trChi2PerDoF > c_maxChi2PerDoF ) return true;
  return false;
}
//=============================================================================


//=============================================================================
// Cutting on the track chi2 probability
//=============================================================================
bool AlignSelTool::cutTrackChi2Prob ( ) const {
  if ( m_trChi2Prob < c_minChi2Prob ) return true;
  return false;
}
//=============================================================================


//=============================================================================
// Cutting on the number of holes
//=============================================================================
bool AlignSelTool::cutNHoles ( ) const {
  if ( m_nHoles > c_maxNHoles ) return true;
  return false;
}
//=============================================================================


//=============================================================================
// Cutting on the number of shared hits
//=============================================================================
bool AlignSelTool::cutNSharedHits ( ) const {
  if ( m_nSharedHits > c_maxNSharedHits ) return true;
  return false;
}
//=============================================================================


//=============================================================================
// Cutting on the number of neighbouring hits
//=============================================================================
bool AlignSelTool::cutNCloseHits ( ) const {
  if ( m_nCloseHits > c_maxNCloseHits ) return true;
  return false;
}
//=============================================================================


//===========================================================================
// Check if this hit is shared with (at least) one other track
//===========================================================================
bool AlignSelTool::isSharedHit ( const LHCb::Track& iTrack,
                                 const LHCb::Node* aNode ) const {

  LHCb::LHCbID measurementID;
  if ( aNode->hasMeasurement() )
    measurementID = aNode->measurement().lhcbID();
  else
    return false;

  LHCb::Tracks::const_iterator iTracks = m_tracks->begin();
  for ( ; iTracks != m_tracks->end(); ++iTracks ) {
    const LHCb::Track* aTrack = *iTracks;
    if ( aTrack != &iTrack )
      if ( aTrack->isMeasurementOnTrack(measurementID) ) {
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
int AlignSelTool::nNeighbouringHits ( const LHCb::Track& aTrack ) const {

  int nNeighbouringHits = 0;
  
  bool isClose = false;
  
  //**********************************************************************
  // For each IT/OT hit check if there are neighbouring clusters/times
  //**********************************************************************
  STLiteClusters::const_iterator iClus = m_itClusters->begin();
  for ( ; iClus != m_itClusters->end(); ++iClus ) {
    // Not already on the track
    if ( aTrack.isOnTrack( (*iClus).channelID() ) ) continue;

    std::vector<LHCb::Node*>::const_iterator iNodes = aTrack.nodes().begin();  
    for ( ; iNodes != aTrack.nodes().end(); ++iNodes ) {
      if ( (*iNodes)->hasMeasurement() ) {
        if ( (*iNodes)->measurement().type() == LHCb::Measurement::IT ) {
          LHCb::STChannelID nodeSTID = (*iNodes)->measurement().lhcbID().stID();
          LHCb::STChannelID clusSTID = (*iClus).channelID();

          // For each IT hit, check that no other cluster is found close to it          
          // Not a TELL1 artefact at beetles boundaries
          if ( (nodeSTID.strip()+clusSTID.strip())%(2*m_nStrips) > 3 )
            
            // Same ladder within 2 strips aside
            if ( (nodeSTID.uniqueSector() == clusSTID.uniqueSector()) &&
                 (abs(int(nodeSTID.strip()-clusSTID.strip()))<=m_nStripsTol) ) {
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
    if ( aTrack.isOnTrack( (*iTimes)->channel() ) ) continue;

    std::vector<LHCb::Node*>::const_iterator iNodes = aTrack.nodes().begin();  
    for ( ; iNodes != aTrack.nodes().end(); ++iNodes ) {    
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
                                       unsigned int hitStraw ) const {
       
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
// Check if track goes through selected modules 
//===========================================================================
bool AlignSelTool::selectDefinedModules ( const LHCb::Track& aTrack ) const {

  if ( alignSelectedModules == 1 ) {
    // Loop over the nodes to get the hits variables
    std::vector<LHCb::Node*>::const_iterator iNodes = aTrack.nodes().begin();
    for ( ; iNodes!=aTrack.nodes().end(); ++iNodes ) {
      const LHCb::Node& aNode = *(*iNodes);
      
      // Only loop on hits with measurement in IT
      if ( !aNode.hasMeasurement() ) continue;
      if ( aNode.measurement().type() != LHCb::Measurement::IT ) continue;
      
      LHCb::STChannelID theSTID = aNode.measurement().lhcbID().stID();

      if ( c_modulesToAlign%10 == 0 ) {
        if ( (int)theSTID.detRegion()*10 != c_modulesToAlign )
          return false;
      }
      else if ( int(theSTID.detRegion()*10 + theSTID.sector()) != c_modulesToAlign )
        return false;
    }
  }
  else if ( alignSelectedModules == 2 ) {
    // Loop over the nodes to get the hits variables
    std::vector<LHCb::Node*>::const_iterator iNodes = aTrack.nodes().begin();
    for ( ; iNodes!=aTrack.nodes().end(); ++iNodes ) {
      const LHCb::Node& aNode = *(*iNodes);

      // Only loop on hits with measurement in OT
      if ( !aNode.hasMeasurement() ) continue;
      if ( aNode.measurement().type() != LHCb::Measurement::OT ) continue;
      LHCb::OTChannelID theOTID = aNode.measurement().lhcbID().otID();

      if ( c_modulesToAlign%10 == 0 ) {
        if ( (int)(theOTID.quarter()+1)*10 != c_modulesToAlign )
          return false;
      }
      else if ( int((theOTID.quarter()+1)*10 + theOTID.module()) != c_modulesToAlign )
        return false;
    }
  }

  return true;
}
//===========================================================================


//===========================================================================
// Check if track goes through selected box overlaps
//===========================================================================
bool AlignSelTool::selectBoxOverlaps ( const LHCb::Track& aTrack ) const {
  // For Box Overlap
  unsigned int overlapStation = abs(defValue);
  int nBoxOverlaps = 0;
  std::vector<int> overlappingBoxes(m_nStations,0);
  
  // Loop over the nodes to get the hits variables
  std::vector<LHCb::Node*>::const_iterator iNodes = aTrack.nodes().begin();
  for ( ; iNodes!=aTrack.nodes().end(); ++iNodes ) {
    const LHCb::Node& aNode = *(*iNodes);

    // Only loop on hits with measurement in IT
    if ( !aNode.hasMeasurement() ) continue;
    if ( aNode.measurement().type() != LHCb::Measurement::IT ) continue;

    LHCb::STChannelID theSTID = aNode.measurement().lhcbID().stID();

    std::vector<LHCb::Node*>::const_iterator iNodes2 = iNodes+1;
    for ( ; iNodes2 != aTrack.nodes().end(); ++iNodes2 ) {
      const LHCb::Node& aNode2 = *(*iNodes2);

      // Only loop on hits with measurement in IT
      if ( !aNode2.hasMeasurement() ) continue;
      if ( aNode2.measurement().type() != LHCb::Measurement::IT ) continue;

      LHCb::STChannelID theSTID2 = aNode2.measurement().lhcbID().stID();
      
      // Different hits in different box of same station
      if ( (theSTID.station() == theSTID2.station())
           && (theSTID.detRegion() != theSTID2.detRegion()) )
        
        // Save only one overlap per station (up to three box overlaps per track)
        if ( theSTID.station() != overlapStation ) {
          overlapStation = theSTID.station();
          ++nBoxOverlaps;
          overlappingBoxes[overlapStation-1] =
            theSTID.detRegion()*10 + theSTID2.detRegion();
        }
    }
  }

  if ( alignSelectedModules == defValue ) {
    if ( nBoxOverlaps > 0 ) return true;
  }
  else if ( (alignSelectedModules == 3) && (nBoxOverlaps > 0) )
    for ( int i = 0; i < m_nStations; ++i )
      if ( (overlappingBoxes[i] == c_modulesToAlign) ||
           (overlappingBoxes[i] ==
            (c_modulesToAlign%10)*10 + c_modulesToAlign/10) )
        return true;
  
  return false;
}
//===========================================================================

//===========================================================================
// Get the probability of the given value
// out of the distribution of sqrt( tx2 + ty2 )
//===========================================================================
double AlignSelTool::flatIllum_distri ( double flatIllum_var ) const {

  std::vector<double> probArray;
  int numBin;

  if ( !c_trackType.compare( "IT" ) && (c_modulesToAlign == defValue) ) {
    double tempProbArray[] =
      { 0.00045, 0.00148, 0.00239, 0.00334, 0.00425,
        0.00530, 0.00809, 0.01108, 0.01337, 0.01534,
        0.01693, 0.01903, 0.01994, 0.02107, 0.02243,
        0.02356, 0.02420, 0.02528, 0.02529, 0.02515,
        0.02520, 0.02484, 0.02451, 0.02380, 0.02312,
        0.02280, 0.02172, 0.02102, 0.02028, 0.01953,
        0.01856, 0.01739, 0.01733, 0.01661, 0.01559,
        0.01487, 0.01462, 0.01410, 0.01306, 0.01272,
        0.01236, 0.01163, 0.01143, 0.01100, 0.01064,
        0.01026, 0.00999, 0.00959, 0.00917, 0.00905 };
    probArray.assign( tempProbArray, tempProbArray + 50 );
    numBin = int( flatIllum_var / ( 0.1 / 50 ) );
    return probArray[numBin];
  }
  else if ( !c_trackType.compare( "OT" ) && (c_modulesToAlign == defValue) ) {
    double tempProbArray[] = 
      { 0.00053, 0.00210, 0.00581, 0.01179, 0.01548,
        0.01823, 0.02019, 0.02207, 0.02353, 0.02456,
        0.02549, 0.02587, 0.02575, 0.02600, 0.02584,
        0.02565, 0.02531, 0.02478, 0.02447, 0.02424,
        0.02356, 0.02313, 0.02302, 0.02225, 0.02220,
        0.02171, 0.02095, 0.02039, 0.01973, 0.01927,
        0.01870, 0.01810, 0.01754, 0.01699, 0.01639,
        0.01594, 0.01543, 0.01487, 0.01434, 0.01417,
        0.01354, 0.01310, 0.01272, 0.01212, 0.01178,
        0.01140, 0.01074, 0.01051, 0.00985, 0.00953 };
    probArray.assign( tempProbArray, tempProbArray + 50 );
    numBin = int( flatIllum_var / ( 0.5 / 50 ) );
    return probArray[numBin];
  }
  else if ( !c_trackType.compare( "ALL" ) && (c_modulesToAlign == defValue) ) {
    double tempProbArray[] =
      { 0.00331, 0.01456, 0.02902, 0.03924, 0.04164,
        0.04042, 0.03781, 0.03571, 0.03373, 0.03196,
        0.03052, 0.02919, 0.02768, 0.02667, 0.02548,
        0.02453, 0.02340, 0.02239, 0.02138, 0.02068,
        0.01966, 0.01890, 0.01834, 0.01738, 0.01706,
        0.01645, 0.01570, 0.01515, 0.01454, 0.01414,
        0.01365, 0.01315, 0.01264, 0.01217, 0.01169,
        0.01132, 0.01095, 0.01052, 0.01010, 0.00997,
        0.00952, 0.00921, 0.00895, 0.00852, 0.00828,
        0.00801, 0.00755, 0.00738, 0.00692, 0.00670 };
    probArray.assign( tempProbArray, tempProbArray + 50 );
    numBin = int( flatIllum_var / ( 0.5 / 50 ) );
    return probArray[numBin];
  }

  return 0;
}
//===========================================================================
