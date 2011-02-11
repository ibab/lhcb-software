// $Id
// $Id: PhotonMakerAlg.cpp,v 1.1 2009-04-21 19:15:41 pkoppenb Exp $
// ============================================================================
// Include files
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "CaloUtils/CaloParticle.h"
// local
#include "PhotonMakerAlg.h"

/** @file
 *
 *  Implementation file for class : PhotonMakerAlg
 *
 *  @date 2006-08-25
 *  @author Olivier Deschamps odescham@in2p3.fr
 *
 */

// ============================================================================
/** @var PhotonMakerAlgFactory
 *  Declaration of mandatory  Tool Factory
 */
// ============================================================================

DECLARE_ALGORITHM_FACTORY( PhotonMakerAlg )

// ============================================================================
/** Standard constructor
 *  @param type   tool type
 *  @param name   tool name
 *  @param parent tool parent
 */
// ============================================================================
  PhotonMakerAlg::PhotonMakerAlg
( const std::string& name,ISvcLocator* pSvcLocator  )
  : ParticleMakerBase           ( name, pSvcLocator )
  , m_photonMakerType  ()
  , m_photonMaker      ()
{
  declareProperty ( "PhotonMakerType"  , m_photonMakerType = "PhotonMaker") ;
}
// ============================================================================

// ============================================================================
/// destructor
// ============================================================================
PhotonMakerAlg::~PhotonMakerAlg() {}
// ============================================================================

StatusCode PhotonMakerAlg::initialize    ()
{
  // initialize the base class
  StatusCode sc = ParticleMakerBase::initialize();
  if( sc.isFailure() ) { return Error(" Unable to initialize ParticleMakerBase",sc);}

  m_photonMaker = tool< ICaloParticleMaker>( m_photonMakerType ,  this ) ;

  return StatusCode::SUCCESS ;
}
// ============================================================================

StatusCode PhotonMakerAlg::finalize      ()
{
  return ParticleMakerBase::finalize ();
}

// ============================================================================
/** Make the particles
 *  @see ICaloParticleMaker
 *  @param particles  vector of particles
 */
// ============================================================================
StatusCode PhotonMakerAlg::makeParticles (LHCb::Particle::Vector & photons )
{
  return m_photonMaker->makeParticles(photons);
}
