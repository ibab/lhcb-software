// Include files 
// -------------
// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// from TrackEvent
#include "Event/State.h"
#include "Event/Measurement.h"

// local
#include "TrackMasterProjector.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TrackMasterProjector
//
// 2005-03-12 : Jose Hernando, Eduardo Rodrigues
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<TrackMasterProjector>          s_factory ;
const        IToolFactory& TrackMasterProjectorFactory = s_factory ;

//=============================================================================
//  Project a state onto a measurement.
// It returns the chi squared of the projection
//=============================================================================
StatusCode TrackMasterProjector::project( const State& state, 
                                          Measurement& meas )
{
  if ( meas.type() != m_selectedMeasType) {
    StatusCode sc = selectProjector( meas.type() );
    if ( sc.isFailure() )
      return Error( "Unable to project this measurement!" );
  }

  return m_selectedProjector -> project( state, meas );  
}

//=============================================================================
// select the projector
//=============================================================================
StatusCode TrackMasterProjector::selectProjector( unsigned int type ) 
{
  if ( m_projectors.find(type) == m_projectors.end() ) {
    fatal() << "No TrackXxxProjector in TrackMasterProjector for this measurement of type = " << type << "!";
    return StatusCode::FAILURE;
  }

  m_selectedProjector = m_projectors[type];
  m_selectedMeasType  = type;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Retrieve the projection matrix H of the (last) projection
//=============================================================================
const HepVector& TrackMasterProjector::projectionMatrix() const
{
  return m_selectedProjector -> projectionMatrix();
}

//=============================================================================
// Retrieve the chi squared of the (last) projection
//=============================================================================
double TrackMasterProjector::chi2() const
{
  return m_selectedProjector -> chi2();
}

//=============================================================================
// Retrieve the residual of the (last) projection
//=============================================================================
double TrackMasterProjector::residual() const
{
  return m_selectedProjector -> residual();
}

//=============================================================================
// Retrieve the error on the residual of the (last) projection
//=============================================================================
double TrackMasterProjector::errResidual() const
{
  return m_selectedProjector -> errResidual();
}

//=============================================================================
// Initialize
//=============================================================================
StatusCode TrackMasterProjector::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() )
    return Error( "Failed to initialize!", sc );

  m_projectors[Measurement::OT]      = tool<ITrackProjector>( m_OTProjName );
  m_projectors[Measurement::ST]      = tool<ITrackProjector>( m_ITProjName );
  m_projectors[Measurement::VeloR]   = tool<ITrackProjector>( m_VeloRProjName );
  m_projectors[Measurement::VeloPhi] = tool<ITrackProjector>( m_VeloPhiProjName );

  return StatusCode::SUCCESS;
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackMasterProjector::TrackMasterProjector( const std::string& type,
                                            const std::string& name,
                                            const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<ITrackProjector>( this );

  declareProperty( "OTProjector",      m_OTProjName = "TrackOTProjector" );
  declareProperty( "ITProjector",      m_ITProjName = "TrackITProjector" );
  declareProperty( "VeloRProjector",   m_VeloRProjName = "TrackVeloRProjector" );
  declareProperty( "VeloPhiProjector", m_VeloPhiProjName = "TrackVeloPhiProjector" );

  m_projectors.clear();
  m_selectedProjector = NULL;
  m_selectedMeasType = Measurement::Unknown;
}

//=============================================================================
// Destructor
//=============================================================================
TrackMasterProjector::~TrackMasterProjector() {}; 

//=============================================================================
