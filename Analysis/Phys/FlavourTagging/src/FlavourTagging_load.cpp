//====================================================================
// FlavourTagging_load.cpp
//--------------------------------------------------------------------
//
//	Package   : FlavourTagging
//
//	Author    : M.Musy
//====================================================================


#include "GaudiKernel/DeclareFactoryEntries.h"
DECLARE_FACTORY_ENTRIES(FlavourTagging) {

  DECLARE_ALGORITHM(BTagging);
  DECLARE_ALGORITHM(BTaggingMonitor);
  DECLARE_ALGORITHM(CheatedSelection);
  DECLARE_TOOL(SecondaryVertexTool);
  DECLARE_TOOL(NNetTool);

}
