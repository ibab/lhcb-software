
/** @file RichGlobalPID_load.cpp
 *
 *  Declaration of objects in the component library RichGlobalPID
 *
 *  CVS Log :-
 *  $Id: RichGlobalPID_load.cpp,v 1.5 2007-02-02 10:03:58 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   17/04/2002
 */

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( RichGlobalPID )
{

  // Reco algs
  DECLARE_ALGORITHM( Likelihood );
  DECLARE_ALGORITHM( Initialize );
  DECLARE_ALGORITHM( Finalize );
  DECLARE_ALGORITHM( TrackSel );
  DECLARE_ALGORITHM( DigitSel );

  // monitors
  DECLARE_ALGORITHM( Monitor );

}
