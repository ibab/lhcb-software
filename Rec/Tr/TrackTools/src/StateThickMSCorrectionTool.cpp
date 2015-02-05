// Include files 
// ------------
// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

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
  : GaudiTool ( type, name , parent )
{
  declareInterface<IStateCorrectionTool>(this);
  declareProperty( "MSFudgeFactor2" , m_msff2 = 1.0 );
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
  const double tx2        = pow_2( state.tx() );
  const double ty2        = pow_2( state.ty() );
  const double norm2      = 1. + tx2 + ty2;
  double scatLength = 0.;
  double t          = wallThickness / material -> radiationLength();
  if ( t > TrackParameters::lowTolerance ) {
    double radThick = sqrt(norm2) * t;
    scatLength = radThick * pow_2( TrackParameters::moliereFactor *
                                   (1. + 0.038*vdt::fast_log(radThick)) );
  }

  // protect against zero momentum
  const double p = std::max( state.p(), 1.0 * MeV );
  const double norm2cnoise = norm2 * m_msff2 * scatLength / (p*p);

  // slope covariances
  const double covTxTx = norm2cnoise * ( 1. + tx2 );
  const double covTyTy = norm2cnoise * ( 1. + ty2 );
  const double covTxTy = norm2cnoise * state.tx() * state.ty();

  // D - depends on whether up or downstream
  const double D = (upstream) ? -0.5 : 0.5 ;

  const double wallThickness2_3 = pow_2(wallThickness)/3.;

  // update covariance matrix C = C + Q
  auto& cov = state.covariance();

  cov(0,0) += covTxTx * wallThickness2_3 ;
  cov(1,0) += covTxTy * wallThickness2_3 ;
  cov(1,1) += covTyTy * wallThickness2_3 ;
  cov(2,0) += covTxTx * wallThickness * D ;
  cov(2,1) += covTxTy * wallThickness * D ;
  cov(2,2) += covTxTx ;
  cov(3,0) += covTxTy * wallThickness * D ;
  cov(3,1) += covTyTy * wallThickness * D ;
  cov(3,2) += covTxTy ;
  cov(3,3) += covTyTy ;
}
//=============================================================================
