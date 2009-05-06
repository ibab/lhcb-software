// $Id: MCTreeFactory.h,v 1.1 2009-05-06 20:27:54 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_MCTREEFACTORY_H 
#define LOKI_MCTREEFACTORY_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/iTree.h"
#include "LoKi/Trees.h"
// ============================================================================
// forward declarations 
namespace LHCb { class MCParticle ; }
// ============================================================================
namespace Decays 
{
  namespace Trees
  {
    // ========================================================================
    /** "Factory" to create the proper Tree from the full description 
     *   @param tree       (OUTPUT) the constructed tree 
     *   @param mother     (INPUT)  the mother particle 
     *   @param oscillated (INPUT)  the oscillation flag 
     *   @param arrow      (INPUT)  the type of arrow 
     *   @param daughters  (INPUT)  the list of daughter trees 
     *   @param inclusive  (INPUT)  the flag for inclusive 
     *   @param optional   (INPUT)  the list of "optional" particles 
     *   @return StatusCode 
     *   @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *   @date   2009-05-06 
     */     
    StatusCode factory 
    ( Decays::Trees::Types_<const LHCb::MCParticle*>::Tree&           tree       ,
      const Decays::iNode&                                            mother     , 
      const Decays::Trees::Oscillation&                               oscillated ,
      const Decays::Trees::Arrow&                                     arrow      ,
      const Decays::Trees::Types_<const LHCb::MCParticle*>::SubTrees& daughters  , 
      const bool                                                      inclusive  ,
      const Decays::Trees::Types_<const LHCb::MCParticle*>::SubTrees& optional   ) ;
    // ========================================================================
    /** "Factory" to create the proper Tree from "short" descriptor
     *   @param tree       (OUTPUT) the constructed tree 
     *   @param mother     (INPUT)  the mother particle 
     *   @return status code 
     *   @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *   @date   2009-05-06 
     */     
    StatusCode factory 
    ( Decays::Trees::Types_<const LHCb::MCParticle*>::Tree& tree       ,
      const Decays::iNode&                                  mother     ) ;
    // ========================================================================
  } // end of namespace Decays::Trees::
  // ==========================================================================
} // end of namespace Decays 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_MCTREEFACTORY_H
// ============================================================================
