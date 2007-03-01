//====================================================================
// FlavourTaggingChecker_load.cpp
//--------------------------------------------------------------------
//
//	Package   : FlavourTaggingChecker
//
//	Author    : P. Koppenburg for Marco Musy
//====================================================================

#include "GaudiKernel/DeclareFactoryEntries.h"
DECLARE_FACTORY_ENTRIES(FlavourTaggingChecker) {

  DECLARE_ALGORITHM(CheatedSelection);
  DECLARE_ALGORITHM(BTaggingChecker);
  DECLARE_ALGORITHM(DoubleTagCorrelation);
  DECLARE_ALGORITHM(BTaggingAnalysis);
  //DECLARE_TOOL(ForcedBDecayTool);
  
}
