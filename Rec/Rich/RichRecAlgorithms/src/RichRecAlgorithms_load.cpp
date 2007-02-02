
/** @file RichRecAlgorithms_load.cpp
 *
 *  Declaration of objects in the component library RichRecAlgorithms
 *
 *  CVS Log :-
 *  $Id: RichRecAlgorithms_load.cpp,v 1.4 2007-02-02 10:05:50 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   17/06/2005
 */

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES ( RichRecAlgorithms )
{
  DECLARE_ALGORITHM( BackgroundEsti );
  DECLARE_ALGORITHM( Initialise  );
  DECLARE_ALGORITHM( SummaryAlg  );
}
