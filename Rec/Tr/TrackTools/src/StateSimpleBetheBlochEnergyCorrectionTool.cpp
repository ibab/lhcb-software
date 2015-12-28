// Include files
// -------------
#include <cmath>

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
DECLARE_TOOL_FACTORY( StateSimpleBetheBlochEnergyCorrectionTool )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
StateSimpleBetheBlochEnergyCorrectionTool::StateSimpleBetheBlochEnergyCorrectionTool( const std::string& type,
                                                                                      const std::string& name,
                                                                                      const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IStateCorrectionTool>(this);
  
  declareProperty( "EnergyLossFactor",           m_energyLossCorr = 354.1 * MeV*mm2/mole );
  declareProperty( "MaximumEnergyLoss",          m_maxEnergyLoss  = 100. * MeV           );
  declareProperty( "MinMomentumAfterEnergyCorr", m_minMomentumAfterEnergyCorr = 10.*MeV  );  
  declareProperty( "EnergyLossError",            m_sqrtEError=1.7 * sqrt(MeV) ); // proportional to sqrt(E)
  declareProperty( "UseEnergyLossError",         m_useEnergyLossError=false) ;
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
    * sqrt( 1. + std::pow(state.tx(),2) + std::pow(state.ty(),2) )
    * m_energyLossCorr * material->Z() * material->density() / material->A();
  bbLoss = std::min( m_maxEnergyLoss, bbLoss );
  if ( !upstream ) bbLoss *= -1.;
  
  // apply correction - note for now only correct the state vector
  Gaudi::TrackVector& tX = state.stateVector();

  //  double minMomentumForEnergyCorrection = 10*Gaudi::Units::MeV;

  double qOverP = 0.0;
  tX[4] < 0.0 ? qOverP = std::min(tX[4], -LHCb::Math::lowTolerance) :
                qOverP = std::max(tX[4], LHCb::Math::lowTolerance);  
  
  double newP = 0.0;
  qOverP < 0.0 ? newP = std::min(1.0/qOverP - bbLoss, -m_minMomentumAfterEnergyCorr) :
                 newP = std::max(1.0/qOverP + bbLoss, m_minMomentumAfterEnergyCorr);
  
  tX[4] = 1.0/newP;
  
  // correction on cov
  if( m_useEnergyLossError && m_sqrtEError > 0 ) {
    // error on dE is proportional to the sqrt of dE:
    // double sigmadE = m_sqrtEError * std::sqrt(std::abs(bbLoss))
    double err2 = m_sqrtEError * m_sqrtEError * std::abs(bbLoss) * tX[4] * tX[4] ;
    Gaudi::TrackSymMatrix& cov = state.covariance(); 
    cov(4,4) += err2;
  }
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
