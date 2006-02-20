// $Id: STCheckers_load.cpp,v 1.3 2006-02-20 16:44:32 mneedham Exp $
#include "GaudiKernel/DeclareFactoryEntries.h"

// Declare  OBJECT / CONVERTER / ALGORITHM / TOOL using the macros DECLARE_xxx
// The statements are like that:
//
// DECLARE_CONVERTER( MyConverter );
// DECLARE_OBJECT( DataObject );
//
// They should be inside the 'DECLARE_FACTORY_ENTRIES' body.

DECLARE_FACTORY_ENTRIES( STCheckers ) {
  DECLARE_ALGORITHM( STNoiseClusterMonitor );
  DECLARE_ALGORITHM( STEffMonitor );

  DECLARE_ALGORITHM( STDumpGeom );
  DECLARE_ALGORITHM( DotDump );

  DECLARE_ALGORITHM( MCSTDepositChecker );
  DECLARE_ALGORITHM( MCSTDigitChecker );
  DECLARE_ALGORITHM( STDigitChecker );
  DECLARE_ALGORITHM( STClusterChecker ); 
  DECLARE_ALGORITHM( STClusterResolution );
  DECLARE_ALGORITHM( ITOccupancy );
  DECLARE_ALGORITHM( TTOccupancy );



  DECLARE_ALGORITHM( LandauTest );
  DECLARE_ALGORITHM( STMCTuner );
}






