// $Id$
// ============================================================================
#ifndef LOKI_GETHEPMC_H 
#define LOKI_GETHEPMC_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <vector>
// ============================================================================
// HepMC 
// ============================================================================
#ifdef __INTEL_COMPILER
#pragma warning (disable:1572) // floating-point equality and inequality comparisons are unreliable
#pragma warning(push)
#endif
// ============================================================================
// HepMC 
// ============================================================================
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenEvent.h"
// ============================================================================
#ifdef __INTEL_COMPILER
#pragma warning(pop)
#endif
// ============================================================================
/** @file LoKi/GetHepMC.h
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23 
 *
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
namespace LoKi 
{
  // ==========================================================================
  /** @class GetHepMC
   *  The helper class to make iteration in python for HepMC graphs 
   *  @author Vana BELYAEV ibelyaev@physics.syr.edu
   *  @date 2007-05-26
   */
  class GetHepMC
  {
  public:
    // ========================================================================
    /** get all particles form the given vertex form the given range 
     *  @see HepMC::GenVertex::particles_begin
     *  @see HepMC::GenVertex::particles_end
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-05-26
     */
    static 
    std::vector<const HepMC::GenParticle*> 
    particles 
    ( const HepMC::GenVertex*    vertex , 
      const HepMC::IteratorRange range  ) ;
    // ========================================================================
    /** get all "in"-particles for the given vertex  
     *  @see HepMC::GenVertex::particles_in_const_begin() 
     *  @see HepMC::GenVertex::particles_in_const_end()
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-05-26
     */
    static
    std::vector<const HepMC::GenParticle*>
    particles_in 
    ( const HepMC::GenVertex* vertex ) ;
    // ========================================================================
    /** get all "out"-particles for the given vertex  
     *  @see HepMC::GenVertex::particles_in_const_begin() 
     *  @see HepMC::GenVertex::particles_in_const_end()
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-05-26
     */
    static
    std::vector<const HepMC::GenParticle*>
    particles_out 
    ( const HepMC::GenVertex* vertex ) ;
    // ========================================================================
    /** get all "parents" particles form the given vertex
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-05-26
     */
    static 
    std::vector<const HepMC::GenParticle*> 
    parents
    ( const HepMC::GenVertex*    vertex ) ;
    // ========================================================================
    /** get all "children" particles form the given vertex
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-05-26
     */    
    static 
    std::vector<const HepMC::GenParticle*> 
    children
    ( const HepMC::GenVertex*    vertex ) ;
    // ========================================================================
    /** get all "children" particles form the given particle 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-05-26
     */    
    static 
    std::vector<const HepMC::GenParticle*> 
    children 
    ( const HepMC::GenParticle* particle ) ;
    // ========================================================================
    /** get all "family" particles form the given vertex
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-05-26
     */    
    static 
    std::vector<const HepMC::GenParticle*> 
    family
    ( const HepMC::GenVertex*    vertex ) ;
    // ========================================================================
    /** get all "ancestors" particles form the given vertex
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-05-26
     */    
    static 
    std::vector<const HepMC::GenParticle*> 
    ancestors
    ( const HepMC::GenVertex*    vertex ) ;
    // ========================================================================
    /** get all "ancestors" particles form the givel particlle 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-05-26
     */    
    static 
    std::vector<const HepMC::GenParticle*> 
    ancestors 
    ( const HepMC::GenParticle* particle ) ;
    // ========================================================================
    /** get all "descendants" particles form the given vertex
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-05-26
     */    
    static 
    std::vector<const HepMC::GenParticle*> 
    descendants
    ( const HepMC::GenVertex*    vertex ) ;
    // ========================================================================
    /** get all "descendant" particles form the given particle 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-05-26
     */    
    static 
    std::vector<const HepMC::GenParticle*> 
    descendants 
    ( const HepMC::GenParticle* particle ) ;
    // ========================================================================
    /** get all "relatives" particles form the given vertex
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-05-26
     */    
    static 
    std::vector<const HepMC::GenParticle*> 
    relatives
    ( const HepMC::GenVertex*    vertex ) ;
    // ========================================================================
    /** get all particles form the given event 
     *  @see HepMC::GenEvent::particles_begin
     *  @see HepMC::GenEvent::particles_end
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-05-26
     */
    static 
    std::vector<const HepMC::GenParticle*> 
    particles_all
    ( const HepMC::GenEvent* event )  ;
    // ========================================================================
    /** get all vertices form the given event 
     *  @see HepMC::GenEvent::vertices_begin
     *  @see HepMC::GenEvent::vertices_end
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-05-26
     */
    static 
    std::vector<const HepMC::GenVertex*> 
    vertices_all
    ( const HepMC::GenEvent* event )  ;  
    // ========================================================================
  } ;
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_GETHEPMC_H
// ============================================================================
