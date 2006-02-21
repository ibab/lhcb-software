// $Id: TrackVeloPhiProjector.cpp,v 1.6 2006-02-21 12:25:41 dhcroft Exp $
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
  double x  = state.x();
  double y  = state.y();

  VeloPhiMeasurement& veloPhiMeas = *(dynamic_cast<VeloPhiMeasurement*>(&meas));

  // Set r in case it was not set before (= unphysical value)
  if ( veloPhiMeas.r() <= 0.0 ) {
    veloPhiMeas.setR( sqrt( x*x + y*y ) );
  }  

  double sum    = 0.;
  double sums   = 0.;
  double cosPhi = 0.;
  double sinPhi = 0.;

  std::vector < VeloChannelID > channels = veloPhiMeas.cluster()->channels();
  std::vector< VeloChannelID >::const_iterator iChan;
  for( iChan = channels.begin() ; iChan !=  channels.end() ; ++iChan ) {
    double phi    =  m_det -> trgPhiDirectionOfStrip( *iChan );
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

  m_residual = meas.measure() - ( y * cosPhi - x * sinPhi );

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
                   m_veloPath = "/dd/Structure/LHCb/Velo" );
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
TrackVeloPhiProjector::~TrackVeloPhiProjector() {};
