// $Id: MCMatchAlgoDicts.h,v 1.1 2007-06-10 20:21:43 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_MCMATCHALGODICTS_H 
#define LOKI_MCMATCHALGODICTS_H 1
// ============================================================================
// Include files 
// ============================================================================
// Event 
// ============================================================================
#include "Event/MCParticle.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Keeper.h"
#include "LoKi/UniqueKeeper.h"
#include "LoKi/MCTypes.h"
#include "LoKi/MCMatch.h"
#include "LoKi/Loop.h"
// ============================================================================
namespace LoKi 
{
  namespace Dicts 
  {
    namespace MCMatchDicts 
    {
      /// do these MC particles make contribution to this RC particle?    
      bool match 
      ( const LoKi::MCMatch&    m  ,
        const LoKi::Loop&       l  ,
        const LHCb::MCParticle* mc ) ;
      /// do these MC particles make contribution to this RC particle?
      bool match 
      ( const LoKi::MCMatch&        m  ,
        const LoKi::Loop&           l  ,
        const LoKi::Types::MCRange& mc ) ;
      /// do these MC particles make contribution to this RC particle?    
      bool match 
      ( const LoKi::MCMatch&        m  ,
        const LoKi::Loop&           l  ,
        const LHCb::MCParticle::Vector& mc ) ;
      /// do these MC particles make contribution to this RC particle?    
      bool match 
      ( const LoKi::MCMatch&        m  ,
        const LoKi::Loop&           l  ,
        const LHCb::MCParticle::ConstVector& mc ) ;
      /// do these MC particles make contribution to this RC particle?
      bool match 
      ( const LoKi::MCMatch&        m  ,
        const LoKi::Loop&           l  ,
        const LoKi::Keeper<LHCb::MCParticle>& mc ) ;
      /// do these MC particles make contribution to this RC particle?    
      bool match 
      ( const LoKi::MCMatch&        m  ,
        const LoKi::Loop&           l  ,
        const LoKi::UniqueKeeper<LHCb::MCParticle>& mc ) ;
    } // end of namespace LoKi::Dicts::MCMatchDicts
  } // end of namespace LoKi::Dicts
} // end of namespace LoKi
// =============================================================================
// The END 
// =============================================================================
#endif // LOKI_MCMATCHALGODICTS_H
// =============================================================================
