
#ifndef LESTER_RICH_SPACE_H
#define LESTER_RICH_SPACE_H

#include "RichParams.h"
#include "MarkovChainSampler/Space.h"

namespace Lester {

  template <class Mode, const Mode & mode>
  class RichSpace : 
    public Space<RichParams<Mode,mode> > {
  public: 
  };

}

#endif
