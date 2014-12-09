// Include files
// -------------
// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
#include "GaudiKernel/PhysicalConstants.h"

// from GSL
#include <cmath>
#include "gsl/gsl_math.h"
#include "vdt/exp.h"
#include "vdt/log.h"

// from DetDesc
#include "DetDesc/Material.h"

// from PartProp
#include "Kernel/ParticleProperty.h"

// local
#include "StateDetailedBetheBlochEnergyCorrectionTool.h"

using namespace Gaudi::Units;

namespace {
// For convenient trailing-return-types in C++11:
#define AUTO_RETURN(...) noexcept(noexcept(__VA_ARGS__)) -> decltype(__VA_ARGS__) {return (__VA_ARGS__);}
template <typename T> inline auto pow_2(T x) AUTO_RETURN( x*x )
}

//-----------------------------------------------------------------------------
// Implementation file for class : StateDetailedBetheBlochEnergyCorrectionTool
//
// 2006-08-18 : Eduardo Rodrigues
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( StateDetailedBetheBlochEnergyCorrectionTool )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
StateDetailedBetheBlochEnergyCorrectionTool::StateDetailedBetheBlochEnergyCorrectionTool( const std::string& type,
                                                                                      const std::string& name,
                                                                                      const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IStateCorrectionTool>(this);  
  declareProperty( "EnergyLossFactor",  m_energyLossCorr = 1.0 );
  declareProperty( "MaximumEnergyLoss", m_maxEnergyLoss  = 100. * MeV           );
}

//=============================================================================
// Correct a State for dE/dx energy losses
//=============================================================================
void StateDetailedBetheBlochEnergyCorrectionTool::correctState( LHCb::State& state,
                                                              const Material* material,
                                                              double wallThickness,
                                                              bool upstream, 
                                                              LHCb::ParticleID pid) const
{
  
  // apply correction - note for now only correct the state vector
  Gaudi::TrackVector& tX = state.stateVector();

  //TODO:FIXME: cache ParticleProperty of to-be-expected pids...
  const LHCb::ParticleProperty* partProp = m_pp->find(pid);
  if( 0 == partProp ) return;

  double mass = partProp->mass();
  
  double eta = 1/(tX[4]*mass);

  double beta2 = (eta*eta)/(1.0+eta*eta);

  double me = Gaudi::Units::electron_mass_c2;

  double x = vdt::fast_log(eta*eta)/4.606;
  double rho = 0;
  
  if (x > material->X0()) {
      if (x < material->X1())
        rho = 4.606*x-material->C() + material->a()*std::pow(material->X1()-x,material->m());
      else
        rho = 4.606*x-material->C();
  }
 
  double eLoss =  m_energyLossCorr*wallThickness
     * sqrt( 1. + pow_2(state.tx()) + pow_2(state.ty()) )
    * 30.71 * MeV*mm2/mole * material->Z() * material->density() / material->A()
      *(vdt::fast_log(2*me*eta*eta/material->I()) - beta2 - rho*0.5)/beta2;
 
  
  eLoss = GSL_MIN( m_maxEnergyLoss, eLoss );
   if ( !upstream ) eLoss *= -1.;
  
  
  if ( tX[4] > 0. ) tX[4] = 1. / ( 1./tX[4]+(eLoss) );
  else              tX[4] = 1. / ( 1./tX[4]-(eLoss) );

}

//=============================================================================
// Destructor
//=============================================================================
StateDetailedBetheBlochEnergyCorrectionTool::~StateDetailedBetheBlochEnergyCorrectionTool() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode StateDetailedBetheBlochEnergyCorrectionTool::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;  // error already reported by base class
  
  m_pp = svc<LHCb::IParticlePropertySvc>( "LHCb::ParticlePropertySvc", true );
 
  return StatusCode::SUCCESS;
}

//=============================================================================
