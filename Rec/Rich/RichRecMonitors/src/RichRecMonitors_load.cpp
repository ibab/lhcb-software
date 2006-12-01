
//=============================================================================
/** @file RichRecMonitors_load.cpp
 *
 *  Declaration of objects in the component library RichRecMonitors
 *
 *  CVS Log :-
 *  $Id: RichRecMonitors_load.cpp,v 1.6 2006-12-01 14:01:40 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//=============================================================================

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( RichRecMonitors )
{

  // Common reconstruction monitors
  DECLARE_ALGORITHM( RichTrackGeomMoni         ); ///< Monitors the track geometry information
  DECLARE_ALGORITHM( RichTrackResolutionMoni   ); ///< Monitors the track resolutions
  DECLARE_ALGORITHM( RichPixelPositionMonitor  ); ///< Monitors the position of the RichPixels
  DECLARE_ALGORITHM( RichPixelRecoEffMonitor   ); ///< Monitors the reco efficiency of the RichPixels
  DECLARE_ALGORITHM( RichRecTimeMonitor        ); ///< Monitor for recontruction algorithm timing
  DECLARE_ALGORITHM( RichRecDataObjVerifier    ); ///< Printout of the reconstruction objects
  DECLARE_ALGORITHM( RichCherenkovResMoni      ); ///< CK theta resolution monitor
  DECLARE_ALGORITHM( RichPhotonSignalMonitor   ); ///< Photon signal monitor
  DECLARE_ALGORITHM( RichCherenkovAngleMonitor ); ///< Cherenkov angle monitor
  DECLARE_ALGORITHM( RichPhotonRecoEffMonitor  ); ///< Photon reconstruction efficiency monitor
  DECLARE_ALGORITHM( RichPhotonGeomMonitor     ); ///< Photon parameters

  // Allignment monitor
  DECLARE_ALGORITHM( RichAlignmentMonitor      ); ///< Mirror allignment monitor

  // MC algorithms
  DECLARE_ALGORITHM( RichMCMassHypoRingsAlg    ); ///< Produce MC mass hypothesis rings

  // Ghosts
  DECLARE_NAMESPACE_ALGORITHM( Rich, RichGhostTrackMoni ); ///< Monitors the properties of ghost tracks

  // test algorithms
  DECLARE_ALGORITHM( RichPhotonRecoTestAlg     ); ///< Runs so hard-coded fixed reconstructions.
  DECLARE_ALGORITHM( RichTrackRayTraceTest     ); ///< Tests ray tracing

}
