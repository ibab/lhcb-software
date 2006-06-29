// $Id: TrackTools_load.cpp,v 1.16 2006-06-29 12:50:09 mneedham Exp $
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( TrackTools ) {

  DECLARE_TOOL( MeasurementProvider );
  DECLARE_TOOL( TrackChi2Calculator );
  DECLARE_TOOL( TrackCloneFinder );
  DECLARE_TOOL( TrackPtKick );                                                 

  DECLARE_TOOL( TrajectoryProvider );
  DECLARE_TOOL( LongTrackReferenceCreator );
  DECLARE_TOOL( TrackCaloMatch );
  DECLARE_TOOL( TrackSelector );

}
