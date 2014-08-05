// Include files 
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/Property.h"
#include "Kernel/IRelatedInfoTool.h"

#include "boost/algorithm/string.hpp"
#include "Event/RelatedInfoMap.h"

// local
#include "AddRelatedInfo.h"

//-----------------------------------------------------------------------
// Implementation file for class : AddRelatedInfo
//
// Author: Anton Poluektov
//-----------------------------------------------------------------------

using namespace LHCb ;
using namespace Gaudi::Units;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( AddRelatedInfo )

//=======================================================================
AddRelatedInfo::AddRelatedInfo(const std::string& name,
                             ISvcLocator* pSvcLocator):
    DaVinciAlgorithm(name, pSvcLocator)
{
  declareProperty("Tool", m_toolName, "Name of RelatedInfoTool" );
  declareProperty("MaxLevel", m_maxLevel = 0, "Maximum recursion level");
  declareProperty("InfoLocations", m_infoLocations, "Locations of RelatedInfo objects"); 
}

//=======================================================================
StatusCode AddRelatedInfo::initialize()
{
  const StatusCode sc = DaVinciAlgorithm::initialize();
  if ( sc.isFailure() ) return sc;

  m_tool = tool<IRelatedInfoTool>(m_toolName,this);

  if (!m_tool) {
    error() << "Tuple not found, name = " << m_toolName << endreq;
    return StatusCode::FAILURE;
  }
  
  return sc;
}

AddRelatedInfo::~AddRelatedInfo() {}

//=======================================================================
// Main execution
//=======================================================================
StatusCode AddRelatedInfo::execute()
{
  setFilterPassed( true ); // Filter always passes

  m_relMap.clear(); 

  // Loop over input locations
  std::vector<std::string>::const_iterator iLoc = inputLocations().begin();
  std::vector<std::string>::const_iterator endLoc = inputLocations().end();
  for ( ; iLoc != endLoc; ++iLoc) 
  {
    const std::string location = (*iLoc) + "/Particles";

    const Particle::Range parts = getIfExists<Particle::Range>( location );
    if( parts.empty() ) 
    {
      if (msgLevel(MSG::VERBOSE)) verbose() << "No particles found at " << location << endreq;
      continue;
    }

    if (msgLevel(MSG::VERBOSE)) verbose() << " Found "<< parts.size() << " particles" <<endreq;

    // Loop over particles in the locations
    Particle::Range::const_iterator icand;
    for ( icand = parts.begin(); icand != parts.end(); icand++) {

      Particle* c = const_cast<Particle*>(*icand);

      fill(c, c, 0, *iLoc);
    }

  }

  return StatusCode::SUCCESS;
}

//==========================================================================

void AddRelatedInfo::fill(const Particle* top, Particle* c, int level, const std::string &top_location) {

  std::string c_location = c && c->parent() && c->parent()->registry() ?
                           c->parent()->registry()->identifier() : "NotInTES"; 

  bool isInLocations = false; 

  // check if the particle is in the list of info locations
  std::map<std::string, std::string>::const_iterator iInfoLocation = m_infoLocations.begin(); 
  for (; iInfoLocation != m_infoLocations.end(); iInfoLocation++) {
    if (c_location.compare((*iInfoLocation).first) == 0) {
      isInLocations = true; 
      break;
    }
  }

  if (isInLocations) {

    if (msgLevel(MSG::DEBUG)) debug() << "Filling RelatedInfo for particle at " << c_location << endreq;

//    std::string key_location = c_location; 
//    boost::replace_all(key_location, "/Particles", ""); 

//    if (msgLevel(MSG::DEBUG)) debug() << "Key is " << key_location << ", info location is " << m_infoLocations[key_location] << endreq;

    std::string map_location = top_location + "/" + m_infoLocations[c_location];
    
    if (msgLevel(MSG::DEBUG)) debug() << "GetOrCreate RelatedInfo at " << map_location << endreq;

    ParticleInfoRelation* relation = getOrCreate<ParticleInfoRelation, ParticleInfoRelation>(map_location); 

    StatusCode sc = m_tool->calculateRelatedInfo(top, c);

    if (sc.isFailure()) {
      warning() << "Error calculating related info" << endreq;
      return;
    }

    RelatedInfoMap* map = m_tool->getInfo(); 

    if (msgLevel(MSG::DEBUG)) {
      debug() << "Got RelatedInfoMap, contents as follows: " << endreq;
      RelatedInfoMap::iterator i; 
      for (i = map->begin(); i != map->end(); i++) {
        short key = (*i).first;
        float val = (*i).second; 
        debug() << "  Key = " << key << ", val = " << val << endreq; 
      }
    }
      
    relation->i_relate(c, *map); 

  } else {
    if (msgLevel(MSG::VERBOSE)) verbose() << "Particle at " << c_location << " not in the list, skipping" << endreq; 
  }

  // If we reached the maximum recursion level, we're done
  if (level >= m_maxLevel) return;
  
  // Otherwise loop over the daughters of the particle
  const SmartRefVector< LHCb::Particle > & daughters = c->daughters();

  // -- if the candidate is not a stable particle, call the function recursively
  if( ! c->isBasicParticle() ) {
    for( SmartRefVector< LHCb::Particle >::const_iterator idau = daughters.begin() ; idau != daughters.end() ; ++idau){
      
      const Particle* const_dau = (*idau);
      Particle* dau = const_cast<Particle*>(const_dau); 
    
      if ( msgLevel(MSG::DEBUG) ) debug() << " Filling RelatedInfo for daughters of ID " << dau->particleID().pid() << endmsg;
      fill( top, dau, level+1, top_location );
    }
  }

}
