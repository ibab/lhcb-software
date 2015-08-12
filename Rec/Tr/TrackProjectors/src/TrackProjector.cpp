// Include files 

// local
#include "TrackProjector.h"

// from GaudiKernel
#include "GaudiKernel/IMagneticFieldSvc.h"

// from TrackFitEvent
#include "Event/FitNode.h"
#include "Event/State.h"
#include "Event/Measurement.h"
#include "Event/StateVector.h"

// from TrackInterfaces
#include "Kernel/ITrajPoca.h"

// from LHCbKernel
#include "Kernel/AlignTraj.h"

// from TrackKernel
#include "TrackKernel/StateZTraj.h"

using namespace Gaudi;
using namespace LHCb;
using namespace ROOT::Math;
using ROOT::Math::SMatrix;

DECLARE_TOOL_FACTORY( TrackProjector )

//-----------------------------------------------------------------------------
/// Retrieve the chi squared of the (last) projection
//-----------------------------------------------------------------------------
double TrackProjector::chi2() const
{
  return ( m_errResidual > 0.0 ) ?
    ( m_residual / m_errResidual ) * ( m_residual / m_errResidual ) : 0.0 ;
}


//-----------------------------------------------------------------------------
/// Standard constructor, initializes variables
//-----------------------------------------------------------------------------
TrackProjector::TrackProjector( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : GaudiTool ( type, name , parent ),
    m_residual(0),
    m_errResidual(0)
{
  declareInterface<ITrackProjector>( this );
  declareProperty( "UseBField", m_useBField = false );
  declareProperty( "Tolerance", m_tolerance = 0.0005*Gaudi::Units::mm );
}

//-----------------------------------------------------------------------------
/// Initialize
//-----------------------------------------------------------------------------
StatusCode TrackProjector::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if( sc.isFailure() ) { return Error( "Failed to initialize!", sc ); }

  m_pIMF = svc<IMagneticFieldSvc>( "MagneticFieldSvc", true );
  m_poca = tool<ITrajPoca>( "TrajPoca" );
  
  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
TrackProjector::~TrackProjector() {}


// trivial helpers to make code clearer...
namespace
{
  typedef Gaudi::Matrix1x3 DualVector;

  DualVector dual( const Gaudi::XYZVector& v )
  {
    DualVector d;
    v.GetCoordinates( d.Array() );
    return d;
  }

  //  double dot( const DualVector& a, const Gaudi::XYZVector& b )
  //  {
  //    return a(0,0)*b.X() + a(0,1)*b.Y() + a(0,2)*b.Z();
  //  }
}

//-----------------------------------------------------------------------------
/// Project a statevector onto a measurement
/// It returns the chi squared of the projection
//-----------------------------------------------------------------------------

StatusCode TrackProjector::project( const StateVector& statevector,
				    const Measurement& meas )
{
  // Project onto the reference. First create the StateTraj with or without BField information.
  Gaudi::XYZVector bfield(0,0,0) ;
  if( m_useBField) m_pIMF -> fieldVector( statevector.position(), bfield ).ignore();
  const StateZTraj refTraj( statevector, bfield );

  // Get the measurement trajectory representing the centre of gravity
  const Trajectory& measTraj = meas.trajectory();

  // Determine initial estimates of s1 and s2
  m_sState = statevector.z() ; // Assume state is already close to the minimum
  m_sMeas = measTraj.muEstimate( refTraj.position(m_sState) );
  
  // Determine the actual minimum with the Poca tool
  Gaudi::XYZVector dist ;
  StatusCode sc = m_poca -> minimize( refTraj, m_sState,
				      measTraj, m_sMeas, dist, m_tolerance );
  if( sc.isFailure() ) { return sc; }

  // Set up the vector onto which we project everything. This should
  // actually be parallel to dist.
  m_unitPocaVector = (measTraj.direction(m_sMeas).Cross( refTraj.direction(m_sState) ) ).Unit() ;
  m_doca = m_unitPocaVector.Dot(dist) ;
  
  // compute the projection matrix from parameter space onto the (signed!) unit
  m_residual = -m_doca ;
  m_H = dual( m_unitPocaVector ) * refTraj.derivative(m_sState);
  
  // Set the error on the measurement so that it can be used in the fit
  double errMeasure2 = meas.resolution2( refTraj.position(m_sState), 
                                         refTraj.direction(m_sState) );
  m_errResidual = m_errMeasure = sqrt(errMeasure2);

  return StatusCode::SUCCESS;
}
//-----------------------------------------------------------------------------
/// Project a state onto a measurement
//-----------------------------------------------------------------------------
StatusCode TrackProjector::project( const State& state,
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
/// Project the state vector in this fitnode and update projection matrix and reference residual
//-----------------------------------------------------------------------------

StatusCode TrackProjector::projectReference( LHCb::FitNode& node ) const 
{
  // temporary, until we update interface
  TrackProjector* me = const_cast<TrackProjector*>(this) ;
  StatusCode sc = StatusCode::FAILURE ;
  if( node.hasMeasurement() ) {
    sc = me -> project(node.refVector(), node.measurement() );
    if( sc.isSuccess() ) {
      node.updateProjection( m_H, m_residual, m_errMeasure ) ;
      node.setPocaVector( m_unitPocaVector ) ;
      node.setDoca( m_doca ) ;
    }
  }
  return sc ;
}

//-----------------------------------------------------------------------------
/// Derivatives wrt.the measurement's alignment...
//-----------------------------------------------------------------------------
TrackProjector::Derivatives
TrackProjector::alignmentDerivatives( const StateVector& statevector,
				      const Measurement& meas,
				      const Gaudi::XYZPoint& pivot ) const
{
  // clean up this const_cast later
  TrackProjector* me = const_cast<TrackProjector*>(this) ;
  me->project( statevector, meas ).ignore() ;
  DualVector unit = dual( m_unitPocaVector ) ;
  
 // Calculate the derivative of the poca on measTraj to alignment parameters. Only non-zero elements:
  Gaudi::XYZVector arm = meas.trajectory().position(m_sMeas) - pivot ;
  ROOT::Math::SMatrix<double,3,6> dPosdAlpha ;
  // Derivative to translation
  dPosdAlpha(0,0) = dPosdAlpha(1,1) = dPosdAlpha(2,2) = 1 ;
  // Derivative to rotation around x-axis
  dPosdAlpha(1,3) = -arm.z() ;
  dPosdAlpha(2,3) =  arm.y() ;
  // Derivative to rotation around y-axis
  dPosdAlpha(0,4) =  arm.z() ;
  dPosdAlpha(2,4) = -arm.x() ;
  // Derivative to rotation around z-axis
  dPosdAlpha(0,5) = -arm.y() ;
  dPosdAlpha(1,5) =  arm.x() ;
  
  return unit * dPosdAlpha ;
  // compute the projection matrix from parameter space onto the (signed!) unit
  //return unit*AlignTraj( meas.trajectory(), pivot ).derivative( m_sMeas );
}

