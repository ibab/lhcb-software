
//=============================================================================
/** @file RichRecMonitors_load.cpp
 *
 *  Declaration of objects in the component library RichRecMonitors
 *
 *  CVS Log :-
 *  $Id: RichRecMonitors_load.cpp,v 1.7 2007-02-02 10:07:13 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//=============================================================================

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( RichRecMonitors )
{

  DECLARE_ALGORITHM( TimeMonitor           ); ///< Monitor for recontruction algorithm timing
  DECLARE_ALGORITHM( DataObjVerifier       ); ///< Printout of the reconstruction objects

  // test algorithms
  DECLARE_ALGORITHM( PhotonRecoTestAlg     ); ///< Runs so hard-coded fixed reconstructions.
  DECLARE_ALGORITHM( TrackRayTraceTest     ); ///< Tests ray tracing

  // Componenents in MC namespace

  // Common reconstruction monitors
  DECLARE_ALGORITHM( TrackGeomMoni         ); ///< Monitors the track geometry information
  DECLARE_ALGORITHM( TrackResolutionMoni   ); ///< Monitors the track resolutions
  DECLARE_ALGORITHM( PixelPositionMonitor  ); ///< Monitors the position of the RichPixels
  DECLARE_ALGORITHM( PixelRecoEffMonitor   ); ///< Monitors the reco efficiency of the RichPixels
  DECLARE_ALGORITHM( CherenkovResMoni      ); ///< CK theta resolution monitor
  DECLARE_ALGORITHM( PhotonSignalMonitor   ); ///< Photon signal monitor
  DECLARE_ALGORITHM( CherenkovAngleMonitor ); ///< Cherenkov angle monitor
  DECLARE_ALGORITHM( PhotonRecoEffMonitor  ); ///< Photon reconstruction efficiency monitor
  DECLARE_ALGORITHM( PhotonGeomMonitor     ); ///< Photon parameters

  // Allignment monitor
  DECLARE_ALGORITHM( AlignmentMonitor      ); ///< Mirror allignment monitor

  // MC algorithms
  DECLARE_ALGORITHM( MCMassHypoRingsAlg    ); ///< Produce MC mass hypothesis rings

  // Ghosts
  DECLARE_ALGORITHM( GhostTrackMoni        ); ///< Monitors the properties of ghost tracks

}
