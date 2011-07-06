#ifndef AMPLITUDE_HH
#define AMPLITUDE_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:12 GMT

#include <complex>

#include "DecayTree.h"
#include "DDEventAccess.h"

class SimpleAmplitude : public DDEventAccess{

  DecayTree _decay;
  
  double spinFactor;
 public:
  std::complex<double> getVal(){
    return 0;
  }
  
  SimpleAmplitude(IDDEventList* events, const DecayTree& ) 
    : DDEventAccess(events)
    , spinFactor(1) // for now
    {}
  

};



#endif
//
