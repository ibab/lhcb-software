// $Id: HepMCUtils.h,v 1.2 2005-10-03 09:39:18 robbep Exp $
#ifndef GENERATORS_HEPMCUTILS_H 
#define GENERATORS_HEPMCUTILS_H 1

#include <vector>
#include "HepMC/GenEvent.h"

class HepMCUtils {
 public:
  typedef std::vector< HepMC::GenParticle * > ParticleVector ;
  
  static bool Compare( const HepMC::GenParticle * thePart1 , 
                       const HepMC::GenParticle * thePart2 ) ;
  
  template<class ITTYPE>
  static void SortHepMC( ParticleVector & theHepMCVector , 
                         const ParticleVector::size_type vectorSize ,
                         const ITTYPE &pBegin, const ITTYPE &pEnd ) ; 

  static bool IsBAtProduction( const HepMC::GenParticle * thePart ) ;
};

// Implementation of templated function
template <class ITTYPE>
inline void HepMCUtils::SortHepMC( ParticleVector &theHepMCVector ,
                                   const ParticleVector::size_type vectorSize ,
                                   const ITTYPE &pBegin , 
                                   const ITTYPE &pEnd ) {
  theHepMCVector.reserve( vectorSize ) ;
  ITTYPE pit ;
  for ( pit = pBegin ; pit != pEnd ; ++pit ) 
    theHepMCVector.push_back( (*pit) ) ; 
  // Sort the temporary vector to be able to reproduce events
  std::sort ( theHepMCVector.begin() , theHepMCVector.end() ,
              HepMCUtils::Compare ) ;
}

#endif // GENERATORS_HEPMCUTILS_H
