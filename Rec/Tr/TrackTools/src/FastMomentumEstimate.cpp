
// Include files
#include <cmath>

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
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
  
  declareProperty( "ParamsTCubic", m_paramsTCubic 
		   = boost::assign::list_of(-6.3453)(-4.77725)(-14.9039)(3.13647e-08)); 
  declareProperty( "ParamsTParabola", m_paramsTParab  
		   = boost::assign::list_of(-6.31652)(-4.46153)(-16.694)(2.55588e-08));
  declareProperty( "ParamsVeloTCubic", m_paramsVeloTCubic 
		   = boost::assign::list_of(1.21909)(0.627841)(-0.235216)(0.433811)
		   (2.92798)(-21.3909) ); 
  declareProperty( "ParamsVeloTParabola", m_paramsVeloTParab 
		   = boost::assign::list_of(1.21485)(0.64199)(-0.27158)(0.440325)
		   (2.9191)(-20.4831));
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

  if (m_paramsTParab.size()!=4)     return Error ( "Not enough ParamsTParabola values" ); 
  if (m_paramsTCubic.size()!=4)     return Error ( "Not enough ParamsTCubic values" );  
  if (m_paramsVeloTParab.size()!=6) return Error ( "Not enough ParamsVeloTParabola values" ); 
  if (m_paramsVeloTCubic.size()!=6) return Error ( "Not enough ParamsVeloTCubic values" );
  
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
  sigmaQOverP = m_tResolution * std::fabs(qOverP);

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
  sigmaQOverP = m_veloPlusTResolution * std::fabs(qOverP);
  
  return StatusCode::SUCCESS;
}

