// $Id: DaVinciMCTools_load.cpp,v 1.13 2006-03-15 13:37:21 pkoppenb Exp $
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

/// @todo Restore DebugTool
//  DECLARE_TOOL( DebugTool );
  DECLARE_TOOL( MCParticleMaker );
/// @todo Restore TrueMCFilterCriterion
//  DECLARE_TOOL( TrueMCFilterCriterion );
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


}
