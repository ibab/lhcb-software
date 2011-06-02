// $Id$
// ============================================================================
#ifndef LOKI_MCSECTIONS_H 
#define LOKI_MCSECTIONS_H 1
// ============================================================================
// Include files
// ============================================================================
// Event 
// ============================================================================
#include "Event/MCParticle.h"
#include "LoKi/MCChild.h"
// ============================================================================
/** @file
 *  Collection of varios functions related to the "sections" for 
 *  the certain graph/tree
 *  @author Vanya  BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2008-04-29
 */
// ============================================================================
namespace Decays 
{
  // ==========================================================================
  /// the actual type for the graph/tree section 
  typedef LHCb::MCParticle::ConstVector MCSection ;
  // ==========================================================================
  /// the actual type for collection of sections  
  typedef std::vector<MCSection>        MCSections ;
  // ==========================================================================
  /** simple function to extract all section from the graph/tree 
   *  @param particle the head of th egraph/tree 
   *  @param output the sections of the graph/tree 
   *  @param decay the boolean flag to consider <b>only</b> decay products
   *  @return size of the section container 
   *  @author Vanya  BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date   2008-04-29
   */
  GAUDI_API 
  size_t sections 
  ( const LHCb::MCParticle* particle        , 
    MCSections&             output          , 
    const bool              decay    = true ) ;
  // ==========================================================================
  /** simple function to extract all section from the graph/tree 
   *  @param particle the head of the graph/tree 
   *  @param decay the boolean flag to consider <b>only</b> decay products
   *  @return sice of the section container 
   *  @author Vanya  BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date   2008-04-29
   */
  inline MCSections sections 
  ( const LHCb::MCParticle* particle        , 
    const bool              decay    = true ) 
  {
    MCSections result ;
    sections ( particle , result , decay ) ;
    return result ;
  }
  // ==========================================================================
  /** get the most trivial section (daughters) 
   *  @author Vanya  BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date   2008-05-25
   */ 
  inline size_t section
  ( const LHCb::MCParticle* particle        , 
    MCSection&              output          , 
    const bool              decay    = true ) 
  {
    return LoKi::MCChild::daughters ( particle , output , decay ) ; 
  }
  // ==========================================================================
  /** get the most trivial section (daughters) 
   *  @author Vanya  BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date   2008-05-25
   */ 
  inline MCSection section
  ( const LHCb::MCParticle* particle        , 
    const bool              decay    = true ) 
  {
    MCSection result ;
    section ( particle , result , decay ) ;
    return result ;
  }
  // ==========================================================================
} //                                                    end of namespace Decays
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_MCSECTIONS_H
// ============================================================================
