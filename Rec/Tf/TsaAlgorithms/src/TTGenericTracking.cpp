// boost
#include <boost/foreach.hpp>

// from DigiEvent and TrackEvent
#include "Event/STCluster.h"
#include "Event/Track.h"
#include "Event/StateParameters.h"

// TfKernel
#include "TfKernel/STHit.h"

// local
#include "TTGenericTracking.h"

// boost
#include <boost/assign/list_of.hpp> // for 'vector_list_of()'

using namespace LHCb;
using namespace Tf;
using namespace Gaudi::Units;
using namespace boost::assign;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TTGenericTracking )

//--------------------------------------------------------------------
//
//  TTGenericTracking : TT Generic track finding algorithm
//
//        More details can be found in the TTGenericTracking.h
//
//--------------------------------------------------------------------

TTGenericTracking::TTGenericTracking( const std::string& name, 
                                      ISvcLocator* pSvcLocator ) :
  GaudiAlgorithm(name, pSvcLocator ),
  m_tracker(NULL)
{
  // constructer
  declareProperty("InputData", m_clusterLocation=STClusterLocation::TTClusters);
  declareProperty("OutputLocation", m_outputLocation  = TrackLocation::TT );
  declareProperty("MaxNumClusters", m_maxNumClusters  = 200      );
  declareProperty("ChargeCut",      m_chargeCut       = 15.0     );// ADC counts
  declareProperty("MergeNeighbours",m_mergeNeighbours = true     );
  declareProperty("ToleranceY",     m_toleranceY      = 1.0 * mm );
  declareProperty("DistCut",        m_distCut         = 1.0 * mm );
  declareProperty("WindowCenter",   m_winCenter       = std::vector<double>(3, 0.0));
  declareProperty("HalfWindowXSize",m_windowX         =-1.0 * mm );
  declareProperty("HalfWindowYSize",m_windowY         =-1.0 * mm );
  declareProperty("AddRich1State",  m_addRich1State   = true     );  
}

StatusCode TTGenericTracking::initialize()
{
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);

  if( m_winCenter.size() != 3 ) {
    sc = StatusCode::FAILURE;
    return Error("WindowCenter does not have 3 coordinates.", sc);
  } 

  // Internal counters
  m_nTrackCandidates = 0;
  m_nTriggered = 0;
  m_nEvent = 0;
  
  // Get the detector element
  m_tracker = getDet<DeSTDetector>(DeSTDetLocation::location("TT"));

  return StatusCode::SUCCESS;
}

StatusCode TTGenericTracking::finalize()
{
  info() << "Events with tracks " << m_nTriggered 
         << ", TT tracks " << m_nTrackCandidates << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode TTGenericTracking::execute()
{
  // Initialize the event
  setFilterPassed(false);
  ++m_nEvent;

  // Register the tracks
  Tracks* tracks = new Tracks();
  tracks->reserve(10);
  put( tracks, m_outputLocation);

  // Retrieve the clusters
  const STClusters* clusterCont = get<STClusters>(m_clusterLocation);  
  if (clusterCont->size() > m_maxNumClusters) {
    info() << "Number of clusters too high: " << clusterCont->size() 
           << " (cut set at " << m_maxNumClusters << ") -> Skip this event." 
           << endmsg;  
    return StatusCode::SUCCESS;
  }
  
  // make the ST hits (stores geometrical information)
  STHits hits; hits.reserve(clusterCont->size());
  STClusters::const_iterator iter = clusterCont->begin();
  for( ; iter != clusterCont->end(); ++iter ) {

    // Select clusters above a certain threshold
    if ((*iter)->totalCharge() < m_chargeCut ) continue;

    // Ignore bad channels (e.g. broken bonds)
    const DeSTSector* aSector = m_tracker->findSector((*iter)->channelID()); 
    if( !aSector->isOKStrip((*iter)->channelID()) ) continue;

    // Check if there is a neighbouring cluster that is bigger: if yes skip
    if( m_mergeNeighbours ) {
      STClusters::const_iterator prevIter = iter; --prevIter;
      if( iter != clusterCont->begin() &&
          (*iter)->firstChannel() - (*prevIter)->lastChannel() == 1 &&
          (*prevIter)->totalCharge() > (*iter)->totalCharge() ) continue;
      STClusters::const_iterator nextIter = iter; ++nextIter;
      if( nextIter != clusterCont->end() &&
          (*nextIter)->firstChannel() - (*iter)->lastChannel() == 1 &&
          (*nextIter)->totalCharge() >= (*iter)->totalCharge() ) continue;
    }

    // Create a Tf::STHit and add it to the container
    STHit* newHit = new STHit(*aSector,(*iter)->liteCluster());
    hits.push_back(newHit);
  }

  // Step 1: Make all possible combinations with 4 hits in 4 different layers
  findTTtracks( tracks, hits );
  
  // Skip the rest if there are no tracks
  if( tracks->size() == 0 ) {
    BOOST_FOREACH( const STHit* hit, hits ) delete hit;
    return StatusCode::SUCCESS;
  }
  setFilterPassed(true);

  // Step 2: Add additional hits to the track
  addAdditionalHits( tracks, hits );

  // Step 3: Remove clones
  removeClones( tracks );  

  // Update counters
  m_nTrackCandidates += tracks->size();
  ++m_nTriggered;
  
  // Print number of track candidates in event
  info() << "Event " << m_nEvent << " has " << tracks->size() 
         << " track candidate" << ((tracks->size()!=1) ? "s." : ".") << endmsg;

  BOOST_FOREACH( const STHit* hit, hits ) delete hit;
  return StatusCode::SUCCESS;
}


// Make all possible combinations with 4 hits in 4 different layers
void TTGenericTracking::findTTtracks(Tracks* tracks, STHits& hits )
{
  // Arrange the hits by layer 
  STHits x1Hits; x1Hits.reserve(hits.size());
  STHits x2Hits; x2Hits.reserve(hits.size());
  STHits uHits;  uHits.reserve(hits.size());
  STHits vHits;  vHits.reserve(hits.size());
  STHits::const_iterator iHit = hits.begin();
  for( ; iHit != hits.end(); ++iHit ) {
    if( (*iHit)->station() == 0  ) {
      ((*iHit)->layer()==0) ? x1Hits.push_back(*iHit) : uHits.push_back(*iHit);
    } else {
      ((*iHit)->layer()==0) ? vHits.push_back(*iHit) : x2Hits.push_back(*iHit);
    }
  }

  // Loop over the hits in the first layer (X1 layer)
  STHits::const_iterator iX1Hit = x1Hits.begin();
  for( ; iX1Hit != x1Hits.end(); ++iX1Hit ) {

    // Get the coordinate of the X1 hit
    double x = (*iX1Hit)->x();
    double z = (*iX1Hit)->z();

  // Loop over the hits in the last layer (X2 layer)
    STHits::const_iterator iX2Hit = x2Hits.begin();
    for( ; iX2Hit != x2Hits.end(); ++iX2Hit ) {
      
      // Get the slope through both hits
      double zX2 = (*iX2Hit)->z();
      double tx  = ( (*iX2Hit)->x() - x )/( zX2 - z );

      // Apply window cuts in x if necessary
      if( m_windowX > 0.0 ) {
        double xAtWindow = x + tx*(m_winCenter[2] - z);
        if( fabs(xAtWindow - m_winCenter[0]) > m_windowX ) continue;        
      }

      // Preliminary check on the y window using the endpoints of the strips
      if( m_windowY > 0.0 ) {
        double minTy = ( (*iX2Hit)->yMin() - (*iX1Hit)->yMax() )/( zX2 - z ) ;
        double maxTy = ( (*iX2Hit)->yMax() - (*iX1Hit)->yMin() )/( zX2 - z ) ;
        double minYAtWindow = (*iX1Hit)->yMax() + minTy*(m_winCenter[2] - z);
        double maxYAtWindow = (*iX1Hit)->yMin() + maxTy*(m_winCenter[2] - z);
        if( minYAtWindow - m_winCenter[1] > m_windowY && 
            maxYAtWindow - m_winCenter[1] > m_windowY ) continue;
        if( minYAtWindow - m_winCenter[1] < -m_windowY && 
            maxYAtWindow - m_winCenter[1] < -m_windowY ) continue;
      }

      // Add u-layer hits
      STHits::const_iterator iUHit = uHits.begin();
      for( ; iUHit != uHits.end(); ++iUHit ) {

        // Calculate intersection and check if compatible with u-layer hit
        double zU = (*iUHit)->z();
        double xU = x + tx*(zU - z);
        double yU = ( xU - (*iUHit)->x() )/(*iUHit)->dxDy();
        if( !(*iUHit)->isYCompatible( yU, m_toleranceY ) ) continue;

        // Preliminary check if intersection is compatible with x-layer hits
        double minTx = ( (*iX2Hit)->yMin() - m_toleranceY - yU )/( zX2 - zU ) ;
        double maxTx = ( (*iX2Hit)->yMax() + m_toleranceY - yU )/( zX2 - zU ) ;
        double minYatX1 = yU + maxTx*(z - zU);
        double maxYatX1 = yU + minTx*(z - zU);
        if( !(*iX1Hit)->isYCompatible( minYatX1 , m_toleranceY ) &&
            !(*iX1Hit)->isYCompatible( maxYatX1 , m_toleranceY ) ) continue;
        
        // Another preliminary check on the y window using the endpoints of 
        // the strip in the X2 layer and the intersection y-point.
        if( m_windowY > 0.0 ) {
          double minTy = ( (*iX2Hit)->yMin() - yU )/( zX2 - zU ) ;
          double maxTy = ( (*iX2Hit)->yMax() - yU )/( zX2 - zU ) ;
          double minYAtWindow = yU + minTy*(m_winCenter[2] - zU);
          double maxYAtWindow = yU + maxTy*(m_winCenter[2] - zU);
          if( minYAtWindow - m_winCenter[1] > m_windowY && 
              maxYAtWindow - m_winCenter[1] > m_windowY ) continue;
          if( minYAtWindow - m_winCenter[1] < -m_windowY && 
              maxYAtWindow - m_winCenter[1] < -m_windowY ) continue;
        }

        // Add v-layer hits
        STHits::const_iterator iVHit = vHits.begin();
        for( ; iVHit != vHits.end(); ++iVHit ) {
          
          // Calculate intersection and check if intersection is compatible
          double zV = (*iVHit)->z();
          double xV = x + tx*(zV - z);
          double yV = ( xV - (*iVHit)->x() )/(*iVHit)->dxDy();
          if( !(*iVHit)->isYCompatible( yV, m_toleranceY ) ) continue;

          // Check if x hits are compatible in y 
          double ty = (yV - yU)/( zV - zU );
          double y  = yU + ty*(z - zU);
          double yX2 = yV + ty*(zX2 - zV);
          if( !(*iX1Hit)->isYCompatible( y  , m_toleranceY ) ) continue;
          if( !(*iX2Hit)->isYCompatible( yX2, m_toleranceY ) ) continue;

          // Apply final window cut in y
          if( m_windowY > 0.0 ) {
            double yAtWindow = y + ty*(m_winCenter[2] - z);
            if( fabs(yAtWindow - m_winCenter[1]) > m_windowY ) continue;        
          }

          // Add state at first measurement
          LHCb::State firstState;
          firstState.setState( x, y, z, tx, ty, 1./(10.0*GeV) );
          firstState.setLocation( State::FirstMeasurement );
          // Make a vector with the lhcbIDs
          std::vector<LHCbID> ids = list_of((*iX1Hit)->lhcbID())
            ((*iUHit )->lhcbID())((*iVHit )->lhcbID())((*iX2Hit)->lhcbID());
          // Make a track
          Track* track = new Track(Track::TTGeneric, Track::TT, 
                                   Track::PatRecIDs, ids, firstState );
          // Add state at start of RICH1
          if( m_addRich1State ) {
            LHCb::State rich1State;
            double zRich1 = StateParameters::ZBegRich1;
            rich1State.setState( x + tx*(zRich1-z), y + ty*(zRich1-z), zRich1,
                                 tx, ty, 1./(10.0*GeV) );
            rich1State.setLocation( State::BegRich1 );
            track->addToStates( rich1State );
          }
          tracks->add( track );

          // Found track
          debug() << "Found track, event " << m_nEvent << endmsg;
          debug() << "State: " << x << ", " << y << ", " 
                  << z << ", " << tx << ", " << ty << endmsg;
        }        
      }
    }
  }
  return;
}


// Add additional hits to the track
void TTGenericTracking::addAdditionalHits( Tracks* tracks, STHits& hits )
{
  Tracks::iterator iTrack = tracks->begin();
  for( ; iTrack != tracks->end(); ++iTrack ) {

    // Loop over all TT clusters
    STHits::const_iterator iHit = hits.begin();
    for( ; iHit != hits.end(); ++iHit ) {

      // Check if cluster not already on sector that was hit
      bool found = false;
      std::vector<LHCbID>::const_iterator id = ((*iTrack)->lhcbIDs()).begin();
      while( id != ((*iTrack)->lhcbIDs()).end() && !found ) {
        if( (*iHit)->uniqueSector() == (*id).stID().uniqueSector() ) found=true;
        ++id;
      }
      if( found ) continue;

      // Check if hit compatible with track
      State myState = (*iTrack)->firstState();
      double hitX = myState.x()+myState.tx()*((*iHit)->zMid()-myState.z());
      double hitY = myState.y()+myState.ty()*((*iHit)->zMid()-myState.z());
      double b = - (*iHit)->dxDy();
      double dist = fabs(hitX + hitY*b - (*iHit)->x())/sqrt(1+b*b);
      if( dist > m_distCut ) continue;

      // Check if hit within its physical boundaries
      if( !(*iHit)->isYCompatible( hitY, m_toleranceY ) ) continue;

      // Add the hit to the track
      (*iTrack)->addToLhcbIDs( (*iHit)->lhcbID() );
      debug() << "Found additional hit: distance = " << dist << endmsg;
    }
  }
  return;
}


// Find and remove the clones in the container of tracks
void TTGenericTracking::removeClones( Tracks* tracks ) 
{
  std::set<Tracks::key_type> removedKeys;
  Tracks::iterator iTrack = tracks->begin();
  for( ; iTrack != tracks->end(); ++iTrack ) {
    Tracks::iterator jTrack = iTrack;
    ++jTrack;
    for( ; jTrack != tracks->end(); ++jTrack ) {
      // Check this
      unsigned int nCommon = (*iTrack)->nCommonLhcbIDs(*(*jTrack));
      if( nCommon == (*jTrack)->nLHCbIDs() ) {
        debug() << "Found clone. Remove track " << jTrack-tracks->begin()
                << " from second loop." << endmsg;
        removedKeys.insert( (*jTrack)->key() );
      } else if( nCommon == (*iTrack)->nLHCbIDs() ) {
        debug() << "Found clone. Remove track " << iTrack-tracks->begin()
                << " from first loop." << endmsg;
        removedKeys.insert( (*iTrack)->key() );    
        break;
      }
    }
  }

  // Loop over the keys that need to be removed from the track container
  std::set<Tracks::key_type>::const_iterator iKey = removedKeys.begin();
  for( ; iKey != removedKeys.end(); ++iKey ) {
    tracks->erase( *iKey );
  }

  return;
}
