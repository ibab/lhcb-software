// $Id: GenSections.cpp,v 1.1 2008-05-04 15:20:49 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <vector>
// ============================================================================
// Event 
// ============================================================================
#include "Event/HepMCEvent.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/GenChild.h"
#include "LoKi/Combiner.h"
#include "LoKi/GenSections.h"
// ============================================================================
/** @file 
 *  Implementation file for function LoKi::Decay::sections 
 *  @author Vanya BELYAEV Ivan.Belayev@nikhef.nl
 *  @date 2008-04-29 
 */
// ============================================================================
/*  simple function to extract all section from the graph/tree 
 *  @param particle the head of th egraph/tree 
 *  @param output the sections of the graph/tree 
 *  @param decay the boolean flag to consider <b>only</b> decay products
 *  @return size of the section container 
 */
// ============================================================================
/*  simple function to extarct all section from the graph/tree 
 *  @param particle the head of th egraph/tree 
 *  @param output the sections of the graph/tree 
 *  @return size of the section container 
 */
size_t LoKi::Decay::sections 
( const HepMC::GenParticle* particle , 
  LoKi::Decay::GenSections& output   ) 
{
  // trivial case 1
  if ( 0 == particle )                   { return output.size() ; } // RETURN  
  // add the particle itself as one of the section 
  output.push_back ( GenSection ( 1 , particle ) ) ;  // the primitive section
  if ( 0 == particle->end_vertex() )     { return output.size() ; } // RETURN  
  // get all children particles:
  typedef std::vector<const HepMC::GenParticle*> Daughters ;
  Daughters children ;
  LoKi::GenChild::daughters ( particle , children ) ;
  if ( children.empty() )                { return output.size () ; } // RETURN
  // get all sections from daughetr particles:
  typedef std::vector<GenSections>  SECTIONS ;
  SECTIONS all ( children.size() ) ;
  for ( size_t index = 0 ; index < children.size() ; ++index )
  { sections ( children[index] , all[index] ) ; }                // RECURSION
  //
  typedef LoKi::Combiner_<GenSections>  Loop   ;
  typedef Loop::Range                   Range  ;
  typedef Loop::Select                  Select ;
  // create the proper combiner 
  Loop loop ;
  // fill the combiner with the information
  for ( SECTIONS::const_iterator it = all.begin() ; all.end() != it ; ++it )
  { loop.add ( Range ( *it ) ) ; }                           // Fill the LOOP!
  /// 
  // make the real looping over all combinations of sections 
  for ( ; loop.valid() ; ++loop ) 
  {
    // get the "current" multi-iterator
    const Select& sel = loop.current() ;
    // construct the section
    GenSection section ;
    for ( Select::const_iterator it = sel.begin() ; sel.end() != it ; ++it )
    { section.insert ( section.end() , (*it)->begin() , (*it)->end() ); }
    // add it into the collection of sections 
    output.push_back ( section ) ;
  }
  // 
  return output.size () ;                                          // RETURN 
}
// ============================================================================
// The END 
// ============================================================================
