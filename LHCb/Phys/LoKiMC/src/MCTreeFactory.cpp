// $Id: MCTreeFactory.cpp,v 1.1 2009-05-06 20:27:54 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $
// ============================================================================
// Include files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/iTree.h"
#include "LoKi/Trees.h"
#include "LoKi/MCTreeFactory.h"
#include "LoKi/MCDecays.h"
// ============================================================================
/** @file 
 *   Implementation file for function from the file LoKi/MCTreeFactory.h
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
( Decays::Trees::Types_<const LHCb::MCParticle*>::Tree& tree   ,
  const Decays::iNode&                                  mother ) 
{
  tree = Decays::Trees::MCExclusive ( mother ) ;
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
( Decays::Trees::Types_<const LHCb::MCParticle*>::Tree&           tree       ,
  const Decays::iNode&                                            mother     , 
  const Decays::Trees::Oscillation&                               oscillated ,
  const Decays::Trees::Arrow&                                     arrow      ,
  const Decays::Trees::Types_<const LHCb::MCParticle*>::SubTrees& daughters  , 
  const bool                                                      inclusive  ,
  const Decays::Trees::Types_<const LHCb::MCParticle*>::SubTrees& optional   ) 
{
  if ( !Decays::Trees::valid( arrow ) ) 
  { return StatusCode ( Decays::Trees::InvalidArrow ) ; }             // RETURN
  
  // decode the arrow 
  const Decays::Trees::Alg alg       = Decays::Trees::algorithm ( arrow ) ;
  const bool               photos    = Decays::Trees::photos    ( arrow ) ;
  const bool               decayOnly = Decays::Trees::decayOnly ( arrow ) ;
  
  if ( inclusive && !optional.empty() ) 
  { return StatusCode ( Decays::Trees::InclusiveOptional ) ; }        // RETURN
  if ( inclusive && photos            ) 
  { return StatusCode ( Decays::Trees::InclusivePhotos   ) ; }        // RETURN
  
  // create the basic decay tree: 
  Decays::Trees::MCExclusive decay ( mother     , 
                                     daughters  ,
                                     alg        , 
                                     decayOnly  , 
                                     oscillated ) ;
  
  if ( optional.empty() ) 
  {
    if      ( !inclusive )                                   // exclusive decays 
    {
      if  ( photos ) { tree = Decays::Trees::Photos ( decay ) ; } // + photos 
      else           { tree =                         decay   ; }
      return StatusCode::SUCCESS ;
    }
    else                                                    // inclusive decays 
    {
      tree = Decays::Trees::MCInclusive ( decay ) ;
      return StatusCode::SUCCESS ;
    }
  }
  else                                          // decays with optional elements 
  {
    Decays::Trees::MCOptional opt ( decay , optional ) ;
    if  ( photos ) { tree = Decays::Trees::PhotosOptional ( opt) ; } // + photos 
    else           { tree =                                 opt  ; }
    return StatusCode::SUCCESS ;
  }
  
  //
  return StatusCode ( Decays::Trees::InvalidBranch ) ;
}

// ============================================================================
// The END 
// ============================================================================
