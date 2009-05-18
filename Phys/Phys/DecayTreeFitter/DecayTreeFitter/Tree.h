#ifndef DECAYTREEFITTER_TREE_H
#define DECAYTREEFITTER_TREE_H

#include <map>
#include <vector>

namespace LHCb
{
  class Particle ;
  class ParticleID ;
}

namespace decaytreefit
{
  class Tree
  {
  public:
    //
    typedef std::vector<const LHCb::Particle*> ParticleContainer ;
    // constructor
    Tree( const LHCb::Particle& head ) { m_head = cloneTree(head,m_clonemap) ; }
    // copy constructor is destructive ( do we really want that? )
    Tree( Tree& rhs ) : m_head(rhs.m_head), m_clonemap(rhs.m_clonemap) { rhs.release() ; }
    // destructor
    ~Tree() { if (m_head ) deleteTree(*m_head) ; }
    // return the const head
    const LHCb::Particle* head() const { return m_head ; }
    // return the non-const head
    LHCb::Particle* head() { return m_head ; }
    // take ownership of all particles in decay tree
    LHCb::Particle* release() ;
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
  private:
    typedef std::map< const LHCb::Particle*, const LHCb::Particle*> CloneMap ;
    static LHCb::Particle* cloneTree ( const LHCb::Particle& original, CloneMap& clonemap );
    static void deleteTree( LHCb::Particle& particle ) ;
    
  private:
    LHCb::Particle* m_head ;
    CloneMap m_clonemap ;
  } ;  
}

#endif
