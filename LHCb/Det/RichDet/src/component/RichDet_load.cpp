// $Id: RichDet_load.cpp,v 1.5 2003-11-21 17:23:26 papanest Exp $
// Include files 


#include "GaudiKernel/DeclareFactoryEntries.h"


// Declare  OBJECT / CONVERTER / ALGORITHM / TOOL using the macros DECLARE_xxx
// The statements are like that:
//
// DECLARE_CONVERTER( MyConverter );
// DECLARE_OBJECT( DataObject );
//
// They should be inside the 'DECLARE_FACTORY_ENTRIES' body.

DECLARE_FACTORY_ENTRIES(RichDet) {
  DECLARE_CONVERTER( XmlDeRich2Cnv );
  DECLARE_CONVERTER( XmlDeRich1Cnv );
  DECLARE_CONVERTER( XmlDeRichFlatMirrorCnv );
  DECLARE_CONVERTER( XmlDeRichSphMirrorCnv );
  DECLARE_CONVERTER( XmlDeRich1HPDPanelCnv );
  DECLARE_CONVERTER( XmlDeRich2HPDPanelCnv );
  DECLARE_CONVERTER( XmlDeRich1CdfHPDPanelCnv );
  DECLARE_CONVERTER( XmlDeRichRadiatorCnv );
}
