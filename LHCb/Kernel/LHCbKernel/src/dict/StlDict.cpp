// $Id: StlDict.cpp,v 1.1 2003-12-18 15:32:59 cattanem Exp $

// Dictionaries for Stl classes used in LHCb

#include "GaudiObjDesc/SealDictionary.h"
#include <vector>

namespace
{
  
  struct _InitDict {
    _InitDict()  {
      
      GaudiDict::PairDict<long,double>();
      GaudiDict::VectorDict<std::pair<long,double> >();
      
    }
  };
  
  static _InitDict __init;
}

_InitDict* __init_InitDict()  {
  return &__init;
}
