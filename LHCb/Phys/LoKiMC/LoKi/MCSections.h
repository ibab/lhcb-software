// $Id: MCSections.h,v 1.1 2008-05-04 15:23:29 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_MCSECTIONS_H 
#define LOKI_MCSECTIONS_H 1
// ============================================================================
// Include files
// ============================================================================
// Event 
// ============================================================================
#include "Event/MCParticle.h"
// ============================================================================
/** @file
 *  Collection of varios functions related to the "sections" for 
 *  the certain graph/tree
 *  @author Vanya  BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2008-04-29
 */
namespace LoKi 
{
  namespace Decay 
  {
    // ========================================================================
    /// the actual type for the graph/tree section 
    typedef LHCb::MCParticle::ConstVector MCSection ;
    // ========================================================================
    /// the actual type for collection of sections  
    typedef std::vector<MCSection>        MCSections ;
    // ========================================================================
    /** simple function to extract all section from the graph/tree 
     *  @param particle the head of th egraph/tree 
     *  @param output the sections of the graph/tree 
     *  @param decay the boolean flag to consider <b>only</b> decay products
     *  @return size of the section container 
     */
    size_t sections 
    ( const LHCb::MCParticle* particle        , 
      MCSections&             output          , 
      const bool              decay    = true ) ;
    // ========================================================================
    /** simple function to extract all section from the graph/tree 
     *  @param particle the head of th egraph/tree 
     *  @param decay the boolean flag to consider <b>only</b> decay products
     *  @return sice of the section container 
     */
    inline MCSections sections 
    ( const LHCb::MCParticle* particle        , 
      const bool              decay    = true ) 
    {
      MCSections result ;
      sections ( particle , result , decay ) ;
      return result ;
    }
    // ========================================================================      
  } // end of namespace LoKi::Decay 
  // ==========================================================================
} // end of namespace LoKi 
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_MCSECTIONS_H
// ============================================================================
