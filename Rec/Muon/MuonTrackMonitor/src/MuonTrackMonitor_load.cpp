// $Id: MuonTrackMonitor_load.cpp,v 1.1 2009-02-26 11:10:46 ggiacomo Exp $
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

DECLARE_FACTORY_ENTRIES(MuonTrackMonitor) {
  DECLARE_ALGORITHM( MuonTrackMonitor );
  DECLARE_ALGORITHM( MuonEfficiencyMonitor );
  DECLARE_ALGORITHM( MuonAlignmentMonitor );
}
