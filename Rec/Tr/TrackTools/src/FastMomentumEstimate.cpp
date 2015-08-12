// Include files
#include <cmath>

// from Gaudi
#include "GaudiKernel/SystemOfUnits.h" 

// from TrackEvent
#include "FastMomentumEstimate.h"
#include "Event/TrackParameters.h"
#include "Event/State.h"
#include <boost/assign/list_of.hpp>

//-----------------------------------------------------------------------------
// Implementation file for class : FastMomentumEstimate
//
// 2007-10-30 : S. Hansmann-Menzemer
//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( FastMomentumEstimate )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FastMomentumEstimate::FastMomentumEstimate( const std::string& type,
                                            const std::string& name,
                                            const IInterface* parent )
: GaudiTool ( type, name , parent )
{
  
  declareInterface<ITrackMomentumEstimate>(this);
  
  std::vector<double> tmp(1, 0.0);
  declareProperty( "ParamsTCubic", m_paramsTCubic = tmp);
  declareProperty( "ParamsTParabola", m_paramsTParab = tmp);
  declareProperty( "ParamsVeloTCubic", m_paramsVeloTCubic = tmp);
  declareProperty( "ParamsVeloTParabola", m_paramsVeloTParab = tmp);
  declareProperty( "TResolution", m_tResolution = 0.025);
  declareProperty( "VeloPlusTResolution", m_veloPlusTResolution = 0.015);
}

//=============================================================================
// Destructor
//============================================================================
FastMomentumEstimate::~FastMomentumEstimate() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode FastMomentumEstimate::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;  // error already reported by base class
  
  m_magFieldSvc = svc<ILHCbMagnetSvc>( "MagneticFieldSvc", true );
  
  if ( 4 != m_paramsTParab.size() || 4 != m_paramsTCubic.size() || 
       6 != m_paramsVeloTParab.size() || 6 != m_paramsVeloTCubic.size()){
    
    m_paramsTParab.clear();
    m_paramsVeloTParab.clear();
    m_paramsTCubic.clear();
    m_paramsVeloTCubic.clear();
    
    if (m_magFieldSvc->useRealMap()){
      m_paramsTParab      = { -6.30991, -4.83533, -12.9192, 4.23025e-08 } ;
      m_paramsVeloTParab  = { 1.20812, 0.636694, -0.251334, 0.414017, 2.87247, -20.0982 };
      m_paramsTCubic      = { -6.34025, -4.85287, -12.4491, 4.25461e-08 };
      m_paramsVeloTCubic  = { 1.21352, 0.626691, -0.202483, 0.426262, 2.47057, -13.2917 };
    } else {
      m_paramsTParab      = { -6.3453, -4.77725, -14.9039, 3.13647e-08 };
      m_paramsVeloTParab  = { 1.21909, 0.627841, -0.235216, 0.433811, 2.92798, -21.3909 };
      m_paramsTCubic      = { -6.31652, -4.46153, -16.694, 2.55588e-08 };
      m_paramsVeloTCubic  = { 1.21485, 0.64199, -0.27158, 0.440325, 2.9191, -20.4831 };
    }
  }
  
  return StatusCode::SUCCESS;
}
//=============================================================================
// Calculate momentum, given T state only
//=============================================================================
StatusCode FastMomentumEstimate::calculate( const LHCb::State* tState, double& qOverP, 
                                            double& sigmaQOverP, bool tCubicFit ) const
{
  double p;
  
  const double tx = tState->tx(); 
  const double ty = tState->ty();
  const double x0 = tState->x() - tx * tState->z();   
  
  if(tCubicFit){
    p = m_paramsTCubic[0] +
      m_paramsTCubic[1] * tx * tx +
      m_paramsTCubic[2] * ty * ty +
      m_paramsTCubic[3] * x0 * x0;
  } else {
    p = m_paramsTParab[0] +
      m_paramsTParab[1] * tx * tx +
      m_paramsTParab[2] * ty * ty +
      m_paramsTParab[3] * x0 * x0;
  }
  
  const double scaleFactor = m_magFieldSvc->signedRelativeCurrent();
  const double denom = p * scaleFactor * 1e6 * (-1);
  
  if (std::abs(scaleFactor) < 1e-6){
    qOverP = 0.01/Gaudi::Units::GeV;
    sigmaQOverP = 1.0/Gaudi::Units::MeV;
  } else {
    qOverP = x0/denom;
    sigmaQOverP = m_tResolution * std::fabs(qOverP);
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Calculate momentum, given T and Velo state (for matching)
//=============================================================================
StatusCode FastMomentumEstimate::calculate( const LHCb::State* veloState, const LHCb::State* tState, 
                                            double& qOverP, double& sigmaQOverP, 
                                            bool tCubicFit ) const
{
  
  double coef;
  
  const double txT = tState->tx();
  const double txV = veloState->tx();
  const double tyV = veloState->ty();
  
  if(tCubicFit){
    coef = ( m_paramsVeloTCubic[0] +
             m_paramsVeloTCubic[1] * txT*txT +
             m_paramsVeloTCubic[2] * txT*txT*txT*txT +
             m_paramsVeloTCubic[3] * txT * txV +
             m_paramsVeloTCubic[4] * tyV*tyV +
             m_paramsVeloTCubic[5] * tyV*tyV*tyV*tyV );
  } else {
    coef = ( m_paramsVeloTParab[0] +
             m_paramsVeloTParab[1] * txT*txT +
             m_paramsVeloTParab[2] * txT*txT*txT*txT +
             m_paramsVeloTParab[3] * txT * txV +
             m_paramsVeloTParab[4] * tyV*tyV +
             m_paramsVeloTParab[5] * tyV*tyV*tyV*tyV );
  }
  
  const double proj = sqrt( ( 1. + txV*txV + tyV*tyV ) / ( 1. + txV*txV ) );

  const double scaleFactor = m_magFieldSvc->signedRelativeCurrent();

  if (std::abs(scaleFactor) < 1e-6){
    qOverP = 1.0/Gaudi::Units::GeV;
    sigmaQOverP = 1.0/Gaudi::Units::MeV;
  } else {
    qOverP = (txV-txT)/( coef * Gaudi::Units::GeV * proj*scaleFactor);
    sigmaQOverP = m_veloPlusTResolution * std::fabs(qOverP);
  }
  
  
  return StatusCode::SUCCESS;
}

