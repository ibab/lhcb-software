// $Id: RichTools_load.cpp,v 1.2 2004-06-17 14:52:09 jonesc Exp $
// Include files 

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( RichTools ) {

  // Declare Tools
  //===============

  // The tool registry
  DECLARE_TOOL( RichToolRegistry );

  // Ray tracing
  DECLARE_TOOL( RichRayTracing );  

  // photon reconstruction
  DECLARE_TOOL( RichPhotonReco );  
  
  // track segment makers
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
