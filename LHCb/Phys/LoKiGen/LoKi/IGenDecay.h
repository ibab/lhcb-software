// $Id: IGenDecay.h,v 1.2 2009-06-02 16:49:17 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_IGENDECAY_H 
#define LOKI_IGENDECAY_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/iTree.h"
#include "LoKi/DecayFinder.h"
#include "LoKi/IDecayNode.h"
// ============================================================================
// forward declaration
// ============================================================================
namespace HepMC { class GenParticle ; }
// ============================================================================
namespace Decays 
{
  // ==========================================================================
  /** @class IGenDecay LoKi/IGenDecay.h
   *  New "Decay Finder"
   *  @author Ivan BELYAEV
   *  @date   2009-05-22
   */
  class IGenDecay : public virtual IDecayNode
  {
    // ========================================================================
  public:
    // ========================================================================
    // the actual type of the tree 
    typedef Decays::Tree_<const HepMC::GenParticle*>                   Tree   ;
    // the actual type of decay finder:
    typedef Decays::Finder_<const HepMC::GenParticle*>                 Finder ;
    // ========================================================================
  public:
    // ========================================================================
    /** create the decay tree from the descriptor
     *  @param descriptor (INPUT)  the decay descriptor 
     *  @param tree       (OUTPUT) the decay tree 
     */
    virtual Tree tree ( const std::string& decay = "" ) const = 0 ;
    // ========================================================================
  public:
    // ========================================================================
    /// the unique interface ID 
    static const InterfaceID& interfaceID() ;        // the unique interface ID 
    // ========================================================================
  protected:
    // ========================================================================
    /// virtual & protected destructor 
    virtual ~IGenDecay () ;                   // virtual & protected destructor 
    // ========================================================================
  } ;
  // ==========================================================================
} // end of namespace Decays 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_IGENDECAY_H
// ============================================================================
