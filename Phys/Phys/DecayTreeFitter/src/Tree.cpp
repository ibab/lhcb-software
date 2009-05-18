#include "DecayTreeFitter/Tree.h"
#include "Event/Particle.h"
#include <boost/foreach.hpp>

namespace decaytreefit
{
  LHCb::Particle* Tree::release ()
  {
    LHCb::Particle* tmp = m_head ;
    m_head = 0 ;
    return tmp ;
  }

  LHCb::Particle* Tree::cloneTree( const LHCb::Particle& original,
				   CloneMap& clonemap)
  {
    LHCb::Particle* clone = new LHCb::Particle(original) ;
    clonemap[&original] = clone ;
    // copy the daughters
    SmartRefVector<LHCb::Particle> origdaughters = clone->daughters() ;
    // clear the daughters
    clone->setDaughters(SmartRefVector<LHCb::Particle>() ) ;
    for(SmartRefVector<LHCb::Particle>::const_iterator idau = origdaughters.begin() ;
	idau != origdaughters.end() ; ++idau ) 
      clone->addToDaughters( cloneTree( **idau, clonemap ) ) ;
    return clone ;
  }
  
  void Tree::deleteTree( LHCb::Particle& head ) 
  {
    // delete the daughters if they are of type TreeParticle
    BOOST_FOREACH( const LHCb::Particle* daughter, head.daughters() ) 
      if( daughter->parent() == 0 ) {
	deleteTree( const_cast<LHCb::Particle&>(*daughter) ) ;
	delete daughter ;
      }
    head.setDaughters(SmartRefVector<LHCb::Particle>() ) ;
  }
  
  const LHCb::Particle* Tree::findClone( const LHCb::Particle& orig ) const {
    CloneMap::const_iterator it = m_clonemap.find( &orig ) ;
    return it != m_clonemap.end() ? it->second : 0 ; 
  }

  void Tree::findInTree(const LHCb::Particle& particle,
			const LHCb::ParticleID& pid,
			ParticleContainer& result) {
    if( particle.particleID() == pid )  result.push_back( &particle ) ;
    BOOST_FOREACH( const LHCb::Particle* daughter, particle.daughters() ) 
      findInTree( *daughter, pid, result ) ;
  }
  
  const LHCb::Particle * Tree::findFirstInTree(const LHCb::Particle& particle,
					       const LHCb::ParticleID& pid) {
    const LHCb::Particle* rc(0) ;
    if( particle.particleID() == pid ) 
      rc = &particle ;
    else 
      BOOST_FOREACH( const LHCb::Particle* daughter, particle.daughters() ) {
	if( (rc = findFirstInTree(*daughter,pid) ) ) break ;
      }
    return rc ;
  }
  
  const LHCb::Particle* Tree::find( const LHCb::ParticleID& pid ) const {
    return findFirstInTree( *m_head, pid ) ;
  }
  
}
