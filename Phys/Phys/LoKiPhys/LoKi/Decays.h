// $Id: Decays.h,v 1.8 2009-05-23 15:59:51 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_DECAYS_H 
#define LOKI_DECAYS_H 1
// ============================================================================
// Include files
// ============================================================================
// PartProp
// ============================================================================
#include "Kernel/Nodes.h"
#include "Kernel/NodesPIDs.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Trees.h"
// ============================================================================
/** @file
 *  Implementation file for LoKi MC-specific decay/tree-functors 
 *  @author Vanya BELYAEV Ivan.BELYAEV@nikhef.nl
 *  @date   2008-05-25
 */
// ============================================================================
namespace Decays 
{
  // ========================================================================
  namespace Trees
  {
    // ======================================================================
    /** @class Exclusive 
     *  Simple sub tree which consists of the node ("mother") and 
     *  subtrees ("children"). Essentially it represent also the 
     *  "semi-exclusive" decay (as well as any decay with the fixed 
     *  number of components for comparisons).
     *  
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-06-11
     */
    class Exclusive: public Decays::iTree_<const LHCb::Particle*>
    {
    protected:
      // ======================================================================
      typedef const LHCb::Particle*                                  PARTICLE ;
      typedef Decays::Trees::Types_<PARTICLE>                           Types ;
      // ======================================================================
    public:
      // ======================================================================
      /// the actual type of the container of children trees 
      typedef Types::TreeList                                        TreeList ;
      // ======================================================================
    public:
      // ======================================================================
      /** full constructor from the node (mother), subtrees & flag
       *  @param mother the mother node
       *  @param children the list of daughter substrees
       *  @param alg the matching algorithm
       */
      Exclusive
      ( const Decays::iNode&       mother                 ,
        const TreeList&            children               ,
        const Alg                  alg        = Daughters ) ;
      /** constructor from the node (mother) & flag
       *  @param mother the mother node 
       *  @param alg the matching algorithm 
       */
      Exclusive
      ( const Decays::iNode&       mother                 , 
        const Alg                  alg        = Daughters ) ;
      /** constructor from the decay 
       *  @param decay  the decay       
       *  @param alg the matching algorithm 
       */
      Exclusive
      ( const Decays::Decay&       decay                  , 
        const Alg                  alg        = Daughters ) ;
      /// MANDATORY: virtual destructor 
      virtual ~Exclusive () {}
      // ====================================================================
    public:
      // ====================================================================
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Exclusive* clone() const { return new Exclusive ( *this ) ; }
      /// MANDATORY: the only one essential method: 
      virtual bool operator() 
        ( Decays::iTree_<PARTICLE>::argument p ) const ;
      /// MANDATORY: check the validity 
      virtual bool valid() const ;
      /// MANDATORY: validate the tree 
      virtual StatusCode validate 
      ( const LHCb::IParticlePropertySvc* svc ) const ;
      /// MANDATORY: reset the tree 
      virtual void reset () const ;
      /// collect the marked elements 
      virtual size_t collect 
      ( Decays::iTree_<PARTICLE>::Collection& output ) const ;
      /// MANDATORY: the specific printout 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
      /// has marked elements in tree? 
      virtual bool marked() const ;
      // ====================================================================
    public:
      // ====================================================================
      /// check the validness & mother 
      inline bool ok ( const LHCb::Particle* p ) const 
      { return 0 != p && mother ( p -> particleID() ) ; }
      // ====================================================================
      /// get the algorithm 
      Alg alg() const { return m_alg ; }
      /// set the algortihm 
      void setAlgorith ( const Alg value ) { m_alg = value ; }
      // ====================================================================
    public:
      // ====================================================================        
      /// set children 
      void setChildren ( const TreeList& children ) { m_children = children ; }
      /// set children 
      void setChildren ( const std::vector<std::string>& children ) ;
      /// set children 
      void setChildren ( const std::vector<Decays::Decay::Item>& children ) ;
      /// set children 
      void setChildren ( const std::vector<LHCb::ParticleID>& children ) ;
      /// set children 
      void setChildren 
      ( const std::vector<const LHCb::ParticleProperty*>& children ) ;
      // ====================================================================
    public:
      // ====================================================================        
      /// add one more daughter to the decay 
      void addDaughter ( const Decays::iTree_<PARTICLE>& tree ) ;
      /// add one more daughter to the decay 
      void addDaughter ( const Decays::iNode&          node ) ; 
      /// add one more daughter to the decay 
      void addDaughter ( const Decays::Decay::Item&    node ) ; 
      /// add one more daughter to the decay 
      void addDaughter ( const std::string&            node ) ; 
      /// add one more daughter to the decay 
      void addDaughter ( const LHCb::ParticleID&       node ) ; 
      /// add one more daughter to the decay 
      void addDaughter ( const LHCb::ParticleProperty* node ) ; 
      // ====================================================================        
    public:
      // ====================================================================
      /// add one more node to the tree 
      Exclusive& operator+= ( const Decays::iTree_<PARTICLE>& node )
      { addDaughter ( node ) ; return *this ; }
      /// add one more node to the tree 
      Exclusive& operator+= ( const Decays::iNode& node ) 
      { addDaughter ( node ) ; return *this ; }
      /// add one more node to the tree 
      Exclusive& operator+= ( const Decays::Decay::Item&   node ) 
      { addDaughter ( node ) ; return *this ; }
      /// add one more node to the tree 
      Exclusive& operator+= ( const std::string&           node ) 
      { addDaughter ( node ) ; return *this ; }
      /// add one more node to the tree 
      Exclusive& operator+= ( const LHCb::ParticleID&      node ) 
      { addDaughter ( node ) ; return *this ; }
      /// add one more node to the tree 
      Exclusive& operator+= ( const LHCb::ParticleProperty* node ) 
      { addDaughter ( node ) ; return *this ; }
      // ====================================================================
    protected:
      // ====================================================================
      inline const TreeList& children() const { return m_children ; }
      // ====================================================================
      TreeList::const_iterator  childBegin () const { return m_children.begin () ; }
      TreeList::const_iterator  childEnd   () const { return m_children.end   () ; }
      TreeList::const_reference front      () const { return m_children.front () ; }
      TreeList::const_reference back       () const { return m_children.back  () ; }
      // ======================================================================
      size_t nChildren () const { return m_children.size() ; }    
      // ======================================================================
      // reset the cache 
      inline void i_reset () const 
      {
        std::for_each 
          ( childBegin() , childEnd() , 
            std::mem_fun_ref (&_Tree_<PARTICLE>::reset) ) ;
      }
      // ======================================================================
      const Decays::iNode& mother () const { return m_mother ; }
      // ======================================================================
      inline bool mother ( const LHCb::ParticleID& pid ) const 
      { return m_mother.node ( pid ) ;  }
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      Exclusive () ;                     // the default constructor is disabled
      // ======================================================================
    private:
      // ======================================================================
      /// The mother 
      Decays::Node       m_mother   ;                          //    the mother 
      /// The children
      TreeList           m_children ;                          //  the children 
      /// The algorithm 
      Decays::Trees::Alg m_alg      ;                          // the algorithm 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Inclusive 
     *  Simple sub tree which consists of the node ("mother") and 
     *  subtrees ("children"). Essentially it represent the 
     *  inclusive decay.
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-06-11
     */
    class Inclusive : public Exclusive 
    {
    public:
      // ======================================================================
      /** full constructor from the node (mother), subtrees & flag
       *  @param mother the mother node
       *  @param children the list of daughter substrees
       *  @param alg the matching algorithm
       */
      Inclusive 
      ( const Decays::iNode&       mother                 ,
        const TreeList&            children               ,
        const Alg                  alg        = Daughters ) ;
      /** constructor from the node (mother) & flag
       *  @param mother the mother node 
       *  @param alg the matching algorithm 
       */
      Inclusive 
      ( const Decays::iNode&       mother                 , 
        const Alg                  alg        = Daughters ) ;
      /** constructor from the decay 
       *  @param decay  the decay       
       *  @param alg the matching algorithm 
       */
      Inclusive 
      ( const Decays::Decay&       decay                  , 
        const Alg                  alg        = Daughters ) ;
      /// constructor from exclusive decay 
      Inclusive ( const Exclusive& right ) ;
      /// MANDATORY: virtual destructor 
      virtual ~Inclusive () {}
      // ====================================================================
    public:
      // ====================================================================
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Inclusive* clone() const { return new Inclusive ( *this ) ; }
      /// MANDATORY: the only one essential method: 
      virtual bool operator() 
        ( Decays::iTree_<PARTICLE>::argument p ) const ;
      /// MANDATORY: check the validity of the tree 
      virtual bool valid() const ;
      /// MANDATORY: the specific printout 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
      // ====================================================================
    public:
      // ====================================================================
      /// add one more node to the tree 
      Inclusive& operator+= ( const Decays::iTree_<PARTICLE>& tree ) 
      { addDaughter ( tree ) ; return *this ; }
      /// add one more node to the tree 
      Inclusive& operator+= ( const Decays::iNode& node ) 
      { addDaughter ( node ) ; return *this ; }
      /// add one more node to the tree 
      Inclusive& operator+= ( const Decays::Decay::Item&   node ) 
      { addDaughter ( node ) ; return *this ; }
      /// add one more node to the tree 
      Inclusive& operator+= ( const std::string&         node ) 
      { addDaughter ( node ) ; return *this ; }
      /// add one more node to the tree 
      Inclusive& operator+= ( const LHCb::ParticleID&    node ) 
      { addDaughter ( node ) ; return *this ; }
      /// add one more node to the tree 
      Inclusive& operator+= ( const LHCb::ParticleProperty*    node ) 
      { addDaughter ( node ) ; return *this ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      Inclusive () ;                     // the default constructor is disabled
      // ======================================================================
    };
    // ========================================================================
    /** @class Optional
     *  Simple sub tree which consists of the node ("mother") and 
     *  subtrees ("children") and optional nodes. Essentially it represent the 
     *  decays with optional particles 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-04-13
     */
    class Optional : public Exclusive
    {
    public:
      // ======================================================================
      /** full constructor from the node (mother) and subtrees
       *  @param mother the mother node 
       *  @param children the list of children 
       *  @param optional the list of optional components 
       *  @param alg the matching algorithm 
       */
      Optional
      ( const Decays::iNode&       mother                   ,
        const TreeList&            children   = TreeList () ,
        const TreeList&            optional   = TreeList () ,
        const Alg                  alg        = Daughters   ) ;
      // ====================================================================
      /** constructor from decay descriptor, optional and flags 
       *  @param decay the decay descriptor
       *  @param optional the list of optional components 
       *  @param alg the matching algorithm 
       */
      Optional
      ( const Decays::Decay&       decay                    ,
        const TreeList&            optional   = TreeList () ,
        const Alg                  alg        = Daughters   ) ;
      /// constructor form the constructed tree 
      Optional 
      ( const Exclusive& right                  , 
        const TreeList&  optional = TreeList () ) ;
      /// MANDATORY: virtual destructor 
      virtual ~Optional () {}
      // ====================================================================
    public:
      // ====================================================================
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Optional* clone() const { return new Optional ( *this ) ; }
      /// MANDATORY: the only one essential method
      virtual bool operator() 
        ( Decays::iTree_<PARTICLE>::argument p ) const ;
      /// MANDATORY: check the validity of the tree 
      virtual bool valid() const ;
      /// MANDATORY: the specific printout 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
      /// MANDATORY: the proper validation of the tree
      virtual  StatusCode validate 
      ( const LHCb::IParticlePropertySvc* svc ) const ;
      /// has marked elements in tree? 
      virtual bool marked() const ;
      // ====================================================================
    public:
      // ====================================================================
      /// set optional
      void setOptional ( const TreeList& optional ) { m_optional = optional ; }
      /// set optional
      void setOptional ( const std::vector<std::string>&       optional ) ;
      /// set children 
      void setOptional ( const std::vector<Decays::Decay::Item>& optional ) ;
      /// set optional 
      void setOptional ( const std::vector<LHCb::ParticleID>&  optional ) ;
      /// set optional 
      void setOptional 
      ( const std::vector<const LHCb::ParticleProperty*>& optional ) ;
      // ====================================================================
    public:
      // ====================================================================
      /// add one more daughter to the decay 
      void addOptional ( const Decays::iTree_<PARTICLE>& tree ) ;
      /// add one more daughter to the decay 
      void addOptional ( const Decays::iNode& node ) ; 
      /// add one more daughter to the decay 
      void addOptional ( const std::string& node ) ; 
      /// add one more daughter to the decay 
      void addOptional ( const LHCb::ParticleID& node ) ; 
      /// add one more daughter to the decay 
      void addOptional ( const Decays::Decay::Item& node ) ; 
      /// add one more daughter to the decay 
      void addOptional ( const LHCb::ParticleProperty* node ) ; 
      // ====================================================================
    public:
      // ====================================================================
      /// add one more node to the tree 
      Optional& operator+= ( const Decays::iTree_<PARTICLE>& node ) 
      { addDaughter ( node ) ; return *this ; }
      /// add one more node to the tree 
      Optional& operator+= ( const Decays::iNode& node ) 
      { addDaughter ( node ) ; return *this ; }
      /// add one more node to the tree 
      Optional& operator+= ( const std::string& node ) 
      { addDaughter ( node ) ; return *this ; }
      /// add one more node to the tree 
      Optional& operator+= ( const LHCb::ParticleID& node ) 
      { addDaughter ( node ) ; return *this ; }
      /// add one more node to the tree 
      Optional& operator+= ( const Decays::Decay::Item& node ) 
      { addDaughter ( node ) ; return *this ; }
      /// add one more node to the tree 
      Optional& operator+= ( const LHCb::ParticleProperty* node ) 
      { addDaughter ( node ) ; return *this ; }
      // ======================================================================
    protected:
      // ======================================================================
      TreeList::const_iterator optBegin () const { return m_optional.begin () ; }
      TreeList::const_iterator optEnd   () const { return m_optional.end   () ; }
      size_t                   optSize  () const { return m_optional.size  () ; }
      size_t nOptional() const { return optSize() ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      Optional () ;                      // the default constructor is disabled
      // ======================================================================
    private:
      // ======================================================================
      /// the optional particles in the tree 
      TreeList m_optional ;               // the optional particles in the tree 
      // ======================================================================
    } ;
    // ========================================================================
  } //                                           end of namespace Decays::Trees 
  // ==========================================================================
} //                                              end of namespace LoKi::Decays
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_DECAYS_H
// ============================================================================
