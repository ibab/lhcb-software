#ifndef ADDRELATEDINFO_H 
#define ADDRELATEDINFO_H 1

// Gaudi
#include "GaudiKernel/Property.h"

// Kernel
#include "Kernel/DaVinciAlgorithm.h"
#include "Kernel/IRelatedInfoTool.h"
#include "Relations/Relations.h"

// boost
#include "boost/algorithm/string.hpp"

// event model
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

  void fill(const LHCb::Particle* top, LHCb::Particle* c, const int level, const std::string &top_location);

private:

  std::string m_toolName;
  IRelatedInfoTool* m_tool;
  int m_maxLevel;

  typedef std::map<std::string, std::vector<std::string> > InfoMap;
  InfoMap m_infoLocations;

  typedef std::map<std::string, ParticleInfoRelation> RelationLocationMap;
  mutable RelationLocationMap m_relMap;

  std::map<std::string, unsigned int> m_locationCounter; 

};

//=======================================================================//
#endif // AddRelatedInfo_H
