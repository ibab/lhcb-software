// $Id: ITTrackProjector.cpp,v 1.1 2005-04-08 15:45:46 erodrigu Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "TrackProjectors/ITTrackProjector.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ITTrackProjector
//
// 2005-04-08 : Jose Hernando, Eduardo Rodrigues
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<ITTrackProjector>          s_factory ;
const        IToolFactory& ITTrackProjectorFactory = s_factory ; 

//=============================================================================
//  Project a state onto a measurement.
// It returns the chi squared of the projection
//=============================================================================
double ITTrackProjector::project( const State& state,
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
               - state.x() * cos(stereoAngle) + state.y() * sin(stereoAngle);

  computeErrorResidual(state,meas);
  
  return chi2();

}

//=============================================================================
// Initialize
//=============================================================================
StatusCode ITTrackProjector::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() )
    return Error( "Failed to initialize!", sc );

  m_det = getDet<DeSTDetector>( "DeSTDetectorLocation::Default" );

  return StatusCode::SUCCESS;
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ITTrackProjector::ITTrackProjector( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : TrackProjector ( type, name , parent )
{
  declareInterface<ITrackProjector>(this);

}

//=============================================================================
// Destructor
//=============================================================================
ITTrackProjector::~ITTrackProjector() {}; 

//=============================================================================
