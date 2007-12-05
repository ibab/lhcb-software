
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// from GaudiKernel
#include "GaudiKernel/IMagneticFieldSvc.h"

// from OTDet
#include"OTDet/DeOTDetector.h"

// from TrackFitEvent
#include "Event/OTMeasurement.h"
#include "Event/StateVector.h"
#include "Event/StateZTraj.h"

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

StatusCode TrajOTProjector::project( const StateVector& statevector,
                                     const Measurement& meas )
{
  return meas.checkType( Measurement::OT ) ?
    project(statevector, dynamic_cast<const OTMeasurement&>(meas) ) :
    StatusCode::FAILURE;
}

//-----------------------------------------------------------------------------
/// Project a state onto a measurement
/// It returns the chi squared of the projection
//-----------------------------------------------------------------------------
StatusCode TrajOTProjector::project( const LHCb::StateVector& statevector, 
                                     const OTMeasurement& meas )
{
  // Project onto the reference. First create the StateTraj with or without BField information.
  XYZVector bfield(0,0,0) ;
  if( m_useBField) m_pIMF -> fieldVector( statevector.position(), bfield ).ignore();
  const StateZTraj refTraj( statevector, bfield );
  
  // Get the measurement trajectory representing the centre of gravity
  const Trajectory& measTraj = meas.trajectory();

  // Determine initial estimates of s1 and s2
  double s1 = statevector.z() ; // Assume state is already close to the minimum
  double s2 = measTraj.muEstimate( refTraj.position(s1) );

  // Determine the actual minimum with the Poca tool
  static XYZVector dist; // avoid constructing this every call to project...
  StatusCode sc = m_poca -> minimize( refTraj, s1,
				      measTraj, s2, dist, m_tolerance );
  if( sc.isFailure() ) { 
    error() << "statevector: " << statevector << endmsg ;
    return sc; }

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
  double distToWire = dot( unit, dist ) ;
  (const_cast<OTMeasurement&>(meas)).setAmbiguity( distToWire > 0 ? 1 : -1 ) ;
  
  if (m_useDrift) {
    OTDet::RadiusWithError radiusWithError(meas.driftRadiusWithError(s2)) ;
    m_residual   = -( distToWire - meas.ambiguity() * radiusWithError.val) ;
    m_errMeasure = radiusWithError.err ;
  } else {
    m_residual = -distToWire;
    m_errMeasure  = meas.module().cellRadius()/std::sqrt(3.0) ;
  }
  m_errResidual = m_errMeasure ;

  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
/// Project a state onto a measurement
//-----------------------------------------------------------------------------
StatusCode TrajOTProjector::project( const State& state,
                                     const Measurement& meas )
{
  // Project onto the reference (prevent the virtual function call)
  StatusCode sc = project(LHCb::StateVector(state.stateVector(), state.z()), meas ) ;
  
  if(sc.isSuccess())
    // Calculate the error on the residual
    m_errResidual = sqrt( m_errMeasure*m_errMeasure + Similarity( m_H, state.covariance() )(0,0) );
  
  return sc ;
}

//-----------------------------------------------------------------------------
/// Derivatives wrt. the measurement's alignment...
//-----------------------------------------------------------------------------
TrajOTProjector::Derivatives
TrajOTProjector::alignmentDerivatives( const StateVector& statevector,
                                       const Measurement& meas, 
                                       const Gaudi::XYZPoint& pivot ) const
{
  // create the track trajectory...
  static XYZVector bfield;
  // Create StateTraj with or without BField information.
  if( m_useBField )
    {
      m_pIMF -> fieldVector( statevector.position(),bfield ).ignore();
    }
  else { bfield.SetXYZ( 0., 0., 0. ); }
  const StateZTraj refTraj(statevector, bfield );

  // Get the measurement trajectory
  const Trajectory& measTraj = meas.trajectory();  

  // Determine initial estimates of s1 and s2
  double s1 = meas.z() ; // Assume state is already close to the minimum
  double s2 = measTraj.muEstimate( refTraj.position(s1) );

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
  declareProperty( "UseDrift", m_useDrift = true );
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
TrajOTProjector::~TrajOTProjector() {}
