
#ifndef RICHMARKOV_JAMES_RANDOM_SEED_H
#define RICHMARKOV_JAMES_RANDOM_SEED_H

#include <cassert>
#include "urandom/urandom.h"

namespace RichMarkov {
	
  long jamesRandomSeed() {
    typedef unsigned long USI;
    typedef long SI;	
    const USI jamesMaxUSI=900000000;
    //const USI jamesMinUSI=0;
#ifndef NDEBUG
    const SI jamesMaxSI=900000000;
    const SI jamesMinSI=0;
#endif
   
    USI suggestion;
    SI SIsuggestion;
    while ( (SIsuggestion = 
	     static_cast<SI>(suggestion = urand<USI>()) , suggestion>jamesMaxUSI 
	     /* || suggestion<jamesMinUSI*/  ) ) {};
    assert(SIsuggestion>=jamesMinSI && SIsuggestion<=jamesMaxSI);
    return SIsuggestion;
    
  };
  
}

#endif
