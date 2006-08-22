// $Id: StateElectronEnergyCorrectionTool.cpp,v 1.1 2006-08-22 12:37:51 erodrigu Exp $
// Include files
// -------------
// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// from GSL
#include "gsl/gsl_math.h"

// from DetDesc
#include "DetDesc/Material.h"

// local
#include "StateElectronEnergyCorrectionTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : StateElectronEnergyCorrectionTool
//
// 2006-08-18 : Eduardo Rodrigues
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( StateElectronEnergyCorrectionTool );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
StateElectronEnergyCorrectionTool::StateElectronEnergyCorrectionTool( const std::string& type,
                                                                      const std::string& name,
                                                                      const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IStateCorrectionTool>(this);
  
  declareProperty( "MaximumRadLength", m_maxRadLength  = 10. );
}

//=============================================================================
// Correct a State for electron dE/dx energy losses
//=============================================================================
void StateElectronEnergyCorrectionTool::correctState( LHCb::State& state,
                                                      const Material* material,
                                                      double wallThickness,
                                                      bool upstream )
{
  //hard energy loss for electrons
  double t;
  double radLength = material -> radiationLength();
  double norm      = sqrt( 1. + gsl_pow_2(state.tx()) + gsl_pow_2(state.ty()) );

  upstream ?  t = radLength*norm : t = -radLength*norm;

  // protect against t too big
  if ( fabs(t) > m_maxRadLength )  t = GSL_SIGN(t) * m_maxRadLength;

  // apply correction
  Gaudi::TrackVector&    tX = state.stateVector();
  Gaudi::TrackSymMatrix& tC = state.covariance();

  tC(4,4) += gsl_pow_2(tX[4]) * ( exp(-t*log(3.0)/log(2.0))-exp(-2.0*t) );
  tX[4]   *= exp(-t);
}

//=============================================================================
// Destructor
//=============================================================================
StateElectronEnergyCorrectionTool::~StateElectronEnergyCorrectionTool() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode StateElectronEnergyCorrectionTool::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;  // error already reported by base class
  
  return StatusCode::SUCCESS;
}

//=============================================================================
