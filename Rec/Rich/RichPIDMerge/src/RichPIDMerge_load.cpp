
//------------------------------------------------------------------------
/** @file RichPIDMerge_load.cpp
 *
 *  Object declaration file for component library RichPIDMerge
 *
 *  CVS Log :-
 *  $Id: RichPIDMerge_load.cpp,v 1.4 2006-01-23 13:59:05 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2002-07-10
 */
//------------------------------------------------------------------------

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( RichPIDMerge ) 
{

  // Performs a merging base on a hierachy of algorithms
  DECLARE_ALGORITHM( RichHierarchicalPIDMerge );

  // Debug algorithm, selects only Global results if there is an associated 
  // Ring PID available for the same track. Used to compare the 2 algorithms
  // Disabled until Rich Refit algorithm is back in use
  //DECLARE_ALGORITHM( RichGPIDSelByRing );

}
