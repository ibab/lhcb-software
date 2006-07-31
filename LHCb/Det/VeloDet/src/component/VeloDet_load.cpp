// $Id: VeloDet_load.cpp,v 1.3 2006-07-31 17:01:17 mtobin Exp $

#include "GaudiKernel/DeclareFactoryEntries.h"

// Using the MACROS in the previous include file declare all "components" of
// the library.

DECLARE_FACTORY_ENTRIES(VeloDet) {
  DECLARE_CONVERTER( XmlDeVeloCnv );
  DECLARE_CONVERTER( XmlDeVeloPhiTypeCnv );
  DECLARE_CONVERTER( XmlDeVeloRTypeCnv );

  DECLARE_ALGORITHM( VeloDetChecker )
}
