// $Id: DaVinciMCTools_load.cpp,v 1.16 2006-08-03 14:39:37 jpalac Exp $
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
/// @todo Finish and test TrueMCFilterCriterion
  DECLARE_TOOL( TrueMCFilterCriterion );
/// @todo Restore CheatedLifetimeFitter tool if needed
//  DECLARE_TOOL( CheatedLifetimeFitter ); 
/// @todo Restore BackgroundCategory tool
//  DECLARE_TOOL( BackgroundCategory ); 

/// @todo Restore MCPIDProtoPAlg and move to someplace in REC
//  DECLARE_ALGORITHM( MCPIDProtoPAlg ); 
  DECLARE_ALGORITHM( PrintMCTree );
  DECLARE_ALGORITHM( PrintTree );
  DECLARE_ALGORITHM( FilterGoodDecay );
  DECLARE_ALGORITHM( IsBEvent );
/// @todo Restore DecayChainNTuple
//  DECLARE_ALGORITHM( DecayChainNTuple );  
 /// @todo Restore PrintBackgroundCategory
//  DECLARE_ALGORITHM( PrintBackgroundCategory ); 
  DECLARE_ALGORITHM( MCParticleLinkerMaker ); 


}
