
/** @file RichRecAlgorithms_load.cpp
 *
 *  Declaration of objects in the component library RichRecCommon
 *
 *  CVS Log :-
 *  $Id: RichRecAlgorithms_load.cpp,v 1.2 2006-02-16 16:03:21 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   17/06/2005
 */

#include "GaudiKernel/DeclareFactoryEntries.h"


DECLARE_FACTORY_ENTRIES ( RichRecAlgorithms ) 
{
  DECLARE_ALGORITHM( RichRecBackgroundEsti );
  DECLARE_ALGORITHM( RichRecInit           );
}
