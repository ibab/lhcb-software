// $Id: MCExtractDicts.h,v 1.2 2007-08-13 10:49:21 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_MCEXTRACTDICTS_H 
#define LOKI_MCEXTRACTDICTS_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/MCTypes.h"
// ============================================================================
namespace LoKi
{
  namespace Dicts 
  {
    namespace Extract
    {
      /// get MC-particles form decay tree 
      LHCb::MCParticle::ConstVector
      mcParticles
      ( const LHCb::MCParticle*      particle           ,
        const LoKi::MCTypes::MCCuts& cuts               ,
        const bool                   decayOnly = false  ) ;
      /// get MC-particles from decay tree 
      LHCb::MCParticle::ConstVector
      mcParticles 
      ( const LHCb::MCVertex*        vertex             ,
        const LoKi::MCTypes::MCCuts& cuts               ,
        const bool                   decayOnly = false  ) ;
    } // end of namespace LoKi::Dicts::Extract
  } // end of namepace Loki::Dicts 
} // end oif namespacde LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_MCEXTRACTDICTS_H
// ============================================================================
