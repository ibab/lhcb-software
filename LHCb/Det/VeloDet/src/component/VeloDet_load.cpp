// $Id: VeloDet_load.cpp,v 1.1 2001-10-25 06:13:27 ocallot Exp $

#include "GaudiKernel/DeclareFactoryEntries.h"

// Using the MACROS in the previous include file declare all "components" of
// the library.

DECLARE_FACTORY_ENTRIES(VeloDet) {
  DECLARE_CONVERTER( XmlDeVeloCnv );
}
