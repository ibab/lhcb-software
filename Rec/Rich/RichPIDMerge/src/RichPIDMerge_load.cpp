
//------------------------------------------------------------------------
/** @file RichPIDMerge_load.cpp
 *
 *  Object declaration file for component library RichPIDMerge
 *
 *  CVS Log :-
 *  $Id: RichPIDMerge_load.cpp,v 1.2 2004-07-26 17:56:24 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2002-07-10
 */
//------------------------------------------------------------------------

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( RichPIDMerge ) {

  // Simple merging algorithm
  DECLARE_ALGORITHM( RichPIDSimpleMerge );

}
