// 
// Include files 

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/ObjectFactory.h"

DECLARE_FACTORY_ENTRIES( VertexFit ) {

/// @todo Restore all vertex fitters using new interfaces
// Vertex Fitters
//   DECLARE_TOOL( LagrangeDirectionFitter ); 
//   DECLARE_TOOL( LagrangeGeomVertexFitter );
//   DECLARE_TOOL( LagrangeMassVertexFitter );
//   DECLARE_TOOL( UnconstVertexFitter );  
//   DECLARE_TOOL( KFFitTool );
  DECLARE_TOOL( TrgVertexFitter );  
  DECLARE_TOOL ( OfflineVertexFitter );
  DECLARE_TOOL ( BlindVertexFitter );
  /// @todo move src/MatrixUtils.h to LHCbMath and fix BlindVertexFitter accordingly
/// @todo Restore all other fitters
//   DECLARE_TOOL( LifetimeFitter );
//   DECLARE_TOOL( NewDirectionFitter );
//  DECLARE_TOOL( LagrangeGlobalFitter );
}
