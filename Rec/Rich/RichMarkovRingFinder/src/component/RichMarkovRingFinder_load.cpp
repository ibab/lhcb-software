// $Id: RichMarkovRingFinder_load.cpp,v 1.2 2004-06-01 12:14:02 jonesc Exp $
// Include files 

#include "GaudiKernel/DeclareFactoryEntries.h"


DECLARE_FACTORY_ENTRIES(RichMarkovRingFinder) {

  // Rich1 Markov Chain ring finder upper panel
  DECLARE_ALGORITHM( Rich1UMarkovRingFinderAlg ); 

  // Rich1 Markov Chain ring finder lower panel
  DECLARE_ALGORITHM( Rich1LMarkovRingFinderAlg ); 

  // Rich2 Markov Chain ring finder left panel
  DECLARE_ALGORITHM( Rich2LMarkovRingFinderAlg ); 

  // Rich2 Markov Chain ring finder right panel
  DECLARE_ALGORITHM( Rich2RMarkovRingFinderAlg ); 

  // Monitor for Markov Chain ring finder
  DECLARE_ALGORITHM( RichMarkovRingFinderMoni ); 

}
