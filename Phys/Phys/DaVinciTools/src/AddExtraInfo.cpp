// Include files 
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/Property.h"

#include "Kernel/IExtraInfoTool.h"
// local
#include "AddExtraInfo.h"

//-----------------------------------------------------------------------
// Implementation file for class : AddExtraInfo
//
// Author: Anton Poluektov
//-----------------------------------------------------------------------

using namespace LHCb ;
using namespace Gaudi::Units;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( AddExtraInfo )

//=======================================================================
  AddExtraInfo::AddExtraInfo(const std::string& name,
                             ISvcLocator* pSvcLocator):
    DaVinciAlgorithm(name, pSvcLocator)
{
  declareProperty("Tools", m_toolNames, "Names of ExtraInfoTools" );
  declareProperty("MaxLevel", m_maxLevel = 0, "Maximum recursion level");
}

//=======================================================================
StatusCode AddExtraInfo::initialize()
{
  const StatusCode sc = DaVinciAlgorithm::initialize();
  if ( sc.isFailure() ) return sc;

  m_tools.clear();

  std::vector<std::string>::iterator iTool;
  for (iTool = m_toolNames.begin(); iTool != m_toolNames.end(); iTool++) {
    IExtraInfoTool* t = tool<IExtraInfoTool>(*iTool,this);

    if (t) {
      m_tools.push_back(t);
    } else {
      error() << "Tuple not found, name = " << (*iTool) << endreq;
      return StatusCode::FAILURE;
    }
  }

  return sc;
}

AddExtraInfo::~AddExtraInfo() {}

//=======================================================================
// Main execution
//=======================================================================
StatusCode AddExtraInfo::execute()
{
  setFilterPassed( true ); // Filter always passes

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

      fill(c, c, 0);
    }

  }

  return StatusCode::SUCCESS;
}

//==========================================================================

void AddExtraInfo::fill(const Particle* top, Particle* c, int level) {

  std::list<IExtraInfoTool*>::iterator iTool;

  // Calculate extra information using each tool
  for (iTool = m_tools.begin(); iTool != m_tools.end(); iTool++) {

    StatusCode sc = (*iTool)->calculateExtraInfo(top, c);
    if (sc.isFailure()) {
      warning() << "Error calculating extra info" << endreq;
      continue;
    }

    int index = (*iTool)->getFirstIndex();

    for (int i=0 ; i < (*iTool)->getNumberOfParameters(); i++ ) {
      std::string name;
      double value;

      int result = (*iTool)->getInfo(index+i, value, name);

      if (result) {
        c->addInfo( index+i, value);
        if (msgLevel(MSG::DEBUG)) debug() << "Added extra info: " << name << "=" << value << endreq;
      }

    }

  }

  // If we reached the maximum recursion level, we're done
  if (level >= m_maxLevel) return;
  
  // Otherwise loop over the daughters of the particle
  const SmartRefVector< LHCb::Particle > & daughters = c->daughters();

  for( SmartRefVector< LHCb::Particle >::const_iterator idau = daughters.begin() ; idau != daughters.end() ; ++idau){
    if( !(*idau)->isBasicParticle() ) {
      // -- if it is not stable, call the function recursively
      
      const Particle* const_dau = (*idau);
      Particle* dau = const_cast<Particle*>(const_dau); 
    
      if ( msgLevel(MSG::DEBUG) ) debug() << " Filling ExtraInfo for daughters of ID " << dau->particleID().pid() << endmsg;
      fill( top, dau, level+1 );
    }
  }

}
