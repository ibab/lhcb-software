// $Id$ 
// ============================================================================
#ifndef DAVINCIKERNEL_DECAYTREE_H
#define DAVINCIKERNEL_DECAYTREE_H
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <map>
#include <vector>
#include <memory>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
namespace LHCb
{
  // ==========================================================================
  /** @class DecayTree   Kernel/DecayTree.h
   *  Simple representation of the decay tree 
   *  Unline the standard particles, the ownership is kept by the mother particle
   *
   *  @attention this class have "destructive copy" semantics!
   *
   *  @see LHCb::Particle
   *  @author Wouter Hulsbergen 
   */
  class GAUDI_API DecayTree
  {
    // ========================================================================
  public:
    // ========================================================================
    /// the helper map to keep the links ebtween original and cloned tree
    typedef std::map< const LHCb::Particle*, LHCb::Particle*> CloneMap ;
    // ========================================================================
  public:
    // ========================================================================
    /** constructor from the decay head 
     *  @attention the decay tree is cloned!
     *  @param head the decay tree to be cloned
     */
    DecayTree ( const LHCb::Particle& head ) ;
    /** copy constructor 
     *  @attention "destructive copy" semantics: 
     *                head&clonemap for "right" are cleared!
     *  @param right object to be copied (invalid after copy) 
     */
    DecayTree ( const DecayTree& right ) ;
    /// default constructor 
    DecayTree () ;
    /** destructor
     *  @attention the cloned decay tree is deleted!
     */
    virtual ~DecayTree () ;
    // ========================================================================
  public:
    // ========================================================================
    /// return the const head
    inline const LHCb::Particle* head () const { return m_head.get () ; }
    /// return the non-const head
    inline       LHCb::Particle* head ()       { return m_head.get () ; }
    // ========================================================================
  public:
    // ========================================================================
    /// automatic dereference to a const particle
    inline const LHCb::Particle* operator->() const { return head () ; }
    /// automatic dereference to a non-const particle
    inline       LHCb::Particle* operator->()       { return head () ; }
    // ========================================================================
  public:
    // ========================================================================
    /// automatic type conversion to a const particle
    operator const LHCb::Particle* () const { return head () ; }
    /// automatic type conversion to a non-const particle
    operator       LHCb::Particle* ()       { return head () ; }
    // ========================================================================
  public:
    // ========================================================================
    /// valid decay tree ?
    bool valid    () const { return 0 != head  () ; }
    /// check the validity of the decay tree
    bool operator!() const { return   ! valid  () ; }
    // ========================================================================
  public:
    // ========================================================================
    /// take ownership of all particles in decay tree
    LHCb::Particle* release () ;
    // ========================================================================
  public:
    // ========================================================================
    /// find clone of particle in original decay tree
    const LHCb::Particle* findClone ( const LHCb::Particle&   particle ) const ;    
    /// find particle based on PID
    const LHCb::Particle* find      ( const LHCb::ParticleID& pid      ) const ;
    // ========================================================================
  public:
    // ========================================================================
    /// find a particle in a decay tree based on PID
    static const LHCb::Particle* 
    findFirstInTree
    ( const LHCb::Particle*        particle ,
      const LHCb::ParticleID&      pid      ) ;
    /// find a particle in a decay tree based on PID
    static void findInTree
    ( const LHCb::Particle*        particle ,
      const LHCb::ParticleID&      pid      ,
      LHCb::Particle::ConstVector& result   ) ;
    /// get access to the map for finding the clone of a particle 
    const CloneMap& cloneMap() const { return m_clonemap ; }
    // ========================================================================
  private:
    // ========================================================================
    /// the head of decay tree 
    mutable std::auto_ptr<LHCb::Particle> m_head ;    // the head of decay tree 
    /// links from the original tree to the cloned tree 
    mutable CloneMap m_clonemap ;               // links from the original tree
    // ========================================================================
  } ;  
  // ==========================================================================
} //                                                      end of namespace LHCb
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // DAVINCIKERNEL_DECAYTREE_H
// ============================================================================
