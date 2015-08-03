// Include files
// -------------

// from Gaudi
#include "GaudiKernel/IMagneticFieldSvc.h"
#include "GaudiKernel/SystemOfUnits.h"

// Math Definitions
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"

// local
#include "BIntegrator.h"

//-----------------------------------------------------------------------------
// Implementation file for class : BIntegrator
//
// 2000-08-16 : M. Needham
// 2005-05-12 : Eduardo Rodrigues (adaptations to GaudiTool)
//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( BIntegrator )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
BIntegrator::BIntegrator( const std::string& type,
                          const std::string& name,
                          const IInterface* parent )
  : GaudiTool ( type, name , parent ), m_pIMF(0)
{
  declareInterface<IBIntegrator>(this);

  declareProperty( "NSteps", m_nSteps = 101 );
  declareProperty( "FirstZ", m_firstZ =  0.1*Gaudi::Units::mm );
  declareProperty( "LastZ",  m_lastZ = 9400.*Gaudi::Units::mm );
}
//=============================================================================
// Destructor
//=============================================================================
BIntegrator::~BIntegrator() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode BIntegrator::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;  // error already reported by base class

  // Retrieve a pointer to the magnetic field service
  m_pIMF = svc<IMagneticFieldSvc>( "MagneticFieldSvc", true );

  sc = calculateBdlCenter();
  info() << "Center of the field is at the z positions "
         << m_centerZ << endmsg;

  return sc;
}

//=============================================================================
// Get the z of center and the total Bdl
//=============================================================================
StatusCode BIntegrator::calculateBdlAndCenter(const Gaudi::XYZPoint& beginPoint,
                                               const Gaudi::XYZPoint& endPoint,
                                               const double tX,
                                               const double tY,
                                               double& zCenter,
                                               Gaudi::XYZVector& Bdl ) const
{
  // Point where field should be calculated
  Gaudi::XYZPoint  point(0.001,0.001,0.0001);
  Gaudi::XYZVector bField;      // returned field
  Bdl.SetXYZ(0.,0.,0.);

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
  StatusCode sc;
  for(iStep=0;iStep<m_nSteps;iStep++)    {

    if(point.z()>zCen)      {
      angleX = tX;
      angleY = tY;
    }
    double dX = angleX*stepSize;
    double dY = angleY*stepSize;
    double dZ = stepSize;
    point.SetX( point.x()+ dX);
    point.SetY( point.y()+ dY);
    point.SetZ( point.z()+ dZ);
    sc = m_pIMF->fieldVector(point,bField);
    if( !sc.isSuccess() ) warning() << "field vector not calculated" << endmsg;

    //Cacluate the Bdl
    Bdl.SetX( Bdl.x() + dY* bField.z()- dZ*bField.y() );
    Bdl.SetY( Bdl.y() + dZ*bField.x() -dX*bField.z());
    Bdl.SetZ( Bdl.z() + dX*bField.y() -dY*bField.x());

  } // iStep

  /// Now do the steps again but find the half of the magnetic field....

  double Bdlx_half =0.5*Bdl.x();
  double Bdly_half =0.5*Bdl.y();
  double Bdlz_half =0.5*Bdl.z();

  Bdl.SetXYZ( 0., 0., 0. );

  double min_Bdlx =10000.;
  double min_Bdly =10000.;
  double min_Bdlz =10000.;
  Gaudi::XYZPoint centerZ(0.,0.,0.);
  //reset al the variables used
  angleX = xCen/zCen;
  angleY = yCen/zCen;
  point.SetXYZ( 0., 0., 0. );
  for(iStep=0;iStep<m_nSteps;iStep++)    {

    if(point.z()>zCen)      {
      angleX = tX;
      angleY = tY;
    }
    double dX = angleX*stepSize;
    double dY = angleY*stepSize;
    double dZ = stepSize;
    point.SetX( point.x()+ dX);
    point.SetY( point.y()+ dY);
    point.SetZ( point.z()+ dZ);
    sc = m_pIMF->fieldVector(point,bField);
    if( !sc.isSuccess() ) warning() << "field vector not calculated" << endmsg;

    //Cacluate the Bdl
    Bdl.SetX( Bdl.x() + dY* bField.z()- dZ*bField.y() );
    Bdl.SetY( Bdl.y() + dZ*bField.x() -dX*bField.z());
    Bdl.SetZ( Bdl.z() + dX*bField.y() -dY*bField.x());

    if(fabs(Bdl.x()-Bdlx_half)< min_Bdlx){
      min_Bdlx=fabs(Bdl.x()-Bdlx_half);
      centerZ.SetX(point.z());
    }
    if(fabs(Bdl.y()-Bdly_half)< min_Bdly){
      min_Bdly=fabs(Bdl.y()-Bdly_half);
      centerZ.SetY(point.z());
    }
    if(fabs(Bdl.z()-Bdlz_half)< min_Bdlz){
      min_Bdlz=fabs(Bdl.z()-Bdlz_half);
      centerZ.SetZ(point.z());
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
  Gaudi::XYZVector bField;

  Gaudi::XYZVector BdlTotal(0.,0.,0.);
  Gaudi::XYZPoint  position = Gaudi::XYZPoint(0.0,0.0,0.);

  double stepSize = (m_lastZ-m_firstZ) / (double)m_nSteps;

  // Get the integral field
  int iStep;
  StatusCode sc;

  for( iStep=0;iStep < m_nSteps;iStep++) {
    position.SetXYZ( 0.1, 0.1, m_firstZ+((double)iStep+0.5)*stepSize );
    sc = m_pIMF -> fieldVector( position,bField );
    if( !sc.isSuccess() ) warning() << "field vector not calculated" << endmsg;

    //Calculate the Bdl
    BdlTotal.SetX( BdlTotal.x() - stepSize*bField.y() );
    BdlTotal.SetY( BdlTotal.y() + stepSize*bField.x() );
    BdlTotal.SetZ( BdlTotal.z() + 0.);
  } // iStep

  double Bdlx_half = 0.5*BdlTotal.x();
  double Bdly_half = 0.5*BdlTotal.y();
  double Bdlz_half = 0.5*BdlTotal.z();

  BdlTotal.SetXYZ( 0., 0., 0. );

  double min_Bdlx = 10000.;
  double min_Bdly = 10000.;
  double min_Bdlz = 10000.;

  //Loop again and find the middle of each of the components
  for ( iStep=0; iStep < m_nSteps; iStep++ ) {
    double z = m_firstZ+ (iStep+0.5)*stepSize;
    position.SetXYZ( 0.1, 0.1, z );
    sc = m_pIMF -> fieldVector( position,bField );
    if( !sc.isSuccess() ) warning() << "field vector not calculated" << endmsg;
    //Cacluate the Bdl
    BdlTotal.SetX( BdlTotal.x() - stepSize*bField.y() );
    BdlTotal.SetY( BdlTotal.y() + stepSize*bField.x() );
    BdlTotal.SetZ( BdlTotal.z() + 0.);
    if ( fabs(BdlTotal.x()-Bdlx_half) < min_Bdlx ) {
      min_Bdlx = fabs( BdlTotal.x()-Bdlx_half );
      m_centerZ.SetX(z);
    }
    if ( fabs(BdlTotal.y()-Bdly_half)< min_Bdly ) {
      min_Bdly = fabs( BdlTotal.y()-Bdly_half );
      m_centerZ.SetY(z);
    }
    if ( fabs(BdlTotal.z()-Bdlz_half)< min_Bdlz ) {
      min_Bdlz = fabs( BdlTotal.z()-Bdlz_half );
      m_centerZ.SetZ(z);
    }
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
