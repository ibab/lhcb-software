// $Id: TrajOTProjector.cpp,v 1.2 2006-01-17 09:20:50 ebos Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// from GaudiKernel
#include "GaudiKernel/IMagneticFieldSvc.h"

// from OTDet
#include"OTDet/DeOTDetector.h"

// from TrackFitEvent
#include "Event/OTMeasurement.h"
#include "Kernel/LineTraj.h"
#include "Event/StateTraj.h"

// from TrackInterfaces
#include "TrackInterfaces/ITrajPoca.h"

// local
#include "TrajOTProjector.h"

// Declaration of the Tool Factory
static const  ToolFactory<TrajOTProjector>          s_factory ;
const        IToolFactory& TrajOTProjectorFactory = s_factory ;

///  Project a state onto a measurement.
/// It returns the chi squared of the projection
StatusCode TrajOTProjector::project( const State& state,
                                     Measurement& meas )
{
  HepVector3D bfield;
  m_pIMF -> fieldVector( state.position(), bfield );

  // Get the reference state trajectory
  OTMeasurement& otmeastmp = *( dynamic_cast<OTMeasurement*>(&meas) );
  HepVector refVec = HepVector(5,0);
  refVec = otmeastmp.referenceVector( state.stateVector() ) ;
  StateTraj refTraj = StateTraj( refVec, otmeastmp.z(), bfield );

  // Get the measurement trajectory
  OTChannelID OTChan   = meas.lhcbID().otID();
  DeOTModule* module   = m_det -> module( OTChan );
  double stereoAngle   = module -> stereoAngle();
  HepPoint3D centrePos = module -> centerOfStraw( OTChan.straw() );
  double cosA = cos( stereoAngle );
  double sinA = sin( stereoAngle );
  HepVector3D dir =  HepVector3D( -sinA, cosA, 0. );
  //TODO : WHAT WE WANT!!!
  // Trajectory& measTraj = m_det -> trajectory( meas.lhcbID().otID() );
  LineTraj measTraj = LineTraj( centrePos, dir );

  double s1, s2;
  HepVector3D distance(0.0, 0.0, 0.0);

  // Determine initial estimates of s1 and s2
  s1 = 0.0; // state is already close to the minimum
  m_poca -> minimize( measTraj, s2, refTraj.position(s1), distance, 20*mm );

  // Determine the actual minimum with the Poca tool
  m_poca -> minimize( refTraj, s1, measTraj, s2, distance, 20*mm ); 

  //TODO: temporary hack because HepVector3D * HepMatrix not possible in CLHEP!!
  //m_H =  distance.unit() * ( refTraj.derivative( s1 ) );
  HepVector3D deriv1 = distance.unit();
  HepVector deriv1tmp = HepVector(3, 0);
  deriv1tmp[0] = deriv1[0];
  deriv1tmp[1] = deriv1[1];
  deriv1tmp[2] = deriv1[2];
  m_H = ( refTraj.derivative( s1 ) ).T() * ( deriv1tmp ) ;

  // Calculate the projected OT time
  double distToWire = distance.mag() + dot(m_H, state.stateVector() - refVec );
  double sign = ( distance.x() > 0.0 ) ? 1.0 : -1.0 ;
  double distToReadout = module -> wireLength() - fabs( state.y() ) ;
  double time = m_det->driftDelay() * sign * distToWire
    + m_det->propagationDelay() * otmeastmp.ambiguity() * distToReadout;
  m_H *= sign * m_det->driftDelay();
 
  // Calculate the residual
  m_residual = meas.measure() - time;

  // TODO: use the reference trajectory again.
  computeErrorResidual( state, meas );

  return StatusCode::SUCCESS;
}

/// Initialize
StatusCode TrajOTProjector::initialize()
{
  StatusCode sc = GaudiTool::initialize();

  m_det = getDet<DeOTDetector>( m_otTrackerPath );

  m_pIMF = svc<IMagneticFieldSvc>( "MagneticFieldSvc",true );

  m_poca = tool<ITrajPoca>( "TrajPoca" );

  return sc;
}

/// Standard constructor, initializes variables
TrajOTProjector::TrajOTProjector( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : TrackProjector ( type, name , parent )
{
  declareInterface<ITrackProjector>(this);

  declareProperty( "OTGeometryPath",
                   m_otTrackerPath = DeOTDetectorLocation::Default );
}

/// Destructor
TrajOTProjector::~TrajOTProjector() {}; 
