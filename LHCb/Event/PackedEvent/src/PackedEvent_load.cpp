// $Id: PackedEvent_load.cpp,v 1.1.1.1 2005-06-02 08:54:21 cattanem Exp $
// Include files 


#include "GaudiKernel/DeclareFactoryEntries.h"

// Declare  OBJECT / CONVERTER / ALGORITHM / TOOL using the macros DECLARE_xxx
// The statements are like that:
//
// DECLARE_ALGORITHM( MyAlgorithm );
// DECLARE_TOOL( MyTool );
// DECLARE_OBJECT( DataObject );
//
// They should be inside the 'DECLARE_FACTORY_ENTRIES' body.

DECLARE_FACTORY_ENTRIES(PackedEvent) {
  DECLARE_ALGORITHM( PackTrStateL );
  DECLARE_ALGORITHM( PackTrStateP );
  DECLARE_ALGORITHM( PackTrStoredTrack );
  DECLARE_ALGORITHM( PackMCParticle );
  DECLARE_ALGORITHM( PackMCVertex );
  DECLARE_ALGORITHM( PackCollision );

  DECLARE_ALGORITHM( UnpackTrStateL );
  DECLARE_ALGORITHM( UnpackTrStateP );
  DECLARE_ALGORITHM( UnpackTrStoredTrack );
  DECLARE_ALGORITHM( UnpackMCParticle );
  DECLARE_ALGORITHM( UnpackMCVertex );
  DECLARE_ALGORITHM( UnpackCollision );

  DECLARE_ALGORITHM( CompareTrStateL );
  DECLARE_ALGORITHM( CompareTrStateP );
  DECLARE_ALGORITHM( CompareTrStoredTrack );
  DECLARE_ALGORITHM( CompareMCParticle );
  DECLARE_ALGORITHM( CompareMCVertex );
  DECLARE_ALGORITHM( CompareCollision );
}
