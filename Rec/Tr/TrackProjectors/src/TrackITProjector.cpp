// Include files 
// -------------
// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "TrackProjectors/TrackITProjector.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TrackITProjector
//
// 2005-04-08 : Jose Hernando, Eduardo Rodrigues
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<TrackITProjector>          s_factory ;
const        IToolFactory& TrackITProjectorFactory = s_factory ; 

//=============================================================================
//  Project a state onto a measurement.
// It returns the chi squared of the projection
//=============================================================================
double TrackITProjector::project( const State& state,
                                  Measurement& meas )
{
  ITChannelID ITChan = meas.lhcbID().stID();
  const STDetectionLayer* ITLay = m_det -> layer( ITChan );
  double stereoAngle  = ITLay->stereoAngle();

  unsigned int n = state.nParameters();
  m_H = HepVector(n,0);
  m_H[0] = cos( stereoAngle );
  m_H[1] = sin( stereoAngle );

  // equivalent to computeResidual(state,meas);
  m_residual = meas.measure()
               - state.x() * cos( stereoAngle )
               - state.y() * sin( stereoAngle );

  computeErrorResidual( state, meas );
  
  return chi2();

}

//=============================================================================
// Initialize
//=============================================================================
StatusCode TrackITProjector::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() )
    return Error( "Failed to initialize!", sc );

  m_det = getDet<DeSTDetector>( m_itTrackerPath );

  return StatusCode::SUCCESS;
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackITProjector::TrackITProjector( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : TrackProjector ( type, name , parent )
{
  declareInterface<ITrackProjector>(this);

  declareProperty( "ITGeometryPath",
                   m_itTrackerPath = DeSTDetectorLocation::Default );
}

//=============================================================================
// Destructor
//=============================================================================
TrackITProjector::~TrackITProjector() {}; 

//=============================================================================
