// $Id: RichPIDMerge_load.cpp,v 1.1.1.1 2003-06-30 16:35:39 jonesc Exp $
// Include files 

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( RichPIDMerge ) {

  // Simple merging algorithm
  DECLARE_ALGORITHM( RichPIDSimpleMerge );

}
