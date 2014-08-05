#ifndef ADDRELATEDINFO_H 
#define ADDRELATEDINFO_H 1

#include "Kernel/DaVinciAlgorithm.h"
#include "Relations/Relations.h"
#include "Kernel/IRelatedInfoTool.h"
#include "Event/RelatedInfoMap.h"

/** @class AddRelatedInfo AddRelatedInfo.h
 *
 *  Algorithm to add Particle isolation variables
 *  calculated by ConeVariables tool to particle extra info
 *
 *  @author Anton Poluektov
 *  @date   15/01/2014
 */

typedef LHCb::Relation1D<LHCb::Particle, LHCb::RelatedInfoMap> ParticleInfoRelation; 

class AddRelatedInfo : public DaVinciAlgorithm
{

public:

  /// Standard constructor
  AddRelatedInfo( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~AddRelatedInfo( ); ///< Destructor
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

private:

  std::string m_toolName;
  IRelatedInfoTool* m_tool;
  int m_maxLevel;
  std::map<std::string, std::string> m_infoLocations;

  void fill(const LHCb::Particle* top, LHCb::Particle* c, int level, const std::string &top_location);

  typedef std::map<std::string, ParticleInfoRelation> RelationLocationMap;
  mutable RelationLocationMap m_relMap;

};

//=======================================================================//
#endif // AddRelatedInfo_H
