// $Id: OTDet_load.cpp,v 1.2 2003-06-11 11:49:36 cattanem Exp $

#include "GaudiKernel/DeclareFactoryEntries.h"

// Using the MACROS in the previous include file declare all "components" of
// the library.

DECLARE_FACTORY_ENTRIES(OTDet) {
  DECLARE_CONVERTER( XmlOTDetectorCnv );
  DECLARE_CONVERTER( XmlOTStationCnv );
  DECLARE_CONVERTER( XmlOTLayerCnv );
  DECLARE_CONVERTER( XmlOTModuleCnv );
}
