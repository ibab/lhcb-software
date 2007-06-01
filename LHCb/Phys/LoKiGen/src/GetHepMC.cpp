// $Id: GetHepMC.cpp,v 1.1 2007-06-01 11:48:07 ibelyaev Exp $
// ============================================================================
// CVS trag $Name: not supported by cvs2svn $, version $Revision: 1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
// Include files 
// ============================================================================
// HepMC 
// ============================================================================
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenEvent.h"
// ============================================================================
// Local
// ============================================================================
#include "LoKi/GetHepMC.h"
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
{
  if ( 0 == vertex ) { return std::vector<const HepMC::GenParticle*>() ; }
  HepMC::GenVertex* _v = const_cast<HepMC::GenVertex*> ( vertex ) ;
  return std::vector<const HepMC::GenParticle*>
    ( _v -> particles_begin ( range ) , 
      _v -> particles_end   ( range ) ) ;
}
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
{
  if ( 0 == vertex ) { return std::vector<const HepMC::GenParticle*>() ; }
  return std::vector<const HepMC::GenParticle*>
    ( vertex -> particles_in_const_begin () , 
      vertex -> particles_in_const_end   () ) ;
}
// ============================================================================
/*  get all "out"-particles for the given vertex  
 *  @see HepMC::GenVertex::particles_in_const_begin() 
 *  @see HepMC::GenVertex::particles_in_const_end()
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2007-05-26
 */
// ============================================================================
std::vector<const HepMC::GenParticle*>
LoKi::GetHepMC::particles_out
( const HepMC::GenVertex* vertex ) 
{
  if ( 0 == vertex ) { return std::vector<const HepMC::GenParticle*>() ; }
  return std::vector<const HepMC::GenParticle*>
    ( vertex -> particles_out_const_begin () , 
      vertex -> particles_out_const_end   () ) ;
}
// ============================================================================
/*  get all "parents" particles form the given vertex
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2007-05-26
 */
// ============================================================================
std::vector<const HepMC::GenParticle*> 
LoKi::GetHepMC::parents 
( const HepMC::GenVertex*    vertex ) 
{ return particles ( vertex , HepMC::parents ) ; }
// ============================================================================
/*  get all "children" particles form the given vertex
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2007-05-26
 */    
// ============================================================================
std::vector<const HepMC::GenParticle*> 
LoKi::GetHepMC::children
( const HepMC::GenVertex*    vertex ) 
{ return particles ( vertex , HepMC::children ) ; }
// ============================================================================
/*  get all "family" particles form the given vertex
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2007-05-26
 */    
// ============================================================================
std::vector<const HepMC::GenParticle*> 
LoKi::GetHepMC::family
( const HepMC::GenVertex*    vertex ) 
{ return particles ( vertex , HepMC::family ) ; }
// ============================================================================
/*  get all "ancestors" particles form the given vertex
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2007-05-26
 */    
// ============================================================================
std::vector<const HepMC::GenParticle*> 
LoKi::GetHepMC::ancestors
( const HepMC::GenVertex*    vertex ) 
{ return particles ( vertex , HepMC::ancestors ) ; }
// ============================================================================
/*  get all "descendants" particles form the given vertex
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2007-05-26
 */    
// ============================================================================
std::vector<const HepMC::GenParticle*> 
LoKi::GetHepMC::descendants
( const HepMC::GenVertex*    vertex ) 
{ return particles ( vertex , HepMC::descendants ) ; }
// ============================================================================
/** get all "relatives" particles form the given vertex
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2007-05-26
 */    
// ============================================================================
std::vector<const HepMC::GenParticle*> 
LoKi::GetHepMC::relatives
( const HepMC::GenVertex*    vertex ) 
{ return particles ( vertex , HepMC::relatives ) ; }
// ============================================================================
/*  get all particles form the given event 
 *  @see HepMC::GenEvent::particles_begin
 *  @see HepMC::GenEvent::particles_end
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2007-05-26
 */
// ============================================================================
std::vector<const HepMC::GenParticle*> 
LoKi::GetHepMC::particles_all 
( const HepMC::GenEvent* event )  
{
  if ( 0 == event ) { return std::vector<const HepMC::GenParticle*>() ; }
  return std::vector<const HepMC::GenParticle*>
    ( event -> particles_begin () , 
      event -> particles_end   () ) ;
}
// ============================================================================
/** get all vertices form the given event 
 *  @see HepMC::GenEvent::vertices_begin
 *  @see HepMC::GenEvent::vertices_end
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2007-05-26
 */
// ============================================================================
std::vector<const HepMC::GenVertex*> 
LoKi::GetHepMC::vertices_all 
( const HepMC::GenEvent* event )  
{
  if ( 0 == event ) { return std::vector<const HepMC::GenVertex*>() ; }
  return std::vector<const HepMC::GenVertex*>
    ( event -> vertices_begin () , 
      event -> vertices_end   () ) ;
}
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
