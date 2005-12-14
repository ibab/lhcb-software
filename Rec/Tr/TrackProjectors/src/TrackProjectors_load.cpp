// $Id: TrackProjectors_load.cpp,v 1.4 2005-12-14 11:04:33 erodrigu Exp $
// Include files 

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( TrackProjectors ) {

  DECLARE_TOOL( TrackMasterProjector );
  DECLARE_TOOL( TrackOTProjector );
  DECLARE_TOOL( TrackITProjector );
  DECLARE_TOOL( TrackVeloRProjector );
  DECLARE_TOOL( TrackVeloPhiProjector );
  DECLARE_TOOL( TrajOTProjector );
}
