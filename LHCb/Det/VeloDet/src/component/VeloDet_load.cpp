// $Id: VeloDet_load.cpp,v 1.2 2004-02-13 13:28:52 cattanem Exp $

#include "GaudiKernel/DeclareFactoryEntries.h"

// Using the MACROS in the previous include file declare all "components" of
// the library.

DECLARE_FACTORY_ENTRIES(VeloDet) {
  DECLARE_CONVERTER( XmlDeVeloCnv );
  DECLARE_CONVERTER( XmlDeVeloPhiTypeCnv );
  DECLARE_CONVERTER( XmlDeVeloRTypeCnv );
}
