// $Id: HepMCUtils.cpp,v 1.3 2005-11-04 10:54:21 robbep Exp $
// Include files

// local
#include "Generators/HepMCUtils.h"

bool HepMCUtils::Compare( const HepMC::GenParticle * thePart1 , 
                          const HepMC::GenParticle * thePart2 ) { 
  return thePart1->barcode() < thePart2->barcode() ; 
}

bool HepMCUtils::IsBAtProduction( const HepMC::GenParticle * thePart ) {
  if ( ( abs( thePart -> pdg_id() ) != 511 ) && 
       ( abs( thePart -> pdg_id() ) != 531 ) ) return true ;
  if ( 0 == thePart -> production_vertex() ) return true ;
  HepMC::GenVertex * theVertex = thePart -> production_vertex() ;
  if ( 1 != theVertex -> particles_in_size() ) return true ;
  HepMC::GenParticle * theMother = 
    (* theVertex -> particles_in_const_begin() ) ;
  if ( theMother -> pdg_id() == - thePart -> pdg_id() ) return false ;
  return true ;
}

void HepMCUtils::Clear( HepMC::GenEvent * theEvent ) {
  if ( ! theEvent -> vertices_empty() ) {
    HepMC::GenEvent::vertex_iterator iter ;
    for ( iter = theEvent -> vertices_begin() ; 
          iter != theEvent -> vertices_end()  ; ++iter ) {
      theEvent -> remove_vertex( *iter ) ;
      delete (*iter) ;
    }
  }
}

