#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( HltCommon ) {

  DECLARE_ALGORITHM( TrgVertexToTVertex);
  DECLARE_ALGORITHM( HltTrackIPSelection );

  DECLARE_ALGORITHM( HltEntry );
  DECLARE_ALGORITHM( HltDummy );

}
