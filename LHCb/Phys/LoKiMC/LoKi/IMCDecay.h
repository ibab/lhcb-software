// $Id: IMCDecay.h,v 1.2 2009-06-02 16:48:22 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_IMCDECAY_H 
#define LOKI_IMCDECAY_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/iTree.h"
#include "LoKi/DecayFinder.h"
#include "LoKi/IDecayNode.h"
// ============================================================================
// forward declarations
// ============================================================================
namespace LHCb { class MCParticle ; }
// ============================================================================
namespace Decays 
{
  // ==========================================================================
  /** @class IMCDecay LoKi/IMCDecay.h
   *  New "Decay Finder"
   *  @author Ivan BELYAEV
   *  @date   2009-05-22
   */
  class IMCDecay : public virtual IDecayNode
  {
    // ========================================================================
  public:
    // ========================================================================
    // the actual type of the tree 
    typedef Decays::Tree_<const LHCb::MCParticle*>                     Tree   ;
    // the actual type of decay finder:
    typedef Decays::Finder_<const LHCb::MCParticle*>                   Finder ;
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
    virtual ~IMCDecay () ;                    // virtual & protected destructor 
    // ========================================================================
  } ;
  // ==========================================================================
} // end of namespace Decays 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_IMCDECAY_H
// ============================================================================
