// $Id: RichTools_load.cpp,v 1.1.1.1 2004-06-17 12:04:08 cattanem Exp $
// Include files 

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( RichTools ) {

  // Declare Tools
  //===============

  // The tool registry
  DECLARE_TOOL( RichToolRegistry );

  // Ray tracing
  DECLARE_TOOL( RichRayTracing );  
  DECLARE_TOOL( CdfRayTracing ); 

  DECLARE_TOOL( RichPhotonReco );  
  DECLARE_TOOL( CdfPhotonReco ); 
  
  DECLARE_TOOL( CdfRichTrSegMakerFromTrStoredTracks ); 
  DECLARE_TOOL( RichTrSegMakerFromTrStoredTracks ); 
  
  // RichSmartID manipulation 
  DECLARE_TOOL( RichSmartIDTool );

  // new tools
  DECLARE_TOOL( RichMirrorSegFinder );

  // refractive index tools
  DECLARE_TOOL( RichTabulatedRefractiveIndex );

  // detector parameters
  DECLARE_TOOL( RichDetParameters );
  
}
