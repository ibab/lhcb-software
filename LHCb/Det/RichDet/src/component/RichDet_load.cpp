
/** @file RichDet_load.cpp
 *
 *  Declaration of objects in the component library RichDet
 *
 *  CVS Log :-
 *  $Id: RichDet_load.cpp,v 1.7 2004-07-27 08:55:24 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES(RichDet)
{

  DECLARE_CONVERTER( XmlDeRich2Cnv );
  DECLARE_CONVERTER( XmlDeRich1Cnv );
  DECLARE_CONVERTER( XmlDeRichFlatMirrorCnv );
  DECLARE_CONVERTER( XmlDeRichSphMirrorCnv );
  DECLARE_CONVERTER( XmlDeRich1HPDPanelCnv );
  DECLARE_CONVERTER( XmlDeRich2HPDPanelCnv );
  DECLARE_CONVERTER( XmlDeRich1CdfHPDPanelCnv );
  DECLARE_CONVERTER( XmlDeRichSingleSolidRadiatorCnv );
  DECLARE_CONVERTER( XmlDeRichMultiSolidRadiatorCnv );

}
