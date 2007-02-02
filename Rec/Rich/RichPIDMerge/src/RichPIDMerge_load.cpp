
//------------------------------------------------------------------------
/** @file RichPIDMerge_load.cpp
 *
 *  Object declaration file for component library RichPIDMerge
 *
 *  CVS Log :-
 *  $Id: RichPIDMerge_load.cpp,v 1.5 2007-02-02 10:05:17 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2002-07-10
 */
//------------------------------------------------------------------------

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( RichPIDMerge )
{
  // Performs a merging base on a hierachy of algorithms
  DECLARE_ALGORITHM( HierarchicalPIDMerge );
}
