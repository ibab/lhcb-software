#include "GaudiKernel/ObjectFactory.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

#include "Event/HepMCEvent.h"
_ImplementContainedObjectFactory( HepMCEvent  ) ;
_ImplementDataObjectFactory(      HepMCEvents ) ;

DECLARE_FACTORY_ENTRIES(GenEvent) 
{
  DECLARE_OBJECT( HepMCEvents ) ;
};



