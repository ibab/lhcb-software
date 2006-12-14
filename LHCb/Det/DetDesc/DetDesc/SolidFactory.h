// $Id: SolidFactory.h,v 1.6 2006-12-14 13:03:40 ranjard Exp $ 
// ===========================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ===========================================================================
#ifndef DETDESC_SOLIDFACTORY_H 
#define DETDESC_SOLIDFACTORY_H 1

#include "Reflex/PluginService.h"

// The following is needed to be backward compatible with the old factories of Gaudi. Otherwise the components
// having the constructor/destructor protected will not be working

class ISolid;
template <typename T> class SolidFactory {
	public: static ISolid* create(void) { 
    return new T(); 
  }
};

namespace {
  template < typename P > class Factory<P, ISolid*(void)> {
    public: static void* Func( void*, const std::vector<void*>&, void*) {
      return SolidFactory<P>::create();
    }
  };
}

// Macros to declare component factories
#define DECLARE_SOLID_FACTORY(x)  PLUGINSVC_FACTORY(x,ISolid*(void))
#define IMPLEMENT_SOLID(x)  PLUGINSVC_FACTORY(x,ISolid*(void))


#endif ///< DETDESC_SOLIDFACTORY_H

