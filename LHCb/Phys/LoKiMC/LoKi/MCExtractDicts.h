// $Id: MCExtractDicts.h,v 1.4 2009-07-12 12:34:58 ibelyaev Exp $
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
  // ==========================================================================
  namespace MCExtract
  {
    // ========================================================================
    /// get MC-particles form decay tree 
    LHCb::MCParticle::ConstVector
    mcParticles
    ( const LHCb::MCParticle*      particle           ,
      const LoKi::MCTypes::MCCuts& cuts               ,
      const bool                   decayOnly = true   ) ;
    // ========================================================================
    /// get MC-particles from decay tree 
    LHCb::MCParticle::ConstVector
    mcParticles 
    ( const LHCb::MCVertex*        vertex             ,
      const LoKi::MCTypes::MCCuts& cuts               ,
      const bool                   decayOnly = true   ) ;
    // ========================================================================
  } //                                         end of namespace LoKi::MCExtract
  // ==========================================================================
} //                                                     end of namespacde LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_MCEXTRACTDICTS_H
// ============================================================================
