// $Id$
// ============================================================================
#ifndef LOKI_MCDECAYS_H
#define LOKI_MCDECAYS_H 1
// ============================================================================
// Include files
// ============================================================================
// Part Prop
// ============================================================================
#include "Kernel/Nodes.h"
#include "Kernel/NodesPIDs.h"
// ============================================================================
// Event
// ============================================================================
#include "Event/MCParticle.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Trees.h"
#include "LoKi/MCSections.h"
// ============================================================================
/** @file
 *  Implementation file for LoKi MC-specific decay/tree-functors
 *  @author Vanya BELYAEV Ivan.BELYAEV@nikhef.nl
 *  @date   2008-05-25
 */
// ============================================================================
namespace Decays
{
  // ==========================================================================
  namespace Trees
  {
    // ======================================================================
    /** @class MCExclusive
     *  Simple sub tree which consists of the node ("mother") and
     *  subtrees ("children"). Essentially it represent also the
     *  "semi-exclusive" decay (as well as any decay with the fixed
     *  number of components for comparisons).
     *
     *  Define the regular decay, e.g. \f$ B^0 \to \pi^+ \pi^- \f$: 
     *
     *  @code 
     *
     *   // " B0 -> pi+ pi- " 
     *
     *   using namespace Decays ;
     *   typedef Decays::Trees::MCExclusive::TreeList Children ;
     *
     *   Children children ;
     *   children.push_back ( Trees::MCExclusive( "pi+" ) ) ;
     *   children.push_back ( Trees::MCExclusive( "pi-" ) ) ;
     * 
     *   const Trees::MCExclusive decay 
     *                ( "B0"     ,    // the parent 
     *                  children ) ;  // children  
     * 
     *  @endcode 
     * 
     *  Define the "decay-in-sections", e.g. all following 
     *  decays will be correctly matched with the descriptor 
     *  <c>" B0 --> pi+ pi- pi0 "</c>":
     *    - \f$ B^0 \to \left( \rho^0 \to \pi^+ \pi^-   \right) 
     *               \left( \pi^0  \to \gamma \gamma \right) \f$
     *    - \f$ B^0 \to \left( \rho^+ \to \pi^+ \pi^0   \right) \pi^-\f$ 
     *    - \f$ B^0 \to \left( \rho^- \to \pi^- \pi^0   \right) \pi^+\f$ 
     *
     *  @code 
     *
     *   // " B0 --> pi+ pi- pi0 " 
     *
     *   using namespace Decays ;
     *   typedef Decays::Trees::MCExclusive::SubTrees Children ;
     *
     *   Children children ;
     *   children.push_back ( Trees::MCExclusive ( "pi+" ) ) ;
     *   children.push_back ( Trees::MCExclusive ( "pi-" ) ) ;
     *   children.push_back ( Trees::MCExclusive ( "pi0" ) ) ;
     * 
     *   const Trees::MCExclusive decay 
     *                ( "B0"            ,   // the parent 
     *                  children        ,   // children  
     *                  Trees::Sections ) ; // matching algorithm 
     * 
     *  @endcode 
     * 
     * 
     *  <c>Algorithm</c> and <c>DecayOnly</c> are defined through 
     *  the type of "arrow":
     *
     *     - <c>"->"</c> Search withing the direct daughter particles, 
     *                   ignore the partricles form "non-decay" vertices 
     *     - <c>"-->"</c> Search withing the decay sections. 
     *                   ignore the partricles form "non-decay" vertices 
     *     - <c>"-x>"</c> Search withing the direct daughter particles, 
     *                    including the particles from non-decay vertices 
     *     - <c>"--x>"</c> Search withing the decay sections. 
     *                    including the particles from non-decay vertices 
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-04-13
     */
    class GAUDI_API MCExclusive: public Decays::iTree_<const LHCb::MCParticle*>
    {
    protected:
      // ======================================================================
      typedef const LHCb::MCParticle*                                PARTICLE ;
      /// type provider 
      typedef Decays::Trees::Types_<PARTICLE>                        Types    ;
      // ======================================================================
    public:
      // ======================================================================
      /// the actual type of the ocntainer of children trees
      typedef Types::TreeList                                        TreeList ;
      // ======================================================================
    public:
      // ======================================================================
      /** full constructor from the node (mother), subtrees & flags 
       *  @param mother the mother node
       *  @param children the list of daughter substrees
       *  @param alg the matching algorithm
       *  @param decayOnly the flag to search only through decay products
       *  @param oscillated check the oscilaltion flag
       */
      MCExclusive
      ( const Decays::iNode&       mother                 ,
        const TreeList&            children               ,
        const Alg                  alg        = Daughters ,
        const bool                 decayOnly  = true      ,
        const Oscillation          oscillated = Undefined ) ;
      /** constructor from the node (mother) and flags
       *  @param mother the mother node
       *  @param alg the matching algorithm
       *  @param decayOnly the flag to search only through decay products
       *  @param oscillated check the oscilaltion flag
       */
      MCExclusive
      ( const Decays::iNode&       mother                 ,
        const Alg                  alg        = Daughters ,
        const bool                 decayOnly  = true      ,
        const Oscillation          oscillated = Undefined ) ;
      /** constructor from the node (mother) and flags
       *  @param oscillated check the oscilaltion flag
       *  @param mother the mother node
       */
      MCExclusive
      ( const Oscillation          oscillated ,
        const Decays::iNode&       mother     ) ;
      /** constructor from the decay and flags 
       *  @param decay the decay descriptor
       *  @param alg the matching algorithm
       *  @param decayOnly the flag to search only through decay products
       *  @param oscillated check the oscilaltion flag
       */
      MCExclusive
      ( const Decays::Decay&       decay                  ,
        const Alg                  alg        = Daughters ,
        const bool                 decayOnly  = true      ,
        const Oscillation          oscillated = Undefined ) ;
      /// MANDATORY: virtual destructor
      virtual ~MCExclusive () {}
      // ======================================================================
    public:
      // ======================================================================
      /// MANDATORY: clone method ("virtual constructor")
      virtual  MCExclusive* clone() const { return new MCExclusive ( *this ) ; }
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
      /// has marked decays in tree ?
      virtual bool marked() const ;
      /// MANDATORY: the specific printout
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// MANDATORY: implementation of operator()
      virtual bool p_match ( Decays::iTree_<PARTICLE>::argument p ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// check "decay-only" flag
      bool decayOnly() const { return m_decayOnly ; }
      /// set "decay-only" flag
      void setDecayOnly  ( const bool value ) { m_decayOnly  = value ; }
      /// check "oscillation" flag
      Oscillation   oscillation() const { return m_oscillation ; }
      /// set "oscillated" flag
      void setOscillation ( const Oscillation   value ) { m_oscillation = value ; }
      /// check the validness and oscillation conditions
      inline bool ok ( const LHCb::MCParticle* p ) const
      {
        // valid particle?
        if ( 0 == p                        ) { return false ; }       // RETURN
        // check mother ID
        if ( !mother ( p -> particleID() ) ) { return false ; }       // RETURN
        // check the oscillations:
        switch ( oscillation () )
        {
        case Decays::Trees::Oscillated    :
          return  p->hasOscillated () ;                               // RETURN
        case Decays::Trees::NotOscillated :
          return !p->hasOscillated () ;                               // RETURN
        default:
          return true ;                                               // RETURN
        }
      }
      // ======================================================================
      /// get the algorithm
      Alg alg() const { return m_alg ; }
      /// set the algorithm
      void setAlg ( const Alg value ) { m_alg = value ; }
      // ======================================================================
    public:
      // ======================================================================
      /// add one more daughter to the decay
      void addDaughter ( const Decays::iTree_<PARTICLE>& tree ) ;
      /// add one more daughter to the decay
      void addDaughter ( const Decays::iNode& node ) ;
      /// add one more daughter to the decay
      void addDaughter ( const Decays::Decay::Item&   node ) ;
      /// add one more daughter to the decay
      void addDaughter ( const std::string&         node ) ;
      /// add one more daughter to the decay
      void addDaughter ( const LHCb::ParticleID&    node ) ;
      /// add one more daughter to the decay
      void addDaughter ( const LHCb::ParticleProperty*  node ) ;
      // ======================================================================
    public:
      // ======================================================================
      /// add one more node to the tree
      MCExclusive& operator+= ( const Decays::iTree_<PARTICLE>& node)
      { addDaughter ( node ) ; return *this ; }
      /// add one more node to the tree
      MCExclusive& operator+= ( const Decays::iNode& node )
      { addDaughter ( node ) ; return *this ; }
      /// add one more node to the tree
      MCExclusive& operator+= ( const Decays::Decay::Item&   node )
      { addDaughter ( node ) ; return *this ; }
      /// add one more node to the tree
      MCExclusive& operator+= ( const std::string&         node )
      { addDaughter ( node ) ; return *this ; }
      /// add one more node to the tree
      MCExclusive& operator+= ( const LHCb::ParticleID&    node )
      { addDaughter ( node ) ; return *this ; }
      /// add one more node to the tree
      MCExclusive& operator+= ( const LHCb::ParticleProperty*    node )
      { addDaughter ( node ) ; return *this ; }
      // ======================================================================
    protected:
      // ======================================================================
      inline const TreeList& children() const { return m_children ; }
      // ======================================================================
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
      MCExclusive () ;                   // the default constructor is disabled
      // ======================================================================
    private:
      // ======================================================================
      /// The mother
      Decays::Node       m_mother      ;                        //   the mother
      /// The children
      TreeList           m_children    ;                        // the children
      // ======================================================================
      /// The algorithm
      Decays::Trees::Alg m_alg         ;                       // the algorithm
      /// flag for "decay-only"
      bool               m_decayOnly   ;                       //    decay-only
      /// flag for oscillated
      Oscillation        m_oscillation ;                       //    oscillated
      // ======================================================================
    } ;
    // ========================================================================
    /** @class MCInclusive
     *  Simple sub tree which consists of the node ("mother") and
     *  subtrees ("children"). Essentially it represent the
     *  inclusive decays.
     *
     *  Define the regular decay, e.g. \f$ B^0 \to \pi^+ \pi^- + ... \f$: 
     *
     *  @code 
     *
     *   // " B0 -> pi+ pi- ... " 
     *
     *   using namespace Decays ;
     *   typedef Decays::Trees::MCExclusive::TreeList Children ;
     *
     *   Children children ;
     *   children.push_back ( Trees::MCExclusive( "pi+" ) ) ;
     *   children.push_back ( Trees::MCExclusive( "pi-" ) ) ;
     * 
     *   const Trees::MCInclusive decay 
     *                ( "B0"     ,    // the parent 
     *                  children ) ;  // children  
     * 
     *  @endcode 
     * 
     *  Define the "decay-in-sections", e.g. all following 
     *  decays will be correctly matched with the descriptor 
     *  <c>" B0 --> pi+ pi- pi0 "</c>":
     *    - \f$ B^0 \to \left( \rho^0 \to \pi^+ \pi^-  + ... \right) 
     *               \left( \pi^0  \to \gamma \gamma \right) \f$
     *    - \f$ B^0 \to \left( \rho^0 \to \pi^+ \pi^- \right) 
     *               \left( \pi^0  \to \gamma \gamma \right) + ... \f$
     *    - \f$ B^0 \to \left( \rho^+ \to \pi^+ \pi^0  + ... \right) \pi^-\f$ 
     *    - \f$ B^0 \to \left( \rho^+ \to \pi^+ \pi^0  \right) \pi^- + ... \f$ 
     *    - \f$ B^0 \to \left( \rho^- \to \pi^- \pi^0  + ... \right) \pi^+\f$ 
     *    - \f$ B^0 \to \left( \rho^- \to \pi^- \pi^0   \right) + ... \pi^+\f$ 
     *    - \f$ B^0 \to \pi^+ \pi^- \pi^0 + ... \f$ 
     *
     *  @code 
     *
     *   // " B0 --> pi+ pi- pi0 ... " 
     *
     *   using namespace Decays ;
     *   typedef Decays::Trees::MCExclusive::TreeList Children ;
     *
     *   Children children ;
     *   children.push_back ( Trees::MCExclusive ( "pi+" ) ) ;
     *   children.push_back ( Trees::MCExclusive ( "pi-" ) ) ;
     *   children.push_back ( Trees::MCExclusive ( "pi0" ) ) ;
     * 
     *   const Trees::MCInclusive decay 
     *                ( "B0"            ,   // the parent 
     *                  children        ,   // children  
     *                  Trees::Sections ) ; // matching algorithm 
     * 
     *  @endcode 
     *
     *  <c>Algorithm</c> and <c>DecayOnly</c> are defined through 
     *  the type of "arrow":
     *
     *     - <c>"->"</c> Search withing the direct daughter particles, 
     *                   ignore the partricles form "non-decay" vertices 
     *     - <c>"-->"</c> Search withing the decay sections. 
     *                   ignore the partricles form "non-decay" vertices 
     *     - <c>"-x>"</c> Search withing the direct daughter particles, 
     *                    including the particles from non-decay vertices 
     *     - <c>"--x>"</c> Search withing the decay sections. 
     *                    including the particles from non-decay vertices 
     *
     *  The most convinine tway to create the inclusive decay is from 
     *  the regular decay:
     * 
     *  @code 
     *
     *   using namespace Decays ;
     * 
     *   const Trees::MCExclsuive& regular = .... ;   
     * 
     *   const Trees::MCInclusive decay ( regular ) ;
     * 
     *  @endcode 
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-04-13
     */
    class GAUDI_API MCInclusive : public MCExclusive
    {
    public:
      // ======================================================================
      /** full constructor from the node (mother), subtrees and "final" flag
       *  @param mother the mother node
       *  @param children the list of daughter substrees
       *  @param alg the matching algorithm
       *  @param decayOnly the flag to search only through decay products
       *  @param oscillated check the oscilaltion flag
       */
      MCInclusive
      ( const Decays::iNode&       mother                 ,
        const TreeList&            children               ,
        const Alg                  alg        = Daughters ,
        const bool                 decayOnly  = true      ,
        const Oscillation          oscillated = Undefined ) ;
      /** constructor from the node (mother) & flags
       *  @param mother the mother node
       *  @param alg the matching algorithm
       *  @param decayOnly the flag to search only through decay products
       *  @param oscillated check the oscilaltion flag
       */
      MCInclusive
      ( const Decays::iNode&       mother                 ,
        const Alg                  alg        = Daughters ,
        const bool                 decayOnly  = true      ,
        const Oscillation          oscillated = Undefined ) ;
      /** constructor from the decay descriptor and flags 
       *  @param decay the decay 
       *  @param alg the matching algorithm
       *  @param decayOnly the flag to search only through decay products
       *  @param oscillated check the oscilaltion flag
       */
      MCInclusive
      ( const Decays::Decay&       decay                  ,
        const Alg                  alg        = Daughters ,
        const bool                 decayOnly  = true      ,
        const Oscillation          oscillated = Undefined ) ;
      /// constructor from any exclusive 
      MCInclusive ( const MCExclusive& exclusive ) ;
      // ======================================================================
      /// MANDATORY: virtual destructor
      virtual ~MCInclusive () {}
      // ======================================================================
    public:
      // ======================================================================
      /// MANDATORY: clone method ("virtual constructor")
      virtual  MCInclusive* clone() const { return new MCInclusive ( *this ) ; }
      /// MANDATORY: check the validity of the tree
      virtual bool valid() const ;
      /// MANDATORY: the specific printout
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// MANDATORY: implementation of operator()
      virtual bool p_match ( Decays::iTree_<PARTICLE>::argument p ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// add one more node to the tree
      MCInclusive& operator+= ( const Decays::iTree_<PARTICLE>& tree ) 
      { addDaughter ( tree ) ; return *this ; }
      /// add one more node to the tree
      MCInclusive& operator+= ( const Decays::iNode& node ) 
      { addDaughter ( node ) ; return *this ; }
      /// add one more node to the tree
      MCInclusive& operator+= ( const Decays::Decay::Item&   node )
      { addDaughter ( node ) ; return *this ; }
      /// add one more node to the tree
      MCInclusive& operator+= ( const std::string&         node )
      { addDaughter ( node ) ; return *this ; }
      /// add one more node to the tree
      MCInclusive& operator+= ( const LHCb::ParticleID&    node )
      { addDaughter ( node ) ; return *this ; }
      /// add one more node to the tree
      MCInclusive& operator+= ( const LHCb::ParticleProperty*    node )
      { addDaughter ( node ) ; return *this ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled
      MCInclusive () ;                   // the default constructor is disabled
      // ======================================================================
    };
    // ========================================================================
    /** @class MCOptional
     *  Simple sub tree which consists of the node ("mother") and
     *  subtrees ("children") and optional nodes. Essentially it represent the
     *  decays with optional particles
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-04-13
     */
    class GAUDI_API MCOptional : public MCExclusive
    {
    public:
      // ======================================================================
      /** full constructor from the node (mother) and subtrees
       *  @param mother the mother node 
       *  @param children the list of children 
       *  @param optional the list of optional components 
       *  @param alg the matching algorithm 
       *  @param decayOnly the flag to search only through decay products 
       *  @param oscillation check the oscilaltion flag
       */
      MCOptional
      ( const Decays::iNode&       mother                   ,
        const TreeList&            children   = TreeList () ,
        const TreeList&            optional   = TreeList () ,
        const Alg                  alg        = Daughters   ,
        const bool                 decayOnly  = true        ,
        const Oscillation          oscillated = Undefined   ) ;
      // ======================================================================
      /** constructor from decay descriptor, optional and flags 
       *  @param decay the decay descriptor
       *  @param optional the list of optional components 
       *  @param alg the matching algorithm 
       *  @param decayOnly the flag to search only through decay products 
       *  @param oscillation check the oscilaltion flag
       */
      MCOptional
      ( const Decays::Decay&       decay                    ,
        const TreeList&            optional   = TreeList () ,
        const Alg                  alg        = Daughters   ,
        const bool                 decayOnly  = true        ,
        const Oscillation          oscillated = Undefined   ) ;
      /// constructor form the constructed tree 
      MCOptional 
      ( const MCExclusive& right                  , 
        const TreeList&    optional = TreeList () ) ;
      /// MANDATORY: virtual destructor
      virtual ~MCOptional () {}
      // ======================================================================
    public:
      // ======================================================================
      /// MANDATORY: clone method ("virtual constructor")
      virtual  MCOptional* clone() const
      { return new MCOptional ( *this ) ; }
      /// MANDATORY: check the validity of the tree
      virtual bool valid() const ;
      /// MANDATORY: the specific printout
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
      /// MANDATORY: the proper validation of the tree
      virtual  StatusCode validate 
      ( const LHCb::IParticlePropertySvc* svc ) const ;
      /// has marked decays in tree ?
      virtual bool marked() const ;
      // ======================================================================
    private:
      // ======================================================================
      /// MANDATORY: implementation of operator()
      virtual bool p_match ( Decays::iTree_<PARTICLE>::argument p ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// add one more daughter to the decay
      void addOptional ( const Decays::iTree_<PARTICLE>& tree ) ;
      /// add one more daughter to the decay
      void addOptional ( const Decays::iNode& node ) ;
      /// add one more daughter to the decay
      void addOptional ( const Decays::Decay::Item&   node ) ;
      /// add one more daughter to the decay
      void addOptional ( const std::string&         node ) ;
      /// add one more daughter to the decay
      void addOptional ( const LHCb::ParticleID&    node ) ;
      /// add one more daughter to the decay
      void addOptional ( const LHCb::ParticleProperty*    node ) ;
      // ======================================================================
    public:
      // ======================================================================
      void setOptional ( const TreeList& optional ) { m_optional = optional ; }
      void setOptional ( const std::vector<std::string>&      optional ) ;
      void setOptional ( const std::vector<LHCb::ParticleID>& optional ) ;
      // ======================================================================
    public:
      // ======================================================================
      /// add one more node to the tree
      MCOptional& operator+= ( const Decays::iTree_<PARTICLE>& node ) 
      { addDaughter ( node ) ; return *this ; }
      /// add one more node to the tree
      MCOptional& operator+= ( const Decays::iNode& node ) 
      { addDaughter ( node ) ; return *this ; }
      /// add one more node to the tree
      MCOptional& operator+= ( const Decays::Decay::Item&   node )
      { addDaughter ( node ) ; return *this ; }
      /// add one more node to the tree
      MCOptional& operator+= ( const std::string&         node )
      { addDaughter ( node ) ; return *this ; }
      /// add one more node to the tree
      MCOptional& operator+= ( const LHCb::ParticleID&    node )
      { addDaughter ( node ) ; return *this ; }
      /// add one more node to the tree
      MCOptional& operator+= ( const LHCb::ParticleProperty*    node )
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
      MCOptional () ;                    // the default constructor is disabled
      // ======================================================================
    private:
      // ======================================================================
      /// the optional particles in the tree
      TreeList m_optional ;               // the optional particles in the tree
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Photos
     *  Simple sub-tree which can contains an undefined number of "photons"
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-04-21
     */
    class GAUDI_API Photos : public Decays::Trees::MCExclusive
    {
    public:
      // ======================================================================
      /** full constructor from the node (mother) and subtrees
       *  @param mother the mother node 
       *  @param children the list of children
       *  @param alg the matching algorithm 
       *  @param decayOnly the flag to search only through decay products 
       *  @param oscillation check the oscilaltion flag
       */
      Photos
      ( const Decays::iNode&     mother                 ,
        const TreeList&          children               ,
        const Decays::Trees::Alg alg        = Daughters ,
        const bool               decayOnly  = true      ,
        const Oscillation        oscillated = Undefined ) ;
      /** constructor from the decay descriptor and flags 
       *  @param decay the decay descriptor 
       *  @param alg the matching algorithm 
       *  @param decayOnly the flag to search only through decay products 
       *  @param oscillation check the oscilaltion flag
       */  
      Photos
      ( const Decays::Decay&     decay                  ,
        const Decays::Trees::Alg alg        = Daughters ,
        const bool               decayOnly  = true      ,
        const Oscillation        oscillated = Undefined ) ;
      /// constructor from MC-exclusive )                   
      Photos ( const MCExclusive& decay ) ;
      /// MANDATORY: virtual destructor
      virtual ~Photos () {}
      // ======================================================================
    public:
      // ======================================================================
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Photos* clone() const { return new Photos ( *this ) ; }
      /// MANDATORY: check the validness
      virtual bool valid () const
      { return m_photon.valid() && MCExclusive::valid() ; }
      /// MANDATORY: the proper validation of the tree
      virtual  StatusCode validate 
      ( const LHCb::IParticlePropertySvc* svc ) const ;
      /// MANDATORY: the specific printout
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// MANDATORY: implementation of operator()
      virtual bool p_match ( Decays::iTree_<PARTICLE>::argument p ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// add one more node to the tree
      Photos& operator+= ( const Decays::iTree_<PARTICLE>& node ) 
      { addDaughter ( node ) ; return *this ; }          
      /// add one more node to the tree
      Photos& operator+= ( const Decays::iNode& node ) 
      { addDaughter ( node ) ; return *this ; }          
      /// add one more node to the tree
      Photos& operator+= ( const Decays::Decay::Item&   node )
      { addDaughter ( node ) ; return *this ; }
      /// add one more node to the tree
      Photos& operator+= ( const std::string&         node )
      { addDaughter ( node ) ; return *this ; }
      /// add one more node to the tree
      Photos& operator+= ( const LHCb::ParticleID&    node )
      { addDaughter ( node ) ; return *this ; }
      /// add one more node to the tree
      Photos& operator+= ( const LHCb::ParticleProperty*    node )
      { addDaughter ( node ) ; return *this ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled
      Photos () ;                        // the default constructor is disabled
      // ======================================================================
    private:
      // ======================================================================
      /// the photon subtree/node ("photon")
      Decays::Nodes::Pid m_photon     ;   // the photon subtree/node ("photon")
      // ======================================================================
    } ;
    // ========================================================================
    /** @class PhotosOptional
     *  Simple sub tree which consists of the node ("mother") and
     *  subtrees ("children") and optional nodes. Essentially it represent the
     *  decays with optional particles
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-04-13
     */
    class GAUDI_API PhotosOptional : public MCOptional
    {
    public:
      // ======================================================================
      /** full constructor from the node (mother) and subtrees
       *  @param mother the mother node 
       *  @param children the list of children 
       *  @param optional the list of optional components 
       *  @param alg the matching algorithm 
       *  @param decayOnly the flag to search only through decay products 
       *  @param oscillation check the oscilaltion flag
       */
      PhotosOptional
      ( const Decays::iNode& mother                   ,
        const TreeList&      children   = TreeList () ,
        const TreeList&      optional   = TreeList () ,
        const Alg            alg        = Daughters   ,
        const bool           decayOnly  = true        ,
        const Oscillation    oscillated = Undefined   ) ;
      /** constructor from decay descriptor, optional and flags 
       *  @param decay the decay descriptor
       *  @param optional the list of optional components 
       *  @param alg the matching algorithm 
       *  @param decayOnly the flag to search only through decay products 
       *  @param oscillation check the oscilaltion flag
       */
      PhotosOptional
      ( const Decays::Decay&       decay                    ,
        const TreeList&            optional   = TreeList () ,
        const Alg                  alg        = Daughters   ,
        const bool                 decayOnly  = true        ,
        const Oscillation          oscillated = Undefined   ) ;
      /// constructor from MCOptional
      PhotosOptional ( const MCOptional& right ) ;
      /// constructor from the tree with optional stuff
      PhotosOptional 
      ( const MCExclusive& right    ,
        const TreeList&    optional ) ;          
      /// MANDATORY: virtual destructor
      virtual ~PhotosOptional () {}
      // ======================================================================
    public:
      // ======================================================================
      /// MANDATORY: clone method ("virtual constructor")
      virtual  PhotosOptional* clone() const
      { return new PhotosOptional ( *this ) ; }
      /// MANDATORY: the specific printout
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
      /// MANDATORY: check the validness
      virtual bool valid () const ;
      /// MANDATORY: the proper validation of the tree
      virtual  StatusCode validate 
      ( const LHCb::IParticlePropertySvc* svc ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// MANDATORY: implementation of operator()
      virtual bool p_match ( Decays::iTree_<PARTICLE>::argument p ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// add one more node to the tree
      PhotosOptional& operator+= ( const Decays::iTree_<PARTICLE>& node ) 
      { addDaughter ( node ) ; return *this ; }
      /// add one more node to the tree
      PhotosOptional& operator+= ( const Decays::iNode& node ) 
        { addDaughter ( node ) ; return *this ; }
      /// add one more node to the tree
      PhotosOptional& operator+= ( const Decays::Decay::Item&   node )
      { addDaughter ( node ) ; return *this ; }
      /// add one more node to the tree
      PhotosOptional& operator+= ( const std::string&         node )
      { addDaughter ( node ) ; return *this ; }
      /// add one more node to the tree
      PhotosOptional& operator+= ( const LHCb::ParticleID&    node )
      { addDaughter ( node ) ; return *this ; }
      /// add one more node to the tree
      PhotosOptional& operator+= ( const LHCb::ParticleProperty*    node )
      { addDaughter ( node ) ; return *this ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled
      PhotosOptional () ;                // the default constructor is disabled
      // ======================================================================
    private:
      // ======================================================================
      /// the photon subtree/node ("photon")
      Decays::Nodes::Pid m_photon     ;   // the photon subtree/node ("photon")
      // ======================================================================
    } ;
    // ========================================================================
    // treat properly the stable trees
    // ========================================================================
    template <> bool Stable_<const LHCb::MCParticle*>::operator() 
      ( Decays::iTree_<const LHCb::MCParticle*>::argument p ) const ;
    // ========================================================================
  } //                                           end of namespace Decays::Trees
  // ==========================================================================
} //                                                    end of namespace Decays
// ============================================================================
//                                                                      The END
// ============================================================================
#endif // LOKI_MCDECAYS_H
// ============================================================================
