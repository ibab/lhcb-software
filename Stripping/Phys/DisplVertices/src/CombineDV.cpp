// $Id$
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "Kernel/IParticleCombiner.h"

#include "GaudiAlg/GaudiAlgorithm.h"
// local
#include "CombineDV.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CombineDV
//
// Merge DV candidates into one candidate
//
// 2011-10-12 : Veerle Heijne
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CombineDV )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  CombineDV::CombineDV( const std::string& name,
                        ISvcLocator* pSvcLocator)
    : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "Inputs", m_candidateLocation ) ;
  declareProperty( "Output", m_outputLocation );
  declareProperty( "ParticleCombinerName" ,m_combinerName = "MomentumCombiner");
}
//=============================================================================
// Destructor
//=============================================================================
CombineDV::~CombineDV() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode CombineDV::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;
  m_combiner = tool<IParticleCombiner> ( m_combinerName , this ) ;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode CombineDV::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  // Create the output container
  LHCb::Particles* CombDVs = new LHCb::Particles();
  put( CombDVs , m_outputLocation );

  if(!exist<LHCb::Particles>(m_candidateLocation))
    return Warning( m_candidateLocation +" not found", StatusCode::SUCCESS, 0);
  const LHCb::Particles * myCand = get<LHCb::Particles>(m_candidateLocation);
  LHCb::Particle::ConstVector daughters ;

  // loop on candidates to merge:
  for ( LHCb::Particles::const_iterator i = myCand->begin() ; myCand->end() != i ; ++i )
  {
    const LHCb::Particle* p = *i ;
    daughters.push_back ( p ) ;
  }

  LHCb::Particle* newparticle = new LHCb::Particle();
  LHCb::Vertex* newvertex= new LHCb::Vertex();

  StatusCode sc = m_combiner->combine ( daughters , *newparticle , *newvertex ) ;
  if ( sc.isFailure() )
  {
    std::cout<< "Error from momentum combiner"<<std::endl;
    delete newvertex;
    delete newparticle;
    return StatusCode::SUCCESS;
  }

  CombDVs->insert(newparticle);


  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode CombineDV::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
