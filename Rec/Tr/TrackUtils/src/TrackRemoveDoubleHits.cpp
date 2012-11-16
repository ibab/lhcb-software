//-----------------------------------------------------------------------------
// Implementation file for class : TrackRemoveDoubleHits
//
// Louis Nicolas
// Started: 14.09.2007
///-----------------------------------------------------------------------------

//===========================================================================
// Includes
//===========================================================================
// Local
#include "TrackRemoveDoubleHits.h"
//===========================================================================

//===========================================================================
// Declare
//===========================================================================
// Declaration of the algorithm factory
DECLARE_ALGORITHM_FACTORY ( TrackRemoveDoubleHits )
//===========================================================================

//===========================================================================
// Constructor
//===========================================================================
TrackRemoveDoubleHits::TrackRemoveDoubleHits ( const std::string& name,
                                               ISvcLocator* pSvcLocator )
  : GaudiAlgorithm ( name, pSvcLocator )
  , m_nTTHits(0)
  , m_nITHits(0)
  , m_nOTHits(0)
  , m_tracks(NULL)
  , m_ttLiteClusters(NULL)
  , m_ttClusters(NULL)
  , m_itLiteClusters(NULL)
  , m_itClusters(NULL)

{  
  // Location of the tracks container
  this -> declareProperty ( "TracksLocation", m_tracksPath = "Rec/Track/Best" );

  // Location of the cluster containers
  this -> declareProperty ( "TTLiteClustersLocation",
                            m_ttLiteClustersPath = LHCb::STLiteClusterLocation::TTClusters );
  this -> declareProperty ( "TTClustersLocation",
                            m_ttClustersPath = LHCb::STClusterLocation::TTClusters         );

  this -> declareProperty ( "ITLiteClustersLocation",
                            m_itLiteClustersPath = LHCb::STLiteClusterLocation::ITClusters );
  this -> declareProperty ( "ITClustersLocation",
                            m_itClustersPath = LHCb::STClusterLocation::ITClusters         );

  this -> declareProperty ( "OTTimesLocation",
                            m_otTimesPath = LHCb::OTTimeLocation::Default                  );
  

  // Do we want to keep the strip with the highest charge of a double hit?
  // OR
  // Do we want to keep the strip with a high threshold of a double hit?
  this -> declareProperty ( "KeepHighCharge",    m_keepHighCharge    = false );
  this -> declareProperty ( "KeepHighThreshold", m_keepHighThreshold = false );
}
//===========================================================================


//===========================================================================
// Destructor
//===========================================================================
TrackRemoveDoubleHits::~TrackRemoveDoubleHits ( ) {
}
//===========================================================================


//===========================================================================
// Initialize
//===========================================================================
StatusCode TrackRemoveDoubleHits::initialize ( ) {

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Initialize" << endmsg;

  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return Error( "Failed to initialize" );

  if ( m_keepHighThreshold && m_keepHighCharge ) {
    warning() << "Cannot keep cluster with highest charge AND cluster with high threshold!"
              << endmsg
              << "User has to set one of the two properties to false!"
              << endmsg
              << "Will remove both LHCbIDs of the track in case of a ST double hit!"
              << endmsg;
    m_keepHighThreshold = false;
    m_keepHighCharge    = false;
  }
  else if ( m_keepHighThreshold )
    info() << "Double ST hit: will remove one LHCbID from the track"
           << " if one of the two only has a high threshold." << endmsg;
  else if ( m_keepHighCharge )
    info() << "Double ST hit: will remove from the track the LHCbID"
           << " of the strip that has the lowest charge." << endmsg;
  else
    info() << "Double ST hit: will remove both LHCbIDs from the track."
           << endmsg;

  info() << "Double OT hit: will remove both LHCbIDs from the track."
	 << endmsg;

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) {
    debug() << "TrackRemoveDoubleHits initialized successfully" << endmsg;    
  }

  return StatusCode::SUCCESS;
}
//===========================================================================


//===========================================================================
// Execute
//===========================================================================
StatusCode TrackRemoveDoubleHits::execute ( ) {

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug() << "TrackRemoveDoubleHits starting execution" << endmsg;

  // Get the tracks
  m_tracks = get<LHCb::Tracks>( m_tracksPath );  

  if ( m_keepHighThreshold ) {
    m_itLiteClusters = get<STLiteClusters>( m_itLiteClustersPath );
    m_ttLiteClusters = get<STLiteClusters>( m_ttLiteClustersPath );
  }
  else if ( m_keepHighCharge ) {
    m_itClusters = get<LHCb::STCluster::Container>( m_itClustersPath );
    m_ttClusters = get<LHCb::STCluster::Container>( m_ttClustersPath );
  }

  // Loop over tracks
  LHCb::Tracks::const_iterator iTracks = m_tracks->begin();
  for ( ; iTracks != m_tracks->end(); ++iTracks ) {
    LHCb::Track& aTrack = *(*iTracks);

    std::vector<LHCb::LHCbID> lhcbIDsToRemove;

    // Loop over the lhcbIDs to check if
    // there are two of the same track in the same ST sector
    std::vector<LHCb::LHCbID>::const_iterator iLHCbIDs = aTrack.lhcbIDs().begin();
    for ( ; iLHCbIDs != aTrack.lhcbIDs().end(); ++iLHCbIDs ) {
      
      const LHCb::LHCbID& aLHCbID = *iLHCbIDs;
      
      // Only loop on hits in ST or OT
      if ( !aLHCbID.isST() && !aLHCbID.isOT() ) continue;

      if ( msgLevel( MSG::DEBUG ) ) {
	if ( aLHCbID.isTT() )
	  ++m_nTTHits;
	else if ( aLHCbID.isIT() )
	  ++m_nITHits;
	else if ( aLHCbID.isOT() )
	  ++m_nOTHits;
      }
      
      std::vector<LHCb::LHCbID>::const_iterator iLHCbIDs2 = iLHCbIDs+1;
      for ( ; iLHCbIDs2 != aTrack.lhcbIDs().end(); ++iLHCbIDs2 ) {
        
        const LHCb::LHCbID& aLHCbID2 = *iLHCbIDs2;
        
        // Only loop on hits in ST or OT
	if ( !aLHCbID2.isST() && !aLHCbID2.isOT() ) continue;
        
        //**********************************************************************
        // Double IT hit
        // (splitted clusters at beetles boundaries or simulated dead strips)
        //**********************************************************************
	if ( aLHCbID.isIT() && aLHCbID2.isIT() ) {

	  if ( aLHCbID.stID().uniqueSector() == aLHCbID2.stID().uniqueSector() ) {
	    
	    if ( m_keepHighThreshold ) {
	      Warning("Found a double hit in IT. Removing from track LHCbID of strip with low threshold!!!",
		      StatusCode::SUCCESS, 1).ignore();
	      if ( isHighThreshold ( aLHCbID ) &&
		   !isHighThreshold ( aLHCbID2 ) )
		lhcbIDsToRemove.push_back( aLHCbID2 );
	      else if ( isHighThreshold ( aLHCbID2 ) &&
			!isHighThreshold ( aLHCbID ) )
		lhcbIDsToRemove.push_back( aLHCbID );
	      else {
		lhcbIDsToRemove.push_back( aLHCbID );
		lhcbIDsToRemove.push_back( aLHCbID2 );
	      }
	    }
	    else if ( m_keepHighCharge ) {
	      Warning("Found a double hit in IT. Removing from track LHCbID of strip with less charge!!!",
		      StatusCode::SUCCESS, 1).ignore();
	      if ( charge ( aLHCbID ) > charge ( aLHCbID2 ) )
		lhcbIDsToRemove.push_back( aLHCbID2 );
	      else if ( charge ( aLHCbID2 ) > charge ( aLHCbID ) )
		lhcbIDsToRemove.push_back( aLHCbID );
	      else {
		lhcbIDsToRemove.push_back( aLHCbID );
		lhcbIDsToRemove.push_back( aLHCbID2 );
	      }
	    }
	    else {
	      Warning("Found a double hit in IT. Removing both LHCbIDs from track!!!",
		      StatusCode::SUCCESS, 1).ignore();
	      lhcbIDsToRemove.push_back( aLHCbID );
	      lhcbIDsToRemove.push_back( aLHCbID2 );
	    }
	    break;
	  }
	  //**********************************************************************
	}
        //**********************************************************************
        // Double TT hit
        // (splitted clusters at beetles boundaries or simulated dead strips)
        //**********************************************************************
	else if ( aLHCbID.isTT() && aLHCbID2.isTT() ) {
	  
	  int sectorDiff = aLHCbID.stID().sector() - aLHCbID2.stID().sector();
	  if ( (aLHCbID.stID().uniqueLayer() == aLHCbID2.stID().uniqueLayer()) &&
	       (abs(sectorDiff) < 2) ) {
	    
	    if ( m_keepHighThreshold ) {
	      Warning("Found a double hit in TT. Removing from track LHCbID of strip with low threshold!!!",
		      StatusCode::SUCCESS, 1).ignore();
	      if ( isHighThreshold ( aLHCbID ) &&
		   !isHighThreshold ( aLHCbID2 ) )
		lhcbIDsToRemove.push_back( aLHCbID2 );
	      else if ( isHighThreshold ( aLHCbID2 ) &&
			!isHighThreshold ( aLHCbID ) )
		lhcbIDsToRemove.push_back( aLHCbID );
	      else {
		lhcbIDsToRemove.push_back( aLHCbID );
		lhcbIDsToRemove.push_back( aLHCbID2 );
	      }
	    }
	    else if ( m_keepHighCharge ) {
	      Warning("Found a double hit in TT. Removing from track LHCbID of strip with less charge!!!",
		      StatusCode::SUCCESS, 1).ignore();
	      if ( charge ( aLHCbID ) > charge ( aLHCbID2 ) )
		lhcbIDsToRemove.push_back( aLHCbID2 );
	      else if ( charge ( aLHCbID2 ) > charge ( aLHCbID ) )
		lhcbIDsToRemove.push_back( aLHCbID );
	      else {
		lhcbIDsToRemove.push_back( aLHCbID );
		lhcbIDsToRemove.push_back( aLHCbID2 );
	      }
	    }
	    else {
	      // Depending on the case, the warning is different.
	      if ( aLHCbID.stID().sector() == aLHCbID2.stID().sector() )
		Warning("Found a double hit in TT (same sector). Removing both LHCbIDs from track!!!",
			StatusCode::SUCCESS, 1).ignore();
	      else if ( abs(sectorDiff) == 1 )
		Warning("Found a double hit in TT (different sectors). Removing both LHCbIDs from track!!!",
			StatusCode::SUCCESS, 1).ignore();
	      // But in both case we remove the two LHCbIDs
	      lhcbIDsToRemove.push_back( aLHCbID );
	      lhcbIDsToRemove.push_back( aLHCbID2 );
	    }
	    break;
	  }
	  //**********************************************************************
	}
        //**********************************************************************
        // Double OT hit
        //**********************************************************************
	else if ( aLHCbID.isOT() && aLHCbID2.isOT() ) {

	  int strawDiff = aLHCbID.otID().straw() - aLHCbID2.otID().straw();
	  if ( (aLHCbID.otID().uniqueModule() == aLHCbID2.otID().uniqueModule()) &&
	       (abs(strawDiff) < 10) ) {

	    const LHCb::State& tState = aTrack.closestState(7500.0);
	    double tx = tState.tx();

	    if ( fabs ( tx ) > 0.350 ) {
	      Warning("Found a double hit in OT. Removing both LHCbIDs from track!!!",
		      StatusCode::SUCCESS, 1).ignore();
	      lhcbIDsToRemove.push_back( aLHCbID );
	      lhcbIDsToRemove.push_back( aLHCbID2 );
	      break;
	    }
	    Warning("Found an acceptable double hit in OT. Keeping both LHCbIDs on track.",
		    StatusCode::SUCCESS, 1).ignore();
	  }
	  //**********************************************************************
	}
      }
    }

    std::vector<LHCb::LHCbID>::const_iterator rLHCbIDs = lhcbIDsToRemove.begin();
    for ( ; rLHCbIDs != lhcbIDsToRemove.end(); ++rLHCbIDs )
      aTrack.removeFromLhcbIDs( *rLHCbIDs );
  }
  
  return StatusCode::SUCCESS;  
}
//===========================================================================

  
//===========================================================================
// Finalize
//===========================================================================
StatusCode TrackRemoveDoubleHits::finalize ( ) {

  if ( msgLevel( MSG::DEBUG ) ) {
    debug() << "==> Finalize" << endmsg;
    
    debug() << "Total number of TT hits seen = " << m_nTTHits << std::endl
            << "Total number of IT hits seen = " << m_nITHits << std::endl
            << "Total number of OT hits seen = " << m_nOTHits << endmsg;
  }

  return GaudiAlgorithm::finalize ( );
}
//===========================================================================


//===========================================================================
// Get the high threshold bit of the lite cluster associated to an ST LHCbID
//===========================================================================
bool TrackRemoveDoubleHits::isHighThreshold ( const LHCb::LHCbID& theLHCbID ) {
  
  if ( theLHCbID.isTT() ) {
    STLiteClusters::const_iterator iLiteClus =
      m_ttLiteClusters->find<LHCb::STLiteCluster::findPolicy>( theLHCbID.stID() );
    
    if ( iLiteClus != m_ttLiteClusters->end() )
      return (*iLiteClus).highThreshold();
  }
  else if ( theLHCbID.isIT() ) {
    STLiteClusters::const_iterator iLiteClus =
      m_itLiteClusters->find<LHCb::STLiteCluster::findPolicy>( theLHCbID.stID() );
    
    if ( iLiteClus != m_itLiteClusters->end() )
      return (*iLiteClus).highThreshold();
  }
  
  return false;
}
//===========================================================================


//===========================================================================
// Get the charge of the cluster associated to an ST LHCbID
//===========================================================================
double TrackRemoveDoubleHits::charge ( const LHCb::LHCbID& theLHCbID ) {

  if ( theLHCbID.isTT() ) {
    LHCb::STCluster* theClus =
      m_ttClusters->object ( theLHCbID.stID() );
    
    if ( theClus != 0 )
      return theClus->totalCharge();
  }
  else if ( theLHCbID.isIT() ) {
    LHCb::STCluster* theClus =
      m_itClusters->object ( theLHCbID.stID() );
    
    if ( theClus != 0 )
      return theClus->totalCharge();
  }
  
  return 0.0;
}
//===========================================================================
