// 
// Include files 

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/ObjectFactory.h"

DECLARE_FACTORY_ENTRIES( VertexFitCheck ) {

  DECLARE_ALGORITHM( VtxChecker );
  DECLARE_ALGORITHM( ProperTimeChecker );
  DECLARE_ALGORITHM( PVChecker );

}
