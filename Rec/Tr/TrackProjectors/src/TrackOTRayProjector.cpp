// Include files 
// -------------
// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// from GSL
#include "gsl/gsl_math.h"

// local
#include "TrackOTRayProjector.h"
#include "Event/OTMeasurement.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TrackOTRayProjector
//
// 2005-04-08 : Jose Hernando, Eduardo Rodrigues
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<TrackOTRayProjector>          s_factory ;
const        IToolFactory& TrackOTRayProjectorFactory = s_factory ; 

//=============================================================================
//  Project a state onto a measurement.
// It returns the chi squared of the projection
//=============================================================================
StatusCode TrackOTRayProjector::project( const State& state,
                                         Measurement& meas )
{
  double x  = state.x();
  double y  = state.y();
  double z  = state.z();
  double tx = state.tx();
  double ty = state.ty();

  EVector x1 = EVector(x,y,z);
  EVector v1 = EVector(tx,ty,1.);

  OTChannelID OTChan = meas.lhcbID().otID();
  DeOTModule* module = m_det -> module( OTChan );
  HepPoint3D VwirePos = module->centerOfStraw( OTChan.straw() );
  double stereoAngle = module -> stereoAngle();
  double driftVelocity = m_det -> driftDelay(); // ns/mm
  
  EVector x2 = EVector(VwirePos.x(),VwirePos.y(),VwirePos.z());
  EVector w = EVector(sin(stereoAngle),cos(stereoAngle),0.);
  
  EVector dis = m_intersector.distance(x1,v1,x2,w);
  double dd = dis.mag();
  
  double hx = m_intersector.ddistance(EVector(1,0,0),EVector(0,0,0));
  double hy = m_intersector.ddistance(EVector(0,1,0),EVector(0,0,0));
  double htx = m_intersector.ddistance(EVector(0,0,0),EVector(1,0,0));
  double hty = m_intersector.ddistance(EVector(0,0,0),EVector(0,1,0));
  unsigned int n = state.nParameters();
  m_H = HepVector(n,0);
  m_H[0] = hx;
  m_H[1] = hy;
  m_H[2] = htx;
  m_H[3] = hty;

  double time = dd/driftVelocity;
  
  m_residual = fabs(meas.measure())-time;

  computeErrorResidual( state, meas );  

  return StatusCode::SUCCESS; 
}

//=============================================================================
// Initialize
//=============================================================================
StatusCode TrackOTRayProjector::initialize()
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
TrackOTRayProjector::TrackOTRayProjector( const std::string& type,
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
TrackOTRayProjector::~TrackOTRayProjector() {}; 

//=============================================================================
