// Include files 
// -------------
// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// from GSL
#include "gsl/gsl_math.h"

// from DetDesc
#include "DetDesc/Material.h"

// from TrackEvent
#include "Event/TrackParameters.h"

// local
#include "StateThinMSCorrectionTool.h"

using namespace Gaudi::Units;

//-----------------------------------------------------------------------------
// Implementation file for class : StateThinMSCorrectionTool
//
// 2006-08-21 : Eduardo Rodrigues
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( StateThinMSCorrectionTool )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
StateThinMSCorrectionTool::StateThinMSCorrectionTool( const std::string& type,
                                                      const std::string& name,
                                                      const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IStateCorrectionTool>(this);
  declareProperty( "MSFudgeFactor2" , m_msff2 = 1.0 );
}

//=============================================================================
// Correct a State for multiple scattering
// in the approximation of a thin scatter
//=============================================================================
void StateThinMSCorrectionTool::correctState( LHCb::State& state,
                                              const Material* material,
                                              double wallThickness,
                                              bool, 
                                              LHCb::ParticleID  ) const
{
  const double t          = wallThickness / material -> radiationLength();
  const double norm2      = 1.0 + gsl_pow_2(state.tx()) + gsl_pow_2(state.ty());
  double scatLength = 0.;
  if ( t > TrackParameters::lowTolerance ) {
    const double radThick = sqrt(norm2) * t;
    scatLength = radThick*gsl_pow_2( TrackParameters::moliereFactor *
                                     (1.+0.038*log(radThick)) );
  }

  // protect 0 momentum
  const double p = GSL_MAX( state.p(), 1.0*MeV );

  const double norm2cnoise = norm2 * m_msff2 * scatLength / gsl_pow_2(p);

  // multiple scattering covariance matrix - initialized to 0
  Gaudi::TrackSymMatrix Q = Gaudi::TrackSymMatrix();

  Q(2,2) = norm2cnoise * ( 1. + gsl_pow_2(state.tx()) );
  Q(3,3) = norm2cnoise * ( 1. + gsl_pow_2(state.ty()) );
  Q(3,2) = norm2cnoise * state.tx() * state.ty();

  // update covariance matrix C = C + Q
  Gaudi::TrackSymMatrix& tC = state.covariance();
  tC += Q;  
}

//=============================================================================
// Destructor
//=============================================================================
StateThinMSCorrectionTool::~StateThinMSCorrectionTool() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode StateThinMSCorrectionTool::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;  // error already reported by base class
  
  return StatusCode::SUCCESS;
}

//=============================================================================
