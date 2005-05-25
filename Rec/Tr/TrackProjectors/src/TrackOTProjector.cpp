// Include files 
// -------------
// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "TrackOTProjector.h"
#include "Event/OTMeasurement.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TrackOTProjector
//
// 2005-04-08 : Jose Hernando, Eduardo Rodrigues
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<TrackOTProjector>          s_factory ;
const        IToolFactory& TrackOTProjectorFactory = s_factory ; 

//=============================================================================
//  Project a state onto a measurement.
// It returns the chi squared of the projection
//=============================================================================
StatusCode TrackOTProjector::project( const State& state,
                                  Measurement& meas )
{
  double x  = state.x();
  double y  = state.y();
  double tx = state.tx();
  double ty = state.ty();

  OTMeasurement& otmeas = *( dynamic_cast<OTMeasurement*>(&meas) );

  OTChannelID OTChan = meas.lhcbID().otID();
  DeOTModule* module = m_det -> module( OTChan );
  double stereoAngle = module -> stereoAngle();

  double driftVelocity = m_det -> driftDelay(); // ns/mm
  double wireVelocity  = m_det -> propagationDelay();      // ns/mm

  HepPoint3D VwirePos = module->centerOfStraw( OTChan.straw() );
  double wirePos      =  VwirePos.x() * cos(stereoAngle) 
                         + VwirePos.y() * sin(stereoAngle);

  double wireLength = module -> wireLength();

  double cosA = cos( stereoAngle );
  double sinA = sin( stereoAngle );
  double tu = ( otmeas.tu() > 990.0 ) ? (tx * cosA + ty * sinA) : tu = otmeas.tu();
  double cosU     = 1./sqrt( 1.+ tu*tu );
  double du       = (x * cosA + y * sinA - wirePos) * driftVelocity;
  double wireDist = ( x * cosA + y * sinA - wirePos ) * cosU;
  double time     = driftVelocity * wireDist
                    + wireVelocity * otmeas.ambiguity() * ( wireLength - fabs(y) );

  unsigned int n = state.nParameters();
  m_H = HepVector(n,0);  
  m_H[0] = cosA * cosU * driftVelocity;
  m_H[1] = sinA * cosU * driftVelocity
           - otmeas.ambiguity() * wireVelocity * y/fabs(y);
  if ( tu > 990.0 ) {
    m_H[2] = -du * tu * pow( cosU, 3) * cosA;
    m_H[3] = -du * tu * pow( cosU, 3) * sinA;
  }
  else {
    m_H[2] = 0.;
    m_H[3] = 0.;
  }

  // this shouls be ~ equivalent to : computeResidual(state,meas);
  m_residual = meas.measure() - time;

  computeErrorResidual( state, meas );  

  return StatusCode::SUCCESS; 
}

//=============================================================================
// Initialize
//=============================================================================
StatusCode TrackOTProjector::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() )
    return Error( "Failed to initialize!", sc );

  m_det = getDet<DeOTDetector>( m_otTrackerPath );

  return StatusCode::SUCCESS;
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackOTProjector::TrackOTProjector( const std::string& type,
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
TrackOTProjector::~TrackOTProjector() {}; 

//=============================================================================
