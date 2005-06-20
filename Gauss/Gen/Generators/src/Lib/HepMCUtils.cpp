// $Id: HepMCUtils.cpp,v 1.1.1.1 2005-06-20 21:42:17 robbep Exp $
// Include files

// local
#include "Generators/HepMCUtils.h"

bool HepMCUtils::Compare( const HepMC::GenParticle * thePart1 , 
              const HepMC::GenParticle * thePart2 ) { 
  return thePart1->barcode() < thePart2->barcode() ; 
}

