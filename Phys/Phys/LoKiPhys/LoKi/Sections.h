// $Id: Sections.h,v 1.1 2008-05-04 15:26:25 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_SECTIONS_H 
#define LOKI_SECTIONS_H 1
// ============================================================================
// Include files
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
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
    typedef LHCb::Particle::ConstVector Section ;
    // ========================================================================
    /// the actual type for collection of sections  
    typedef std::vector<Section>        Sections ;
    // ========================================================================
    /** simple function to extarct all section from the graph/tree 
     *  @param particle the head of th egraph/tree 
     *  @param output the sections of the graph/tree 
     *  @return size of the section container 
     */
    size_t sections 
    ( const LHCb::Particle* particle , Sections& output ) ;
    // ========================================================================
    /** simple function to extarct all section from the graph/tree 
     *  @param particle the head of th egraph/tree 
     *  @return sice of the section container 
     */
    inline Sections sections ( const LHCb::Particle* particle )
    {
      Sections result ;
      sections ( particle , result ) ;
      return result ;
    }
    // ========================================================================      
  } // end of namespace LoKi::Decay 
  // ==========================================================================
} // end of namespace LoKi 
// ============================================================================
#endif // LOKI_SECTIONS_H
// ============================================================================
