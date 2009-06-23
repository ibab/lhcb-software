#ifndef DECAYTREEFITTER_TREE_H
#define DECAYTREEFITTER_TREE_H

#include <map>
#include <vector>
#include <memory>
#include "Event/Particle.h"

//namespace LHCb
//{
//  class Particle ;
//  class ParticleID ;
//}

namespace DecayTreeFitter
{
  class Tree
  {
  public:
    //
    typedef std::map< const LHCb::Particle*, LHCb::Particle*> CloneMap ;
    typedef std::vector<const LHCb::Particle*> ParticleContainer ;
    // constructor
    Tree( const LHCb::Particle& head ) { m_head.reset(cloneTree(head,m_clonemap)) ; }
    // copy constructor
    Tree( const Tree& rhs ) : m_head(rhs.m_head), m_clonemap(rhs.m_clonemap) {}
    // destructor
    ~Tree() { if (m_head.get()) deleteTree(*m_head) ; }
    // return the const head
    const LHCb::Particle* head() const { return m_head.get() ; }
    // return the non-const head
    LHCb::Particle* head() { return m_head.get() ; }
    // automatic dereference to a const particle
    const LHCb::Particle* operator->() const { return head() ; }
    // automatic dereference to a non-const particle
    LHCb::Particle* operator->() { return head() ; }
    // automatic type conversion to a const particle
    operator const LHCb::Particle* () const { return head() ;}
    // automatic type conversion to a non-const particle
    operator LHCb::Particle* () {return head() ;}
    // take ownership of all particles in decay tree
    LHCb::Particle* release() { return m_head.release() ; } 
    // find clone of particle in original decay tree
    const LHCb::Particle* findClone( const LHCb::Particle& particle ) const ;
    // find particle based on PID
    const LHCb::Particle* find( const LHCb::ParticleID& pid ) const ;
    // find a particle in a decay tree based on PID
    static const LHCb::Particle* findFirstInTree(const LHCb::Particle& particle,
						 const LHCb::ParticleID& pid ) ;
    // find a particle in a decay tree based on PID
    static void findInTree(const LHCb::Particle& particle,
			   const LHCb::ParticleID& pid,
			   ParticleContainer& result) ;

    // get access to the map for finding the clone of a particle 
    const CloneMap& cloneMap() const { return m_clonemap ; }
  private:
    static LHCb::Particle* cloneTree ( const LHCb::Particle& original, CloneMap& clonemap );
    static void deleteTree( LHCb::Particle& particle ) ;
    
  private:
    mutable std::auto_ptr<LHCb::Particle> m_head ;
    CloneMap m_clonemap ;
  } ;  
}

#endif
