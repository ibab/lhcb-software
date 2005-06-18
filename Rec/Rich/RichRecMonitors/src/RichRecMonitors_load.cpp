
/** @file RichRecMonitors_load.cpp
 *
 *  Declaration of objects in the component library RichRecMonitors
 *
 *  CVS Log :-
 *  $Id: RichRecMonitors_load.cpp,v 1.1.1.1 2005-06-18 11:44:46 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( RichRecMonitors )
{

  // Common reconstruction monitors
  DECLARE_ALGORITHM( RichTrackGeomMoni       ); ///< Monitors the track geometry information
  DECLARE_ALGORITHM( RichTrackResolutionMoni ); ///< Monitors the track resolutions
  DECLARE_ALGORITHM( RichPixelMonitor        ); ///< Monitors the RichPixels
  DECLARE_ALGORITHM( RichPhotonMonitor       ); ///< Monitors the photon reconstruction
  DECLARE_ALGORITHM( RichRecTimeMonitor      ); ///< Monitor for recontruction algorithm timing
  DECLARE_ALGORITHM( RichRecDataObjVerifier  ); ///< Printout of the reconstruction objects

  // Allignment monitor
  DECLARE_ALGORITHM( RichAlignmentMonitor    ); ///< Mirror allignment monitor

  // MC algorithms
  DECLARE_ALGORITHM( RichMCMassHypoRingsAlg  ); ///< Produce MC mass hypothesis rings

}
