// $Id: RichGlobalPID_load.cpp,v 1.1.1.1 2003-06-30 16:10:54 jonesc Exp $
// Include files 

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( RichGlobalPID ) {

  // Reco algs
  DECLARE_ALGORITHM( RichGlobalPIDAlg );
  DECLARE_ALGORITHM( RichGlobalPIDInitialize );
  DECLARE_ALGORITHM( RichGlobalPIDFinalize );
  DECLARE_ALGORITHM( RichGlobalPIDTrTrackSel );
  DECLARE_ALGORITHM( RichGlobalPIDDigitSel );

  // monitors
  DECLARE_ALGORITHM( RichGlobalPIDMonitor );

}
