// $Id: TrackSTProjector.cpp,v 1.3 2006-02-27 19:56:04 jvantilb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "TrackSTProjector.h"

#include "STDet/DeSTLayer.h"

using namespace Gaudi;
using namespace LHCb;

// Declaration of the Tool Factory
static const  ToolFactory<TrackSTProjector>          s_factory ;
const        IToolFactory& TrackSTProjectorFactory = s_factory ; 

//-----------------------------------------------------------------------------
/// Project a state onto a measurement
/// It returns the chi squared of the projection
//-----------------------------------------------------------------------------
StatusCode TrackSTProjector::project( const State& state,
                                      Measurement& meas )
{
  STChannelID stChan = meas.lhcbID().stID();

  DeSTDetector* mDet;
  if( stChan.isTT() ) { mDet = m_ttdet; }
  else { mDet = m_itdet; }

  DeSTLayer* stLayer = mDet -> findLayer( stChan );

  if ( stLayer == 0 ) 
    return Error( "Could't find layer from STChannelID.", StatusCode::FAILURE);

  // StereoAngle
  double stereoAngle = stLayer->angle() ;

  m_H = TrackVector();
  m_H[0] = cos( stereoAngle );
  m_H[1] = sin( stereoAngle );

  // equivalent to computeResidual(state,meas);
  m_residual = meas.measure()
               - state.x() * cos( stereoAngle )
               - state.y() * sin( stereoAngle );

  computeErrorResidual( state, meas );

  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
/// Initialize
//-----------------------------------------------------------------------------
StatusCode TrackSTProjector::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if( sc.isFailure() ) { return Error( "Failed to initialize!", sc ); }

  m_ttdet = getDet<DeSTDetector>( m_ttTrackerPath );
  m_itdet = getDet<DeSTDetector>( m_itTrackerPath );

  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
/// Standard constructor, initializes variables
//-----------------------------------------------------------------------------
TrackSTProjector::TrackSTProjector( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : TrackProjector ( type, name , parent )
{
  declareInterface<ITrackProjector>(this);

  declareProperty( "TTGeometryPath",
                   m_ttTrackerPath = DeSTDetLocation::TT );

  declareProperty( "ITGeometryPath",
                   m_itTrackerPath = DeSTDetLocation::IT );
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
TrackSTProjector::~TrackSTProjector() {};
