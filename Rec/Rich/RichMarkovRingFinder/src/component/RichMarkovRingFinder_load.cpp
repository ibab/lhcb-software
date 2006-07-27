// $Id: RichMarkovRingFinder_load.cpp,v 1.5 2006-07-27 20:15:25 jonrob Exp $
// Include files 

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES(RichMarkovRingFinder) 
{
  DECLARE_ALGORITHM( Rich2LeftPanelMarkovRingFinderAlg ); 
  DECLARE_ALGORITHM( Rich2RightPanelMarkovRingFinderAlg ); 
}
