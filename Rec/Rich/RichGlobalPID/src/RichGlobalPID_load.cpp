
/** @file RichGlobalPID_load.cpp
 *
 *  Declaration of objects in the component library RichGlobalPID
 *
 *  CVS Log :-
 *  $Id: RichGlobalPID_load.cpp,v 1.3 2004-07-27 10:56:37 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   17/04/2002
 */

#include "GaudiKernel/DeclareFactoryEntries.h"

// Global PID objects
#include "Event/RichGlobalPIDTrack.h"
_ImplementContainedObjectFactory ( RichGlobalPIDTrack )
_ImplementDataObjectFactory      ( RichGlobalPIDTracks )
#include "Event/RichGlobalPID.h"
_ImplementContainedObjectFactory ( RichGlobalPID )
_ImplementDataObjectFactory      ( RichGlobalPIDs )

DECLARE_FACTORY_ENTRIES( RichGlobalPID ) 
{

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
