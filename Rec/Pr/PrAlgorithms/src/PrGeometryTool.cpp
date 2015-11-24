// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "PrGeometryTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PrGeometryTool
//
// 2012-03-22 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( PrGeometryTool )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PrGeometryTool::PrGeometryTool( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<PrGeometryTool>(this);
  declareProperty( "zReference"      , m_zReference    = 8520. * Gaudi::Units::mm );
  declareProperty( "zMagnetParams"   , m_zMagnetParams    );
  declareProperty( "xParams"         , m_xParams          );
  declareProperty( "byParams"        , m_byParams         );
  declareProperty( "cyParams"        , m_cyParams         );
  declareProperty( "momentumParams"  , m_momentumParams   );
  declareProperty( "covarianceValues", m_covarianceValues );
}
//=============================================================================
// Destructor
//=============================================================================
PrGeometryTool::~PrGeometryTool() {}


//=========================================================================
//  Initialization
//=========================================================================
StatusCode PrGeometryTool::initialize ( ) {
  StatusCode sc = GaudiTool::initialize();
  if ( !sc ) return sc;

  m_magFieldSvc = svc<ILHCbMagnetSvc>( "MagneticFieldSvc", true );

  if ( m_zMagnetParams.size() == 0 ) {
    m_zMagnetParams.push_back(  5212.38  );
    m_zMagnetParams.push_back(   406.609 );
    m_zMagnetParams.push_back( -1102.35  );
    m_zMagnetParams.push_back(  -498.039 );
  }
  if ( m_xParams.size() == 0 ) {
    m_xParams.push_back(  18.6195  );
    m_xParams.push_back(  -5.55793 );
  }
  if ( m_byParams.size() == 0. ) m_byParams.push_back( -0.667996    );
  if ( m_cyParams.size() == 0. ) m_cyParams.push_back( -3.68424e-05 );
  if ( m_momentumParams.size() == 0 ) {
    m_momentumParams.push_back(   1.21014  );
    m_momentumParams.push_back(   0.637339 );
    m_momentumParams.push_back(  -0.200292 );
    m_momentumParams.push_back(   0.632298 );
    m_momentumParams.push_back(   3.23793  );
    m_momentumParams.push_back( -27.0259   );
  }
  if ( m_covarianceValues.size() == 0 ) {
    m_covarianceValues.push_back( 4.0   ); // ErrX = 2mm 
    m_covarianceValues.push_back( 400.0 ); // ErrY = 20 mm
    m_covarianceValues.push_back( 4.e-6 ); // ErrSlX = 2 mrad
    m_covarianceValues.push_back( 1.e-4 ); // ErrSlY = 10 mrad
    m_covarianceValues.push_back( 0.1   ); // errQQoverP = 10% of qOverP
  }
  return StatusCode::SUCCESS;
}

//=========================================================================
//  Compute the x projection at the reference plane
//=========================================================================
float PrGeometryTool::xAtReferencePlane( PrForwardTrack& track, PrHit* hit ) {
  float zHit    = hit->z();
  float yHit    = track.yFromVelo( zHit );
  float xHit    = hit->x(yHit);
  float dSlope  = ( track.xFromVelo(zHit) - xHit ) / ( zHit - m_zMagnetParams[0]);
  float zMag    = zMagnet( track ) + m_zMagnetParams[1] * dSlope * dSlope;
  float xMag    = track.xFromVelo( zMag );
  float dz      = 1.e-3 * ( zHit - m_zReference );
  float dxCoef  = dz * dz * ( m_xParams[0] + dz * m_xParams[1] );
  xHit          = xHit + dxCoef * dSlope ;
  float x       = xMag + ( m_zReference - zMag ) * ( xHit - xMag ) / ( zHit - zMag );
  hit->setCoord( x );
  return x;
}

//=========================================================================
//  Set the parameters of the track, from the (average) x at reference
//=========================================================================
void PrGeometryTool::setTrackParameters ( PrForwardTrack& track, const float xAtRef ) {
  float dSlope  = ( track.xFromVelo(m_zReference) - xAtRef ) / ( m_zReference - m_zMagnetParams[0]);
  float zMag    = zMagnet( track ) + m_zMagnetParams[1] * dSlope * dSlope;
  float xMag    = track.xFromVelo( zMag );
  float slopeT  = ( xAtRef - xMag ) / ( m_zReference - zMag );
  dSlope        = slopeT - track.slX();
  float dyCoef  = dSlope * dSlope * track.slY();
  
  track.setParameters( xAtRef,
                       slopeT, 
                       1.e-6 * m_xParams[0] * dSlope, 
                       1.e-9 * m_xParams[1] * dSlope,
                       track.yFromVelo( m_zReference ),
                       track.slY() + dyCoef * m_byParams[0],
                       dyCoef * m_cyParams[0] );
}
//=========================================================================
//  Set the parameters of the track, using the hits on the track
//=========================================================================
void PrGeometryTool::setTrackParameters ( PrForwardTrack& track ) {
  if ( track.hits().size() == 0 ) return;
  
  float xAtRef = 0.;
  PrHits::const_iterator itEnd = track.hits().end();
  for ( PrHits::iterator itH = track.hits().begin(); itEnd != itH; ++itH ) {
    xAtRef += xAtReferencePlane( track, *itH );
  }
  xAtRef /= track.hits().size();
  setTrackParameters( track, xAtRef );
  float s0 = 0.;
  float sd = 0.;

  for ( PrHits::iterator itH = track.hits().begin(); itEnd != itH; ++itH ) {
    if ( fabs( (*itH)->dxDy() ) > 0.001 ) {
      float d = -track.deltaY( *itH ) * m_zReference / (*itH)->z();
      float w = (*itH)->w();
      s0   += w;
      sd   += w * d;
    }
  }
  if ( s0 > 0 ) {
    float dy = sd/s0;
    track.updateParameters( 0., 0., 0., 0., dy );
  }
}
//=========================================================================
//  Returns an approximation of the Z at centre of magnet
//=========================================================================
float PrGeometryTool::zMagnet ( const PrForwardTrack& track ) {
  return ( m_zMagnetParams[0] +
           m_zMagnetParams[2] * track.slX2() +
           m_zMagnetParams[3] * track.slY2() );
}

//=========================================================================
//  Returns the best momentum estimate
//=========================================================================
float PrGeometryTool::qOverP ( const PrForwardTrack& track) {
  float qop(1.0/Gaudi::Units::GeV) ;
  float magscalefactor = m_magFieldSvc->signedRelativeCurrent() ;
  if( std::abs(magscalefactor) > 1e-6 ) {
    float bx   = track.xSlope( m_zReference );
    float bx2  = bx * bx;
    float coef = ( m_momentumParams[0] +
                   m_momentumParams[1] * bx2 +
                   m_momentumParams[2] * bx2 * bx2 +
                   m_momentumParams[3] * bx * track.slX() +
                   m_momentumParams[4] * track.slY2() +
                   m_momentumParams[5] * track.slY2() * track.slY2() );
    float proj = sqrt( ( 1. + track.slX2() + track.slY2() ) / ( 1. + track.slX2() ) );
    qop = ( track.slX() - bx ) / ( coef * Gaudi::Units::GeV * proj * magscalefactor ) ;
  }
  return qop ;
}
//=========================================================================
//  Returns the best momentum estimate for a seed: assumes it comes from (0,0,0)
//  and use the same parameterisation.
//=========================================================================
float PrGeometryTool::qOverP ( const PrSeedTrack& track) {
  float qop(1.0/Gaudi::Units::GeV) ;
  float magscalefactor = m_magFieldSvc->signedRelativeCurrent() ;
  if( std::abs(magscalefactor) > 1e-6 ) {
    float bx   = track.xSlope( 0. );
    float bx2  = bx * bx;
    //== Compute the slopes before the magnet: Assume the track comes from (0,0,0) and
    //== crosses the T station part at zMagnet
    float zMagnet = ( m_zMagnetParams[0] );
    float xMagnet = track.x( zMagnet );
    float yMagnet = track.y( zMagnet );
    float slXFront = xMagnet / zMagnet;
    float slYFront = yMagnet / zMagnet;
    float slX2 = slXFront * slXFront;
    float slY2 = slYFront * slYFront;
    float dSlope = slXFront - bx;
    //== Iterate as ZMagnet depends on the slope before...
    zMagnet = ( m_zMagnetParams[0] +
                m_zMagnetParams[1] * dSlope * dSlope +
                m_zMagnetParams[2] * slX2 +
                m_zMagnetParams[3] * slY2 );
    xMagnet = track.x( zMagnet );
    yMagnet = track.y( zMagnet );
    slXFront = xMagnet / zMagnet;
    slYFront = yMagnet / zMagnet;
    slX2 = slXFront * slXFront;
    slY2 = slYFront * slYFront;

    float coef = ( m_momentumParams[0] +
                   m_momentumParams[1] * bx2 +
                   m_momentumParams[2] * bx2 * bx2 +
                   m_momentumParams[3] * bx * slXFront +
                   m_momentumParams[4] * slY2 +
                   m_momentumParams[5] * slX2 * slY2 );
    float proj = sqrt( ( 1. + slX2 + slY2 ) / ( 1. + slX2 ) );
    qop = dSlope / ( coef * Gaudi::Units::GeV * proj * magscalefactor ) ;
  }
  return qop ;
}
float PrGeometryTool::qOverP ( const PrHybridSeedTrack& track) {
  float qop(1.0/Gaudi::Units::GeV) ;
  float magscalefactor = m_magFieldSvc->signedRelativeCurrent() ;
  if( std::abs(magscalefactor) > 1e-6 ){
    float bx   = track.xSlope( 0. );
    float bx2  = bx * bx;
    //== Compute the slopes before the magnet: Assume the track comes from (0,0,0) and
    //== crosses the T station part at zMagnet
    float zMagnet = ( m_zMagnetParams[0] );
    float xMagnet = track.x( zMagnet );
    float yMagnet = track.y( zMagnet );
    float slXFront = xMagnet / zMagnet;
    float slYFront = yMagnet / zMagnet;
    float slX2 = slXFront * slXFront;
    float slY2 = slYFront * slYFront;
    float dSlope = slXFront - bx;
    //== Iterate as ZMagnet depends on the slope before...
    zMagnet = ( m_zMagnetParams[0] +
                m_zMagnetParams[1] * dSlope * dSlope +
                m_zMagnetParams[2] * slX2 +
                m_zMagnetParams[3] * slY2 );
    xMagnet = track.x( zMagnet );
    yMagnet = track.y( zMagnet );
    slXFront = xMagnet / zMagnet;
    slYFront = yMagnet / zMagnet;
    slX2 = slXFront * slXFront;
    slY2 = slYFront * slYFront;
    float coef = ( m_momentumParams[0] +
                   m_momentumParams[1] * bx2 +
                   m_momentumParams[2] * bx2 * bx2 +
                   m_momentumParams[3] * bx * slXFront +
                   m_momentumParams[4] * slY2 +
                   m_momentumParams[5] * slX2 * slY2 );
    float proj = sqrt( ( 1. + slX2 + slY2 ) / ( 1. + slX2 ) );
    qop = dSlope / ( coef * Gaudi::Units::GeV * proj * magscalefactor ) ;
  }
  return qop ;
}


//=========================================================================
//  Default covariance matrix: Large errors as input to Kalman fitter.
//=========================================================================
Gaudi::TrackSymMatrix PrGeometryTool::covariance ( const float qOverP ) {
  Gaudi::TrackSymMatrix cov;
  cov(0,0) = m_covarianceValues[0];
  cov(1,1) = m_covarianceValues[1];
  cov(2,2) = m_covarianceValues[2];
  cov(3,3) = m_covarianceValues[3];
  cov(4,4) = m_covarianceValues[4] * qOverP * qOverP;
  return cov;
}
//=============================================================================
