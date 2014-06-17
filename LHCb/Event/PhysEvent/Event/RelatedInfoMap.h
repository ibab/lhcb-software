#ifndef EVENT_RELATEDINFOMAP_H 
#define EVENT_RELATEDINFOMAP_H 1

// from Gaudi
#include "GaudiKernel/VectorMap.h"

namespace LHCb { 

  class RelatedInfoMap : public GaudiUtils::VectorMap<short, float> {
    public:
      typedef GaudiUtils::VectorMap<short,float>             MAP  ;
      typedef GaudiUtils::VectorMap<short,float>::_vector    VECT ;
      typedef GaudiUtils::VectorMap<short,float>::value_type VAL  ;
     /// has information for specified key
    bool hasInfo( short  key) const 
    { return this->end() != this->find ( key ) ; }

    ///  add/replace new information, associated with the key
    bool addInfo( short key , float info ) { return this->insert ( key , info ).second  ; }
    
    /// extract the information associated with the given key. If there is no such infomration the default value will be returned.
    double info( short  key, float def ) const
    {
      const_iterator i = this->find( key ) ;
      return this->end() == i ? def : i->second ;
    }
    /// erase the information associated with the given key
    unsigned long eraseInfo( short  key) { return this->erase ( key ) ; }
    
  }; 

}
#endif // KERNEL_IRELATEDINFOTOOL_H
