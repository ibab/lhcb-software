
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
  m_ignoreUnmatchedDescriptors = false;
  declareProperty("Tool", m_toolName, "Name of RelatedInfoTool" );
  declareProperty("Location", m_topInfo, "Location of RelatedInfo objects for top-level particle");
  declareProperty("DaughterLocations", m_daughterInfo, "Locations of RelatedInfo objects for daughters");
  declareProperty("IgnoreUnmatchedDescriptors", m_ignoreUnmatchedDescriptors, 
                  "If set to true, silently ignore daughter descriptors not matching any particle");
}

//=======================================================================
StatusCode AddRelatedInfo::initialize()
{
  const StatusCode sc = DaVinciAlgorithm::initialize();
  if ( sc.isFailure() ) return sc;

  m_tool = tool<IRelatedInfoTool>(m_toolName,this);

  for ( const auto & loc : m_daughterInfo )
  {
     auto selector = std::unique_ptr<LoKi::Child::Selector>(new LoKi::Child::Selector( loc.first ));
     m_childSelectors.emplace(loc.first, std::move(selector));
  }

  debug() << "Daughter Info Locations : " << m_daughterInfo << endmsg;
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
      verbose() << " Found "<< parts.size() << " particles at " << location << endmsg;

    // Loop over particles in the locations
    for ( const Particle * p : parts )
    {
      Particle * c = const_cast<Particle*>(p);

      if (m_topInfo.size() > 0) {
        fill( c, c, tmpLoc + "/" + m_topInfo );
      }

      for ( const auto & loc : m_daughterInfo ) {
        const Particle* daughter = m_childSelectors[loc.first]->child(c);
        if (daughter) {
          fill( c, daughter, tmpLoc + "/" + loc.second );
        } else {
          if (!m_ignoreUnmatchedDescriptors) {
            if ( msgLevel(MSG::WARNING) ) warning() << "No daughter found in " << location << 
                                          " matching " << loc.first << endmsg;
            return StatusCode::FAILURE;
          }
        }
      }
    }
  }

  return StatusCode::SUCCESS;
}

//==========================================================================

void AddRelatedInfo::fill( const Particle* top,
                           const Particle* c,
                           const std::string &map_location )
{
    if (msgLevel(MSG::DEBUG))
      debug() << "GetOrCreate RelatedInfo at " << map_location << endmsg;

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
      if (msgLevel(MSG::DEBUG)) debug() << "Got empty RelatedInfoMap. " << endmsg;
    } else {
      if (msgLevel(MSG::DEBUG)) debug() << "Got RelatedInfoMap : " << *map << endmsg;
      relation->i_relate(top, *map);
    }

}
