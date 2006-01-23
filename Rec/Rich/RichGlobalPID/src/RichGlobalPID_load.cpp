
/** @file RichGlobalPID_load.cpp
 *
 *  Declaration of objects in the component library RichGlobalPID
 *
 *  CVS Log :-
 *  $Id: RichGlobalPID_load.cpp,v 1.4 2006-01-23 13:42:16 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   17/04/2002
 */

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( RichGlobalPID ) 
{

   // Reco algs
  DECLARE_ALGORITHM( RichGlobalPIDAlg );
  DECLARE_ALGORITHM( RichGlobalPIDInitialize );
  DECLARE_ALGORITHM( RichGlobalPIDFinalize );
  DECLARE_ALGORITHM( RichGlobalPIDTrTrackSel );
  DECLARE_ALGORITHM( RichGlobalPIDDigitSel );

  // monitors
  DECLARE_ALGORITHM( RichGlobalPIDMonitor );

}
