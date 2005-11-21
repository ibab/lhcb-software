// Include files 
// -------------
// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "TrackVeloPhiProjector.h"
#include "Event/VeloPhiMeasurement.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TrackVeloPhiProjector
//
// 2005-04-13 : Jose Hernando, Eduardo Rodrigues
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<TrackVeloPhiProjector>          s_factory ;
const        IToolFactory& TrackVeloPhiProjectorFactory = s_factory ; 

//=============================================================================
//  Project a state onto a measurement.
// It returns the chi squared of the projection
//=============================================================================
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

  int sensor = veloPhiMeas.cluster() -> sensor();

  double sum    = 0.;
  double sum2   = 0.;
  double sums   = 0.;
  double second = 0.;
  double phi    = -999.;
  double cosPhi = 0.;
  double sinPhi = 0.;

  std::vector< std::pair<long,double> > sign = 
    veloPhiMeas.cluster()->stripSignals();
  std::vector< std::pair<long,double> >::const_iterator strIt;
  int strip    = (*sign.begin()).first;
  VeloChannelID channel(sensor,strip);
  for ( strIt = sign.begin() ; sign.end() != strIt ; strIt++ ) {
    strip  = (*strIt).first;
    phi    =  m_det -> trgPhiDirectionOfStrip( channel );
    second = (*strIt).second;
    sum    += second;
    sum2   += second * second;
    sums   += second * phi ;
  }
  if ( 0 < sum ) {
    double phi = sums / sum;
    cosPhi = cos( phi );
    sinPhi = sin( phi );
  }

  unsigned int n = state.nParameters();
  m_H = HepVector(n,0);

  m_H[0] = - sinPhi;
  m_H[1] = cosPhi;
  m_H[2] = 0.;

  m_residual = meas.measure() - ( y * cosPhi - x * sinPhi );

  computeErrorResidual( state, meas );

  return StatusCode::SUCCESS ; 
}

//=============================================================================
// Initialize
//=============================================================================
StatusCode TrackVeloPhiProjector::initialize()
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
TrackVeloPhiProjector::TrackVeloPhiProjector( const std::string& type,
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
TrackVeloPhiProjector::~TrackVeloPhiProjector() {}; 

//=============================================================================
