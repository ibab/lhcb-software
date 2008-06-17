// $Id: StateSimpleBetheBlochEnergyCorrectionTool.cpp,v 1.5 2008-06-17 15:10:32 lnicolas Exp $
// Include files
// -------------
// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// from GSL
#include "gsl/gsl_math.h"

// from DetDesc
#include "DetDesc/Material.h"

// local
#include "StateSimpleBetheBlochEnergyCorrectionTool.h"


#include "LHCbMath/LHCbMath.h"

using namespace Gaudi::Units;

//-----------------------------------------------------------------------------
// Implementation file for class : StateSimpleBetheBlochEnergyCorrectionTool
//
// 2006-08-18 : Eduardo Rodrigues
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( StateSimpleBetheBlochEnergyCorrectionTool );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
StateSimpleBetheBlochEnergyCorrectionTool::StateSimpleBetheBlochEnergyCorrectionTool( const std::string& type,
                                                                                      const std::string& name,
                                                                                      const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IStateCorrectionTool>(this);
  
  declareProperty( "EnergyLossFactor",  m_energyLossCorr = 354.1 * MeV*mm2/mole );
  declareProperty( "MaximumEnergyLoss", m_maxEnergyLoss  = 100. * MeV           );
}

//=============================================================================
// Correct a State for dE/dx energy losses
//=============================================================================
void StateSimpleBetheBlochEnergyCorrectionTool::correctState( LHCb::State& state,
                                                              const Material* material,
                                                              double wallThickness,
                                                              bool upstream, 
                                                              LHCb::ParticleID ) const
{
  double bbLoss = wallThickness
    * sqrt( 1. + gsl_pow_2(state.tx()) + gsl_pow_2(state.ty()) )
    * m_energyLossCorr * material->Z() * material->density() / material->A();
  bbLoss = GSL_MIN( m_maxEnergyLoss, bbLoss );
  if ( !upstream ) bbLoss *= -1.;
  
  // apply correction - note for now only correct the state vector
  Gaudi::TrackVector& tX = state.stateVector();

  double qOverP;
  tX[4] < 0 ? qOverP = GSL_MIN(tX[4], LHCb::Math::looseTolerance ) :
              qOverP = GSL_MAX(tX[4], LHCb::Math::looseTolerance);  
  
  double newP;
  qOverP > 0.0 ? newP = GSL_MAX(1.0/qOverP + bbLoss, LHCb::Math::looseTolerance) :
                 newP = GSL_MIN(1.0/qOverP - bbLoss, LHCb::Math::looseTolerance);

  tX[4] = 1/newP;
}

//=============================================================================
// Destructor
//=============================================================================
StateSimpleBetheBlochEnergyCorrectionTool::~StateSimpleBetheBlochEnergyCorrectionTool() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode StateSimpleBetheBlochEnergyCorrectionTool::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;  // error already reported by base class
  
  return StatusCode::SUCCESS;
}

//=============================================================================
