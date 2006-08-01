// $Id: TrackVeloRProjector.cpp,v 1.10 2006-08-01 08:38:04 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "TrackVeloRProjector.h"

// Tr/TrackFitEvent
#include "Event/VeloRMeasurement.h"

using namespace Gaudi;
using namespace LHCb;
using namespace std;

DECLARE_TOOL_FACTORY( TrackVeloRProjector );

//-----------------------------------------------------------------------------
/// Project a state onto a measurement
/// It returns the chi squared of the projection
//-----------------------------------------------------------------------------
StatusCode TrackVeloRProjector::project( const State& state,
                                         Measurement& meas )
{
  // Set refVector in case it was not set before
  if ( !meas.refIsSet() ) meas.setRefVector( state.stateVector() );

  // Determine "phi"
  const TrackVector& ref = meas.refVector();
  double R = sqrt(ref(0)*ref(0)+ref(1)*ref(1));
  double cosPhi = ref(0)/R;
  double sinPhi = ref(1)/R;

  double phi = atan2( ref(1), ref(0) );
  assert(fabs(cosPhi-cos(phi))<0.0001);
  assert(fabs(sinPhi-sin(phi))<0.0001);

  // Calculate the projection matrix
  m_H = TrackProjectionMatrix();
  m_H(0,0) = cosPhi;
  m_H(0,1) = sinPhi;
  m_H(0,2) = m_H(0,3) = m_H(0,4) = 0.;
  
  // calculate the residual
  m_residual = meas.measure() - Vector1(m_H*state.stateVector())(0);

  computeErrorResidual( state, meas );

  return StatusCode::SUCCESS; 
}

//-----------------------------------------------------------------------------
/// Initialize
//-----------------------------------------------------------------------------
StatusCode TrackVeloRProjector::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if( sc.isFailure() ) { return Error( "Failed to initialize!", sc ); }

  m_det = getDet<DeVelo>( m_veloPath );

  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
/// Standard constructor, initializes variables
//-----------------------------------------------------------------------------
TrackVeloRProjector::TrackVeloRProjector( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
  : TrackProjector ( type, name , parent )
{
  declareInterface<ITrackProjector>(this);

  declareProperty( "VeloGeometryPath",
                   m_veloPath = DeVeloLocation::Default );
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
TrackVeloRProjector::~TrackVeloRProjector() {};
