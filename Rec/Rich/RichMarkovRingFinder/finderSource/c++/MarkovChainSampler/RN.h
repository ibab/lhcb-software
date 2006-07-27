
#ifndef LESTER_RN_H
#define LESTER_RN_H

#include "MarkovChainSampler/Space.h"
#include "MarkovChainSampler/Ntuple.h"

namespace Lester {
  
  template<int dim>
  class RN : public Space<Ntuple<dim, double> > {
  };

};

#endif 
