#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( TrackTools ) {

  DECLARE_TOOL( MeasurementProvider );
  DECLARE_TOOL( TrackChi2Calculator );
  DECLARE_TOOL( TrackCloneFinder );
  DECLARE_TOOL( TrackPtKick );                                                 
  DECLARE_TOOL( TrajPoca );

}
