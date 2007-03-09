
/** @file RichRecAlgorithms_load.cpp
 *
 *  Declaration of objects in the component library RichRecAlgorithms
 *
 *  CVS Log :-
 *  $Id: RichRecAlgorithms_load.cpp,v 1.5 2007-03-09 22:48:41 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   17/06/2005
 */

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES ( RichRecAlgorithms )
{
  DECLARE_ALGORITHM( BackgroundEstiAvHPD );
  DECLARE_ALGORITHM( RichRecBackgroundEstiClustering );
  DECLARE_ALGORITHM( Initialise          );
  DECLARE_ALGORITHM( SummaryAlg          );
}
