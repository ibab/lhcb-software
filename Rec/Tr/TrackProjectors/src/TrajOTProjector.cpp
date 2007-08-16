// $Id: TrajOTProjector.cpp,v 1.27 2007-08-16 13:24:06 graven Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// from GaudiKernel
#include "GaudiKernel/IMagneticFieldSvc.h"

// from OTDet
#include"OTDet/DeOTDetector.h"

// from TrackFitEvent
#include "Event/OTMeasurement.h"
#include "Event/StateTraj.h"

// from TrackInterfaces
#include "Kernel/ITrajPoca.h"

// from LHCbKernel
#include "Kernel/AlignTraj.h"

// local
#include "TrajOTProjector.h"

// Namespaces
using namespace Gaudi;
using namespace LHCb;
using ROOT::Math::SMatrix;

DECLARE_TOOL_FACTORY( TrajOTProjector );

// trivial helpers to make code clearer...
namespace
{
  typedef Matrix1x3 DualVector;
  
  DualVector dual( const XYZVector& v )
  {
    DualVector d;
    v.GetCoordinates( d.Array() );
    return d;
  }
  
  double dot( const DualVector& a, const XYZVector& b )
  {
    return a(0,0)*b.X() + a(0,1)*b.Y() + a(0,2)*b.Z();
  }
}

StatusCode TrajOTProjector::project( const State& state,
                                     Measurement& meas )
{
  return meas.checkType( Measurement::OT ) ?
    project( state, dynamic_cast<OTMeasurement&>(meas) ) :
    StatusCode::FAILURE;
}

// FIXME: should move this code to OTMeasurement... 
double TrajOTProjector::driftDistance( const OTMeasurement& meas,
				       double arclen ) const 
{
  // Get the distance to the readout
  double distToReadout = meas.trajectory().endRange() - arclen;
  // Correct measure for the propagation along the wire
  return meas.measure() -
    distToReadout*m_det->propagationDelay()/m_det->driftDelay();
}

//-----------------------------------------------------------------------------
/// Project a state onto a measurement
/// It returns the chi squared of the projection
//-----------------------------------------------------------------------------
StatusCode TrajOTProjector::project( const State& state,
                                     OTMeasurement& meas )
{
  // Set refVector in case it was not set before
  if( !meas.refIsSet() ) { meas.setRefVector( state.stateVector() ); }

  // Create reference trajectory
  static XYZVector bfield; // avoid constructing this every call to project...
  const TrackVector& refVec = meas.refVector();
  // Create StateTraj with or without BField information.
  if( m_useBField )
    {
      m_pIMF -> fieldVector( XYZPoint( refVec[0],
				       refVec[1], meas.z() ), bfield ).ignore();
    }
  else { bfield.SetXYZ( 0., 0., 0. ); }
  const StateTraj refTraj( refVec, meas.z(), bfield );

  // Get the measurement trajectory
  const Trajectory& measTraj = meas.trajectory();

  // Determine initial estimates of s1 and s2
  double s1 = 0.; // Assume state is already close to the minimum
  double s2 = measTraj.arclength( refTraj.position(s1) );

  // Determine the actual minimum with the Poca tool
  XYZVector dist;
  StatusCode sc = m_poca -> minimize( refTraj, s1,
				      measTraj, s2, dist, m_tolerance );
  if( sc.isFailure() ) { return sc; }

  // Determine the (oriented!) axis onto which we project. This ugly
  // construct is the result of a bug fix that was not supposed to
  // change the sign of the ambiguity.
  XYZVector measdir = measTraj.direction(s2) ;
  int signdir       = measdir.y() >0 ? 1 : -1 ;
  DualVector unit   = signdir * dual( (measdir.Cross( refTraj.direction(s1) ) ).Unit() ) ;
  
  // project the space-point derivatives onto the axis
  // to get the derivatives of the residual
  m_H = unit*refTraj.derivative(s1);

  // Calculate the reference distance and set the ambiguity "on the fly"
  double distToWireRef = dot( unit, dist ) ;
  meas.setAmbiguity( distToWireRef > 0 ? 1 : -1 ) ;
  
  // Add the first order correction
  double distToWire = distToWireRef + Vector1( m_H * ( state.stateVector() - refVec ) )(0);

  // Calculate the residual: 
  m_residual = -( distToWire - meas.ambiguity()*driftDistance( meas, s2 ) );

  // Set the error on the measurement so that it can be used in the fit
  double errMeasure2 = meas.resolution2( refTraj.position(s1), 
                                         refTraj.direction(s1) );
  m_errMeasure = sqrt(errMeasure2);

  // Calculate the error on the residual
  m_errResidual = sqrt( errMeasure2 +
			Similarity( m_H, state.covariance() )(0,0) );

  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
/// Derivatives wrt. the measurement's alignment...
//-----------------------------------------------------------------------------
TrajOTProjector::Derivatives
TrajOTProjector::alignmentDerivatives( const Measurement& meas, 
                                       const Gaudi::XYZPoint& pivot ) const
{
  // create the track trajectory...
  const TrackVector& refVec = meas.refVector();
  static XYZVector bfield;
  // Create StateTraj with or without BField information.
  if( m_useBField )
    {
      m_pIMF -> fieldVector( XYZPoint( refVec[0],
				       refVec[1], meas.z() ), bfield ).ignore();
    }
  else { bfield.SetXYZ( 0., 0., 0. ); }
  const StateTraj refTraj( refVec, meas.z(), bfield );

  // Get the measurement trajectory
  const Trajectory& measTraj = meas.trajectory();  

  // Determine initial estimates of s1 and s2
  double s1 = 0.; // Assume state is already close to the minimum
  double s2 = measTraj.arclength( refTraj.position(s1) );

  // Determine the actual minimum with the Poca tool
  static XYZVector dist;
  m_poca -> minimize( refTraj, s1, measTraj, s2, dist, m_tolerance );

  // Set up the vector onto which we project everything. This ugly
  // construct is the result of a bug fix that was not supposed to
  // change the sign of the ambiguity.
  XYZVector measdir = measTraj.direction(s2) ;
  int signdir       = measdir.y() >0 ? 1 : -1 ;
  DualVector unit   = signdir * dual( (measdir.Cross( refTraj.direction(s1) ) ).Unit() ) ;
 
  // compute the projection matrix from parameter space onto the (signed!) unit
  return unit*AlignTraj( measTraj, pivot ).derivative(s2);
}

//-----------------------------------------------------------------------------
/// Initialize
//-----------------------------------------------------------------------------
StatusCode TrajOTProjector::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if( sc.isFailure() ) { return Error( "Failed to initialize!", sc ); }

  m_det = getDet<DeOTDetector>(m_otTrackerPath);
  m_pIMF = svc<IMagneticFieldSvc>( "MagneticFieldSvc", true );
  m_poca = tool<ITrajPoca>( "TrajPoca" );

  return sc;
}

//-----------------------------------------------------------------------------
/// Standard constructor, initializes variables
//-----------------------------------------------------------------------------
TrajOTProjector::TrajOTProjector( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : TrackProjector( type, name, parent )
{
  declareInterface<ITrackProjector>(this);

  declareProperty( "OTGeometryPath",
		   m_otTrackerPath = DeOTDetectorLocation::Default );
  declareProperty( "Tolerance", m_tolerance = 0.01 );
  declareProperty( "UseBField", m_useBField = false );
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
TrajOTProjector::~TrajOTProjector() {}
