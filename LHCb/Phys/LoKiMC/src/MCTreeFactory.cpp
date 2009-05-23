// $Id: MCTreeFactory.cpp,v 1.2 2009-05-23 15:58:12 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $
// ============================================================================
// Include files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/iTree.h"
#include "LoKi/Trees.h"
#include "LoKi/MCDecays.h"
// ============================================================================
#include "LoKi/TreeHelpers.h"
#include "LoKi/MCTreeFactory.h"
#include "LoKi/ParserFactory.h"
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
 *   @param oscillated (INPUT)  the oscillation flag 
 *   @return status code 
 *   @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *   @date   2009-05-06 
 */     
// ============================================================================
StatusCode Decays::Trees::factory 
( Decays::Trees::Types_<const LHCb::MCParticle*>::Tree& tree       ,
  const Decays::iNode&                                  mother     , 
  const Decays::Trees::Oscillation&                     oscillated ) 
{
  tree = Decays::Trees::MCExclusive ( oscillated , mother ) ;
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
 *  @param stream     (OUTPUT) the stream to report errors 
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
  const Decays::Trees::Types_<const LHCb::MCParticle*>::TreeList& daughters  , 
  const bool                                                      inclusive  ,
  const Decays::Trees::Types_<const LHCb::MCParticle*>::TreeList& optional   , 
  std::ostream&                                                   stream     )
{
  if ( !Decays::Trees::valid( arrow ) ) 
  { 
    StatusCode code = StatusCode ( Decays::Trees::InvalidArrow ) ;
    stream << "ERROR: Invalid arrow : " << arrow << " " << code << std::endl ;
    return code ;
  }            
  
  // decode the arrow 
  const Decays::Trees::Alg alg       = Decays::Trees::algorithm ( arrow ) ;
  const bool               photos    = Decays::Trees::photos    ( arrow ) ;
  const bool               decayOnly = Decays::Trees::decayOnly ( arrow ) ;
  
  if ( inclusive && !optional.empty() ) 
  {
    StatusCode code = StatusCode ( Decays::Trees::InclusiveOptional ) ; 
    stream << "ERROR: 'Inclusive' & 'optional' are mutually exclusive "
           <<  code << std::endl ;
    return code ;                                                     // RETURN
  }        
  if ( inclusive && photos            ) 
  { 
    StatusCode code = StatusCode ( Decays::Trees::InclusivePhotos   ) ; 
    stream << "ERROR: 'Inclusive' & 'photos' are mutually exclusive   "
           << code << std::endl ;
    return code ;                                                      // RETURN
  }        
  
  // create the basic decay tree: 
  Decays::Trees::MCExclusive decay ( mother     , 
                                     daughters  ,
                                     alg        , 
                                     decayOnly  , 
                                     oscillated ) ;
  // final decoration
  if ( optional.empty() ) 
  {
    if      ( !inclusive )                                   // exclusive decays 
    {
      if  ( photos ) { tree = Decays::Trees::Photos ( decay ) ; } // + photos 
      else           { tree =                         decay   ; }
    }
    else { tree = Decays::Trees::MCInclusive ( decay ) ; }   // inclusive decays
  }
  else                                          // decays with optional elements 
  {
    Decays::Trees::MCOptional opt ( decay , optional ) ;
    if  ( photos ) { tree = Decays::Trees::PhotosOptional ( opt) ; } // + photos 
    else           { tree =                                 opt  ; } // optional
  }  
  //
  return StatusCode::SUCCESS ;
}
// ===========================================================================
/*  "Factory" to create the proper Tree from the parsed tree 
 *   @param tree       (OUTPUT) the constructed tree 
 *   @param parsed     (INPUT)  the parsed tree  
 *   @return status code  
 *   @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *   @date   2009-05-22 
 */     
// ============================================================================
StatusCode Decays::Trees::factory 
( Decays::Trees::Types_<const LHCb::MCParticle*>::Tree& tree   ,
  const Decays::Parsers::Tree&                          parsed ,
  std::ostream&                                         stream ) 
{ 
  return Decays::Parsers::factory ( tree , parsed , stream ) ;        // RETURN 
}
// ============================================================================



// ============================================================================
// The END 
// ============================================================================
