// $Id: TrackTools_load.cpp,v 1.14 2006-02-28 19:12:14 jvantilb Exp $
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( TrackTools ) {

  DECLARE_TOOL( MeasurementProvider );
  DECLARE_TOOL( TrackChi2Calculator );
  DECLARE_TOOL( TrackCloneFinder );
  DECLARE_TOOL( TrackPtKick );                                                 
  DECLARE_TOOL( TrajPoca );
  DECLARE_TOOL( TrajectoryProvider );

}
