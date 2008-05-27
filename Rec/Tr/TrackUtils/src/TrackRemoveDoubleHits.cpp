// $Id: TrackRemoveDoubleHits.cpp,v 1.5 2008-05-27 08:42:01 lnicolas Exp $
//

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
DECLARE_ALGORITHM_FACTORY ( TrackRemoveDoubleHits );
//===========================================================================

//===========================================================================
// Constructor
//===========================================================================
TrackRemoveDoubleHits::TrackRemoveDoubleHits ( const std::string& name,
					       ISvcLocator* pSvcLocator ):
  GaudiAlgorithm ( name, pSvcLocator ) {
  
  // Location of the tracks container
  this -> declareProperty ( "TracksLocation", m_tracksPath = "Rec/Track/Best" );

  // Location of the cluster containers
  this -> declareProperty ( "TTLiteClustersLocation",
                            m_ttLiteClustersPath = LHCb::STLiteClusterLocation::TTClusters );
  this -> declareProperty ( "TTClustersLocation",
                            m_ttClustersPath = LHCb::STClusterLocation::TTClusters );

  this -> declareProperty ( "ITLiteClustersLocation",
                            m_itLiteClustersPath = LHCb::STLiteClusterLocation::ITClusters );
  this -> declareProperty ( "ITClustersLocation",
                            m_itClustersPath = LHCb::STClusterLocation::ITClusters );

  this -> declareProperty ( "OTTimesLocation",
                            m_otTimesPath = LHCb::OTTimeLocation::Default );
  

  // Do we want to keep the strip with the highest charge of a double hit?
  // OR
  // Do we want to keep the strip with a high threshold of a double hit?
  this -> declareProperty ( "KeepHighCharge", m_keepHighCharge = false );
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

  if ( msgLevel( MSG::DEBUG ) )
    debug() << "==> Initialize" << endmsg;

  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return Error( "Failed to initialize" );

  if ( m_keepHighThreshold && m_keepHighCharge ) {
    warning() << "Cannot keep cluster with highest charge AND cluster with high threshold!"
              << endmsg
              << "User has to set one of the two properties to false!"
              << endmsg
              << "Will remove both measurements of the track in case of a ST double hit!"
              << endmsg;
    m_keepHighThreshold = false;
    m_keepHighCharge = false;
  }
  else if ( m_keepHighThreshold )
    info() << "Double ST hit: will remove one measurement LHCbID from the track"
           << " if one of the two only has a high threshold." << endmsg;
  else if ( m_keepHighCharge )
    info() << "Double ST hit: will remove from the track the measurement"
           << " of the strip that has the lowest charge." << endmsg;
  else
    info() << "Double ST hit: will remove both measurements from the track."
           << endmsg;

  info() << "Double OT hit: will remove both measurements from the track."
	 << endmsg;

  debug() << "TrackRemoveDoubleHits initialized successfully" << endmsg;    

  m_nTTHits = 0;
  m_nITHits = 0;
  m_nOTHits = 0;

  return StatusCode::SUCCESS;
}
//===========================================================================


//===========================================================================
// Execute
//===========================================================================
StatusCode TrackRemoveDoubleHits::execute ( ) {

  debug() << "TrackRemoveDoubleHits starting execution" << endmsg;

  std::vector<LHCb::Node*> nodesToRemove;

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

    nodesToRemove.clear();

    // Loop over the nodes to check if
    // there are two of the same track in the same ST sector
    std::vector<LHCb::Node*>::const_iterator iNodes = aTrack.nodes().begin();
    for ( ; iNodes != aTrack.nodes().end(); ++iNodes ) {
      
      LHCb::Node* aNode = *iNodes;

      if ( !aNode->hasMeasurement() ) continue;

      const LHCb::LHCbID& aLHCbID = aNode->measurement().lhcbID();

      // Only loop on hits in ST or OT
      if ( !aLHCbID.isST() && !aLHCbID.isOT() ) continue;

      if ( aLHCbID.isTT() )
	++m_nTTHits;
      else if ( aLHCbID.isIT() )
	++m_nITHits;
      else if ( aLHCbID.isOT() )
	++m_nOTHits;
      
      std::vector<LHCb::Node*>::const_iterator iNodes2 = iNodes+1;
      for ( ; iNodes2 != aTrack.nodes().end(); ++iNodes2 ) {
        
        LHCb::Node* aNode2 = *iNodes2;

        if ( !aNode2->hasMeasurement() ) continue;

	const LHCb::LHCbID& aLHCbID2 = aNode2->measurement().lhcbID();

        // Only loop on hits in ST or OT
	if ( !aLHCbID2.isST() && !aLHCbID2.isOT() ) continue;
        
        //**********************************************************************
        // Double IT hit
        // (splitted clusters at beetles boundaries or simulated dead strips)
        //**********************************************************************
	if ( aLHCbID.isIT() && aLHCbID2.isIT() ) {

	  if ( aLHCbID.stID().uniqueSector() == aLHCbID2.stID().uniqueSector() ) {
	    
	    if ( m_keepHighThreshold ) {
	      Warning("Found a double hit in IT. Removing from track measurement of strip with low threshold!!!",
		      StatusCode::SUCCESS, 1);
	      if ( isHighThreshold ( aLHCbID ) &&
		   !isHighThreshold ( aLHCbID2 ) )
		nodesToRemove.push_back( aNode2 );
	      else if ( isHighThreshold ( aLHCbID2 ) &&
			!isHighThreshold ( aLHCbID ) )
		nodesToRemove.push_back( aNode );
	      else {
		nodesToRemove.push_back( aNode );
		nodesToRemove.push_back( aNode2 );
	      }
	    }
	    else if ( m_keepHighCharge ) {
	      Warning("Found a double hit in IT. Removing from track measurement of strip with less charge!!!",
		      StatusCode::SUCCESS, 1);
	      if ( charge ( aLHCbID ) > charge ( aLHCbID2 ) )
		nodesToRemove.push_back( aNode2 );
	      else if ( charge ( aLHCbID2 ) > charge ( aLHCbID ) )
		nodesToRemove.push_back( aNode );
	      else {
		nodesToRemove.push_back( aNode );
		nodesToRemove.push_back( aNode2 );
	      }
	    }
	    else {
	      Warning("Found a double hit in IT. Removing both measurements from track!!!",
		      StatusCode::SUCCESS, 1);
	      nodesToRemove.push_back( aNode );
	      nodesToRemove.push_back( aNode2 );
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
	      Warning("Found a double hit in TT. Removing from track measurement of strip with low threshold!!!",
		      StatusCode::SUCCESS, 1);
	      if ( isHighThreshold ( aLHCbID ) &&
		   !isHighThreshold ( aLHCbID2 ) )
		nodesToRemove.push_back( aNode2 );
	      else if ( isHighThreshold ( aLHCbID2 ) &&
			!isHighThreshold ( aLHCbID ) )
		nodesToRemove.push_back( aNode );
	      else {
		nodesToRemove.push_back( aNode );
		nodesToRemove.push_back( aNode2 );
	      }
	    }
	    else if ( m_keepHighCharge ) {
	      Warning("Found a double hit in TT. Removing from track measurement of strip with less charge!!!",
		      StatusCode::SUCCESS, 1);
	      if ( charge ( aLHCbID ) > charge ( aLHCbID2 ) )
		nodesToRemove.push_back( aNode2 );
	      else if ( charge ( aLHCbID2 ) > charge ( aLHCbID ) )
		nodesToRemove.push_back( aNode );
	      else {
		nodesToRemove.push_back( aNode );
		nodesToRemove.push_back( aNode2 );
	      }
	    }
	    else {
	      if ( aLHCbID.stID().sector() == aLHCbID2.stID().sector() ) {
		Warning("Found a double hit in TT (same sector). Removing both measurements from track!!!",
			StatusCode::SUCCESS, 1);
		nodesToRemove.push_back( aNode );
		nodesToRemove.push_back( aNode2 );
	      }
	      else if ( abs(sectorDiff) == 1 ) {
		Warning("Found a double hit in TT (different sectors). Removing both measurements from track!!!",
			StatusCode::SUCCESS, 1);
		nodesToRemove.push_back( aNode );
		nodesToRemove.push_back( aNode2 );
	      }
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
	      Warning("Found a double hit in OT. Removing both measurements from track!!!",
		      StatusCode::SUCCESS, 1);
	      nodesToRemove.push_back( aNode );
	      nodesToRemove.push_back( aNode2 );
	      break;
	    }
	    Warning("Found an acceptable double hit in OT. Keeping both measurements on track.",
		    StatusCode::SUCCESS, 1);
	  }
	  //**********************************************************************
	}
      }
    }

    std::vector<LHCb::Node*>::const_iterator rNodes = nodesToRemove.begin();
    for ( ; rNodes != nodesToRemove.end(); ++rNodes ) {
      LHCb::Measurement& theMeas = (*rNodes)->measurement();
      (*rNodes)->removeMeasurement();
      (*rNodes)->setType( LHCb::Node::Outlier );
      aTrack.removeFromMeasurements( &theMeas );
    }
  }
  
  return StatusCode::SUCCESS;  
}
//===========================================================================

  
//===========================================================================
// Finalize
//===========================================================================
StatusCode TrackRemoveDoubleHits::finalize ( ) {

  if ( msgLevel( MSG::DEBUG ) )
    debug() << "==> Finalize" << endmsg;

  info() << "Total number of TT hits seen = " << m_nTTHits << endmsg
	 << "Total number of IT hits seen = " << m_nITHits << endmsg
	 << "Total number of OT hits seen = " << m_nOTHits << endmsg;

  return GaudiAlgorithm::finalize ( );
};
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
};
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
};
//===========================================================================
