// $Id: ClearDaughters.cpp 197086 2015-11-01 19:55:47Z graven $
// ============================================================================
// Include files
// ============================================================================
#include <algorithm>
#include <functional>
#include "boost/algorithm/string.hpp"
// ============================================================================
// local
// ============================================================================
#include "ClearDaughters.h"
// ============================================================================
// adpated from FilterDesktop
// ============================================================================
// ============================================================================
// the specific initialization
// ============================================================================
StatusCode ClearDaughters::initialize ()             // the specific initialization
{
  // initialize the base class
  StatusCode sc = DaVinciAlgorithm::initialize () ;
  if ( sc.isFailure () ) { return sc ; }                          // RETURN
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// finalize /reset functors
// ============================================================================
StatusCode ClearDaughters::finalize   ()
{
  // finalize the base
  return DaVinciAlgorithm::finalize () ;
}
// ============================================================================
/* standard constructor
 *  @see DaVinciAlgorithm
 *  @see GaudiTupleAlg
 *  @see GaudiHistoAlg
 *  @see GaudiAlgorithm
 *  @see Algorithm
 *  @see AlgFactory
 *  @see IAlgFactory
 *  @param name the algorithm instance name
 *  @param pSvc pointer to Service Locator
 */
// ============================================================================
ClearDaughters::ClearDaughters                  // standard contructor
( const std::string& name ,                   // the algorithm instance name
  ISvcLocator*       pSvc )                   // pointer to Service Locator
  : DaVinciAlgorithm ( name , pSvc )
      , m_cloneFilteredParticles (true)

{
      declareProperty
   ( "CloneFilteredParticles" ,
    m_cloneFilteredParticles ,
    "Clone filtered particles and end-verti");



}
// ============================================================================
// virtual & protected destructor
// ============================================================================
ClearDaughters::~ClearDaughters (){} // virtual & protected destructor
// ============================================================================
void ClearDaughters::writeEmptyTESContainers()
{

  return m_cloneFilteredParticles ?
    writeEmptyKeyedContainers()  :
    writeEmptySharedContainers() ;

}
// ==========================================================================
// the most interesting method
// ============================================================================
StatusCode ClearDaughters::execute ()       // the most interesting method
{

  // get the input particles
  const LHCb::Particle::ConstVector& particles = i_particles () ;

  markParticles ( particles ) ;
  // make the filter decision
  setFilterPassed ( !i_markedParticles().empty() ) ;

  // some statistics
  counter ( "#passed" ) += i_markedParticles().size();

  //
  return StatusCode::SUCCESS;
}
// ============================================================================
// save (clone if needed) selected particles in TES
// ============================================================================
template <class PARTICLES, class VERTICES, class CLONER>
StatusCode ClearDaughters::_save ( ) const
{
  //
  PARTICLES* p_tes = new PARTICLES () ;
  VERTICES*  v_tes = new VERTICES  () ;
  //
  put ( p_tes ,    particleOutputLocation () ) ;
  put ( v_tes , decayVertexOutputLocation () ) ;
  //
  CLONER cloner ;
  //
  for ( const auto& p : i_markedParticles() )
  {
    if ( !p ) { continue ; } // CONTINUE

    // clone if needeed
    LHCb::Particle* p_cloned = cloner ( p ) ;
    p_cloned->clearDaughters();
    p_tes -> insert ( p_cloned ) ;
    //
    this  -> cloneP2PVRelation ( p , p_cloned ) ;
    //
    const LHCb::Vertex* v = p->endVertex() ;
    if ( v )
    {
      LHCb::Vertex* v_cloned = cloner ( v ) ;
      p_cloned -> setEndVertex ( v_cloned ) ;
      v_tes    -> insert( v_cloned ) ;
    }
  }
  //
  // check that the decay trees are fully in the TES
  //
  for ( const auto& ip : *p_tes )
  {
    if (! DaVinci::Utils::decayTreeInTES( ip ) )
    {
      return Error ( "ClearDaughters::_save Element of saved decay tree not in TES. Likely memory leak!");
    }
  }
  //
  return  i_markedParticles().size() != p_tes->size() ?
          StatusCode::FAILURE : StatusCode::SUCCESS   ;
}
// ============================================================================
namespace
{
  // ==========================================================================
  /// the trivial cloner
  struct _Cloner
  {
    template <class T>
    T* operator() ( const T* a ) const { return a->clone() ; }
  };
  /// helper structure (fake cloner)
  struct _Caster
  {
    template <class T>
    T* operator() ( const T* a ) const { return const_cast<T*> ( a ) ; }
  };
  // ==========================================================================
} // end of anonymous namespace
// ============================================================================
StatusCode ClearDaughters::_saveInTES ()
{
  if (msgLevel(MSG::VERBOSE))
  {
    verbose() << "ClearDaughters::_saveInTES " << i_markedParticles().size() << " Particles" << endmsg;
  }
  return m_cloneFilteredParticles ?
    this -> _save<LHCb::Particle::Container,LHCb::Vertex::Container,_Cloner> ():
    this -> _save<LHCb::Particle::Selection,LHCb::Vertex::Selection,_Caster> ();
}
// ============================================================================
void ClearDaughters::cloneP2PVRelation
( const LHCb::Particle*   particle ,
  const LHCb::Particle*   clone       ) const
{
  const LHCb::VertexBase* bestPV = getStoredBestPV(particle);
  if ( bestPV ) { this->relate ( clone , bestPV ) ; }

}
// ============================================================================
void ClearDaughters::writeEmptyKeyedContainers() const
{
  LHCb::Particle::Container* container = new LHCb::Particle::Container();
  put(container, particleOutputLocation());
  return;
}
// ============================================================================
void ClearDaughters::writeEmptySharedContainers() const
{
  LHCb::Particle::Selection* container = new LHCb::Particle::Selection();
  put(container, particleOutputLocation());
  return;
}
// ============================================================================
// ============================================================================
/// the factory
DECLARE_ALGORITHM_FACTORY(ClearDaughters)
// ============================================================================
// The END
// ============================================================================
