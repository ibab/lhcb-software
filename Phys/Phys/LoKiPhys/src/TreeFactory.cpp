// $Id: TreeFactory.cpp,v 1.2 2009-05-23 15:59:51 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/iTree.h"
#include "LoKi/Trees.h"
#include "LoKi/Decays.h"
#include "LoKi/TreeFactory.h"
#include "LoKi/ParserFactory.h"
// ============================================================================
/**  @file
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
( Decays::Trees::Types_<const LHCb::Particle*>::Tree& tree       ,
  const Decays::iNode&                                mother     ,
  const Decays::Trees::Oscillation&                   oscillated ) 
{
  if ( Decays::Trees::Undefined != oscillated )
  { return StatusCode ( Decays::Trees::InvalidOscillated ) ; }        // RETURN 
  tree = Decays::Trees::Exclusive ( mother ) ;
  return StatusCode::SUCCESS ;
}
// ============================================================================
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
// ============================================================================
StatusCode Decays::Trees::factory
( Decays::Trees::Types_<const LHCb::Particle*>::Tree&           tree       ,
  const Decays::iNode&                                          mother     ,
  const Decays::Trees::Oscillation&                             oscillated ,
  const Decays::Trees::Arrow&                                   arrow      ,
  const Decays::Trees::Types_<const LHCb::Particle*>::TreeList& daughters  ,
  const bool                                                    inclusive  ,
  const Decays::Trees::Types_<const LHCb::Particle*>::TreeList& optional   , 
  std::ostream&                                                 stream     )
{
  if ( !Decays::Trees::valid( arrow ) )
  { 
    StatusCode code = StatusCode ( Decays::Trees::InvalidArrow ) ;
    stream << "ERROR: Invalid arrow : " << arrow << " " << code << std::endl ;
    return code ;
  }
  if ( Decays::Trees::Undefined != oscillated )
  {
    StatusCode code = StatusCode ( Decays::Trees::InvalidOscillated ) ;
    stream << "ERROR: Invalid 'Oscillated' flag   " << code << std::endl ;
    return code ;                                                     // RETURN 
  }
  // decode the arrow
  const Decays::Trees::Alg alg       = Decays::Trees::algorithm ( arrow ) ;
  const bool               photos    = Decays::Trees::photos    ( arrow ) ;
  const bool               decayOnly = Decays::Trees::decayOnly ( arrow ) ;
  
  if ( photos )
  { 
    StatusCode code = StatusCode ( Decays::Trees::InvalidPhotos     ) ; 
    stream << "ERROR: Invalid 'Photos' flag                           "
           << code << std::endl ;
    return code ;                                                      // RETURN
  }        
  if ( !decayOnly      )
  {
    StatusCode code = StatusCode ( Decays::Trees::InvalidDecayOnly ) ; 
    stream << "ERROR: invalid 'Decay Only' flag                       "
           << code << std::endl ;
    return code ;                                                      // RETURN
  }  
  if ( inclusive && !optional.empty() )
  {
    StatusCode code = StatusCode ( Decays::Trees::InclusiveOptional ) ; 
    stream << "ERROR: 'Inclusive' & 'optional' are mutually exclusive "
           <<  code << std::endl ;
    return code ;                                                     // RETURN
  }        
  
  // create the basic decay tree:
  Decays::Trees::Exclusive decay ( mother     ,
                                   daughters  ,
                                   alg        ) ;
  
  if ( optional.empty() )
  {
    if      ( !inclusive )                                   // exclusive decays
    { tree =                            decay   ; }
    else                                                     // inclusive decays
    { tree = Decays::Trees::Inclusive ( decay ) ; }
  }
  else                                          // decays with optional elements
  { tree = Decays::Trees::Optional( decay , optional ) ; }
  
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
/*  "Factory" to create the proper Tree from the parsed tree 
 *   @param tree       (OUTPUT) the constructed tree 
 *   @param parsed     (INPUT)  the parsed tree  
 *   @param stream     (OUTPUT) the stream to report errors 
 *   @return status code  
 *   @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *   @date   2009-05-22 
 */ 
// ============================================================================
StatusCode Decays::Trees::factory 
( Decays::Trees::Types_<const LHCb::Particle*>::Tree& tree   ,
  const Decays::Parsers::Tree&                        parsed ,
  std::ostream&                                       stream ) 
{ 
  return Decays::Parsers::factory ( tree , parsed , stream ) ;        // RETURN 
}
// ============================================================================


// ============================================================================
// The END
// ============================================================================
