// $Id: GetHepMC.cpp,v 1.2 2007-06-03 20:39:38 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// HepMC 
// ============================================================================
//#include "HepMC/GenParticle.h"
//#include "HepMC/GenVertex.h"
//#include "HepMC/GenEvent.h"
// ============================================================================
// Local
// ============================================================================
#include "LoKi/GetHepMC.h"
#include "LoKi/GenChild.h"
// ============================================================================
/** @file
 *  Implementation file for class LoKi::GetHepMC
 *  @date 2007-05-26 : Ivan BELYAE
 *  @author Vana BELYAEV ibelyaev@physics.syr.edu
 */
// ============================================================================
/*  get all particles form the given vertex form the given range 
 *  @see HepMC::GenVertex::particles_begin
 *  @see HepMC::GenVertex::particles_end
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2007-05-26
 */
// ============================================================================
std::vector<const HepMC::GenParticle*> 
LoKi::GetHepMC::particles 
( const HepMC::GenVertex*    vertex , 
  const HepMC::IteratorRange range  )
{ return LoKi::Child::particles ( vertex , range ) ; }
// ============================================================================
/*  get all "in"-particles for the given vertex 
 *  @see HepMC::GenVertex::particles_in_const_begin() 
 *  @see HepMC::GenVertex::particles_in_const_end()
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2007-05-26
 */
// ============================================================================
std::vector<const HepMC::GenParticle*>
LoKi::GetHepMC::particles_in 
( const HepMC::GenVertex* vertex ) 
{ return LoKi::Child::particles_in ( vertex ) ; }
// ============================================================================
/*  get all "out"-particles for the given vertex  
 *  @see HepMC::GenVertex::particles_in_const_begin() 
 *  @see HepMC::GenVertex::particles_in_const_end()
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2007-05-26
 */
// ============================================================================
std::vector<const HepMC::GenParticle*>
LoKi::GetHepMC::particles_out    ( const HepMC::GenVertex* vertex ) 
{ return LoKi::Child::particles_out ( vertex ) ; }
// ============================================================================
/*  get all "parents" particles form the given vertex
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2007-05-26
 */
// ============================================================================
std::vector<const HepMC::GenParticle*> 
LoKi::GetHepMC::parents      ( const HepMC::GenVertex*    vertex ) 
{ return LoKi::Child::parents ( vertex ) ; }
// ============================================================================
/*  get all "children" particles form the given vertex
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2007-05-26
 */    
// ============================================================================
std::vector<const HepMC::GenParticle*> 
LoKi::GetHepMC::children     ( const HepMC::GenVertex*    vertex ) 
{ return LoKi::Child::children ( vertex ) ; }
// ============================================================================
/*  get all "family" particles form the given vertex
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2007-05-26
 */    
// ============================================================================
std::vector<const HepMC::GenParticle*> 
LoKi::GetHepMC::family        ( const HepMC::GenVertex*    vertex ) 
{ return LoKi::Child::family ( vertex ) ; }
// ============================================================================
/*  get all "ancestors" particles form the given vertex
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2007-05-26
 */    
// ============================================================================
std::vector<const HepMC::GenParticle*> 
LoKi::GetHepMC::ancestors     ( const HepMC::GenVertex*    vertex ) 
{ return LoKi::Child::ancestors ( vertex ) ; }
// ============================================================================
/*  get all "descendants" particles form the given vertex
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2007-05-26
 */    
// ============================================================================
std::vector<const HepMC::GenParticle*> 
LoKi::GetHepMC::descendants   ( const HepMC::GenVertex*    vertex ) 
{ return LoKi::Child::descendants ( vertex ) ; }
// ============================================================================
/** get all "relatives" particles form the given vertex
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2007-05-26
 */    
// ============================================================================
std::vector<const HepMC::GenParticle*> 
LoKi::GetHepMC::relatives     ( const HepMC::GenVertex*    vertex ) 
{ return LoKi::Child::relatives ( vertex ) ; }
// ============================================================================
/*  get all particles form the given event 
 *  @see HepMC::GenEvent::particles_begin
 *  @see HepMC::GenEvent::particles_end
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2007-05-26
 */
// ============================================================================
std::vector<const HepMC::GenParticle*> 
LoKi::GetHepMC::particles_all ( const HepMC::GenEvent* event )  
{ return LoKi::Child::particles_all ( event ) ; }
// ============================================================================
/** get all vertices form the given event 
 *  @see HepMC::GenEvent::vertices_begin
 *  @see HepMC::GenEvent::vertices_end
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2007-05-26
 */
// ============================================================================
std::vector<const HepMC::GenVertex*> 
LoKi::GetHepMC::vertices_all ( const HepMC::GenEvent* event )  
{ return LoKi::Child::vertices_all ( event ) ; }
// ============================================================================
/*  get all "children" particles form the given particle 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2007-05-26
 */    
// ============================================================================
std::vector<const HepMC::GenParticle*> 
LoKi::GetHepMC::children   ( const HepMC::GenParticle* particle ) 
{ return LoKi::Child::children ( particle ) ; }
// ============================================================================
/*  get all "ancestors" particles form the givel particlle 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2007-05-26
 */    
// ============================================================================
std::vector<const HepMC::GenParticle*> 
LoKi::GetHepMC::ancestors  ( const HepMC::GenParticle* particle ) 
{ return LoKi::Child::ancestors ( particle ) ; } 
// ============================================================================
/*  get all "descendant" particles form the given particle 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2007-05-26
 */    
// ============================================================================
std::vector<const HepMC::GenParticle*> 
LoKi::GetHepMC::descendants ( const HepMC::GenParticle* particle ) 
{ return LoKi::Child::descendants ( particle ) ; }
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
