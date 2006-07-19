// $Id: TrackTools_load.cpp,v 1.17 2006-07-19 16:11:18 jonrob Exp $
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( TrackTools )
{

  DECLARE_TOOL( MeasurementProvider );
  DECLARE_TOOL( TrackChi2Calculator );
  DECLARE_TOOL( TrackCloneFinder );
  DECLARE_TOOL( TrackPtKick );

  DECLARE_TOOL( TrajectoryProvider );
  DECLARE_TOOL( LongTrackReferenceCreator );
  DECLARE_TOOL( TrackCaloMatch );
  DECLARE_TOOL( TrackSelector );

  DECLARE_TOOL( TrackVelodEdxCharge );

}
