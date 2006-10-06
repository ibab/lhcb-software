// $Id: Scale.cpp,v 1.1 2006-10-06 14:11:16 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
// Include files 
// ============================================================================
// HepMC 
// ============================================================================
#include "HepMC/GenEvent.h"
// ============================================================================
// Local 
// ============================================================================
#include "Generators/Scale.h"
// ============================================================================
/** @file 
 *  implementation of simple functon to rescale 
 *  HepMCEvent in between Pythia and LHCb units 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2006-10-05
 */
// ============================================================================
void GeneratorUtils::scale
( HepMC::GenEvent* event , 
  const double     mom   , 
  const double     time  ) 
{
  if ( 0 == event ) { return ; }
  // rescale particles   
  for ( HepMC::GenEvent::particle_iterator ip = event->particles_begin() ;
        event->particles_end() != ip ; ++ip ) 
  {
    HepMC::GenParticle* p = *ip ;
    if ( 0 != p ) { p->set_momentum( p->momentum() * mom ) ; }  
  }
  // rescale vertices 
  for ( HepMC::GenEvent::vertex_iterator iv = event->vertices_begin() ; 
        event->vertices_end() != iv ; ++iv ) 
  {
    HepMC::GenVertex* v = *iv ;
    if ( 0 == v ) { continue ; }
    CLHEP::HepLorentzVector newPos = v->position() ;
    newPos.setT ( newPos.t() * time ) ;
    v->set_position ( newPos ) ;
  }   
}
// ============================================================================


// ============================================================================
// The END 
// ============================================================================

