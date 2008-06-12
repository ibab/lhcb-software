// $Id: Sections.cpp,v 1.2 2008-06-12 08:25:27 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <vector>
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Combiner.h"
#include "LoKi/Sections.h"
// ============================================================================
/** @file 
 *  Implementation file for function LoKi::Decay::sections 
 *  @author Vanay BELYAEV Ivan.Belayev@nikhef.nl
 *  @date 2008-04-29 
 */
// ============================================================================
/*  simple function to extract all section from the graph/tree 
 *  @param particle the head of the graph/tree 
 *  @param output the sections of the graph/tree 
 *  @return size of the section container 
 */
// ============================================================================
size_t LoKi::Decays::sections 
( const LHCb::Particle*   particle , 
  LoKi::Decays::Sections& output   ) 
{
  // trivial case 1
  if ( 0 == particle ) { return output.size() ; }                   // RETURN  
  typedef SmartRefVector<LHCb::Particle> Daughters ;
  const Daughters& daughters = particle->daughters() ;
  // add the particle itself as one of the section 
  output.push_back( Section ( 1 , particle ) ) ;  // the primitive section
  // trivial case 2 
  if ( daughters.empty() ) { return output.size() ; }               // RETURN 
  // get all sections form the daughter particles:
  typedef std::vector<Sections> SECTIONS ;
  SECTIONS all ( daughters.size() ) ;
  for ( size_t index = 0 ; index < daughters.size() ; ++index )
  { sections ( daughters[index] , all[index] ) ; }               // RECURSION!
  // 
  typedef LoKi::Combiner_<Sections>  Loop   ;
  typedef Loop::Range                Range  ;
  typedef Loop::Select               Select ;
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
    Section section ;
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
