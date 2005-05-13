#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( TrackTools ) {

  DECLARE_TOOL( ClusterLHCbIDCnv );
  DECLARE_TOOL( TrueStateCreator );
  DECLARE_TOOL( TrackAcceptance );
  DECLARE_TOOL( TrackSelector );
  DECLARE_TOOL( BIntegrator );
  DECLARE_TOOL( TrackPtKick );                                                 

}
