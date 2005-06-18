
/** @file RichRecAlgorithms_load.cpp
 *
 *  Declaration of objects in the component library RichRecCommon
 *
 *  CVS Log :-
 *  $Id: RichRecAlgorithms_load.cpp,v 1.1.1.1 2005-06-18 08:42:43 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   17/06/2005
 */

#include "GaudiKernel/DeclareFactoryEntries.h"


DECLARE_FACTORY_ENTRIES ( RichRecAlgorithms ) 
{

  /// computes a background estimate
  DECLARE_ALGORITHM( RichRecBackgroundEsti   );
  
}
