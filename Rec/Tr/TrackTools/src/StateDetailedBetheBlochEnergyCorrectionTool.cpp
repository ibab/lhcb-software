// Include files
// -------------
// from Gaudi
#include "GaudiKernel/PhysicalConstants.h"

#include <cmath>
#include <algorithm>
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
  // tell gcc/clang/icc that vdt_pow has no side-effects whatsoever
  inline double vdt_pow(const double x, const double y) noexcept __attribute__((const));
  inline double vdt_pow(const double x, const double y) noexcept
  { return vdt::fast_exp(y * vdt::fast_log(x)); }
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
  : GaudiTool ( type, name , parent ),
    m_lastCachedPID(std::end(m_pid2mass)),
    m_lastCachedMaterial(std::end(m_material2factors))
{
  declareInterface<IStateCorrectionTool>(this);  
  declareProperty( "EnergyLossFactor",  m_energyLossCorr = 1.0 );
  declareProperty( "MaximumEnergyLoss", m_maxEnergyLoss  = 100. * MeV           );
}

/// stupid little helper for converting scoped enums to unterlying integer type
template <typename T>
static constexpr typename std::underlying_type<T>::type toi(T value) 
{ return static_cast<typename std::underlying_type<T>::type>(value); }

//=============================================================================
// Correct a State for dE/dx energy losses
//=============================================================================
void StateDetailedBetheBlochEnergyCorrectionTool::correctState( LHCb::State& state,
                                                              const Material* material,
                                                              double wallThickness,
                                                              bool upstream, 
                                                              LHCb::ParticleID pid) const
{
  // lookup mass in our cache -- and add it if not already there...
  // keep the most recently used slot in the "hot spot", as it's likely used again
  // (usually, all energy loss corrections in all traversed materials for a
  // given particle are calculated in some sort of loop, so the PID rarely
  // changes from the perspective of this piece of code)
  if (UNLIKELY(std::end(m_pid2mass) == m_lastCachedPID ||
	m_lastCachedPID->first != pid.abspid())) { // use of abspid() assumes CPT holds
    m_lastCachedPID = m_pid2mass.find(pid.abspid());
    if (UNLIKELY(std::end(m_pid2mass) == m_lastCachedPID)) {
      auto partProp = m_pp->find(pid);
      m_lastCachedPID = m_pid2mass.insert(
	  std::make_pair(pid.abspid(), (partProp ? partProp->mass() : 0.))).first;
    }
  }
  const auto mass = m_lastCachedPID->second;
  
  // mass zero is quite UNLIKELY here, and we'll save a lot of computation in
  // that case
  if (UNLIKELY(0 == mass)) return;

  // material lookups are slightly different: the "hot spot" occupied by the
  // last material used is likely to be missed (about 85% of the time), but if
  // we can save the hash by looking at the last used element first, that's a
  // good thing
  if (LIKELY(std::end(m_material2factors) == m_lastCachedMaterial ||
	m_lastCachedMaterial->first != material)) {
    m_lastCachedMaterial = m_material2factors.find(material);
    // cache material quantities:
    // - X0, C, X1, a, m
    // - const. * Z / A * density ("DensityFactor")
    // - log(I)
    // we trade nine virtual function calls plus associated memory accesses, four
    // floating point operations and a logarithm for a lookup
    if (UNLIKELY(std::end(m_material2factors) == m_lastCachedMaterial)) {
      // coming here is the very rare exception - after the first couple of
      // events, the material cache miss rate is essentially zero
      m_lastCachedMaterial = m_material2factors.insert(
	      std::make_pair(material, std::make_tuple(
		      material->X0(), material->C(), material->X1(),
		      material->a(), material->m(), 
		      (30.71 * MeV*mm2/mole) * material->Z() *
		      material->density() / material->A(),
		      std::log(material->I())))).first;
    }
  }
  const auto& mat = m_lastCachedMaterial->second;

  // apply correction - note: for now only correct the state vector
  auto& qOverP = state.stateVector()[4];
  
  const auto eta2_inv = (qOverP * mass) * (qOverP * mass); 
  const auto x4 = -vdt::fast_log(eta2_inv);
  const auto x = x4 / 4.606;
  
  double rho_2 = 0;
  if (LIKELY(x > std::get<toi(Mat::X0)>(mat))) {
    // branch taken in about 3/4 of cases
    rho_2 = x4 - std::get<toi(Mat::C)>(mat);
    if (LIKELY(x < std::get<toi(Mat::X1)>(mat))) {
      // branch taken in about 9/10 cases
      rho_2 += std::get<toi(Mat::a)>(mat) * vdt_pow(
	  std::get<toi(Mat::X1)>(mat) - x, std::get<toi(Mat::m)>(mat));
    }
    rho_2 *= 0.5;
  }
  static const auto log_2_me = std::log(2*Gaudi::Units::electron_mass_c2);
  // correct wall thickness for angle of incidence
  const auto tx = state.tx(), ty = state.ty();
  wallThickness *= std::sqrt(1 + tx * tx + ty * ty);
  const auto beta2_inv = 1 + eta2_inv;
  auto eLoss = m_energyLossCorr * wallThickness * std::get<toi(Mat::DensityFactor)>(mat) *
     (beta2_inv * (log_2_me + x4 - std::get<toi(Mat::LogI)>(mat) - rho_2) - 1);
 
  eLoss = std::copysign(std::min(m_maxEnergyLoss, eLoss),
	  (2 * int(upstream) - 1) * qOverP);

  qOverP /= 1 + eLoss * qOverP;
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
  m_pid2mass.clear();
  m_material2factors.clear();
  m_lastCachedPID = std::end(m_pid2mass);
  m_lastCachedMaterial = std::end(m_material2factors);
 
  return StatusCode::SUCCESS;
}

//=============================================================================
