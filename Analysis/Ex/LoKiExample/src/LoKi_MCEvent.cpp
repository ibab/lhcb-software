// $Id: LoKi_MCEvent.cpp,v 1.2 2004-06-27 14:34:13 ibelyaev Exp $
// ============================================================================
// $CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2004/04/09 07:23:44  ibelyaev
//  add 3 new examples
// 
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
  const GenHeader*  gen = get<GenHeader>  ( GenHeaderLocation::Default ) ;
  
  // get collisions object 
  const Collisions* cls = get<Collisions> ( CollisionLocation::Default ) ;
  
  // select all MC beauty hadrons 
  MCRange beauty  = mcselect ( "Beauty" , BEAUTY ) ;
  const size_t Nb = beauty.size() ;
  
  // select all MC charm hadrons 
  MCRange charm   = mcselect ( "Charm"  , CHARM  ) ;
  const size_t Nc = charm.size() ;
  
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
