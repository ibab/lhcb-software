// $Id: TrajOTProjector.cpp,v 1.1 2005-12-14 11:04:33 erodrigu Exp $
// Include files 
// -------------
// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// from GaudiKernel
#include "GaudiKernel/IMagneticFieldSvc.h"

// from OTDet
#include"OTDet/DeOTDetector.h"

// from TrackFitEvent
#include "Event/OTMeasurement.h"
#include "Event/LineTraj.h"
#include "Event/StateTraj.h"

// from TrackInterfaces
#include "TrackInterfaces/ITrajPoca.h"

// local
#include "TrajOTProjector.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TrajOTProjector
//
// 2005-11-24 : Edwin Bos, Jeroen van Tilburg, Eduardo Rodrigues
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<TrajOTProjector>          s_factory ;
const        IToolFactory& TrajOTProjectorFactory = s_factory ;

//=============================================================================
//  Project a state onto a measurement.
// It returns the chi squared of the projection
//=============================================================================
StatusCode TrajOTProjector::project( const State& state,
                                     Measurement& meas )
{
  HepVector3D bfield;
  m_pIMF -> fieldVector( state.position(), bfield );

  // TODO: Give the reference trajectory
  StateTraj stateTraj = StateTraj( state, bfield );

  OTChannelID OTChan   = meas.lhcbID().otID();
  DeOTModule* module   = m_det -> module( OTChan );
  double stereoAngle   = module -> stereoAngle();
  HepPoint3D centrePos = module -> centerOfStraw( OTChan.straw() );
  double cosA = cos( stereoAngle );
  double sinA = sin( stereoAngle );
  HepVector3D dir =  HepVector3D( sinA, cosA, 0. );

  //TODO : WHAT WE WANT!!!
  // LineTraj& measTraj = m_det -> trajectory( meas.lhcbID().otID() );

  LineTraj measTraj = LineTraj( centrePos, dir );

  //unsigned int n = state.nParameters();
  //m_H = HepVector(n,0);

  double s1, s2;
  HepVector3D distance;
  m_poca -> minimize( stateTraj, measTraj, s1, s2, distance ); 

  // TODO: temparary hack because HepVector3D * HepMatrix not possible in CLHEP!!!
  //m_H = ( m_poca -> derivative1() ) * ( stateTraj.derivative( s1 ) );
  HepVector3D deriv1 = m_poca -> derivative1();
  HepVector deriv1tmp = HepVector(3, 0);
  deriv1tmp[0] = deriv1[0];
  deriv1tmp[1] = deriv1[1];
  deriv1tmp[2] = deriv1[2];
  m_H = ( deriv1tmp ) * ( stateTraj.derivative( s1 ) );

  // TODO: use the reference trajectory again.
  double driftVelocity = m_det -> driftDelay(); // ns/mm
  double wireVelocity  = m_det -> propagationDelay();      // ns/mm
  double wireLength    = module -> wireLength();
  double y  = state.y();
  double wireDist = distance.mag();
  OTMeasurement& otmeas = *( dynamic_cast<OTMeasurement*>(&meas) );
  double time     = driftVelocity * wireDist
                    + wireVelocity * otmeas.ambiguity() * ( wireLength - fabs(y) );

  m_residual = meas.measure() - time;
  // TODO: use the reference trajectory again.
  computeErrorResidual( state, meas );

  return StatusCode::SUCCESS;
}

//=============================================================================
// Initialize
//=============================================================================
StatusCode TrajOTProjector::initialize()
{
  StatusCode sc = GaudiTool::initialize();

  m_det = getDet<DeOTDetector>( m_otTrackerPath );

  m_pIMF = svc<IMagneticFieldSvc>( "MagneticFieldSvc",true );

  m_poca = tool<ITrajPoca>( "TrajPoca" );

  return sc;
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrajOTProjector::TrajOTProjector( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : TrackProjector ( type, name , parent )
{
  declareInterface<ITrackProjector>(this);

  declareProperty( "OTGeometryPath",
                   m_otTrackerPath = DeOTDetectorLocation::Default );
}
//=============================================================================
// Destructor
//=============================================================================
TrajOTProjector::~TrajOTProjector() {}; 

//=============================================================================
