// $Id: TrackTools_load.cpp,v 1.19 2006-08-22 12:37:51 erodrigu Exp $
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( TrackTools )
{

  DECLARE_TOOL( TrackUsedLHCbID );


  DECLARE_TOOL( MeasurementProvider );
  DECLARE_TOOL( TrackChi2Calculator );
  DECLARE_TOOL( TrackCloneFinder );
  DECLARE_TOOL( TrackPtKick );

  DECLARE_TOOL( TrajectoryProvider );
  DECLARE_TOOL( LongTrackReferenceCreator );
  DECLARE_TOOL( TrackCaloMatch );
  DECLARE_TOOL( TrackSelector );

  DECLARE_TOOL( TrackVelodEdxCharge );

  DECLARE_TOOL( StateSimpleBetheBlochEnergyCorrectionTool );
  DECLARE_TOOL( StateElectronEnergyCorrectionTool         );
  DECLARE_TOOL( StateThickMSCorrectionTool                );
  DECLARE_TOOL( StateThinMSCorrectionTool                 );

}
