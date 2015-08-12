// Include files
// ------------

#include <cmath>
#include <limits>
#include <algorithm>

// from VDT
#include "vdt/log.h"

// from DetDesc
#include "DetDesc/Material.h"

// from TrackEvent
#include "Event/TrackParameters.h"

// local
#include "StateThickMSCorrectionTool.h"

using namespace Gaudi::Units;

namespace {
// For convenient trailing-return-types in C++11:
#define AUTO_RETURN(...) noexcept(noexcept(__VA_ARGS__)) -> decltype(__VA_ARGS__) {return (__VA_ARGS__);}
template <typename T> inline auto pow_2(T x) AUTO_RETURN( x*x )
}
//-----------------------------------------------------------------------------
// Implementation file for class : StateThickMSCorrectionTool
//
// 2006-08-21 : Eduardo Rodrigues
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( StateThickMSCorrectionTool )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
StateThickMSCorrectionTool::StateThickMSCorrectionTool( const std::string& type,
                                                        const std::string& name,
                                                        const IInterface* parent )
  : GaudiTool ( type, name , parent ),
// break things good and hard if initialize isn't called
  m_msff2MoliereFact2(std::numeric_limits<double>::quiet_NaN())
{
  declareInterface<IStateCorrectionTool>(this);
  declareProperty( "MSFudgeFactor2" ,      m_msff2 = 1.0 );
  declareProperty( "UseRossiAndGreisen" ,  m_useRossiAndGreisen = false );
  declareProperty( "RossiAndGreisenFact2", m_msff2RossiAndGreisenFact2 = -1e42 );
}
//=============================================================================
// Initialize variables
//=============================================================================
StatusCode StateThickMSCorrectionTool::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;
  // move computation out of the hot code path
  m_msff2MoliereFact2 = m_msff2 * pow_2(TrackParameters::moliereFactor);
  
  if( m_msff2RossiAndGreisenFact2 < 0 ) m_msff2RossiAndGreisenFact2 = m_msff2MoliereFact2;
  if ( msgLevel(MSG::DEBUG) ){
    if (m_useRossiAndGreisen){
      debug() << "Using Rossi & Greisen Factor squared of " 
              <<  m_msff2RossiAndGreisenFact2 << endmsg;
    }else{
      debug() << "Using log term with Moliere factor squared of " 
              << m_msff2MoliereFact2 << endmsg;
    }
  }
  
  
  
  return sc;
}
//=============================================================================
// Correct a State for multiple scattering in the case of a thick scatter
//=============================================================================
void StateThickMSCorrectionTool::correctState( LHCb::State& state,
                                               const Material* material,
                                               double wallThickness,
                                               bool upstream,
                                               LHCb::ParticleID  ) const
{
  const auto t = wallThickness / material->radiationLength();
  // if t is below tolerance, all corrections end up zero anyway, so we can
  // stop early (put UNLIKELY here because it'll influence what gcc inlines
  // below, and we want gcc to think the rest of the code path is hot - and it
  // is - and inline sqrt and vdt::fast_log)
  if (UNLIKELY(t <= TrackParameters::lowTolerance)) return;

  const auto& stv = state.stateVector(); // x, y, tx, ty, q/p
  const auto norm2 = 1 + pow_2(stv[2]) + pow_2(stv[3]);
  // protect against zero momentum
  static constexpr decltype(stv[4]) iMeV = 1. / MeV;
  const auto norm2cnoisetmp = norm2 * pow_2(std::min(std::abs(stv[4]), iMeV));

  const auto radThick = t * std::sqrt(norm2);
  // in a normal tracking run, for around 95% of cases, radThick is in the
  // interval [1e-7, 1e-1] - that's the region where the log turns nasty
  // because it decreases so rapidly, and it's not easily possible to
  // approximate it in a quick and dirty way
  //
  // be FMA friendly
  auto norm2cnoise = norm2cnoisetmp * radThick;
  
  // -- first one omits log-term, which 'solves' the problem of having a non-linear behaviour when going through many layers
  if( m_useRossiAndGreisen ){
    norm2cnoise *= m_msff2RossiAndGreisenFact2;
  }else{
    norm2cnoise *= m_msff2MoliereFact2 *  pow_2(.038 * vdt::fast_log(radThick) + 1);
  }
  
  const auto covTxTx = norm2cnoise * (1 + pow_2(stv[2]));
  const auto covTyTy = norm2cnoise * (1 + pow_2(stv[3]));
  const auto covTxTy = norm2cnoise * stv[2] * stv[3];
  const auto wallThicknessD = wallThickness * (upstream ? -.5 : .5);
  static constexpr decltype(wallThickness) thirds = 1. / 3.;
  const auto wallThickness2_3 = pow_2(wallThickness) * thirds;

  // update covariance matrix C = C + Q
  auto& cov = state.covariance();
  cov(0, 0) += covTxTx * wallThickness2_3, // don't care about the order or
  cov(1, 0) += covTxTy * wallThickness2_3, // update so use commata here,
  cov(1, 1) += covTyTy * wallThickness2_3, // and let compiler decide
  cov(2, 0) += covTxTx * wallThicknessD,
  cov(2, 1) += covTxTy * wallThicknessD,
  cov(2, 2) += covTxTx,
  cov(3, 0) += covTxTy * wallThicknessD,
  cov(3, 1) += covTyTy * wallThicknessD,
  cov(3, 2) += covTxTy,
  cov(3, 3) += covTyTy;
}

