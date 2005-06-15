// Include files 
// -------------
// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "TrackITProjector.h"

#include "STDet/STDetectionLayer.h"

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
StatusCode TrackITProjector::project( const State& state,
                                      Measurement& meas )
{

  debug()
    << " Project state:"<< endreq
    << "  - position = " << state.position() << " mm" << endreq
    << "  - momentum = " << state.momentum() << " MeV" << endreq
    << "  - P        = " << state.p() << " MeV" << endreq
    << " and measurement:"<< endreq
    << "  - z        = " << meas.z() << " mm" << endreq 
    << "  - measure  = " << meas.measure()  << endreq
    << "  - type     = " << meas.type()  << endreq
    << "  - LHCbID   = " << meas.lhcbID()  << endreq
    << "  - ch. ID   = " << meas.lhcbID().stID().channelID() << endreq;
  
  ITChannelID ITChan = meas.lhcbID().stID();
  debug()  << "  - ITChan  = " << ITChan << endreq
           << " station    = " << ITChan.station() << endreq
           << " layer      = " << ITChan.layer() << endreq
           << " wafer      = " << ITChan.wafer() << endreq
           << " strip      = " << ITChan.strip() << endreq;

  const STDetectionLayer* ITLay = m_det -> layer( ITChan );

  double stereoAngle  = ITLay->stereoAngle();

  debug() << "stereoAngle = " << stereoAngle << endreq;

  unsigned int n = state.nParameters();
  m_H = HepVector(n,0);
  m_H[0] = cos( stereoAngle );
  m_H[1] = sin( stereoAngle );

  debug() << "H = " << m_H << endreq;

  // equivalent to computeResidual(state,meas);
  m_residual = meas.measure()
               - state.x() * cos( stereoAngle )
               - state.y() * sin( stereoAngle );

  debug() << "residual = " << m_residual << endreq;

  computeErrorResidual( state, meas );

  debug() << "error residual = " << m_errResidual << endreq;
  
  return StatusCode::SUCCESS; 

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
