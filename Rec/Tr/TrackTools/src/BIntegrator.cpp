// $Id: BIntegrator.cpp,v 1.2 2005-05-25 14:31:35 cattanem Exp $
// Include files 
// -------------

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// from TrackEvent
#include "Event/State.h"
#include "Event/TrackParameters.h"

// from CLHEP
#include "CLHEP/Units/PhysicalConstants.h"

// local
#include "BIntegrator.h"

//-----------------------------------------------------------------------------
// Implementation file for class : BIntegrator
//
// 2000-08-16 : M. Needham
// 2005-05-12 : Eduardo Rodrigues (adaptations to new track event model)
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<BIntegrator>          s_factory ;
const        IToolFactory& BIntegratorFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
BIntegrator::BIntegrator( const std::string& type,
                          const std::string& name,
                          const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IBIntegrator>(this);

  declareProperty( "NSteps", m_nSteps = 501 ); 
  declareProperty( "FirstZ", m_firstZ = 0.1*mm );
  declareProperty( "LastZ",  m_lastZ = 9400.*mm );
}
//=============================================================================
// Destructor
//=============================================================================
BIntegrator::~BIntegrator() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode BIntegrator::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;  // error already reported by base class

  // Retrieve a pointer to the magnetic field service
  m_pIMF = svc<IMagneticFieldSvc>( "MagneticFieldSvc", true );
 
  calculateBdlCenter();
  info() << "Center of the field is at the z positions "
         << m_centerZ << endreq;
  
  return sc;
}

//=============================================================================
// Get the z of center and the total Bdl
//=============================================================================
StatusCode BIntegrator::calculateBdlAndCenter( const HepPoint3D& beginPoint, 
                                               const HepPoint3D& endPoint,
                                               const double tX,
                                               const double tY, 
                                               double& zCenter, 
                                               HepVector3D& Bdl ) const
{
  // Point where field should be calculated
  HepPoint3D  point(0.001,0.001,0.0001);
  HepVector3D bField;      // returned field
  Bdl.setX(0.);
  Bdl.setY(0.);
  Bdl.setZ(0.);

  //First get the Center by walking in two rays..
  double zCen = m_centerZ.x();  // the Bdlx is the important component
  double xCen = endPoint.x() + tX*(zCen-endPoint.z());
  double yCen = endPoint.y() + tY*(zCen-endPoint.z());
  if (xCen/zCen>0.3) { 
    xCen = 0.3*zCen;
  }
  else if (xCen/zCen< -0.3) {
    xCen = -0.3*zCen;
  }
  if( yCen/zCen>0.25) {
    yCen = 0.25*zCen;
  }
  else if (yCen/zCen< -0.25){ 
    yCen = -0.25*zCen;
  }

  double angleX = xCen/zCen;
  double angleY = yCen/zCen;
  double stepSize = (endPoint.z()-beginPoint.z())/(double)m_nSteps;
  int iStep;
  for(iStep=0;iStep<m_nSteps;iStep++)    {

    if(point.z()>zCen)      {
      angleX = tX;
      angleY = tY;
    }
    double dX = angleX*stepSize;
    double dY = angleY*stepSize;
    double dZ = stepSize;
    point.setX( point.x()+ dX);
    point.setY( point.y()+ dY);
    point.setZ( point.z()+ dZ);
    m_pIMF->fieldVector(point,bField);

    //Cacluate the Bdl 
    Bdl.setX( Bdl.x() + dY* bField.z()- dZ*bField.y() );
    Bdl.setY( Bdl.y() + dZ*bField.x() -dX*bField.z());
    Bdl.setZ( Bdl.z() + dX*bField.y() -dY*bField.x());
    
  } // iStep

  /// Now do the steps again but find the half of the magnetic field....

  double Bdlx_half =0.5*Bdl.x();
  double Bdly_half =0.5*Bdl.y();
  double Bdlz_half =0.5*Bdl.z();
  
  Bdl.setX( 0.);
  Bdl.setY( 0.);
  Bdl.setZ( 0.);
  
  double min_Bdlx =10000.;
  double min_Bdly =10000.;
  double min_Bdlz =10000.;
  HepPoint3D centerZ(0.,0.,0.);
  //reset al the variables used
  angleX = xCen/zCen;
  angleY = yCen/zCen;
  point.setX(0.);
  point.setY(0.);
  point.setZ(0.);
  for(iStep=0;iStep<m_nSteps;iStep++)    {

    if(point.z()>zCen)      {
      angleX = tX;
      angleY = tY;
    }
    double dX = angleX*stepSize;
    double dY = angleY*stepSize;
    double dZ = stepSize;
    point.setX( point.x()+ dX);
    point.setY( point.y()+ dY);
    point.setZ( point.z()+ dZ);
    m_pIMF->fieldVector(point,bField);

    //Cacluate the Bdl 
    Bdl.setX( Bdl.x() + dY* bField.z()- dZ*bField.y() );
    Bdl.setY( Bdl.y() + dZ*bField.x() -dX*bField.z());
    Bdl.setZ( Bdl.z() + dX*bField.y() -dY*bField.x());

    if(fabs(Bdl.x()-Bdlx_half)< min_Bdlx){
      min_Bdlx=fabs(Bdl.x()-Bdlx_half);
      centerZ.setX(point.z());
    }
    if(fabs(Bdl.y()-Bdly_half)< min_Bdly){
      min_Bdly=fabs(Bdl.y()-Bdly_half);
      centerZ.setY(point.z());
    }
    if(fabs(Bdl.z()-Bdlz_half)< min_Bdlz){
      min_Bdlz=fabs(Bdl.z()-Bdlz_half);
      centerZ.setZ(point.z());
    }

  }

  //take the x component of the zcenter.....
  zCenter = centerZ.x();

  return StatusCode::SUCCESS;
}

//=============================================================================
// 
//=============================================================================
StatusCode BIntegrator::calculateBdlCenter()
{
  // Centre of the field
  HepVector3D bField;

  HepVector3D BdlTotal(0.,0.,0.);
  HepPoint3D  position = HepPoint3D(0.0,0.0,0.);

  double stepSize = (m_lastZ-m_firstZ) / (double)m_nSteps;

  // Get the integral field
  int iStep;
  for( iStep=0;iStep < m_nSteps;iStep++) {
    position.setX(0.1);
    position.setY(0.1);
    position.setZ( m_firstZ+((double)iStep+0.5)*stepSize );
    m_pIMF -> fieldVector( position,bField );

    //Calculate the Bdl 
    BdlTotal.setX( BdlTotal.x() - stepSize*bField.y() );
    BdlTotal.setY( BdlTotal.y() + stepSize*bField.x() );
    BdlTotal.setZ( BdlTotal.z() + 0.);
  } // iStep

  double Bdlx_half = 0.5*BdlTotal.x();
  double Bdly_half = 0.5*BdlTotal.y();
  double Bdlz_half = 0.5*BdlTotal.z();
  
  BdlTotal.setX( 0.);
  BdlTotal.setY( 0.);
  BdlTotal.setZ( 0.);
  
  double min_Bdlx = 10000.;
  double min_Bdly = 10000.;
  double min_Bdlz = 10000.;

  //Loop again and find the middle of each of the components
  for ( iStep=0; iStep < m_nSteps; iStep++ ) {
    double z = m_firstZ+ (iStep+0.5)*stepSize;
    position.setX(0.1);
    position.setY(0.1);
    position.setZ(z);
    m_pIMF -> fieldVector( position,bField );
    //Cacluate the Bdl 
    BdlTotal.setX( BdlTotal.x() - stepSize*bField.y() );
    BdlTotal.setY( BdlTotal.y() + stepSize*bField.x() );
    BdlTotal.setZ( BdlTotal.z() + 0.);
    if ( fabs(BdlTotal.x()-Bdlx_half) < min_Bdlx ) {
      min_Bdlx = fabs( BdlTotal.x()-Bdlx_half );
      m_centerZ.setX(z);
    }
    if ( fabs(BdlTotal.y()-Bdly_half)< min_Bdly ) {
      min_Bdly = fabs( BdlTotal.y()-Bdly_half );
      m_centerZ.setY(z);
    }
    if ( fabs(BdlTotal.z()-Bdlz_half)< min_Bdlz ) {
      min_Bdlz = fabs( BdlTotal.z()-Bdlz_half );
      m_centerZ.setZ(z);
    }
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
