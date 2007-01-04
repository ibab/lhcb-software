// $Id: STCheckers_load.cpp,v 1.6 2007-01-04 11:08:46 jvantilb Exp $
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( STCheckers ) {

  DECLARE_ALGORITHM( MCSTDepositMonitor );
  DECLARE_ALGORITHM( MCSTDigitMonitor );
  DECLARE_ALGORITHM( STDigitMonitor );
  DECLARE_ALGORITHM( STClusterMonitor ); 
  DECLARE_ALGORITHM( ITOccupancy );
  DECLARE_ALGORITHM( TTOccupancy );

  DECLARE_ALGORITHM( STClusterClassification );
  DECLARE_ALGORITHM( STEffChecker );
  DECLARE_ALGORITHM( STClusterResolution );
  DECLARE_ALGORITHM( STClusterChecker );

  DECLARE_ALGORITHM( STDumpGeom );
  DECLARE_ALGORITHM( LandauTest );
}
