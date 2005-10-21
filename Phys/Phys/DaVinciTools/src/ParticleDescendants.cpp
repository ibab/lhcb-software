// $Id: ParticleDescendants.cpp,v 1.1 2005-10-21 14:48:07 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "ParticleDescendants.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ParticleDescendants
//
// 2005-10-19 : Patrick KOPPENBURG
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<ParticleDescendants>          s_factory ;
const        IToolFactory& ParticleDescendantsFactory = s_factory ; 


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
ParticleDescendants::~ParticleDescendants() {}; 

//=============================================================================
// Return all descendants of a Particle
//=============================================================================
const ParticleVector ParticleDescendants::descendants(const Particle* P){
  return descendants( P, 0 );
}  
//=============================================================================
// Return all stable descendants of a Particle
//=============================================================================
const ParticleVector ParticleDescendants::finalStates(const Particle* P){
  ParticleVector stables ; // 
  ParticleVector all = descendants( P, 0 ); // get them all all
  for ( ParticleVector::const_iterator i = all.begin() ; i!=all.end() ; ++i){
    if ( 0 == (*i)->endVertex() ) {
      stables.push_back( *i );
      debug() << "Saving a " << (*i)->particleID().pid() << endmsg ;
    } else verbose() << "Discarding a " << (*i)->particleID().pid() << endmsg ;
  } 
  return stables;
}
//=============================================================================
// Return all daughters of particles in a vector
//=============================================================================
bool ParticleDescendants::addDaughters(const Particle* M, 
                                       ParticleVector& Parts){
  if ( 0==M ) return false;
  if ( 0==M->endVertex()) return false;
  SmartRefVector< Particle > dauts = M->endVertex()->products();
  for ( SmartRefVector< Particle >::iterator i =  dauts.begin();
        i != dauts.end() ; ++i ){
    Particle* P = *i;
    Parts.push_back(P);
  }
  verbose() << "Added " << dauts.size() << " daughters" << endmsg ;
  return (!dauts.empty());
  
}
//=============================================================================
// Return all daughters of particles in a vector
//=============================================================================
bool ParticleDescendants::addDaughters(const ParticleVector& mothers, 
                                       ParticleVector& Parts){
  if ( mothers.empty() ) return false ;
  bool found = false ;
  for ( ParticleVector::const_iterator i = mothers.begin() ; i!=mothers.end() ; ++i){
    found = ( addDaughters( *i, Parts) || found );
  }
  return found ;
}  
//=============================================================================
// Return all descendants of level i for a Particle
//=============================================================================
const ParticleVector ParticleDescendants::descendants(const Particle* P, int maxlevel){
  
  ParticleVector Parts ;

  int level = 0 ;
  bool found = false ;
  
  ParticleVector mothers ;

  do {
    ++level ;
    ParticleVector leveldaughters ;
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
