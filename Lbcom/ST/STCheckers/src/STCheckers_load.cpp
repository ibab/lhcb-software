// $Id: STCheckers_load.cpp,v 1.5 2006-12-21 17:54:48 jvantilb Exp $
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( STCheckers ) {

  DECLARE_ALGORITHM( STClusterClassification );
  DECLARE_ALGORITHM( STEffMonitor );

  DECLARE_ALGORITHM( STDumpGeom );

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
