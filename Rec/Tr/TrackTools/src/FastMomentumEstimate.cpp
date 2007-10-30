
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
#include "GaudiKernel/SystemOfUnits.h" 

// from TrackEvent
#include "FastMomentumEstimate.h"
#include "Event/TrackParameters.h"
#include "Event/State.h"


//-----------------------------------------------------------------------------
// Implementation file for class : FastMomentumEstimate
//
// 2007-10-30 : S. Hansmann-Menzemer
//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( FastMomentumEstimate );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FastMomentumEstimate::FastMomentumEstimate( const std::string& type,
					    const std::string& name,
					    const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IFastMomentumEstimate>(this);

 declareProperty( "ParamsTCubic", m_paramsTCubic ); 
 declareProperty( "ParamsTParabola", m_paramsTParab );
 declareProperty( "ParamsVeloTCubic", m_paramsVeloTCubic ); 
 declareProperty( "ParamsVeloTParabola", m_paramsVeloTParab );
 declareProperty( "TResolution", m_tResolution = 0.025);
 declareProperty( "VeloPlusTResolution", m_veloPlusTResolution = 0.015);
 
};

//=============================================================================
// Destructor
//============================================================================
FastMomentumEstimate::~FastMomentumEstimate() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode FastMomentumEstimate::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;  // error already reported by base class

  if (m_paramsTParab.size() != 4     || m_paramsTCubic.size()!=4 ||
      m_paramsVeloTParab.size() !=6  || m_paramsVeloTCubic.size() !=6 )
    return StatusCode::FAILURE;


  return StatusCode::SUCCESS;
};


StatusCode FastMomentumEstimate::calculate( const LHCb::State* tState, double& qOverP, 
					    double& sigmaQOverP, bool tCubicFit ) const
{
  double p;

  double tx = tState->tx(); 
  double ty = tState->ty();
  double x0   = tState->x() - tx * tState->z();   
  
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
   
  qOverP = x0/(p*1000000);
  sigmaQOverP = m_tResolution*qOverP;

  return StatusCode::SUCCESS;
}


StatusCode FastMomentumEstimate::calculate( const LHCb::State* veloState, const LHCb::State* tState, 
					    double& qOverP, double& sigmaQOverP, 
					    bool tCubicFit ) const
{
 
  double coef;

  double txT = tState->tx();
  double txV = veloState->tx();
  double tyV = veloState->ty();

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
    
  double proj = sqrt( ( 1. + txV*txV + tyV*tyV ) / ( 1. + txV*txV ) );

  qOverP = (txV-txT)/( coef * Gaudi::Units::GeV * proj );
  sigmaQOverP = m_veloPlusTResolution*qOverP;
  
  return StatusCode::SUCCESS;
}

