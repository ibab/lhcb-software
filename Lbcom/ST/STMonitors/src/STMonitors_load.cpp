// $Id: STMonitors_load.cpp,v 1.1.1.1 2009-03-02 19:13:44 mtobin Exp $
// Include files 


#include "GaudiKernel/DeclareFactoryEntries.h"

// Declare  OBJECT / CONVERTER / ALGORITHM / TOOL using the macros DECLARE_xxx
// The statements are like that:
//
// DECLARE_CONVERTER( MyConverter );
// DECLARE_OBJECT( DataObject );
//
// They should be inside the 'DECLARE_FACTORY_ENTRIES' body.

DECLARE_FACTORY_ENTRIES( STMonitors ) {
  
   DECLARE_ALGORITHM( STNZSMonitor );
   DECLARE_ALGORITHM( STZSMonitor );
   DECLARE_ALGORITHM( STNZSAnalyser );
   DECLARE_ALGORITHM( STFullEventDump );
   
}
