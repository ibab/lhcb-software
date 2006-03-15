
/** @file RichDet_load.cpp
 *
 *  Declaration of objects in the component library RichDet
 *
 *  CVS Log :-
 *  $Id: RichDet_load.cpp,v 1.11 2006-03-15 15:57:05 papanest Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.10  2006/02/01 16:20:49  papanest
 *  Added RichSystem + minor changes alignment
 *
 *  Revision 1.9  2005/12/21 09:50:54  papanest
 *  redesign of HPDPanel
 *
 *  Revision 1.8  2005/12/14 09:34:52  papanest
 *  Move from CLHEP to MathCore
 *
 *  Revision 1.7  2004/07/27 08:55:24  jonrob
 *  Add doxygen file documentation and CVS information
 *
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

}
