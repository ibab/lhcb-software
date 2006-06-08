// 
// Include files 

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/ObjectFactory.h"

DECLARE_FACTORY_ENTRIES( VertexFit ) {

/// @todo Restore remaining vertex fitters using new interfaces
// Vertex Fitters
//  DECLARE_TOOL( LagrangeGlobalFitter );
  DECLARE_TOOL ( TrgVertexFitter );  
  DECLARE_TOOL ( OfflineVertexFitter );
  DECLARE_TOOL ( BlindVertexFitter );
  DECLARE_TOOL ( PropertimeFitter );
  DECLARE_TOOL ( DirectionFitter );
  DECLARE_TOOL ( PVReFitter );

}
