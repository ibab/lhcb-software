
//-----------------------------------------------------------------------------
/** @file ChargedProtoParticleCALOBaseAlg.cpp
 *
 * Implementation file for algorithm ChargedProtoParticleCALOBaseAlg
 *
 * CVS Log :-
 * $Id: ChargedProtoParticleCALOBaseAlg.cpp,v 1.1 2009-08-29 20:37:19 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 28/08/2009
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "ChargedProtoParticleCALOBaseAlg.h"

// namespaces
using namespace LHCb;

//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ChargedProtoParticleCALOBaseAlg::
ChargedProtoParticleCALOBaseAlg( const std::string& name,
                                 ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
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

double ChargedProtoParticleCALOBaseAlg::CaloSpd  ( const LHCb::CaloHypo*  hypo  )  const
{
  if ( NULL == hypo ) return 0;
  LHCb::CaloHypo::Digits digits = hypo->digits();
  LHCb::CaloDataFunctor::IsFromCalo< LHCb::CaloDigit* > isSpd( DeCalorimeterLocation::Spd );
  LHCb::CaloHypo::Digits::iterator it = std::stable_partition ( digits.begin(),digits.end(),isSpd );
  return ( it == digits.begin() ) ? 0. : +1.;
}

double ChargedProtoParticleCALOBaseAlg::CaloPrs  ( const LHCb::CaloHypo*  hypo  )  const
{
  if ( NULL == hypo) return 0;
  LHCb::CaloHypo::Digits digits = hypo->digits();
  LHCb::CaloDataFunctor::IsFromCalo< LHCb::CaloDigit* > isPrs( DeCalorimeterLocation::Prs );
  LHCb::CaloHypo::Digits::iterator it = std::stable_partition ( digits.begin(),digits.end(),isPrs );
  double CaloPrs = 0. ;
  for ( LHCb::CaloHypo::Digits::iterator id = digits.begin(); id != it ; ++id )
  {
    if (0 != *id) CaloPrs += (*id)->e();
  }
  return CaloPrs  ;
}

double ChargedProtoParticleCALOBaseAlg::CaloEcal  ( const LHCb::CaloHypo*  hypo  )  const
{
  if ( NULL == hypo) return 0;
  SmartRefVector<LHCb::CaloCluster> clusters = hypo->clusters();
  if ( clusters.empty() ) return 0.;
  SmartRefVector<LHCb::CaloCluster>::iterator icluster = clusters.begin();
  LHCb::CaloCluster* cluster = *icluster;
  if ( NULL == cluster) return 0;
  return cluster->e();
}
