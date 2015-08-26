// $Id: $
// Include files 



// local
#include "FastVeloSensor.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FastVeloSensor
//
// 2010-09-08 : Olivier Callot
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FastVeloSensor::FastVeloSensor( DeVeloRType* rSensor, double xBeam, double yBeam ) : 
  m_next( -1 ),
  m_previous(-1),
  m_isRSensor( true ),
  m_rSensor( rSensor ), 
  m_phiSensor( NULL )  
{
  m_sensor = dynamic_cast<DeVeloSensor*>( rSensor );
  setGeometry( rSensor, xBeam, yBeam );

  //== Create four vector for the hits
  FastVeloHits dum;
  m_hits.push_back( dum );
  m_hits.push_back( dum );
  m_hits.push_back( dum );
  m_hits.push_back( dum );
}

//=========================================================================
//  Constructor for Phi sensors
//=========================================================================
FastVeloSensor::FastVeloSensor( DeVeloPhiType* phiSensor, double xBeam, double yBeam ) :
  m_next( -1 ),
  m_previous(-1),
  m_isRSensor( false ),
  m_rSensor( NULL ), 
  m_phiSensor( phiSensor )  
{
  m_sensor = dynamic_cast<DeVeloSensor*>( phiSensor );
  setGeometry( phiSensor, xBeam, yBeam );

  //== Create two vector for the hits
  FastVeloHits dum;
  m_hits.push_back( dum );
  m_hits.push_back( dum );  

  //=== Build the parameterisation of the strip lines as ax+by+c = 0.
  m_lineParams.reserve( 2048 );
  m_lineParams.clear();
  
  for ( unsigned int kk = 0 ; 2048 > kk ; ++kk ) {
    LHCb::VeloChannelID id( m_number, kk );
    std::auto_ptr<LHCb::Trajectory> myTraj = phiSensor->trajectory( id, 0. );
    Gaudi::XYZPoint p1 = myTraj->beginPoint();
    Gaudi::XYZPoint p2 = myTraj->endPoint();
    double dist = sqrt( ( p1.x() - p2.x() ) * ( p1.x() - p2.x() ) + (p1.y() - p2.y() ) * ( p1.y() - p2.y() ) );
    double cros = ( p1.y() * p2.x() - p2.y() * p1.x() );
    double c = cros/dist;
    double a = ( p2.y() - p1.y() ) / dist;
    double b = ( p1.x() - p2.x() ) / dist;
    m_lineParams.push_back( FastVeloLineParams( a, b, c, .5 * ( p1.x() + p2.x() ),  .5 * ( p1.y() + p2.y() ) ) );
  }
}

//=========================================================================
//  Set the geometry, i.e. cache information
//=========================================================================
void FastVeloSensor::setGeometry ( DeVeloSensor* sensor, double xBeam, double yBeam ) {
  m_number  = sensor->sensorNumber();
  m_isRight = sensor->isRight();
  m_z       = sensor->z();
  m_centre  = sensor->localToGlobal(  Gaudi::XYZPoint( 0., 0., 0. ) );
  //== Hack to prevent too small numbers when converting to float later!
  double xTmp = m_centre.x();
  double yTmp = m_centre.y();
  if ( fabs( xTmp ) < 1.e-9 ) xTmp = 0.;
  if ( fabs( yTmp ) < 1.e-9 ) yTmp = 0.;
  m_centre  = Gaudi::XYZPoint( xTmp, yTmp, m_centre.z());
  m_xBeam = xBeam;
  m_yBeam = yBeam;

  Gaudi::XYZPoint temp;
  temp = sensor->localToGlobal( Gaudi::XYZPoint( 10., 0., 0. ) );
  m_dzDx = ( temp.z() - m_centre.z() ) / ( temp.x() - m_centre.x() );
  temp = sensor->localToGlobal( Gaudi::XYZPoint( 0., 10., 0. ) );
  m_dzDy = ( temp.z() - m_centre.z() ) / ( temp.y() - m_centre.y() );

  m_rOffset.clear();
  double xOffset = m_centre.x() - xBeam;
  double yOffset = m_centre.y() - yBeam;
  
  if ( m_isRight ) {
    m_rOffset.push_back( -cos( -.375 * Gaudi::Units::pi ) * xOffset - sin(-.375 * Gaudi::Units::pi ) * yOffset );
    m_rOffset.push_back( -cos( -.125 * Gaudi::Units::pi ) * xOffset - sin(-.175 * Gaudi::Units::pi ) * yOffset );
    m_rOffset.push_back( -cos(  .125 * Gaudi::Units::pi ) * xOffset - sin( .175 * Gaudi::Units::pi ) * yOffset );
    m_rOffset.push_back( -cos(  .375 * Gaudi::Units::pi ) * xOffset - sin( .375 * Gaudi::Units::pi ) * yOffset );
  } else {
    m_rOffset.push_back(  cos( -.375 * Gaudi::Units::pi ) * xOffset + sin(-.375 * Gaudi::Units::pi ) * yOffset );
    m_rOffset.push_back(  cos( -.125 * Gaudi::Units::pi ) * xOffset + sin(-.175 * Gaudi::Units::pi ) * yOffset );
    m_rOffset.push_back(  cos(  .125 * Gaudi::Units::pi ) * xOffset + sin( .175 * Gaudi::Units::pi ) * yOffset );
    m_rOffset.push_back(  cos(  .375 * Gaudi::Units::pi ) * xOffset + sin( .375 * Gaudi::Units::pi ) * yOffset );
  }
}
//=============================================================================
