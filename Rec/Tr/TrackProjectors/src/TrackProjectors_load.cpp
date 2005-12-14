// $Id: TrackProjectors_load.cpp,v 1.5 2005-12-14 14:50:51 erodrigu Exp $
// Include files 

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( TrackProjectors ) {

  DECLARE_TOOL( TrackMasterProjector );
  DECLARE_TOOL( TrackOTProjector );
  DECLARE_TOOL( TrackSTProjector );
  DECLARE_TOOL( TrackVeloRProjector );
  DECLARE_TOOL( TrackVeloPhiProjector );
  DECLARE_TOOL( TrajOTProjector );
}
