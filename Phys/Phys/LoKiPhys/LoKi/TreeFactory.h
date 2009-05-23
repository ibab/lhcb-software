// $Id: TreeFactory.h,v 1.2 2009-05-23 15:59:51 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_TREEFACTORY_H 
#define LOKI_TREEFACTORY_H 1
// ============================================================================
// CVS Tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/iTree.h"
#include "LoKi/Trees.h"
// ============================================================================
// forward declarations 
// ============================================================================
namespace LHCb { class Particle ; }
// ============================================================================
namespace Decays 
{
  // ========================================================================== 
  namespace Parsers { class Tree ; }
  // ========================================================================== 
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
     *   @param stream stream to report the errors 
     *   @return status code 
     *   @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *   @date   2009-05-06 
     */     
    StatusCode factory 
    ( Decays::Trees::Types_<const LHCb::Particle*>::Tree&           tree       ,
      const Decays::iNode&                                          mother     , 
      const Decays::Trees::Oscillation&                             oscillated ,
      const Decays::Trees::Arrow&                                   arrow      ,
      const Decays::Trees::Types_<const LHCb::Particle*>::TreeList& daughters  , 
      const bool                                                    inclusive  ,
      const Decays::Trees::Types_<const LHCb::Particle*>::TreeList& optional   , 
      std::ostream&                                                 stream     ) ;
    // ========================================================================
    /** "Factory" to create the proper Tree from "short" descriptor
     *   @param tree       (OUTPUT) the constructed tree 
     *   @param mother     (INPUT)  the mother particle 
     *   @return status code 
     *   @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *   @date   2009-05-06 
     */     
    StatusCode factory 
    ( Decays::Trees::Types_<const LHCb::Particle*>::Tree& tree       ,
      const Decays::iNode&                                mother     , 
      const Decays::Trees::Oscillation&                   oscillated ) ;
    // ========================================================================
    /** "Factory" to create the proper Tree from the parsed tree 
     *   @param tree       (OUTPUT) the constructed tree 
     *   @param parsed     (INPUT)  the parsed tree  
     *   @param stream     (OUTPUT) the stream to report errors 
     *   @return status code  
     *   @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *   @date   2009-05-22 
     */     
    StatusCode factory 
    ( Decays::Trees::Types_<const LHCb::Particle*>::Tree& tree   ,
      const Decays::Parsers::Tree&                        parsed ,
      std::ostream&                                       stream ) ;
    // ========================================================================
  } // end of namespace Decays::Trees
  // ==========================================================================
} // end of namespace Decays 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_TREEFACTORY_H
// ============================================================================
