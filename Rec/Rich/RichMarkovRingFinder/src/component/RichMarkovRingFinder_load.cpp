// $Id: RichMarkovRingFinder_load.cpp,v 1.6 2006-08-12 10:49:35 jonrob Exp $
// Include files 

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES(RichMarkovRingFinder) 
{
  DECLARE_ALGORITHM( Rich2LeftPanelMarkovRingFinderAlg ); 
  DECLARE_ALGORITHM( Rich2RightPanelMarkovRingFinderAlg ); 
  DECLARE_ALGORITHM( RichMarkovRingFinderMoni );
}
