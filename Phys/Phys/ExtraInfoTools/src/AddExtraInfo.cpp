
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
  declareProperty("DaughterLocations", m_daughterLocations, "Locations of daughters");
}

//=======================================================================
StatusCode AddExtraInfo::initialize()
{
  const StatusCode sc = DaVinciAlgorithm::initialize();
  if ( sc.isFailure() ) return sc;

  m_tools.clear();

  for ( std::vector<std::string>::iterator iTool = m_toolNames.begin();
        iTool != m_toolNames.end(); ++iTool )
  {
    IExtraInfoTool* t = tool<IExtraInfoTool>(*iTool,this);
    if ( t )
    {
      m_tools.push_back(t);
    }
    else
    {
      return Error( "Tool not found, name = " + (*iTool) );
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
  for ( std::vector<std::string>::const_iterator iLoc = inputLocations().begin();
        iLoc != inputLocations().end(); ++iLoc )
  {
    const std::string location = (*iLoc) + "/Particles";

    const Particle::Range parts = getIfExists<Particle::Range>( location );
    if ( parts.empty() )
    {
      if (msgLevel(MSG::VERBOSE)) verbose() << "No particles found at " << location << endmsg;
      continue;
    }

    if (msgLevel(MSG::VERBOSE)) verbose() << " Found "<< parts.size() << " particles" <<endmsg;

    // Loop over particles in the locations
    for ( Particle::Range::const_iterator icand = parts.begin(); icand != parts.end(); ++icand )
    {
      Particle* c = const_cast<Particle*>(*icand);
      fill(c, c, 0);
    }

  }

  return StatusCode::SUCCESS;
}

//==========================================================================

void AddExtraInfo::fill( const Particle* top, Particle* c, int level )
{

  const std::string c_location = ( c && c->parent() && c->parent()->registry() ?
                                   c->parent()->registry()->identifier() : "NotInTES" );

  // For particles other than top of the decay tree,
  // check if they are in the list of daughter locations
  const bool isInDaughters = ( c == top ? false :
                               std::find( m_daughterLocations.begin(),
                                          m_daughterLocations.end(),
                                          c_location ) != m_daughterLocations.end() );

  if ( c == top || isInDaughters )
  {

    if (msgLevel(MSG::DEBUG)) debug() << "Filling ExtraInfo for particle at " << c_location << endmsg;

    // Calculate extra information using each tool
    for ( std::list<IExtraInfoTool*>::iterator iTool = m_tools.begin();
          iTool != m_tools.end(); ++iTool )
    {
      StatusCode sc = (*iTool)->calculateExtraInfo(top, c);
      if ( sc.isFailure() )
      {
        Warning( "Error calculating extra info" ).ignore();
        continue;
      }

      int index = (*iTool)->getFirstIndex();

      for ( int i = 0; i < (*iTool)->getNumberOfParameters(); ++i )
      {

        std::string name;
        double value(0.0);
        const int result = (*iTool)->getInfo(index+i, value, name);

        if ( result )
        {

          if (c->hasInfo( index+i) )
          {
            std::ostringstream mess;
            mess << "Particle at " << c_location << " : rewriting ExtraInfo at key=" << index+i;
            Warning( mess.str() ).ignore();
          }

          c->addInfo( index+i, value);
          if (msgLevel(MSG::DEBUG))
            debug() << "Added extra info: " << name << "=" << value << endmsg;

        }

      }
    }
  }
  else
  {
    if (msgLevel(MSG::VERBOSE))
      verbose() << "Particle at " << c_location << " not in the list, skipping" << endmsg;
  }

  // If we reached the maximum recursion level, we're done
  if (level >= m_maxLevel) return;

  // Otherwise loop over the daughters of the particle
  const SmartRefVector< LHCb::Particle > & daughters = c->daughters();

  for ( SmartRefVector< LHCb::Particle >::const_iterator idau = daughters.begin();
        idau != daughters.end() ; ++idau )
  {
    if ( !(*idau)->isBasicParticle() )
    {
      // -- if it is not stable, call the function recursively

      const Particle* const_dau = (*idau);
      Particle* dau = const_cast<Particle*>(const_dau);

      if ( msgLevel(MSG::DEBUG) )
        debug() << " Filling ExtraInfo for daughters of ID " << dau->particleID().pid() << endmsg;
      fill( top, dau, level+1 );
    }
  }

}
