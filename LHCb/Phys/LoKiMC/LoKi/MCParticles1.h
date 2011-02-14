// $Id: MCParticles1.h,v 1.1 2010-05-31 20:33:54 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_MCPARTICLES1_H 
#define LOKI_MCPARTICLES1_H 1
// ============================================================================
// Include files
// ============================================================================
// PartProp
// ============================================================================
#include "Kernel/iNode.h"
// ============================================================================
// LoKi 
// ============================================================================
#include "LoKi/iTree.h"
#include "LoKi/MCTypes.h"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace MCParticles
  {
    // ========================================================================
    /** @class DecNode
     *  simple predicate, which acts on the particleID fo the particle
     *  @see LHCb::ParticleID
     *  @see Decays::iNode
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-12-17
     */
    class GAUDI_API DecNode
      : public LoKi::BasicFunctors<const LHCb::MCParticle*>::Predicate
    {
    public:
      // ======================================================================
      /// constructor from the actual node
      DecNode ( const Decays::iNode& node ) ;
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
      // ======================================================================
    };
    // ========================================================================    
    /** @class DecTree
     *  simple predicate, which acts on the decay structuire/tree for the particle
     *  @see Decays::iTree
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-12-17
     */
    class GAUDI_API DecTree
      : public LoKi::BasicFunctors<const LHCb::MCParticle*>::Predicate
    {
    public:
      // ======================================================================
      /// the actual type of tree (interface
      typedef Decays::iTree_<const LHCb::MCParticle*> iTree ; // the tree
      // ======================================================================
    private:
      // ======================================================================
      /// the actual type of tree (assignable)
      typedef Decays::Tree_<const LHCb::MCParticle*>   Tree ; // the tree
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from the actual node
      DecTree ( const iTree&       node ) ;
      /// constructor from the actual node
      DecTree ( const std::string& node ) ;
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
      void reset() const { tree().reset() ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the decay tree itself
      Tree m_tree ;                                    // the decay tree itself
      // ======================================================================
    };
    // ========================================================================
  } //                                       end of namespace LoKi::MCParticles 
  // ==========================================================================
  namespace Cuts
  {
    // ========================================================================
    /** @typedef MCDECNODE
     *  the trivial predicate whcih acts on ParticleID
     *
     *
     *  @code
     *
     *  // construct some node:
     *  Decays::Node node = Decays::Nodes::Lepton() && Decays::Nodes::Positive() ;
     *
     *  const LHCb::MCParticle* p = .... ;
     *
     *  // create the predicate:
     *  MCCut good = MCDECNODE( node  ) ;
     *
     *  // use the predicate:
     *  const ok = good ( p ) ;
     *
     *  @endcode
     *
     *  @see LHCb::ParticleID
     *  @see LHCb::ParticleID
     *  @see Decays::iNode
     *  @see Decays::Node
     *  @see Decays::Nodes
     *  @author Vanya BELYAEV Ivane.Belyaev@nikhef.nl
     *  @date 2008-12-17
     */
    typedef LoKi::MCParticles::DecNode                              MCDECNODE ;
    // ========================================================================
    /** @typedef MCDECTREE
     *  the trivial predicate which acts on decay structure
     *
     *  @code
     *
     *  // construct some tree
     *  Decays::Tree_<const LHCb::MCParticle*> tree =  ... ;
     *
     *  const LHCb::MCParticle* p = .... ;
     *
     *  // create the predicate:
     *  MCCut good = MCDECTREE ( tree  ) ;
     *
     *  // use the predicate:
     *  const ok = good ( p ) ;
     *
     *  @endcode
     *
     *  @see LHCb::ParticleID
     *  @see Decays::iTree_
     *  @see Decays::Tree_
     *  @see Decays::Trees
     *  @author Vanya BELYAEV Ivane.Belyaev@nikhef.nl
     *  @date 2008-12-17
     */
    typedef LoKi::MCParticles::DecTree                              MCDECTREE ;
    // ========================================================================
  } // end of namespace LoKi::Cuts 
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_MCPARTICLES1_H
// ============================================================================
