// $Id: TrackProjectors_load.cpp,v 1.6 2006-02-21 18:28:25 jvantilb Exp $
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
}
