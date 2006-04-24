// $Id: TrackMasterProjector.cpp,v 1.10 2006-04-24 14:28:53 erodrigu Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "TrackMasterProjector.h"

using namespace Gaudi;
using namespace LHCb;

// Declaration of the Tool Factory
static const  ToolFactory<TrackMasterProjector>          s_factory ;
const        IToolFactory& TrackMasterProjectorFactory = s_factory ;

//-----------------------------------------------------------------------------
/// Project a state onto a measurement.
/// It returns the chi squared of the projection
//-----------------------------------------------------------------------------
StatusCode TrackMasterProjector::project( const State& state, 
                                          Measurement& meas )
{
  if( meas.type() != m_selectedMeasType) {
    StatusCode sc = selectProjector( meas.type() );
    if( sc.isFailure() )
      return Error( "Unable to project this measurement!" );
  }
  
  return m_selectedProjector -> project( state, meas );  
}

//-----------------------------------------------------------------------------
/// select the projector
//-----------------------------------------------------------------------------
StatusCode TrackMasterProjector::selectProjector( unsigned int type ) 
{
  if ( m_projectors.find(type) == m_projectors.end() ) {
    fatal() << "No TrackXxxProjector in TrackMasterProjector for " <<
               "this measurement of type = " << type << "!";
    return StatusCode::FAILURE;
  }

  m_selectedProjector = m_projectors[type];
  m_selectedMeasType  = type;

  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
/// Retrieve the projection matrix H of the (last) projection
//-----------------------------------------------------------------------------
const TrackVector& TrackMasterProjector::projectionMatrix() const
{
  return m_selectedProjector -> projectionMatrix();
}

//-----------------------------------------------------------------------------
/// Retrieve the chi squared of the (last) projection
//-----------------------------------------------------------------------------
double TrackMasterProjector::chi2() const
{
  return m_selectedProjector -> chi2();
}

//-----------------------------------------------------------------------------
/// Retrieve the residual of the (last) projection
//-----------------------------------------------------------------------------
double TrackMasterProjector::residual() const
{
  return m_selectedProjector -> residual();
}

//-----------------------------------------------------------------------------
/// Retrieve the error on the residual of the (last) projection
//-----------------------------------------------------------------------------
double TrackMasterProjector::errResidual() const
{
  return m_selectedProjector -> errResidual();
}

/// Retrieve the error on the measure of the (last) projection
//-----------------------------------------------------------------------------
double TrackMasterProjector::errMeasure() const
{
  return m_selectedProjector -> errMeasure();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/// Initialize
//-----------------------------------------------------------------------------
StatusCode TrackMasterProjector::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() )
    return Error( "Failed to initialize!", sc );

  m_projectors[Measurement::TT]      = tool<ITrackProjector>( m_STProjName );
  m_projectors[Measurement::IT]      = tool<ITrackProjector>( m_STProjName );
  m_projectors[Measurement::OT]      = tool<ITrackProjector>( m_OTProjName );
  m_projectors[Measurement::VeloR]   = tool<ITrackProjector>( m_VeloRProjName );
  m_projectors[Measurement::VeloPhi] = tool<ITrackProjector>( m_VeloPhiProjName );

  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
/// Standard constructor, initializes variables
//-----------------------------------------------------------------------------
TrackMasterProjector::TrackMasterProjector( const std::string& type,
                                            const std::string& name,
                                            const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<ITrackProjector>( this );

  declareProperty( "OTProjector",      m_OTProjName = "TrajOTProjector" );
  declareProperty( "STProjector",      m_STProjName = "TrajSTProjector" );
  declareProperty( "VeloRProjector",   m_VeloRProjName = "TrajVeloRProjector" );
  declareProperty( "VeloPhiProjector", m_VeloPhiProjName = "TrajVeloPhiProjector" );

  m_projectors.clear();
  m_selectedProjector = NULL;
  m_selectedMeasType = Measurement::Unknown;
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
TrackMasterProjector::~TrackMasterProjector() {};
