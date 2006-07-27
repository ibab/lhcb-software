
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/ObjectFactory.h"

DECLARE_FACTORY_ENTRIES( ExampleGaucho ) {

  // Algorithms
  DECLARE_ALGORITHM( HelloWorld  );
  DECLARE_ALGORITHM( SubAlg );
  DECLARE_ALGORITHM( ParentAlg );
  DECLARE_ALGORITHM( GaudiExample );
}

