// $Id: TrajVeloPhiProjector.cpp,v 1.9 2006-06-29 12:51:13 mneedham Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// from GaudiKernel
#include "GaudiKernel/IMagneticFieldSvc.h"

// from TrackFitEvent
#include "Event/StateTraj.h"

// from TrackInterfaces
#include "Kernel/ITrajPoca.h"

// local
#include "TrajVeloPhiProjector.h"

using namespace Gaudi;
using namespace LHCb;
using ROOT::Math::Similarity;
using ROOT::Math::SMatrix;

DECLARE_TOOL_FACTORY( TrajVeloPhiProjector );

//-----------------------------------------------------------------------------
///  Project a state onto a measurement
/// It returns the chi squared of the projection
//-----------------------------------------------------------------------------
StatusCode TrajVeloPhiProjector::project( const State& state,
                                          Measurement& meas )
{
  // Get the reference state trajectory
  XYZVector bfield;
  m_pIMF -> fieldVector( state.position(), bfield );

  // Set refVector in case it was not set before
  if ( !meas.refIsSet() ) meas.setRefVector( state.stateVector() );
  const TrackVector& refVec = meas.refVector();
  const StateTraj refTraj = StateTraj( refVec, meas.z(), bfield ) ; 

  // Get the measurement trajectory representing the centre of gravity
  const Trajectory& measTraj = meas.trajectory();

  // Determine initial estimates of s1 and s2
  double s1 = 0.0; // Assume state is already close to the minimum
  double s2 = measTraj.arclength( refTraj.position(s1) );

  // Determine the actual minimum with the Poca tool
  XYZVector distance;
  m_poca -> minimize( refTraj, s1, measTraj, s2, distance, m_tolerance );

  // Calculate the projection matrix
  SMatrix< double, 1, 3 > unitDistance;
  distance.Unit().GetCoordinates( unitDistance.Array() );
  m_H = unitDistance * refTraj.derivative( s1 ) ;

  // Calculate the projected distance to the centre of gravity
  double projDist = distance.R() + Vector1(m_H*(state.stateVector() - refVec))(0) ;

  // Get the sign of the distance
  int signDist = (distance.Cross(measTraj.direction(s2)).z() > 0.0) ? 1 : -1 ;
 
  // Calculate the residual
  m_residual = - signDist * projDist ;  
  if (signDist<0) m_H = -m_H; // Correct for the sign of the distance 

  // Set the error on the measurement so that it can be used in the fit
  double errMeasure2 = meas.resolution2( refTraj.position(s1), 
                                         refTraj.direction(s1));
  m_errMeasure = sqrt( errMeasure2 );

  // Calculate the error on the residual
  m_errResidual = sqrt( errMeasure2 + Similarity( m_H, state.covariance() )(0,0) );

  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
/// Initialize
//-----------------------------------------------------------------------------
StatusCode TrajVeloPhiProjector::initialize()
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
TrajVeloPhiProjector::TrajVeloPhiProjector( const std::string& type,
                                            const std::string& name,
                                            const IInterface* parent )
  : TrackProjector( type, name , parent )
{
  declareInterface<ITrackProjector>(this);

  declareProperty( "tolerance", m_tolerance = 0.001*Gaudi::Units::mm );
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
TrajVeloPhiProjector::~TrajVeloPhiProjector() {};
