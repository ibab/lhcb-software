// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "Event/VeloPixLiteCluster.h"

// local
#include "PatPixelHitManager.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatPixelHitManager from FastVeloHitManager
//
// 2012-01-05 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( PatPixelHitManager )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  PatPixelHitManager::PatPixelHitManager( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
    : GaudiTool ( type, name , parent )
{
  declareInterface<PatPixelHitManager>(this);

}
//=============================================================================
// Destructor
//=============================================================================
PatPixelHitManager::~PatPixelHitManager() {
}
//=============================================================================
// Initialization
//=============================================================================
StatusCode PatPixelHitManager::initialize() {
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_veloPix      = getDet<DeVeloPix>( DeVeloPixLocation::Default );

  // make sure we are up-to-date on populated VELO stations
  registerCondition( (*(m_veloPix->leftSensorsBegin()))->geometry(), &PatPixelHitManager::rebuildGeometry );
  registerCondition( (*(m_veloPix->rightSensorsBegin()))->geometry(), &PatPixelHitManager::rebuildGeometry );

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
StatusCode PatPixelHitManager::finalize ( ) {
  info() << "Maximum number of Velo hits " << m_maxSize << endmsg;
  for ( std::vector<PatPixelSensor*>::iterator itS = m_sensors.begin(); m_sensors.end() != itS; ++itS ) {
    if ( NULL != *itS ) delete *itS;
  }
  return GaudiTool::finalize();
}
//=========================================================================
//  Rebuild the geometry. Needed in case something changes in the Velo during the run...
//=========================================================================
StatusCode PatPixelHitManager::rebuildGeometry ( ) {

  for ( std::vector<PatPixelSensor*>::iterator itS = m_sensors.begin(); m_sensors.end() != itS; ++itS ) {
    if ( NULL != *itS ) delete *itS;
  }
  m_sensors.clear();
  m_firstSensor = 999;
  m_lastSensor  = 0;

  std::vector<int> previous(2,-1);

  for ( std::vector<DeVeloPixSensor*>::const_iterator itS = m_veloPix->sensorsBegin();
        m_veloPix->sensorsEnd() > itS; ++itS ) {
    //== TO BE DONE ===   if ( !(*itS)->isReadOut() ) continue;
    PatPixelSensor* sens = new PatPixelSensor( *itS );
    while ( m_sensors.size() < sens->number() ) {
      m_sensors.push_back( 0 );
    }
    m_sensors.push_back( sens );
    int side = 0;
    unsigned int number = sens->number();
    if ( sens->isRight() ) side = 1;
    if ( m_firstSensor > number ) m_firstSensor = number;
    if ( m_lastSensor  < number ) m_lastSensor  = number;
    sens->setPrevious( previous[side] );
    previous[side] = number;
  }

  previous[0] = -1;
  previous[1] = -1;


  if ( msgLevel( MSG::DEBUG ) ) {
    info() << "Found sensors from " << m_firstSensor << " to " << m_lastSensor << endmsg;

    for ( std::vector<PatPixelSensor*>::iterator itS = m_sensors.begin(); m_sensors.end() != itS; ++itS ) {
      if ( 0 != *itS) {
        info() << "   Sensor " << (*itS)->number() << " prev " << (*itS)->previous() << endmsg;
      }
    }
  }
  return StatusCode::SUCCESS;
}

//=========================================================================
//  Incident handler
//=========================================================================
void PatPixelHitManager::handle ( const Incident& incident ) {
  if ( IncidentType::BeginEvent == incident.type() ){
    this->clearHits();
    m_eventReady = false;
  }
}

//=========================================================================
//  Clear all the hits from a previous event
//=========================================================================
void PatPixelHitManager::clearHits( ) {
  int lastSize = m_nextInPool - m_pool.begin();
  if ( lastSize > m_maxSize ) m_maxSize = lastSize;

  for ( std::vector<PatPixelSensor*>::iterator itS = m_sensors.begin(); m_sensors.end() != itS; ++itS ) {
    if ( 0 != *itS) (*itS)->reset();
  }
  m_nextInPool = m_pool.begin();
}

//=========================================================================
//  Convert the LiteClusters to PatPixelHit
//=========================================================================
void PatPixelHitManager::buildHits ( ) {
  if ( m_eventReady ) return;
  m_eventReady = true;

  LHCb::VeloPixLiteCluster::VeloPixLiteClusters * liteClusters =
    GaudiTool::get<LHCb::VeloPixLiteCluster::VeloPixLiteClusters>(LHCb::VeloPixLiteClusterLocation::Default);

  if ( liteClusters->size() > m_pool.size() ) {
    m_pool.resize( liteClusters->size() + 100 );
    m_nextInPool = m_pool.begin();
  }
  
  LHCb::VeloPixLiteCluster::VeloPixLiteClusters::iterator iClus;
  unsigned int lastSensor = 9999;
  PatPixelSensor* mySensor = NULL;

  double dx = 0.050 / sqrt( 12. );
  
  for ( iClus = liteClusters->begin(); liteClusters->end() != iClus; ++iClus ) {
    unsigned int sensor = iClus->channelID().sensor();
    if ( sensor > m_sensors.size() ) break;
    if ( sensor != lastSensor ) {
      lastSensor = sensor;
      mySensor = m_sensors[sensor];
    }
    PatPixelHit* hit = &(*(m_nextInPool++));  // get the next object in the pool

    Gaudi::XYZPoint point = mySensor->position( (*iClus).channelID(), 
                                                (*iClus).interPixelFractionX(), 
                                                (*iClus).interPixelFractionY() );
    hit->setHit( LHCb::LHCbID( (*iClus).channelID() ), point, dx, dx, sensor );
    mySensor->addHit( hit );
  }
}

//=========================================================================
//  
//=========================================================================
void PatPixelHitManager::sortByX ( ) {
  for ( std::vector<PatPixelSensor*>::iterator itS = m_sensors.begin(); m_sensors.end() != itS; ++itS ) {
    if ( 0 != *itS) {
      std::sort( (*itS)->hits().begin(), (*itS)->hits().end(), PatPixelHit::LowerByX() );
    }
  }
}
//=============================================================================

