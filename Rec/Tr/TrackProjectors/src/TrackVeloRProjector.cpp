// $Id: TrackVeloRProjector.cpp,v 1.8 2006-05-19 11:58:37 dhcroft Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "TrackVeloRProjector.h"

// Tr/TrackFitEvent
#include "Event/VeloRMeasurement.h"

using namespace Gaudi;
using namespace LHCb;

// Declaration of the Tool Factory
static const  ToolFactory<TrackVeloRProjector>          s_factory ;
const        IToolFactory& TrackVeloRProjectorFactory = s_factory ; 

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
  const TrackVector& refVector = meas.refVector();
  double phi = atan2( refVector(1), refVector(0) );;

  // Calculate the projection matrix
  m_H = TrackVector();
  m_H[0] = cos( phi );
  m_H[1] = sin( phi );
  m_H[2] = 0.;
  m_H[3] = 0.;
  
  // calculate the residual
  m_residual = meas.measure() - (state.x() * cos(phi) + state.y() * sin(phi));

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
