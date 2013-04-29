// $Id$
// ============================================================================
#ifndef LOKI_GENSECTIONS_H 
#define LOKI_GENSECTIONS_H 1
// ============================================================================
// Include files
// ============================================================================
// Event 
// ============================================================================
#include "Event/HepMCEvent.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/GenChild.h"
// ============================================================================
/** @file LoKi/GenSections.h
 *
 *  Collection of varios functions related to the "sections" for 
 *  the certain graph/tree
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya  BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2008-04-29
 */
// ============================================================================
namespace Decays
{
  // ========================================================================
  /// the actual type for the graph/tree section 
  typedef std::vector<const HepMC::GenParticle*> GenSection ;
  // ========================================================================
  /// the actual type for collection of sections  
  typedef std::vector<GenSection>        GenSections ;
  // ========================================================================
  /** simple function to extarct all section from the graph/tree 
   *  @param particle the head of th egraph/tree 
   *  @param output the sections of the graph/tree 
   *  @return size of the section container 
   */
  size_t sections 
  ( const HepMC::GenParticle* particle , GenSections& output ) ;
  // ========================================================================
  /** simple function to extarct all section from the graph/tree 
   *  @param particle the head of th egraph/tree 
   *  @return sice of the section container 
   */
  inline GenSections sections ( const HepMC::GenParticle* particle )
  {
    GenSections result ;
    sections ( particle , result ) ;
    return result ;
  }
  // ========================================================================
  /** get the most trivial section (daughters) 
   *  @author Vanya  BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date   2008-05-25
   */ 
  inline size_t section
  ( const HepMC::GenParticle* particle , 
    GenSection&               output   )
  {
    return LoKi::GenChild::daughters ( particle , output ) ; 
  }
  // ========================================================================
  /** get the most trivial section (daughters) 
   *  @author Vanya  BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date   2008-05-25
   */ 
  inline GenSection section ( const HepMC::GenParticle* particle )  
  {
    GenSection result ;
    section ( particle , result ) ;
    return result ;
  }
  // ========================================================================      
} // end of namespace Decays 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_GENSECTIONS_H
// ============================================================================
