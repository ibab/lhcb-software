// Include files 
// ------------
// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// from GSL
#include "gsl/gsl_math.h"

// from DetDesc
#include "DetDesc/Material.h"

// from TrackEvent
#include "Event/TrackParameters.h"

// local
#include "StateThickMSCorrectionTool.h"

using namespace Gaudi::Units;

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
  const double tx2        = gsl_pow_2( state.tx() );
  const double ty2        = gsl_pow_2( state.ty() );
  const double norm2      = 1. + tx2 + ty2;
  double scatLength = 0.;
  double t          = wallThickness / material -> radiationLength();
  if ( t > TrackParameters::lowTolerance ) {
    double radThick = sqrt(norm2) * t;
    scatLength = radThick * gsl_pow_2( TrackParameters::moliereFactor *
                                       (1. + 0.038*log(radThick)) );
  }

  // protect zero momentum
  const double p = GSL_MAX( state.p(), 1.0 * MeV );
  const double norm2cnoise = norm2 * m_msff2 * scatLength / (p*p);

  // slope covariances
  const double covTxTx = norm2cnoise * ( 1. + tx2 );
  const double covTyTy = norm2cnoise * ( 1. + ty2 );
  const double covTxTy = norm2cnoise * state.tx() * state.ty();

  // D - depends on whether up or downstream
  const double D = (upstream) ? -1. : 1. ;

  Gaudi::TrackSymMatrix Q = Gaudi::TrackSymMatrix();
  
  const double wallThickness2 = gsl_pow_2(wallThickness);

  Q(0,0) = covTxTx * wallThickness2 / 3.;
  Q(1,0) = covTxTy * wallThickness2 / 3.;
  Q(2,0) = 0.5*covTxTx * D * wallThickness;
  Q(3,0) = 0.5*covTxTy * D * wallThickness;

  Q(1,1) = covTyTy * wallThickness2 / 3.;
  Q(2,1) = 0.5*covTxTy * D * wallThickness;
  Q(3,1) = 0.5*covTyTy * D * wallThickness;

  Q(2,2) = covTxTx;
  Q(3,2) = covTxTy;

  Q(3,3) = covTyTy;

  // update covariance matrix C = C + Q
  Gaudi::TrackSymMatrix& tC = state.covariance();
  tC += Q;
}

//=============================================================================
// Destructor
//=============================================================================
StateThickMSCorrectionTool::~StateThickMSCorrectionTool() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode StateThickMSCorrectionTool::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;  // error already reported by base class
  
  return StatusCode::SUCCESS;
}

//=============================================================================
