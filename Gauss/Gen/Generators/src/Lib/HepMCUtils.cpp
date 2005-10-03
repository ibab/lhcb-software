// $Id: HepMCUtils.cpp,v 1.2 2005-10-03 09:39:18 robbep Exp $
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

