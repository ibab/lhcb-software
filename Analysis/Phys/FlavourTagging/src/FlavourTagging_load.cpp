//====================================================================
// FlavourTagging_load.cpp
//--------------------------------------------------------------------
//
//	Package   : Tagging
//
//	Author    : O. Dormond
//  History   :
// +---------+----------------------------------------------+---------
// |    Date |                 Comment                      | Who     
// +---------+----------------------------------------------+---------
// | 15/06/01| Initial version                              | OD
// +---------+----------------------------------------------+---------
//====================================================================


#include "GaudiKernel/DeclareFactoryEntries.h"
DECLARE_FACTORY_ENTRIES(FlavourTagging) {
  DECLARE_ALGORITHM(FlavourTaggingAlgorithm);
  DECLARE_TOOL(ParticleTaggerCriterion);
  //  DECLARE_TOOL(SingleParticleTaggingTool);
  DECLARE_TOOL(OrderedTaggingTool);
}
