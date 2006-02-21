// $Id: TrajSTProjector.cpp,v 1.1 2006-02-21 18:28:25 jvantilb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// from GaudiKernel
#include "GaudiKernel/IMagneticFieldSvc.h"

// from LHCbKernel
#include "Kernel/ISTClusterPosition.h"

// from STDet
#include "STDet/DeSTLayer.h"

// from TrackFitEvent
#include "Event/StateTraj.h"
#include "Event/STMeasurement.h"

// from TrackInterfaces
#include "TrackInterfaces/ITrajPoca.h"

// from GSL
#include "gsl/gsl_math.h"

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
  TrackVector refVec = state.stateVector() ; // TODO: Implement ref. Vector
  StateTraj refTraj = StateTraj( refVec, meas.z(), bfield ) ; 

  // Get the correct DetectorElement (TT or IT)
  STChannelID STChan = meas.lhcbID().stID();
  DeSTDetector* mDet;
  if( STChan.isTT() ) { mDet = m_ttdet; }
  else { mDet = m_itdet; }

  // Get the measurement trajectory representing the centre of gravity
  STMeasurement& stmeas = *( dynamic_cast<STMeasurement*>(&meas) );
  ISTClusterPosition::Measurement measVal =
    m_stPositionTool -> estimate( stmeas.cluster() );
  Trajectory* measTraj = mDet -> trajectory( meas.lhcbID(), 
                                             measVal.first.second ) ;

  double s1, s2;
  XYZVector distance;

  // Determine initial estimates of s1 and s2
  s1 = 0.0; // Assume state is already close to the minimum
  s2 = measTraj->arclength( refTraj.position(s1) );

  // Determine the actual minimum with the Poca tool
  m_poca -> minimize( refTraj, s1, *measTraj, s2, distance, 20*mm );

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

  // Calculate the error on the measurement
  computeErrorResidual( state, meas );

  // We have to delete the pointer the the Measurement Trajectory
  delete measTraj;
 
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
  m_stPositionTool = tool<ISTClusterPosition>( "STOfflinePosition" );

  m_ttdet = getDet<DeSTDetector>( m_ttTrackerPath );
  m_itdet = getDet<DeSTDetector>( m_itTrackerPath );

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

  declareProperty( "TTGeometryPath",
                   m_ttTrackerPath = DeSTDetLocation::TT );

  declareProperty( "ITGeometryPath",
                   m_itTrackerPath = DeSTDetLocation::IT );
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
TrajSTProjector::~TrajSTProjector() {};
