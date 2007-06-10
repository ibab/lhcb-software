// $Id: MCMatchDicts.h,v 1.1 2007-06-10 20:17:53 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_MCMATCHDICTS_H 
#define LOKI_MCMATCHDICTS_H 1
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
#include "LoKi/MCMatchObj.h"
#include "LoKi/MCMatch.h"
// ============================================================================
namespace LoKi 
{
  namespace Dicts 
  {
    /** @namespace  LoKi::Dicst::MCMatchDicts MCMatchDicts.h LoKi/MCMatchDicts.h
     *  @author Ivan BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-06
     */
    namespace MCMatchDicts 
    {
      /// valid object?
      bool valid ( const LoKi::MCMatch& m ) ;
      /// does this MC particle make contribution to this RC particle?
      bool match 
      ( const LoKi::MCMatch&    m  ,
        const LHCb::Particle*   p  ,
        const LHCb::MCParticle* mc ) ;
      /// do these MC particles make contribution to this RC particle?    
      bool match 
      ( const LoKi::MCMatch&        m  ,
        const LHCb::Particle*       p  ,
        const LoKi::Types::MCRange& mc ) ;
      /// do these MC particles make contribution to this RC particle?    
      bool match 
      ( const LoKi::MCMatch&        m  ,
        const LHCb::Particle*       p  ,
        const LHCb::MCParticle::Vector& mc ) ;
      /// do these MC particles make contribution to this RC particle?    
      bool match 
      ( const LoKi::MCMatch&        m  ,
        const LHCb::Particle*       p  ,
        const LHCb::MCParticle::ConstVector& mc ) ;
      /// do these MC particles make contribution to this RC particle?
      bool match 
      ( const LoKi::MCMatch&        m  ,
        const LHCb::Particle*       p  ,
        const LoKi::Keeper<LHCb::MCParticle>& mc ) ;
      /// do these MC particles make contribution to this RC particle?    
      bool match 
      ( const LoKi::MCMatch&        m  ,
        const LHCb::Particle*       p  ,
        const LoKi::UniqueKeeper<LHCb::MCParticle>& mc ) ;
    } // end of namespace LoKi::Dicts::MCMatchDicts
  } // end of namespace LoKi::Dicts 
} // end of namespace LoKi
// ============================================================================
// The END
// ============================================================================
#endif // LOKI_MCMATCHDICTS_H
// ============================================================================
