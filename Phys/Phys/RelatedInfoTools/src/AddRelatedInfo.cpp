
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
AddRelatedInfo::AddRelatedInfo( const std::string& name,
                                ISvcLocator* pSvcLocator )
: DaVinciAlgorithm ( name, pSvcLocator )
{
  declareProperty("Tool", m_toolName, "Name of RelatedInfoTool" );
  declareProperty("MaxLevel", m_maxLevel = 0, "Maximum recursion level");
  declareProperty("InfoLocations", m_infoLocations, "Locations of RelatedInfo objects"); 
  //setProperty( "OutputLevel", 1 );
}

//=======================================================================
StatusCode AddRelatedInfo::initialize()
{
  const StatusCode sc = DaVinciAlgorithm::initialize();
  if ( sc.isFailure() ) return sc;

  m_tool = tool<IRelatedInfoTool>(m_toolName,this);

  // Make sure infoLocations start with /Event/
  InfoMap tmpMap;
  for ( const auto & loc : m_infoLocations )
  {
    std::string tmpLoc = loc.first;
    boost::erase_all( tmpLoc, "/Event/" );
    tmpLoc = "/Event/" + tmpLoc;
    tmpMap[tmpLoc] = loc.second;
  }
  m_infoLocations = tmpMap;

  debug() << "Info Locations : " << m_infoLocations << endmsg;
  
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
  for ( const auto& loc : inputLocations() )
  {
    std::string tmpLoc = loc;
    boost::erase_all( tmpLoc, "/Event/" );
    tmpLoc = "/Event/" + tmpLoc;

    const std::string location = tmpLoc + "/Particles";
  
    const Particle::Range parts = getIfExists<Particle::Range>( location );
    if (msgLevel(MSG::VERBOSE)) 
      verbose() << " Found "<< parts.size() << " particles at " << location << endreq;

    // Loop over particles in the locations
    for ( const Particle * p : parts )
    {
      // Zero locations counter
      for ( const auto& loc2 : m_infoLocations ) 
      {
        std::string tmpLoc2 = loc2.first;
        boost::erase_all( tmpLoc2, "/Event/" );
        tmpLoc2 = "/Event/" + tmpLoc2;
        m_locationCounter[tmpLoc2] = 0;
      }
      Particle * c = const_cast<Particle*>(p);
      fill( c, c, 0, tmpLoc );
    }

  }

  return StatusCode::SUCCESS;
}

//==========================================================================

void AddRelatedInfo::fill( const Particle* top,
                           Particle* c, 
                           const int level, 
                           const std::string &top_location ) 
{
  const std::string c_location = ( c && c->parent() && c->parent()->registry() ?
                                   c->parent()->registry()->identifier() : "NotInTES" ); 
 
  bool isInLocations = false; 

  if (m_maxLevel > 0) 
  { 
    // check if the particle is in the list of info locations
    for ( const auto& infoLoc : m_infoLocations )
    {
      if ( c_location.compare(infoLoc.first) == 0 )
      {
        isInLocations = true; 
        break;
      }
    }
  } else {
    // No need to check locations for the top-level particle
    isInLocations = true; 
  }

  if ( isInLocations )
  {

    std::string map_location; 

    if (m_maxLevel > 0) {
      unsigned int iloc = m_locationCounter[c_location]; 
      
      if (iloc >= m_infoLocations[c_location].size() ) {
        warning() << "Index of particle at " << c_location << "(" << iloc << ")"
                  << "is >= than location vector size " << "(" << m_infoLocations[c_location].size() 
                  << "). Skipping RelatedInfo calculation. " << endreq; 
        return; 
      }
      
      if (msgLevel(MSG::DEBUG)) 
        debug() << "Filling RelatedInfo for particle " << iloc << " at location " << c_location << endreq;
      map_location = top_location + "/" + m_infoLocations[c_location][iloc];
    } else {
      // In case the line just filters the common particles and does not produce
      // its own candidates
      if (msgLevel(MSG::DEBUG)) 
        debug() << "Filling RelatedInfo for particle at top location " << top_location << endreq;
      map_location = top_location + "/" + m_infoLocations[top_location + "/Particles"][0];
    } 
    
    if (msgLevel(MSG::DEBUG)) 
      debug() << "GetOrCreate RelatedInfo at " << map_location << endreq;

    ParticleInfoRelation * relation = 
      getOrCreate<ParticleInfoRelation, ParticleInfoRelation>(map_location); 

    const StatusCode sc = m_tool->calculateRelatedInfo( top, c );
    if ( sc.isFailure() ) 
    {
      Warning( "Problem calculating related info" ).ignore();
      return;
    }

    RelatedInfoMap* map = m_tool->getInfo(); 
    
    if (map->size() == 0) {
      if (msgLevel(MSG::DEBUG)) debug() << "Got empty RelatedInfoMap. " << endreq;
    } else {
      if (msgLevel(MSG::DEBUG)) debug() << "Got RelatedInfoMap : " << *map << endreq;
      relation->i_relate(top, *map); 
      if (m_maxLevel > 0) m_locationCounter[c_location]++;
    }

  } 
  else 
  {
    if (msgLevel(MSG::VERBOSE)) 
      verbose() << "Particle at " << c_location << " not in the list, skipping" << endreq; 
  }
  
  // If we reached the maximum recursion level, we're done
  if ( level >= m_maxLevel ) return;
  
  // -- if the candidate is not a stable particle, call the function recursively
  if( ! c->isBasicParticle() )
  {
    // loop over daughters
    for ( const auto& const_dau : c->daughters() )
    {
      if ( msgLevel(MSG::DEBUG) ) 
        debug() << " Filling RelatedInfo for daughters of ID "
                << const_dau->particleID().pid() << endmsg;
      fill( top, const_cast<Particle*>(&*const_dau), level+1, top_location );
    }
  }

}
