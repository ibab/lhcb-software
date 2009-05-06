// $Id: TreeFactory.cpp,v 1.1 2009-05-06 20:32:42 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/iTree.h"
#include "LoKi/Trees.h"
#include "LoKi/TreeFactory.h"
#include "LoKi/Decays.h"
// ============================================================================
/** @file
 *   Implementation file for function from the file LoKi/TreeFactory.h
 *   @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *   @date   2009-05-06
 */
// ============================================================================
/*  "Factory" to create the proper Tree from "short" descriptor
 *   @param tree       (OUTPUT) the constructed tree
 *   @param mother     (INPUT)  the mother particle
 *   @return status code
 *   @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *   @date   2009-05-06
 */
// ============================================================================
StatusCode Decays::Trees::factory
( Decays::Trees::Types_<const LHCb::Particle*>::Tree& tree   ,
  const Decays::iNode&                                mother )
{
  tree = Decays::Trees::Exclusive ( mother ) ;
  return StatusCode::SUCCESS ;
}
// ==========================================================================
/* "Factory" to create the proper Tree from the full description
 *  @param tree       (OUTPUT) the constructed tree
 *  @param mother     (INPUT)  the mother particle
 *  @param oscillated (INPUT)  the oscillation flag
 *  @param arrow      (INPUT)  the type of arrow
 *  @param daughters  (INPUT)  the list of daughter trees
 *  @param inclusive  (INPUT)  the flag for inclusive
 *  @param optional   (INPUT)  the list of "optional" particles
 *  @return StatusCode
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2009-05-06
 */
// ==========================================================================
StatusCode Decays::Trees::factory
( Decays::Trees::Types_<const LHCb::Particle*>::Tree&           tree       ,
  const Decays::iNode&                                          mother     ,
  const Decays::Trees::Oscillation&                             oscillated ,
  const Decays::Trees::Arrow&                                   arrow      ,
  const Decays::Trees::Types_<const LHCb::Particle*>::SubTrees& daughters  ,
  const bool                                                    inclusive  ,
  const Decays::Trees::Types_<const LHCb::Particle*>::SubTrees& optional   )
{
  if ( !Decays::Trees::valid( arrow ) )
  { return StatusCode ( Decays::Trees::InvalidArrow       ) ; }       // RETURN

  if ( Decays::Trees::Undefined != oscillated )
  { return StatusCode ( Decays::Trees::InvalidOscillated  ) ; }       // RETURN

  // decode the arrow
  const Decays::Trees::Alg alg       = Decays::Trees::algorithm ( arrow ) ;
  const bool               photos    = Decays::Trees::photos    ( arrow ) ;
  const bool               decayOnly = Decays::Trees::decayOnly ( arrow ) ;
  
  if ( photos )
  { return StatusCode ( Decays::Trees::InvalidPhotos      ) ; }       // RETURN
  
  if ( !decayOnly      )
  { return StatusCode ( Decays::Trees::InvalidDecayOnly   ) ; }       // RETURN
  
  
  if ( inclusive && !optional.empty() )
  { return StatusCode ( Decays::Trees::InclusiveOptional ) ; }        // RETURN

  // create the basic decay tree:
  Decays::Trees::Exclusive decay ( mother     ,
                                   daughters  ,
                                   alg        ) ;
  
  if ( optional.empty() )
  {
    if      ( !inclusive )                                   // exclusive decays
    {
      tree =                         decay   ; 
      return StatusCode::SUCCESS ;
    }
    else                                                    // inclusive decays
    {
      tree = Decays::Trees::Inclusive ( decay ) ;
      return StatusCode::SUCCESS ;
    }
  }
  else                                          // decays with optional elements
  {
    tree = Decays::Trees::Optional( decay , optional ) ;
    return StatusCode::SUCCESS ;
  }

  //
  return StatusCode ( Decays::Trees::InvalidBranch ) ;
}

// ============================================================================
// The END
// ============================================================================
