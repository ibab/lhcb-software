
//-----------------------------------------------------------------------------
/** @file RichMCMonitors_load.cpp
 *
 * Declaration of entries in the component library RichMCMonitors
 *
 * CVS Log :-
 * $Id: RichMCMonitors_load.cpp,v 1.1.1.1 2006-11-07 11:56:22 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 03/11/2006
 */
//-----------------------------------------------------------------------------

// Include files 
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES ( RichMCMonitors ) 
{
  DECLARE_NAMESPACE_ALGORITHM( Rich, RichMCCKPhotonYieldAlg );
}
