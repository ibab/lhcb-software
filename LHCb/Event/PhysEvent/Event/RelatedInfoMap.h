#ifndef EVENT_RELATEDINFOMAP_H 
#define EVENT_RELATEDINFOMAP_H 1

// from Gaudi
#include "GaudiKernel/VectorMap.h"

namespace LHCb { 

  class RelatedInfoMap : public GaudiUtils::VectorMap<short int, float> {
    public:
      typedef GaudiUtils::VectorMap<short,float>             MAP  ;
      typedef GaudiUtils::VectorMap<short,float>::_vector    VECT ;
      typedef GaudiUtils::VectorMap<short,float>::value_type VAL  ;
  }; 

}
#endif // KERNEL_IRELATEDINFOTOOL_H
