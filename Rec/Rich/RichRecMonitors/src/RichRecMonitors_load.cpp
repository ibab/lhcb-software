
//=============================================================================
/** @file RichRecMonitors_load.cpp
 *
 *  Declaration of objects in the component library RichRecMonitors
 *
 *  CVS Log :-
 *  $Id: RichRecMonitors_load.cpp,v 1.3 2005-11-07 09:37:20 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//=============================================================================

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( RichRecMonitors )
{

  // Common reconstruction monitors
  DECLARE_ALGORITHM( RichTrackGeomMoni       ); ///< Monitors the track geometry information
  DECLARE_ALGORITHM( RichTrackResolutionMoni ); ///< Monitors the track resolutions
  DECLARE_ALGORITHM( RichPixelMonitor        ); ///< Monitors the RichPixels
  DECLARE_ALGORITHM( RichRecTimeMonitor      ); ///< Monitor for recontruction algorithm timing
  DECLARE_ALGORITHM( RichRecDataObjVerifier  ); ///< Printout of the reconstruction objects
  DECLARE_ALGORITHM( RichCherenkovResMoni    ); ///< CK theta resolution monitor
  DECLARE_ALGORITHM( RichPhotonSignalMonitor ); ///< Photon signal monitor
  DECLARE_ALGORITHM( RichCherenkovAngleMonitor );
  DECLARE_ALGORITHM( RichPhotonRecoEffMonitor );
  DECLARE_ALGORITHM( RichPhotonGeomMonitor );

  // Allignment monitor
  DECLARE_ALGORITHM( RichAlignmentMonitor    ); ///< Mirror allignment monitor

  // MC algorithms
  DECLARE_ALGORITHM( RichMCMassHypoRingsAlg  ); ///< Produce MC mass hypothesis rings

}
