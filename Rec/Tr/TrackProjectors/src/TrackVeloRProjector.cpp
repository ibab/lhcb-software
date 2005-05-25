// Include files 
// -------------
// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "TrackVeloRProjector.h"
#include "Event/VeloRMeasurement.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TrackVeloRProjector
//
// 2005-04-13 : Jose Hernando, Eduardo Rodrigues
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<TrackVeloRProjector>          s_factory ;
const        IToolFactory& TrackVeloRProjectorFactory = s_factory ; 

//=============================================================================
//  Project a state onto a measurement.
// It returns the chi squared of the projection
//=============================================================================
StatusCode TrackVeloRProjector::project( const State& state,
                                         Measurement& meas )
{
  double h = 0;

  double x  = state.x();
  double y  = state.y();

  VeloRMeasurement& veloRMeas = *( dynamic_cast<VeloRMeasurement*>(&meas) );

  double phi = veloRMeas.phi();

  unsigned int n = state.nParameters();
  m_H = HepVector(n,0);
  
  // calculate h (predicted R)
  if ( phi > 990.0 ) {
    h      = sqrt( x*x + y*y );
    m_H[0] = x / h;
    m_H[1] = y / h;
  }
  else {
    h = x * cos( phi ) + y * sin( phi );
    m_H[0] = cos( phi );
    m_H[1] = sin( phi );
  }
  m_H[2] = 0.;
  m_H[3] = 0.;

  m_residual = meas.measure() - h;

  computeErrorResidual( state, meas );

  return StatusCode::SUCCESS; 
}

//=============================================================================
// Initialize
//=============================================================================
StatusCode TrackVeloRProjector::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() )
    return Error( "Failed to initialize!", sc );

  m_det = getDet<DeVelo>( m_veloPath );

  return StatusCode::SUCCESS;
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackVeloRProjector::TrackVeloRProjector( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
  : TrackProjector ( type, name , parent )
{
  declareInterface<ITrackProjector>(this);

  declareProperty( "VeloGeometryPath",
                   m_veloPath = "/dd/Structure/LHCb/Velo" );
}
//=============================================================================
// Destructor
//=============================================================================
TrackVeloRProjector::~TrackVeloRProjector() {}; 

//=============================================================================
