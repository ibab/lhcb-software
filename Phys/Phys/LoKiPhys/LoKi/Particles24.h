// $Id: Particles24.h,v 1.2 2010-01-14 13:30:16 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_PARTICLES24_H 
#define LOKI_PARTICLES24_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <iosfwd>
// ============================================================================
// PartProp
// ============================================================================
#include "Kernel/iNode.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/BasicFunctors.h"
#include "LoKi/iTree.h"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace Particles 
  {
    // ========================================================================
    /** @class DecNode 
     *  simple predicate, which acts on the particleID fo the particle
     *  @see LHCb::ParticleID
     *  @see Decays::iNode 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-12-17
     */
    class DecNode 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Predicate
    {
    public:
      // ======================================================================
      /// constructor from the actual node 
      DecNode ( const Decays::iNode&      node  , 
                const bool autovalidate = true  ) ;
      /// MANDATORY: virtual destructor 
      virtual ~DecNode() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual DecNode* clone() const { return new DecNode ( *this ) ; } 
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      DecNode () ;                       // the default constructor is disabled 
      // ======================================================================
    public:
      // ======================================================================
      /// get the decay node 
      const Decays::iNode& node() const { return m_node.node () ; }
      /// valid node ? 
      bool valid() const { return m_node.valid() ; } 
      // validate the node 
      StatusCode validate ( const LHCb::IParticlePropertySvc* svc ) const 
      { return m_node.validate ( svc ) ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the decay node itself 
      Decays::Node m_node ;                            // the decay node itself 
      /// autovalidate ? 
      bool m_autovalidate ;                                     // autovalidate
      // ======================================================================
    };  
    // ========================================================================
    /** @class DecTree 
     *  simple predicate, which acts on the decay structuire/tree for the particle
     *  @see Decays::iTree 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-12-17
     */
    class DecTree 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Predicate
    {
    public:
      // ======================================================================
      /// the actual type of tree (interface
      typedef Decays::iTree_<const LHCb::Particle*> iTree ; // the tree 
      // ======================================================================
    private:
      // ======================================================================
      /// the actual type of tree (assignable) 
      typedef Decays::Tree_<const LHCb::Particle*>   Tree ; // the tree 
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from the actual node 
      DecTree ( const iTree& node , const bool autovalidate = true ) ;
      /// MANDATORY: virtual destructor 
      virtual ~DecTree() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual DecTree* clone() const { return new DecTree ( *this ) ; } 
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      DecTree () ;                       // the default constructor is disabled 
      // ======================================================================
    public:
      // ======================================================================
      /// get the decay node 
      const iTree& tree() const { return m_tree.tree () ; }
      // ======================================================================
      /// valid tree ? 
      bool valid () const { return m_tree.valid() ; } 
      // validate the teh tree 
      StatusCode validate ( const LHCb::IParticlePropertySvc* svc ) const 
      { return m_tree.validate ( svc ) ; }
      // reset the collection 
      void reset() const { m_tree.reset() ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the decay tree itself 
      Tree m_tree ;                                    // the decay tree itself 
      /// autovalidate ? 
      bool m_autovalidate ;                                     // autovalidate
      // ======================================================================
    };  
    // ========================================================================
  } //                                         end of namespace LoKi::Particles 
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PARTICLES24_H
// ============================================================================
