
#ifndef RICHMARKOV_RICH_SPACE_H
#define RICHMARKOV_RICH_SPACE_H

#include "RichParams.h"
#include "MarkovChainSampler/Space.h"

namespace RichMarkov {

  template <class Mode, const Mode & mode>
  class RichSpace : 
    public Space<RichParams<Mode,mode> > {
  public: 
  };

}

#endif
