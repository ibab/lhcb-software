// $Id: TrajOTProjector.cpp,v 1.9 2006-04-06 13:15:35 jvantilb Exp $
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
#include "TrackInterfaces/ITrajPoca.h"

// local
#include "TrajOTProjector.h"

using namespace Gaudi;
using namespace LHCb;

// Declaration of the Tool Factory
static const  ToolFactory<TrajOTProjector>          s_factory ;
const        IToolFactory& TrajOTProjectorFactory = s_factory ;

//-----------------------------------------------------------------------------
///  Project a state onto a measurement
/// It returns the chi squared of the projection
//-----------------------------------------------------------------------------
StatusCode TrajOTProjector::project( const State& state,
                                     Measurement& meas )
{
  XYZVector bfield;
  m_pIMF -> fieldVector( state.position(), bfield );

  // Set refVector in case it was not set before
  if ( !meas.refIsSet() ) meas.setRefVector( state.stateVector() );

  // Get the reference state trajectory
  const TrackVector& refVec = meas.refVector();
  StateTraj refTraj = StateTraj( refVec, meas.z(), bfield );

  // Get the measurement trajectory
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

  // Calculate the projected drift distance
  double distToWire = distance.R() + Dot(m_H, state.stateVector() - refVec) ;

  // Get the sign of the distance
  int signDist = ( distance.x() > 0.0 ) ? 1 : -1 ;

  // Get the distance to the readout
  double distToReadout = measTraj.endRange() - s2;

  // Correct measure for the propagation along the wire
  double dDrift = meas.measure() - 
    distToReadout * m_det->propagationDelay()/ m_det->driftDelay() ;

  // Calculate the residual
  OTMeasurement& otmeas = *( dynamic_cast<OTMeasurement*>(&meas) );
  m_residual = otmeas.ambiguity() * dDrift - signDist * distToWire ;  
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
StatusCode TrajOTProjector::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if( sc.isFailure() ) { return Error( "Failed to initialize!", sc ); }

  m_det = getDet<DeOTDetector>( m_otTrackerPath );
  m_pIMF = svc<IMagneticFieldSvc>( "MagneticFieldSvc",true );
  m_poca = tool<ITrajPoca>( "TrajPoca" );

  return sc;
}

//-----------------------------------------------------------------------------
/// Standard constructor, initializes variables
//-----------------------------------------------------------------------------
TrajOTProjector::TrajOTProjector( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : TrackProjector( type, name , parent )
{
  declareInterface<ITrackProjector>(this);

  declareProperty( "OTGeometryPath",
                   m_otTrackerPath = DeOTDetectorLocation::Default );
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
TrajOTProjector::~TrajOTProjector() {};
