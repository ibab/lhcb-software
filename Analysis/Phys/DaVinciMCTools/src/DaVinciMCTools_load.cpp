// $Id: DaVinciMCTools_load.cpp,v 1.12 2005-12-19 10:17:08 pkoppenb Exp $
// Include files 


#include "GaudiKernel/DeclareFactoryEntries.h"

// Declare  OBJECT / CONVERTER / ALGORITHM / TOOL using the macros DECLARE_xxx
// The statements are like that:
//
// DECLARE_CONVERTER( MyConverter );
// DECLARE_OBJECT( DataObject );
//
// They should be inside the 'DECLARE_FACTORY_ENTRIES' body.

DECLARE_FACTORY_ENTRIES(DaVinciMCTools) {

  DECLARE_TOOL( DebugTool );
  DECLARE_TOOL( MCParticleMaker );
  DECLARE_TOOL( TrueMCFilterCriterion );
  DECLARE_TOOL( CheatedLifetimeFitter );
  DECLARE_TOOL( BackgroundCategory );

  DECLARE_ALGORITHM( MCPIDProtoPAlg );
  DECLARE_ALGORITHM( PrintMCTree );
  DECLARE_ALGORITHM( PrintTree );
  DECLARE_ALGORITHM( DumpEvent );
  DECLARE_ALGORITHM( FilterGoodDecay );
  DECLARE_ALGORITHM( IsBEvent );
  DECLARE_ALGORITHM( DecayChainNTuple );  
  DECLARE_ALGORITHM( PrintBackgroundCategory );  

}
