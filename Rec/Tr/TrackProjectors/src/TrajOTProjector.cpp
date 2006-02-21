// $Id: TrajOTProjector.cpp,v 1.5 2006-02-21 18:28:25 jvantilb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// from GaudiKernel
#include "GaudiKernel/IMagneticFieldSvc.h"

// from OTDet
#include"OTDet/DeOTDetector.h"

// from LHCbKernel
#include "Kernel/LineTraj.h"

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

  // Get the reference state trajectory
  OTMeasurement& otmeas = *( dynamic_cast<OTMeasurement*>(&meas) );
  TrackVector refVec = otmeas.referenceVector( state.stateVector() );
  StateTraj refTraj = StateTraj( refVec, otmeas.z(), bfield );

  // Get the measurement trajectory
  OTChannelID OTChan = meas.lhcbID().otID();
  DeOTModule* module = m_det -> module( OTChan );
  double stereoAngle = module -> stereoAngle();
  XYZPoint centrePos = module -> centerOfStraw( OTChan.straw() );
  double cosA = cos( stereoAngle );
  double sinA = sin( stereoAngle );
  XYZVector dir =  XYZVector( -sinA, cosA, 0. );
  if ( module->bottomModule() ) dir *= -1.;
  double halfWireLength = module -> wireLength() / 2.;
  const std::pair<double,double> range( -halfWireLength, halfWireLength );
  LineTraj measTraj = LineTraj( centrePos, dir, range );
  // TODO : WHAT WE WANT!!!
  // Trajectory& measTraj = m_det -> trajectory( meas.lhcbID().otID() );

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
  double distToReadout = measTraj.length() / 2. - s2;  

  // Correct measure for the propagation along the wire
  double dDrift = otmeas.measure() - 
    distToReadout * m_det->propagationDelay()/ m_det->driftDelay() ;

  // Calculate the residual
  m_residual = otmeas.ambiguity() * dDrift - signDist * distToWire ;  
  m_H *= signDist; // Correct for the sign of the distance
  
  // TODO: when error on the measurement depends on the position of the
  //       track use the reference trajectory again. This will be the case
  //       when the error depends on the drift distance.
  computeErrorResidual( state, meas );

  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
/// Initialize
//-----------------------------------------------------------------------------
StatusCode TrajOTProjector::initialize()
{
  StatusCode sc = GaudiTool::initialize();

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
