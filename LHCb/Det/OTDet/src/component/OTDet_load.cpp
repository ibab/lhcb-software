// $Id: OTDet_load.cpp,v 1.1.1.1 2002-05-26 22:36:33 jvantilb Exp $

#include "GaudiKernel/DeclareFactoryEntries.h"

// Using the MACROS in the previous include file declare all "components" of
// the library.

DECLARE_FACTORY_ENTRIES(OTDet) {
  DECLARE_CONVERTER( XmlOTDetectorCnv );
}
