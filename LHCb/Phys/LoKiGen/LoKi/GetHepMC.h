// $Id: GetHepMC.h,v 1.2 2007-06-03 20:39:37 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $
// ============================================================================
//  $Log: not supported by cvs2svn $
//  Revision 1.1  2007/06/01 11:48:06  ibelyaev
//  prepare for v3r0
// 
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
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenEvent.h"
// ============================================================================
namespace LoKi 
{
  /** @class GetHepMC
   *  The helper class to make iteration in python for HepMC graphs 
   *  @author Vana BELYAEV ibelyaev@physics.syr.edu
   *  @date 2007-05-26
   */
  class GetHepMC
  {
  public:
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
    /** get all "parents" particles form the given vertex
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-05-26
     */
    static 
    std::vector<const HepMC::GenParticle*> 
    parents
    ( const HepMC::GenVertex*    vertex ) ;
    /** get all "children" particles form the given vertex
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-05-26
     */    
    static 
    std::vector<const HepMC::GenParticle*> 
    children
    ( const HepMC::GenVertex*    vertex ) ;
    /** get all "children" particles form the given particle 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-05-26
     */    
    static 
    std::vector<const HepMC::GenParticle*> 
    children 
    ( const HepMC::GenParticle* particle ) ;
    /** get all "family" particles form the given vertex
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-05-26
     */    
    static 
    std::vector<const HepMC::GenParticle*> 
    family
    ( const HepMC::GenVertex*    vertex ) ;
    /** get all "ancestors" particles form the given vertex
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-05-26
     */    
    static 
    std::vector<const HepMC::GenParticle*> 
    ancestors
    ( const HepMC::GenVertex*    vertex ) ;
    /** get all "ancestors" particles form the givel particlle 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-05-26
     */    
    static 
    std::vector<const HepMC::GenParticle*> 
    ancestors 
    ( const HepMC::GenParticle* particle ) ;
    /** get all "descendants" particles form the given vertex
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-05-26
     */    
    static 
    std::vector<const HepMC::GenParticle*> 
    descendants
    ( const HepMC::GenVertex*    vertex ) ;
    /** get all "descendant" particles form the given particle 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-05-26
     */    
    static 
    std::vector<const HepMC::GenParticle*> 
    descendants 
    ( const HepMC::GenParticle* particle ) ;
    /** get all "relatives" particles form the given vertex
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-05-26
     */    
    static 
    std::vector<const HepMC::GenParticle*> 
    relatives
    ( const HepMC::GenVertex*    vertex ) ;
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
  } ;
} // end of namespace LoKi 

// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_GETHEPMC_H
// ============================================================================
