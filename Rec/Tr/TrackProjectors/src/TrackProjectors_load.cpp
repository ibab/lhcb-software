// $Id: TrackProjectors_load.cpp,v 1.3 2005-04-13 16:54:13 erodrigu Exp $
// Include files 

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( TrackProjectors ) {

  DECLARE_TOOL( TrackMasterProjector );
  DECLARE_TOOL( TrackOTProjector );
  DECLARE_TOOL( TrackITProjector );
  DECLARE_TOOL( TrackVeloRProjector );
  DECLARE_TOOL( TrackVeloPhiProjector );

}
