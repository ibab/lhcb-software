// $Id: DaVinciMCTools_load.cpp,v 1.19 2006-10-18 15:51:07 jpalac Exp $
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
/// @todo test TrueMCFilterCriterion
  DECLARE_TOOL( TrueMCFilterCriterion );
/// @todo Restore CheatedLifetimeFitter tool if needed
//  DECLARE_TOOL( CheatedLifetimeFitter ); 
  DECLARE_TOOL( BackgroundCategory ); 

/// @todo Restore MCPIDProtoPAlg and move to someplace in REC
//  DECLARE_ALGORITHM( MCPIDProtoPAlg ); 
  DECLARE_ALGORITHM( PrintMCTree );
  DECLARE_ALGORITHM( PrintTree );
  DECLARE_ALGORITHM( FilterGoodDecay );
  DECLARE_ALGORITHM( IsBEvent );
  DECLARE_ALGORITHM( DecayChainNTuple );  
 /// @todo Restore PrintBackgroundCategory
//  DECLARE_ALGORITHM( PrintBackgroundCategory ); 
  DECLARE_ALGORITHM( MCParticleLinkerMaker ); 


}
