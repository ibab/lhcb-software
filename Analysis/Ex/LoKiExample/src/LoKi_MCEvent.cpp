// $Id: LoKi_MCEvent.cpp,v 1.1 2004-04-09 07:23:44 ibelyaev Exp $
// ============================================================================
// $CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
// Include files 
// ============================================================================
// Event 
// ============================================================================
#include "Event/EventHeader.h"
#include "Event/GenHeader.h"
#include "Event/Collision.h"
#include "Event/MCParticle.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/LoKi.h"
#include "LoKi/MCParticles.h"
// ============================================================================


LOKI_ALGORITHM( LoKi_MCEvent ) 
{
  using namespace LoKi ;
  using namespace LoKi::Cuts ;

  // get GenHeader from TES 
  const GenHeader* gen = get<GenHeader>( GenHeaderLocation::Default ) ;

  // get collisions object 
  const Collisions* cls = get<Collisions> ( CollisionLocation::Default ) ;
  
  // get all Monte Carlo Particles from TES 
  typedef ::MCParticles MCPs ;
  const MCPs* mcps = get<MCPs>( MCParticleLocation::Default ) ;

  // evaluate number of beauty hadrons in the event 
  const size_t Nb = std::count_if ( mcps->begin () , mcps->end   () , BEAUTY ) ;
  
  // evaluate number of charm hadrons in the event 
  const size_t Nc = std::count_if ( mcps->begin () , mcps->end   () , CHARM  ) ;

  // put information into NTUPLE 
  Tuple tuple = nTuple( "EventType tuple " ) ;
  
  tuple -> column ( "EvtType" , gen->evType() ) ;
  tuple -> column ( "Coll"    , cls->size()   ) ;
  tuple -> column ( "nB"      , Nb            ) ;
  tuple -> column ( "nC"      , Nc            ) ;
  tuple -> write  () ;
  
  return StatusCode::SUCCESS ;  

};



// ============================================================================
// The END 
// ============================================================================
