// $Id: TrajOTProjector.cpp,v 1.3 2006-02-16 10:51:21 ebos Exp $
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

#include <numeric>

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
  OTMeasurement& otmeastmp = *( dynamic_cast<OTMeasurement*>(&meas) );
  TrackVector refVec = otmeastmp.referenceVector( state.stateVector() );
  StateTraj refTraj = StateTraj( refVec, otmeastmp.z(), bfield );

  // Get the measurement trajectory
  OTChannelID OTChan = meas.lhcbID().otID();
  DeOTModule* module = m_det -> module( OTChan );
  double stereoAngle = module -> stereoAngle();
  XYZPoint centrePos = module -> centerOfStraw( OTChan.straw() );
  double cosA = cos( stereoAngle );
  double sinA = sin( stereoAngle );
  XYZVector dir =  XYZVector( -sinA, cosA, 0. );
  //TODO : WHAT WE WANT!!!
  // Trajectory& measTraj = m_det -> trajectory( meas.lhcbID().otID() );
  const std::pair<double,double> range(-999.,999.);
  LineTraj measTraj = LineTraj( centrePos, dir, range );

  double s1, s2;
  XYZVector distance;

  // Determine initial estimates of s1 and s2
  s1 = 0.0; // state is already close to the minimum
  m_poca -> minimize( measTraj, s2, refTraj, s1, distance, 20*mm );

  // Determine the actual minimum with the Poca tool
  m_poca -> minimize( refTraj, s1, measTraj, s2, distance, 20*mm );
  ROOT::Math::SMatrix<double,1,3> vec; distance.Unit().GetCoordinates(vec.Array());  
  ROOT::Math::SMatrix<double,1,5> mat = vec * refTraj.derivative( s1 );
  m_H = mat.Row(0);

  // Calculate the projected OT time
  // ROOT::Math::SVector<double,5> dstate = state.stateVector()-refVec;
  // double distToWire = sqrt(distance.Mag2()) + std::inner_product(m_H.begin(),m_H.end(),dstate.begin(),0.0);
  double distToWire = sqrt(distance.Mag2());
  double distToReadout = measTraj.length() - fabs( refTraj.position(s1).Y() ); // FIXME: not all wires start at y=0!!!

  double t_calib = otmeastmp.measure()*otmeastmp.ambiguity();
  double t_drift = t_calib - m_det->propagationDelay()*distToReadout;
  double d_drift = t_drift*m_det->driftDelay();
  
  int ambig_track = (refTraj.position( s1 ).x()>measTraj.position(s2).x())?+1:-1;

  // Calculate the residual
  m_residual = distToWire - (ambig_track*otmeastmp.ambiguity() )*d_drift;

  // Compensate for using a Trajectory at the wire in stead of at the driftradius
  if (m_residual<0) m_H *=-1;
  
  // TODO: use the reference trajectory again.
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
