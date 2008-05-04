// $Id: GenSections.h,v 1.1 2008-05-04 15:20:49 ibelyaev Exp $
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
/** @file
 *  Collection of varios functions related to the "sections" for 
 *  the certain graph/tree
 *  @author Vanya  BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2008-04-29
 */
// ============================================================================
namespace LoKi 
{
  namespace Decay 
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
  } // end of namespace LoKi::Decay 
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_GENSECTIONS_H
// ============================================================================
