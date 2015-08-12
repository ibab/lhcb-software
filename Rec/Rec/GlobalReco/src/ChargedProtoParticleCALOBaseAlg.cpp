//-----------------------------------------------------------------------------
/** @file ChargedProtoParticleCALOBaseAlg.cpp
 *
 * Implementation file for algorithm ChargedProtoParticleCALOBaseAlg
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 28/08/2009
 */
//-----------------------------------------------------------------------------

// local
#include "ChargedProtoParticleCALOBaseAlg.h"

// namespaces
//using namespace LHCb;

//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ChargedProtoParticleCALOBaseAlg::
ChargedProtoParticleCALOBaseAlg( const std::string& name,
                                 ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ), m_estimator(0)
{
}

//=============================================================================
// Destructor
//=============================================================================
ChargedProtoParticleCALOBaseAlg::~ChargedProtoParticleCALOBaseAlg() { }

//=============================================================================
// Initialization
//=============================================================================
StatusCode ChargedProtoParticleCALOBaseAlg::initialize()
{
  const StatusCode sc = GaudiAlgorithm::initialize();
  if ( sc.isFailure() ) return sc;


  m_estimator = tool<ICaloHypoEstimator>("CaloHypoEstimator","CaloHypoEstimator",this);

  return sc;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode ChargedProtoParticleCALOBaseAlg::finalize()
{
  // execute base class finalise and return
  return GaudiAlgorithm::finalize();
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode ChargedProtoParticleCALOBaseAlg::execute()
{
  return Error( "Should never see this" );
}
//=============================================================================

