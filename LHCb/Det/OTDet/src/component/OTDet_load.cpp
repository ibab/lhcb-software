// $Id: OTDet_load.cpp,v 1.3 2003-12-04 10:22:09 jnardull Exp $

#include "GaudiKernel/DeclareFactoryEntries.h"

// Using the MACROS in the previous include file declare all "components" of
// the library.

DECLARE_FACTORY_ENTRIES(OTDet) {
  DECLARE_CONVERTER( XmlOTDetectorCnv );
  DECLARE_CONVERTER( XmlOTStationCnv );
  DECLARE_CONVERTER( XmlOTLayerCnv );
  DECLARE_CONVERTER( XmlOTQuarterCnv );
  DECLARE_CONVERTER( XmlOTModuleCnv );
}
