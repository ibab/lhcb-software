// 
// Include files 

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/ObjectFactory.h"

DECLARE_FACTORY_ENTRIES( VertexFit ) {

  // Vertex Fitters
  DECLARE_TOOL( LagrangeDirectionFitter );
  DECLARE_TOOL( LagrangeGeomVertexFitter );
  DECLARE_TOOL( LagrangeMassVertexFitter );
  DECLARE_TOOL( UnconstVertexFitter );  

  // Associated tools
  DECLARE_TOOL( GeomDispCalculator );
  DECLARE_TOOL( LifetimeFitter );

}
