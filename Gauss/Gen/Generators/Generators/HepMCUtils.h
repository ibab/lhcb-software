// $Id: HepMCUtils.h,v 1.1.1.1 2005-06-20 21:42:17 robbep Exp $
#ifndef GENERATORS_HEPMCUTILS_H 
#define GENERATORS_HEPMCUTILS_H 1

#include <vector>
#include "HepMC/GenEvent.h"

namespace HepMCUtils {
  typedef std::vector< HepMC::GenParticle * > GenParticleVector ;
  
  bool Compare( const HepMC::GenParticle * thePart1 , 
                const HepMC::GenParticle * thePart2 ) ;
  
  template<class ITTYPE>
  void SortHepMCVector( GenParticleVector & theHepMCVector , 
                        unsigned int size , const ITTYPE &pBegin , 
                        const ITTYPE &pEnd ) ;
}  

// Implementation of templated function
template <class ITTYPE>
void HepMCUtils::SortHepMCVector( GenParticleVector &theHepMCVector ,
                                  unsigned int size , const ITTYPE &pBegin , 
                                  const ITTYPE &pEnd ) {
  theHepMCVector.reserve( size ) ;
  ITTYPE pit ;
  for ( pit = pBegin ; pit != pEnd ; ++pit ) { 
    theHepMCVector.push_back( (*pit) ) ; 
  }
  // Sort the temporary vector to be able to reproduce events
  std::sort ( theHepMCVector.begin() , theHepMCVector.end() ,
              HepMCUtils::Compare ) ;
}

#endif // GENERATORS_HEPMCUTILS_H
