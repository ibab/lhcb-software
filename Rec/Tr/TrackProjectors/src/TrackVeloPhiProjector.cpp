// $Id: TrackVeloPhiProjector.cpp,v 1.10 2006-05-19 11:58:37 dhcroft Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "TrackVeloPhiProjector.h"

// from Tr/TrackFitEvent
#include "Event/VeloPhiMeasurement.h"

using namespace Gaudi;
using namespace LHCb;

// Declaration of the Tool Factory
static const  ToolFactory<TrackVeloPhiProjector>          s_factory ;
const        IToolFactory& TrackVeloPhiProjectorFactory = s_factory ; 

//-----------------------------------------------------------------------------
/// Project a state onto a measurement
/// It returns the chi squared of the projection
//-----------------------------------------------------------------------------
StatusCode TrackVeloPhiProjector::project( const State& state,
                                           Measurement& meas )
{
  // Set refVector in case it was not set before
  if ( !meas.refIsSet() ) meas.setRefVector( state.stateVector() );

  double sum    = 0.;
  double sums   = 0.;
  double cosPhi = 0.;
  double sinPhi = 0.;

  VeloPhiMeasurement& veloPhiMeas= *(dynamic_cast<VeloPhiMeasurement*>(&meas));
  std::vector < VeloChannelID > channels = veloPhiMeas.cluster()->channels();
  std::vector< VeloChannelID >::const_iterator iChan;
  const DeVeloPhiType* phiSens=m_det->phiSensor(veloPhiMeas.cluster()->channelID().sensor());
  for( iChan = channels.begin() ; iChan !=  channels.end() ; ++iChan ) {
    double phi    =  phiSens -> trgPhiDirectionOfStrip( (*iChan).strip() );
    double adc = static_cast<double>(veloPhiMeas.cluster()->
				     adcValue(iChan-channels.begin()));
    sum    += adc;
    sums   += adc * phi ;
  }
  if ( 0 < sum ) {
    double phi = sums / sum;
    cosPhi = cos( phi );
    sinPhi = sin( phi );
  }

  m_H = TrackVector();

  m_H[0] = - sinPhi;
  m_H[1] = cosPhi;
  m_H[2] = 0.;

  m_residual = meas.measure() - ( state.y() * cosPhi - state.x() * sinPhi );

  computeErrorResidual( state, meas );

  return StatusCode::SUCCESS ; 
}

//-----------------------------------------------------------------------------
/// Initialize
//-----------------------------------------------------------------------------
StatusCode TrackVeloPhiProjector::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if( sc.isFailure() ) { return Error( "Failed to initialize!", sc ); }

  m_det = getDet<DeVelo>( m_veloPath );

  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
/// Standard constructor, initializes variables
//-----------------------------------------------------------------------------
TrackVeloPhiProjector::TrackVeloPhiProjector( const std::string& type,
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
TrackVeloPhiProjector::~TrackVeloPhiProjector() {};
