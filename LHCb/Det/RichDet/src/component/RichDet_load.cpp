
/** @file RichDet_load.cpp
 *
 *  Declaration of objects in the component library RichDet
 *
 *  CVS Log :-
 *  $Id: RichDet_load.cpp,v 1.12 2006-11-03 10:07:16 cattanem Exp $
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES(RichDet)
{

  DECLARE_CONVERTER( XmlDeRichSystemCnv );
  DECLARE_CONVERTER( XmlDeRich2Cnv );
  DECLARE_CONVERTER( XmlDeRich1Cnv );
  DECLARE_CONVERTER( XmlDeRichSphMirrorCnv );
  DECLARE_CONVERTER( XmlDeRichHPDPanelCnv );
  DECLARE_CONVERTER( XmlDeRichSingleSolidRadiatorCnv );
  DECLARE_CONVERTER( XmlDeRichMultiSolidRadiatorCnv );
  DECLARE_CONVERTER( XmlDeRichGasRadiatorCnv );
  DECLARE_CONVERTER( XmlDeRichAerogelRadiatorCnv );
  DECLARE_CONVERTER( XmlDeRichHPDCnv );
}
