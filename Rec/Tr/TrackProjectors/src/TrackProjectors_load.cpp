// $Id: TrackProjectors_load.cpp,v 1.7 2006-02-27 19:56:04 jvantilb Exp $
// Include files 

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( TrackProjectors ) {

  DECLARE_TOOL( TrackMasterProjector );
  DECLARE_TOOL( TrackOTProjector );
  DECLARE_TOOL( TrackSTProjector );
  DECLARE_TOOL( TrackVeloRProjector );
  DECLARE_TOOL( TrackVeloPhiProjector );
  DECLARE_TOOL( TrajOTProjector );
  DECLARE_TOOL( TrajSTProjector );
  DECLARE_TOOL( TrajVeloPhiProjector );
  DECLARE_TOOL( TrajVeloRProjector );
}
