// $Id: RichGlobalPID_load.cpp,v 1.2 2003-07-08 14:22:16 cattanem Exp $
// Include files 

#include "GaudiKernel/DeclareFactoryEntries.h"

// Global PID objects
#include "Event/RichGlobalPIDTrack.h"
_ImplementContainedObjectFactory ( RichGlobalPIDTrack )
_ImplementDataObjectFactory      ( RichGlobalPIDTracks )
#include "Event/RichGlobalPID.h"
_ImplementContainedObjectFactory ( RichGlobalPID )
_ImplementDataObjectFactory      ( RichGlobalPIDs )

DECLARE_FACTORY_ENTRIES( RichGlobalPID ) {

  // Global PID objects
  DECLARE_OBJECT( RichGlobalPIDTrack );
  DECLARE_OBJECT( RichGlobalPIDTracks );
  DECLARE_OBJECT( RichGlobalPID );
  DECLARE_OBJECT( RichGlobalPIDs );
	
   // Reco algs
  DECLARE_ALGORITHM( RichGlobalPIDAlg );
  DECLARE_ALGORITHM( RichGlobalPIDInitialize );
  DECLARE_ALGORITHM( RichGlobalPIDFinalize );
  DECLARE_ALGORITHM( RichGlobalPIDTrTrackSel );
  DECLARE_ALGORITHM( RichGlobalPIDDigitSel );

  // monitors
  DECLARE_ALGORITHM( RichGlobalPIDMonitor );

}
