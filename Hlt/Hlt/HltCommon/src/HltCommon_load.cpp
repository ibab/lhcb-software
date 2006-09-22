#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( HltCommon ) {

  DECLARE_ALGORITHM( HltInit );
  DECLARE_ALGORITHM( HltEnd );

  DECLARE_ALGORITHM( HltSequencer );

  DECLARE_ALGORITHM( L0Entry );
  //  DECLARE_ALGORITHM( HltFilterAlgorithm );
  DECLARE_ALGORITHM( HltTrackIPSelection );
  DECLARE_ALGORITHM( HltMonitor );

  DECLARE_TOOL( HltDataStore );

}
