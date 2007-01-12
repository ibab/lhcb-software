// $Id: GaussTracker_load.cpp,v 1.3 2007-01-12 15:41:23 ranjard Exp $
// Include files 

#include "GaudiKernel/DeclareFactoryEntries.h" 

// Declare  OBJECT / CONVERTER / ALGORITHM / TOOL using the macros DECLARE_xxx
// The statements are like that:
//
// DECLARE_ALGORITHM( MyAlgorithm );
// DECLARE_TOOL( MyTool );
// DECLARE_OBJECT( DataObject );
//
// They should be inside the 'DECLARE_FACTORY_ENTRIES' body.
DECLARE_FACTORY_ENTRIES(Test) {

  /// Sensitive Detector 
  DECLARE_TOOL( GiGaSensDetTracker );
 
  /// Algortihms to fill transient store
  DECLARE_ALGORITHM ( GetTrackerHitsAlg );  
  
}
