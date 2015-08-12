// Include files 

#include "Event/VeloLiteCluster.h"

// local
#include "FastVeloHitManager.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FastVeloHitManager
//
// 2010-09-08 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( FastVeloHitManager )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FastVeloHitManager::FastVeloHitManager( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<FastVeloHitManager>(this);

}
//=============================================================================
// Destructor
//=============================================================================
FastVeloHitManager::~FastVeloHitManager() {
} 
//=============================================================================
// Initialization
//=============================================================================
StatusCode FastVeloHitManager::initialize() {
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_velo = getDet<DeVelo>( DeVeloLocation::Default );
  
  // make sure we are up-to-date on populated VELO stations
  registerCondition( (*(m_velo->leftSensorsBegin()))->geometry(), &FastVeloHitManager::rebuildGeometry );
  registerCondition( (*(m_velo->rightSensorsBegin()))->geometry(), &FastVeloHitManager::rebuildGeometry );
  
  // first update
  sc = updMgrSvc()->update(this);
  if(!sc.isSuccess()) {
    return Error( "Failed to update station structure." );
  }

  // invalidate measurements at the beginning of each event
  incSvc()->addListener(this, IncidentType::BeginEvent);

  m_pool.resize( 10000 );
  m_nextInPool = m_pool.begin();
  m_maxSize = 0;
  m_eventReady = false;
  
  return StatusCode::SUCCESS;
}

//=========================================================================
//  Finalize method.
//=========================================================================
StatusCode FastVeloHitManager::finalize ( ) {
  info() << "Maximum number of Velo hits " << m_maxSize << endmsg;
  for ( std::vector<FastVeloSensor*>::iterator itS = m_sensors.begin(); m_sensors.end() != itS; ++itS ) {
    if ( NULL != *itS ) delete *itS;
  }
  return GaudiTool::finalize();
}
//=========================================================================
//  Rebuild the geometry. Needed in case something changes in the Velo during the run...
//=========================================================================
StatusCode FastVeloHitManager::rebuildGeometry ( ) {

  m_lastXOffsetRight = m_velo->halfBoxOffset(0).x();
  m_lastXOffsetLeft  = m_velo->halfBoxOffset(1).x();

  double xBeam = .5 * ( m_velo->halfBoxOffset(0).x() + m_velo->halfBoxOffset(1).x() );
  double yBeam = .5 * ( m_velo->halfBoxOffset(0).y() + m_velo->halfBoxOffset(1).y() );
  
  if ( msgLevel(MSG::DEBUG) ) 
    debug() << "Updating the geometry... Beam at x=" << xBeam << " y=" << yBeam 
            << " opening " << m_velo->halfBoxOffset(1).x()  - m_velo->halfBoxOffset(0).x() << endmsg;

  for ( std::vector<FastVeloSensor*>::iterator itS = m_sensors.begin(); m_sensors.end() != itS; ++itS ) {
    if ( NULL != *itS ) delete *itS;
  }
  m_sensors.clear();
  m_firstR   = 999;
  m_firstPhi = 999;
  m_lastR    = 0;
  m_lastPhi  = 0;
  
  std::vector<int> previous(2,-1);
  
  for ( std::vector<DeVeloRType*>::const_iterator itR = m_velo->rSensorsBegin();
        m_velo->rSensorsEnd() > itR; ++itR ) {
    if ( !(*itR)->isReadOut() ) continue;
    FastVeloSensor* sens = new FastVeloSensor( *itR, xBeam, yBeam );
    while ( m_sensors.size() < sens->number() ) {
      m_sensors.push_back( 0 );
    }
    m_sensors.push_back( sens );
    int side = 0;
    unsigned int number = sens->number();
    if ( sens->isRight() ) side = 1;
    if ( m_firstR > number ) m_firstR = number;
    if ( m_lastR  < number ) m_lastR  = number;
    sens->setPrevious( previous[side] );
    if ( 0 <= previous[side] ) m_sensors[ previous[side] ]->setNext( number );
    previous[side] = number;
  }
  
  previous[0] = -1;
  previous[1] = -1;
  
  for ( std::vector<DeVeloPhiType*>::const_iterator itP = m_velo->phiSensorsBegin();
        m_velo->phiSensorsEnd() > itP; ++itP ) {
    if ( !(*itP)->isReadOut() ) continue;
    FastVeloSensor* sens = new FastVeloSensor( *itP, xBeam, yBeam );
    while ( m_sensors.size() < sens->number() ) {
      m_sensors.push_back( 0 );
    }
    m_sensors.push_back( sens );
    int side = 0;
    unsigned int number = sens->number();
    if ( sens->isRight() ) side = 1;
    if ( m_firstPhi > number ) m_firstPhi = number;
    if ( m_lastPhi  < number ) m_lastPhi  = number;
    sens->setPrevious( previous[side] );
    if ( 0 <= previous[side] ) m_sensors[ previous[side] ]->setNext( number );
    previous[side] = number;
  }

  //== Set the central angle of the various zones
  m_cosPhi.clear();
  m_sinPhi.clear();
  
  double angle = -0.375 * Gaudi::Units::pi;  // - 3/8 pi
  for ( int kk = 0; 8 > kk; ++kk ) {
    m_cosPhi.push_back( cos( angle ) );
    m_sinPhi.push_back( sin( angle ) );
    angle += 0.25 * Gaudi::Units::pi;  // step pi/4
  }

  if ( msgLevel( MSG::DEBUG ) ) {
    info() << "Found sensors:  R from " << m_firstR << " to " << m_lastR
           << ", Phi from " << m_firstPhi << " to " << m_lastPhi<< endmsg;
  
    for ( std::vector<FastVeloSensor*>::iterator itS = m_sensors.begin(); m_sensors.end() != itS; ++itS ) {
      if ( 0 != *itS) {
        info() << "   Sensor " << (*itS)->number() << " prev " << (*itS)->next( true ) 
               << " next " << (*itS)->next( false ) << endmsg;
      }
    }
  }
  return StatusCode::SUCCESS;
}

//=========================================================================
//  Incident handler
//=========================================================================
void FastVeloHitManager::handle ( const Incident& incident ) {
  if ( IncidentType::BeginEvent == incident.type() ){
    this->clearHits();
    m_eventReady = false;
  }
}

//=========================================================================
//  Clear all the hits from a previous event
//=========================================================================
void FastVeloHitManager::clearHits( ) {
  int lastSize = m_nextInPool - m_pool.begin();
  if ( lastSize > m_maxSize ) m_maxSize = lastSize;
  
  for ( std::vector<FastVeloSensor*>::iterator itS = m_sensors.begin(); m_sensors.end() != itS; ++itS ) {
    if ( 0 != *itS) {
      int maxZone = 2;
      if ( (*itS)->isRSensor() ) maxZone = 4;
      for ( int zone = 0; maxZone > zone; ++zone ) {
        (*itS)->hits(zone).clear();
      }
    }
  }
  m_nextInPool = m_pool.begin();
}

//=========================================================================
//  Convert the LiteClusters to FastVeloHit
//=========================================================================
void FastVeloHitManager::buildFastHits ( ) {
  if ( m_eventReady ) return;
  m_eventReady = true;

  //== Force an update when opening has changed if not automatically done
  if ( m_lastXOffsetRight != m_velo->halfBoxOffset(0).x() ||
       m_lastXOffsetLeft  != m_velo->halfBoxOffset(1).x()    ) {
    info() << "Velo has moved, rebuild geometry" << endmsg;
    rebuildGeometry();
  }  

  LHCb::VeloLiteCluster::FastContainer * liteClusters = 
    GaudiTool::get<LHCb::VeloLiteCluster::FastContainer>(LHCb::VeloLiteClusterLocation::Default);

  if ( liteClusters->size() > m_pool.size() ) {
    if ( msgLevel( MSG::DEBUG ) ) {  
      debug() << "Resized container to " << liteClusters->size() + 100 << endmsg;
    }
    m_pool.resize( liteClusters->size() + 100 );
    m_nextInPool = m_pool.begin();
  }

  LHCb::VeloLiteCluster::FastContainer::iterator iClus;
  unsigned int lastSensor = 9999;
  FastVeloSensor* mySensor = NULL;
  for ( iClus = liteClusters->begin(); liteClusters->end() != iClus; ++iClus ) {
    unsigned int sensor = iClus->channelID().sensor();
    if ( sensor > m_sensors.size() ) break;
    if ( sensor != lastSensor ) {
      lastSensor = sensor;
      mySensor = m_sensors[sensor];
    }
    
    //if ( !iClus->highThreshold() ) continue;

    FastVeloHit* hit = &(*(m_nextInPool++));  // get the next object in the pool
    int zone = 0;
    
    if ( mySensor->isRSensor() ) {
      DeVeloRType* deSensor =  mySensor->rSensor();
      unsigned int strip = iClus->channelID().strip();
      zone     = deSensor->globalZoneOfStrip( strip );
      double frac  = iClus->interStripFraction();
      double pitch = deSensor->rPitch( strip );
      double coord = deSensor->rOfStrip( strip ) + frac * pitch;
      double error = iClus->pseudoSize() * pitch;
      double weight = 12./(error*error);
      
      //== Set the correct Z, computed at centre of the zone.
      unsigned int spaceZone = zone;
      if ( deSensor->isRight() ) spaceZone += 4;
      double x = coord * m_cosPhi[spaceZone];
      double y = coord * m_sinPhi[spaceZone];
      hit->setZ( mySensor->z( x, y ) );
      
      hit->setHit( *iClus, zone, mySensor->z(), coord, weight );
      hit->setGlobal( coord + mySensor->rOffset( zone ) );
    } else {
      DeVeloPhiType* deSensor =  mySensor->phiSensor();
      unsigned int strip = iClus->channelID().strip();
      zone     = deSensor->globalZoneOfStrip( strip );
      double frac  = iClus->interStripFraction();
      double error = iClus->pseudoSize() * deSensor->phiPitch( strip );
      double weight = 12./(error*error);
      hit->setHit( *iClus, zone, mySensor->z(), 0, weight );
      hit->setLineParameters( mySensor->lineParams( strip, frac ) );
    }
    hit->setSensorCentre( mySensor->xCentre(), mySensor->yCentre() );
    mySensor->hits(zone).push_back( hit );
  }
}

//=========================================================================
//  Return a pointer to the FastVeloHit of the given id
//=========================================================================
FastVeloHit* FastVeloHitManager::hitByLHCbID ( LHCb::LHCbID id) {
  if ( !id.isVelo() ) return 0;
  if( !m_eventReady ) buildFastHits() ;
  int sensor = id.veloID().sensor();
  int maxZone = 2;
  if ( id.isVeloR() ) maxZone = 4;
  
  for ( int zone = 0 ; maxZone > zone ; ++zone ) {
    for ( FastVeloHits::iterator itH = m_sensors[sensor]->hits(zone).begin(); 
          m_sensors[sensor]->hits(zone).end() != itH ; ++itH ) {
      if ( (*itH)->lhcbID() == id ) return *itH;
    }
  }
  return 0;
}

// get a hit by LHCbID and reset all internal state to the default
FastVeloHit* FastVeloHitManager::defaultStateHitByLHCbID ( LHCb::LHCbID id) {
  FastVeloHit* hit = this->hitByLHCbID(id);
  if(!hit) return nullptr; // no hit
  hit->clearState();
  FastVeloSensor* mySensor = m_sensors[hit->sensor()];
  unsigned int strip = hit->cluster().channelID().strip();
  double       frac  = hit->cluster().interStripFraction();
  if( id.isVeloR() ){
    DeVeloRType* deSensor =  mySensor->rSensor();
    double pitch = deSensor->rPitch( strip );
    double coord = deSensor->rOfStrip( strip ) + frac * pitch;
    hit->setGlobal( coord + mySensor->rOffset( hit->zone() ) );
    unsigned int spaceZone = hit->zone();
    if ( deSensor->isRight() ) spaceZone += 4;
    double x = coord * m_cosPhi[spaceZone];
    double y = coord * m_sinPhi[spaceZone];
    hit->setZ( mySensor->z( x, y ) );
  }else{
    hit->setLineParameters( mySensor->lineParams( strip, frac ) );
    hit->setZ( mySensor->z() );
  }
  hit->setSensorCentre( mySensor->xCentre(), mySensor->yCentre() );
  return hit;
}


//=========================================================================
//  Clear the used flags so that a second instance of Fast Velo will start properly
//=========================================================================
void FastVeloHitManager::resetUsedFlags ( ) {
  for ( std::vector<FastVeloHit>::iterator itH = m_pool.begin(); m_nextInPool != itH; ++itH ) {
    (*itH).resetUsed();
  }
}
//=============================================================================

