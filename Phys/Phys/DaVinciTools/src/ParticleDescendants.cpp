// $Id$
// Include files 

// local
#include "ParticleDescendants.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ParticleDescendants
//
// 2005-10-19 : Patrick KOPPENBURG
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( ParticleDescendants )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ParticleDescendants::ParticleDescendants( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IParticleDescendants>(this);

}
//=============================================================================
// Destructor
//=============================================================================
ParticleDescendants::~ParticleDescendants() {} 

//=============================================================================
// Return all descendants of a Particle
//=============================================================================
const LHCb::Particle::ConstVector ParticleDescendants::descendants(const LHCb::Particle* P){
  return descendants( P, 0 );
}  
//=============================================================================
// Return all stable descendants of a Particle
//=============================================================================
const LHCb::Particle::ConstVector ParticleDescendants::finalStates(const LHCb::Particle* P){
  LHCb::Particle::ConstVector stables ; // 
  LHCb::Particle::ConstVector all = descendants( P, 0 ); // get them all all
  for ( LHCb::Particle::ConstVector::const_iterator i = all.begin() ; i!=all.end() ; ++i){
    if ( (*i)->isBasicParticle() ) {
      stables.push_back( *i );
      debug() << "Saving a " << (*i)->particleID().pid() << endmsg ;
    } else verbose() << "Discarding a " << (*i)->particleID().pid() << endmsg ;
  } 
  return stables;
}
//=============================================================================
// Return all daughters of particles in a vector
//=============================================================================
bool ParticleDescendants::addDaughters(const LHCb::Particle* M, 
                                       LHCb::Particle::ConstVector& Parts){
  if ( 0==M ) return false;
  if ( M->isBasicParticle() ) return false;  
  const LHCb::Particle::ConstVector dauts = M->daughtersVector();
  Parts.insert(Parts.end(),dauts.begin(),dauts.end());
  verbose() << "Added " << dauts.size() << " daughters" << endmsg ;
  return (!dauts.empty());
  
}
//=============================================================================
// Return all daughters of particles in a vector
//=============================================================================
bool ParticleDescendants::addDaughters(const LHCb::Particle::ConstVector& mothers, 
                                       LHCb::Particle::ConstVector& Parts){
  if ( mothers.empty() ) return false ;
  bool found = false ;
  for ( LHCb::Particle::ConstVector::const_iterator i = mothers.begin() ; i!=mothers.end() ; ++i){
    found = ( addDaughters( *i, Parts) || found );
  }
  return found ;
}  
//=============================================================================
// Return all descendants of level i for a Particle
//=============================================================================
const LHCb::Particle::ConstVector ParticleDescendants::descendants(const LHCb::Particle* P, int maxlevel){
  
  LHCb::Particle::ConstVector Parts ;

  int level = 0 ;
  bool found = false ;
  
  LHCb::Particle::ConstVector mothers ;

  do {
    ++level ;
    LHCb::Particle::ConstVector leveldaughters ;
    if ( level == 1 ) found = addDaughters( P, leveldaughters) ;
    else found = addDaughters( mothers, leveldaughters) ;
    if ( level==maxlevel || maxlevel == 0 ) {
      Parts.insert(Parts.end(),leveldaughters.begin(),leveldaughters.end()) ;
      verbose() << "Level: " << level << " - inserted " << leveldaughters.size() 
                << " daughters to get " << Parts.size() << endmsg ;
    }
    verbose() << "Level " << level << " of " << maxlevel << " : " 
              << leveldaughters.size() << " daughters " << found << endmsg ;
    mothers = leveldaughters ;
  } while ( (maxlevel<=0 || level<=maxlevel) && found ) ;
  
  debug() << "Reached " << level << ". Returning " << Parts.size() 
          << " daughters" << endmsg ;
  
  return Parts ;
}  
