
//------------------------------------------------------------------------
/** @file RichPIDMerge_load.cpp
 *
 *  Object declaration file for component library RichPIDMerge
 *
 *  CVS Log :-
 *  $Id: RichPIDMerge_load.cpp,v 1.3 2004-08-19 14:04:36 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.2  2004/07/26 17:56:24  jonrob
 *  Various improvements to the doxygen comments
 *
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
  DECLARE_ALGORITHM( RichGPIDSelByRing );

}
