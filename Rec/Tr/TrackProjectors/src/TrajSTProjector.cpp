// $Id: TrajSTProjector.cpp,v 1.5 2006-04-06 13:15:35 jvantilb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// from GaudiKernel
#include "GaudiKernel/IMagneticFieldSvc.h"

// from TrackFitEvent
#include "Event/StateTraj.h"

// from TrackInterfaces
#include "TrackInterfaces/ITrajPoca.h"

// local
#include "TrajSTProjector.h"

using namespace Gaudi;
using namespace LHCb;

// Declaration of the Tool Factory
static const  ToolFactory<TrajSTProjector>          s_factory ;
const        IToolFactory& TrajSTProjectorFactory = s_factory ;

//-----------------------------------------------------------------------------
///  Project a state onto a measurement
/// It returns the chi squared of the projection
//-----------------------------------------------------------------------------
StatusCode TrajSTProjector::project( const State& state,
                                     Measurement& meas )
{
  // Get the reference state trajectory
  XYZVector bfield;
  m_pIMF -> fieldVector( state.position(), bfield );

  // Set refVector in case it was not set before
  if ( !meas.refIsSet() ) meas.setRefVector( state.stateVector() );

  // Get the reference state trajectory
  const TrackVector& refVec = meas.refVector();
  const StateTraj refTraj = StateTraj( refVec, meas.z(), bfield ) ; 

  // Get the measurement trajectory representing the centre of gravity
  const Trajectory& measTraj = meas.trajectory();  

  double s1, s2;
  XYZVector distance;

  // Determine initial estimates of s1 and s2
  s1 = 0.0; // Assume state is already close to the minimum
  s2 = measTraj.arclength( refTraj.position(s1) );

  // Determine the actual minimum with the Poca tool
  m_poca -> minimize( refTraj, s1, measTraj, s2, distance, 20*mm );

  // Calculate the projection matrix
  ROOT::Math::SVector< double, 3 > unitDistance;
  distance.Unit().GetCoordinates( unitDistance.Array() );
  m_H = unitDistance * refTraj.derivative( s1 ) ;

  // Calculate the projected distance to the centre of gravity
  double projDist = distance.R() + Dot(m_H, state.stateVector() - refVec) ;

  // Get the sign of the distance
  int signDist = ( distance.x() > 0.0 ) ? 1 : -1 ;
 
  // Calculate the residual
  m_residual = - signDist * projDist ;  
  m_H *= signDist; // Correct for the sign of the distance

  // Set the error on the measurement so that it can be used in the fit
  double errMeasure2 = meas.resolution2( refTraj.position(s1), 
                                         refTraj.direction(s1) );
  m_errMeasure = sqrt( errMeasure2 );

  // Calculate the error on the residual
  m_errResidual = sqrt( errMeasure2 + ROOT::Math::Similarity<double,5>
                        ( m_H, state.covariance() ) );

  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
/// Initialize
//-----------------------------------------------------------------------------
StatusCode TrajSTProjector::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if( sc.isFailure() ) { return Error( "Failed to initialize!", sc ); }

  m_pIMF = svc<IMagneticFieldSvc>( "MagneticFieldSvc",true );
  m_poca = tool<ITrajPoca>( "TrajPoca" );

  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
/// Standard constructor, initializes variables
//-----------------------------------------------------------------------------
TrajSTProjector::TrajSTProjector( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : TrackProjector( type, name , parent )
{
  declareInterface<ITrackProjector>(this);
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
TrajSTProjector::~TrajSTProjector() {};
