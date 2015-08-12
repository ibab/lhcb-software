// Include files

#include "Event/Track.h"
// local
#include "FastTTValidationTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FastTTValidationTool
//
// 2011-01-21 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( FastTTValidationTool )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  FastTTValidationTool::FastTTValidationTool( const std::string& type,
                                              const std::string& name,
                                              const IInterface* parent )
    : GaudiTool ( type, name , parent )
{
  declareInterface<ITrackSelector>(this);

  declareProperty( "ZTTProj"       , m_zTTProj        = 2500. * Gaudi::Units::mm );
  declareProperty( "CentralHole"   , m_centralHole    =   50. * Gaudi::Units::mm );
  declareProperty( "HorizontalHole", m_horizontalHole =    3. * Gaudi::Units::mm );
  declareProperty( "MaxTTSize"     , m_maxTTSize      = 1100. * Gaudi::Units::mm );
  declareProperty( "MaxTTProj"     , m_maxTTProj      =    3. * Gaudi::Units::mm );

}
//=============================================================================
// Destructor
//=============================================================================
FastTTValidationTool::~FastTTValidationTool() {}


//=========================================================================
//  Initialization: TT hit manager, begin event incident.
//=========================================================================
StatusCode FastTTValidationTool::initialize ( ) {
  StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiTool

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_ttHitManager = tool<Tf::TTStationHitManager <PatTTHit> >("PatTTStationHitManager");

  // invalidate measurements at the beginning of each event
  incSvc()->addListener(this, IncidentType::BeginEvent);
  m_eventReady = false;
  m_hitsPerLayer.resize( 4 );
  m_zTT.resize( 4, 0. );
  m_dxDy.resize( 4, 0. );

  return StatusCode::SUCCESS;
}
//=========================================================================
//  Incident handler
//=========================================================================
void FastTTValidationTool::handle ( const Incident& incident ) {
  if ( IncidentType::BeginEvent == incident.type() ){
    this->clearHits();
  }
}

//=========================================================================
//  Clear the local hits
//=========================================================================
void FastTTValidationTool::clearHits ( ) {
  for ( unsigned int kk = 0; m_hitsPerLayer.size() > kk; ++kk ) {
    m_hitsPerLayer[kk].clear();
    m_zTT[kk]  = 0.;
    m_dxDy[kk] = 0.;
  }
  m_eventReady = false;
}

//=========================================================================
//  initEvent, called once per event
//=========================================================================
void FastTTValidationTool::initEvent ( ) {

  Tf::TTStationHitManager<PatTTHit>::HitRange hits = m_ttHitManager->hits();

  for ( PatTTHits::const_iterator itTT = hits.begin(); hits.end() != itTT; ++itTT ) {
    PatTTHit* tt = *itTT;
    double z = tt->z();
    updateTTHitForTrack( tt, 0., 0. );
    double projDist = tt->x() * m_zTTProj / z;
    tt->setProjection( projDist );
    m_hitsPerLayer[tt->planeCode()].push_back( tt );
    m_zTT[tt->planeCode()] += z;
    m_dxDy[tt->planeCode()] += tt->hit()->dxDy();
  }

  for ( unsigned int kk = 0; 4 > kk; ++kk ) {
    if ( m_hitsPerLayer[kk].size() > 1 ) {
      std::sort( m_hitsPerLayer[kk].begin(), m_hitsPerLayer[kk].end(), Tf::increasingByX<PatTTHit>() );
      m_zTT[kk]  /= m_hitsPerLayer[kk].size();
      m_dxDy[kk] /= m_hitsPerLayer[kk].size();
    }
  }
  m_eventReady = true;
}

//=========================================================================
// Main method: Process a track and validate...
//=========================================================================
bool FastTTValidationTool::accept(const LHCb::Track& aTrack) const {

  if ( !m_eventReady ) {
    FastTTValidationTool* me = const_cast<FastTTValidationTool*>( this );
    me->initEvent();
  }

  LHCb::State state = aTrack.closestState( m_zTTProj );
  double yTol   = 5. * sqrt( state.errY2() + m_zTTProj * m_zTTProj * state.errTy2() );
  PatTTHits ttHits;

  bool accepted = false;
  double xTT =  state.x() + ( m_zTTProj - state.z() ) * state.tx();
  double yTT =  state.y() + ( m_zTTProj - state.z() ) * state.ty();
  if ( fabs( xTT ) < m_centralHole &&
       fabs( yTT ) < m_centralHole  ) accepted = true;
  if ( fabs(yTT) < m_horizontalHole ) accepted = true;
  if ( fabs( xTT ) > m_maxTTSize ||
       fabs( yTT ) > m_maxTTSize    ) accepted = true;

  std::vector<int> nInPlane( 4, 0 );
  int nbPlanes = 0;
  if ( !accepted ) {
    for ( unsigned int kk = 0; 4 > kk; ++kk ) {
      double xTTLayer =  state.x() + ( m_zTT[kk] - state.z() ) * state.tx();
      double yTTLayer =  state.y() + ( m_zTT[kk] - state.z() ) * state.ty();
      double xProj    = xTTLayer - m_dxDy[kk] * yTTLayer;
      double xMin     = xProj - m_maxTTProj;
      double xMax     = xProj + m_maxTTProj;

      //== Binary search for first hit before xMin
      PatTTHits::const_iterator itB = m_hitsPerLayer[kk].begin();
      PatTTHits::const_iterator itE = m_hitsPerLayer[kk].end();
      unsigned int step = (itE - itB)/2;

      while ( step > 1 ) {
        if ( (*(itB+step))->x() < xMin ) {
          itB += step;
        } else {
          itE -= step;
        }
        step = (itE - itB ) / 2;
      }

      for ( PatTTHits::const_iterator itH = itB; m_hitsPerLayer[kk].end() != itH; ++itH ) {
        PatTTHit* tt = *itH;
        if( !tt->hit()->isYCompatible( yTTLayer, yTol) ) continue;
        if ( tt->x() < xMin ) continue;
        if ( tt->x() > xMax ) break;
        //double projDist = ( tt->x() + m_dxDy[kk] * yTTLayer ) * m_zTTProj / tt->z() - xTT;
        //tt->setProjection( projDist );
        ttHits.push_back( tt );
        if ( nInPlane[kk] == 0 ) nbPlanes++;
        nInPlane[kk]++;
      }
      if ( kk > 0 && ttHits.size() < kk ) break;   // At least 3 hits = n-1 layers
    }
    if ( ttHits.size() > 2 && nbPlanes > 2 ) accepted = true;
  }
  return accepted;
}
//=============================================================================
