// $Id: TrackProjectors_load.cpp,v 1.8 2006-12-15 19:11:38 graven Exp $
// Include files 

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( TrackProjectors ) {
  DECLARE_TOOL( TrajOTProjector );
  DECLARE_TOOL( TrajVeloProjector );
  DECLARE_TOOL( TrajSTProjector );
}
