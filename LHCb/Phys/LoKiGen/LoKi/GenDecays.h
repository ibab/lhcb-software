// $Id: GenDecays.h,v 1.9 2009-05-23 15:56:20 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_GENDECAYS_H 
#define LOKI_GENDECAYS_H 1
// ============================================================================
// Include files
// ============================================================================
// Event 
// ============================================================================
#include "Event/HepMCEvent.h"
// ============================================================================
// PartProp
// ============================================================================
#include "Kernel/Nodes.h"
#include "Kernel/NodesPIDs.h"
// ============================================================================
// LoKiDecay
// ============================================================================
#include "LoKi/Trees.h"
#include "LoKi/DecayDescriptor.h"
// ============================================================================
/** @file
 *  Implementation file for LoKi HepMC-specific decay/tree-functors 
 *  @author Vanya BELYAEV Ivan.BELYAEV@nikhef.nl
 *  @date   2008-06-10
 */
// ============================================================================
namespace Decays 
{
  // ==========================================================================
  namespace Trees 
  {
    // ======================================================================
    /** @class GenExclusive 
     *  Simple sub tree which consists of the node ("mother") and 
     *  subtrees ("children"). Essentially it represent also the 
     *  "semi-exclusive" decay (as well as any decay with the fixed 
     *  number of components for comparisons).
     *
     *  Define the simple regular decay, e.g. \f$ B^0 \to \pi^+ \pi^- \f$:
     *
     *  @code
     *
     *   // " B0 -> pi+ pi- "
     *
     *   using namespace Decays ; 
     *
     *   typedef Trees::GenExclusive::TreeList    Children ;
     *
     *   // define daughters:
     *   Children children ;
     *   children.push_back ( Trees::GenExclusive ( "pi+" ) ) ;
     *   children.push_back ( Trees::GenExclusive ( "pi-" ) ) ;
     *
     *   Decays::Trees::GenExclusive decay 
     *             ( "B0"    ,     // the parent  
     *               chidren ) ;   // children 
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
     *
     *   typedef Trees::GenExclusive::TreeList    Children ;
     *
     *   // define daughters:
     *   Children children ;
     *   children.push_back ( Trees::GenExclusive ( "pi+" ) ) ;
     *   children.push_back ( Trees::GenExclusive ( "pi-" ) ) ;
     *   children.push_back ( Trees::GenExclusive ( "pi0" ) ) ;
     *
     *   Trees::GenExclusive decay 
     *             ( "B0"            ,   // parent  
     *               chidren         ,   // children 
     *               Trees::Sections ) ; // matching algorithm 
     *
     *  @endcode 
     *
     *  Define the "decay-with-oscillation". Here the mother particle
     *  is required to oscillate before the decay, e.g. 
     *  \f$ B^0 \to \bar{B}^0 \to \pi^+ \pi^- \f$:
     *
     *  @code
     *
     *   // " [B0]os -> pi+ pi- "
     *
     *   using namespace Decays ; 
     *
     *   typedef Trees::GenExclusive::SubTrees    Children ;
     *
     *   // define daughters:
     *   Children children ;
     *   children.push_back ( Trees::GenExclusive ( "pi+" ) ) ;
     *   children.push_back ( Trees::GenExclusive ( "pi-" ) ) ;
     *
     *   Trees::GenExclusive decay 
     *             ( "B0"              ,   // parent  
     *               chidren           ,   // children 
     *               Trees::Daughetrs  ,   // matching algorithm
     *               Trees::Oscillated ) ; // oscillation flag 
     *
     *  @endcode 
     *
     *  Define the "decay-without-oscillations":
     *
     *  @code 
     *
     *   // " [B0]nos --> pi+ pi- pi0 "
     * 
     *   using namespace Decays ; 
     *
     *   typedef Trees::GenExclusive::TreeList    Children ;
     *
     *   // define daughters:
     *   Children children ;
     *   children.push_back ( Trees::GenExclusive ( "pi+" ) ) ;
     *   children.push_back ( Trees::GenExclusive ( "pi-" ) ) ;
     *   children.push_back ( Trees::GenExclusive ( "pi0" ) ) ;
     *
     *   Trees::GenExclusive decay 
     *             ( "B0"                 ,   // parent  
     *               chidren              ,   // children 
     *               Trees::Sections      ,   // matching algorithm 
     *               Trees::NotOscillated ) ; // oscillation flag 
     *
     *  @endcode 
     *
     *
     *  Define the complex decay, e.g.
     *  \f$  B^0 \to \bar{B}^0 \to 
     *    \left( \rho^0 \to \pi^+ \pi^- \right) \pi^0 \f$:
     *  
     *  @code 
     *
     *   // " [B0]os -> ( rho(770)0 -> pi+ pi- ) pi0 "
     *
     *
     *   using namespace Decays ; 
     *
     *   typedef Trees::GenExclusive::TreeList    Children ;
     *
     *   // define daughters:
     *   Children children ;
     *   children.push_back ( Trees::GenExclusive ( "pi+" ) ) ;
     *   children.push_back ( Trees::GenExclusive ( "pi-" ) ) ;
     *
     *   Trees::GenExclusive rho
     *             ( "rho(770)0"          ,   // parent  
     *               chidren              ) ; // children 
     *   
     *   children.clear() 
     *   children.push_back ( rho ) ;
     *   children.push_back ( Trees::GenExclusive ( "pi0" ) ) ;
     *
     *   Trees::GenExclusive decay 
     *             ( "B0"                 ,   // parent  
     *               chidren              ,   // children 
     *               Trees::Daughters     ,   // matching algorithm 
     *               Trees::NotOscillated ) ; // oscillation flag 
     *  @endcode    
     *
     *  Define "semi-inclusive" decays, e.g. 
     *  \f$ B^0 \to X^+ X^- \f$, where 
     *  \f$X^+\f$ and \f$X^-\f$ are any posively and negatively 
     *  charged particles:
     *
     *  @code 
     *
     *   // " B0 -> X+ X- "
     *
     *   using namespace Decays ; 
     *
     *   typedef Trees::GenExclusive::TreeList    Children ;
     *
     *   // define daughters:
     *   Children children ;
     *   children.push_back ( Trees::GenExclusive ( Nodes::Positive () ) ) ;
     *   children.push_back ( Trees::GenExclusive ( Nodes::Negative () ) ) ;
     *
     *   Trees::GenExclusive decay 
     *             ( "B0"    ,     // the parent  
     *               chidren ) ;   // chidlren 
     *
     *  @endcode 
     *
     *  The <c>algorithm</c> is defined by the length  of the arrow:
     *  
     *    - <c> "->"</c> the decay is defined in terms of direct daughters 
     *    - <c>"-->"</c> the decay is defined in terms of "Sections"
     *
     *  For some decays useful shortcuts could be created:
     *  @code
     *
     *   // " B0 -> pi+ pi- "
     *
     *   using namespace Decays ; 
     *
     *   Trees::GenExclusive decay ( "B0" ) ;
     *   decay += "pi+" ;
     *   decay += "pi-" ;
     *
     *  @endcode 
     *  Or 
     *  @code
     *
     *   // " [B0]os --> pi+ pi- pi0"
     *
     *   using namespace Decays ; 
     *
     *   Trees::GenExclusive decay ( "B0"              , 
     *                               Trees::Sections   , 
     *                               Trees::Oscilalted ) ;
     *   decay += "pi+" ;
     *   decay += "pi-" ;
     *   decay += "pi0" ;
     *
     *  @endcode 
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-06-10
     */
    class GenExclusive: public Decays::iTree_<const HepMC::GenParticle*>
    {
    protected:
      // ======================================================================
      typedef const HepMC::GenParticle*                              PARTICLE ;
      typedef Decays::Trees::Types_<PARTICLE>                        Types    ;
      // ======================================================================
    public:
      // ======================================================================
      /// the actual type of the container of children trees 
      typedef Types::TreeList                                        TreeList ;
      // ======================================================================
    public:
      // ======================================================================
      /** full constructor from the node (mother), subtrees & flags
       *  @param mother the mother node 
       *  @param children the list of daughter substrees 
       *  @param alg the matching algorithm 
       *  @param oscillation check the oscilaltion flag
       */
      GenExclusive
      ( const Decays::iNode&       mother                  , 
        const TreeList&            children                ,
        const Alg                  alg         = Daughters , 
        const Oscillation          oscillation = Undefined ) ;
      /** constructor from the node (mother) & flags
       *  @param mother the mother node 
       *  @param alg the matching algorithm 
       *  @param oscillation check the oscillation flag
       */
      GenExclusive
      ( const Decays::iNode&       mother                  , 
        const Alg                  alg         = Daughters ,
        const Oscillation          oscillation = Undefined ) ;
      /** constructor from the node (mother) & flags
       *  @param oscillation check the oscillation flag
       *  @param mother the mother node 
       */
      GenExclusive
      ( const Oscillation          oscillation , 
        const Decays::iNode&       mother      ) ;
      /** constructor from the decay & flags 
       *  @param decay the decay 
       *  @param alg the matching algorithm 
       *  @param oscillation check the oscillation flag
       */
      GenExclusive
      ( const Decays::Decay&       decay                   , 
        const Alg                  alg         = Daughters ,
        const Oscillation          oscillation = Undefined ) ;
      /// MANDATORY: virtual destructor 
      virtual ~GenExclusive () {}
      // ======================================================================
    public:
      // ======================================================================
      /// MANDATORY: clone method ("virtual constructor")
      virtual  GenExclusive* clone() const { return new GenExclusive ( *this ) ; }
      /// MANDATORY: the only one essential method: 
      virtual bool operator() ( Decays::iTree_<PARTICLE>::argument p ) const ;
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
      /// has marked elements  in the tree?
      virtual  bool marked() const ;
      // ======================================================================
    protected:
      // ======================================================================
      /** check the validness & mother & oscillation 
       *  @param p particle to be checked
       *  @return oscilalted or not
       */
      const HepMC::GenParticle* ok ( const HepMC::GenParticle* p ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// get the algorithm 
      Alg         alg         () const { return m_alg         ; }
      Oscillation oscillation () const { return m_oscillation ; }
      // ======================================================================
    public:
      // ======================================================================
      /// get the algorithm 
      void setAlg         (  Alg         value ) { m_alg          = value ; }
      void setOscillation (  Oscillation value ) { m_oscillation  = value ; }
      // ======================================================================
    public:
      // ======================================================================
      /// set children 
      void setChildren 
      ( const TreeList& children ) { m_children = children ; }
      /// set children 
      void setChildren
      ( const std::vector<std::string>&                   chidlren ) ;
      /// set children 
      void setChildren
      ( const std::vector<Decays::Decay::Item>&           chidlren ) ;
      /// set children 
      void setChildren
      ( const std::vector<LHCb::ParticleID>&              children ) ;
      /// set children 
      void setChildren
      ( const std::vector<const LHCb::ParticleProperty*>& children ) ;
      // ======================================================================
    public:
      // ======================================================================
      /// add one more daughter to the decay 
      void addDaughter ( const Decays::iTree_<PARTICLE>& tree ) ;
      /// add one more daughter to the decay 
      void addDaughter ( const Decays::iNode&          node ) ; 
      // ======================================================================
      /// add one more daughter to the decay 
      void addDaughter ( const std::string&            node ) ; 
      // ======================================================================
      /// add one more daughter to the decay 
      void addDaughter ( const LHCb::ParticleID&       node ) ; 
      // ======================================================================
      /// add one more daughter to the decay 
      void addDaughter ( const Decays::Decay::Item&    node ) ; 
      // ======================================================================
      /// add one more daughter to the decay 
      void addDaughter ( const LHCb::ParticleProperty* node ) ; 
      // ======================================================================
    public:
      // ======================================================================
      /// add one more node to the tree 
      GenExclusive& operator+= ( const Decays::iTree_<PARTICLE>& node ) 
      { addDaughter ( node ) ; return *this ; }
      /// add one more node to the tree 
      GenExclusive& operator+= ( const Decays::iNode&            node ) 
      { addDaughter ( node ) ; return *this ; }
      // ======================================================================
      /// add one more node to the tree 
      GenExclusive& operator+= ( const std::string&              node ) 
      { addDaughter ( node ) ; return *this ; }
      // ======================================================================
      /// add one more node to the tree 
      GenExclusive& operator+= ( const LHCb::ParticleID&         node ) 
      { addDaughter ( node ) ; return *this ; }
      // ======================================================================
      /// add one more node to the tree 
      GenExclusive& operator+= ( const Decays::Decay::Item&      node ) 
      { addDaughter ( node ) ; return *this ; }
      // ======================================================================
      /// add one more node to the tree 
      GenExclusive& operator+= ( const LHCb::ParticleProperty*   node ) 
      { addDaughter ( node ) ; return *this ; }
      // ======================================================================
    protected:
      // ======================================================================
      inline const TreeList& children() const { return m_children ; }
      // ======================================================================
      TreeList::const_iterator  childBegin () const { return m_children.begin () ; }
      TreeList::const_iterator  childEnd   () const { return m_children.end   () ; }
      // ======================================================================
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
      GenExclusive () ;                  // the default constructor is disabled
      // ======================================================================
    private:
      // ======================================================================
      /// The mother 
      Decays::Node               m_mother      ;               //    the mother 
      /// The children
      TreeList                   m_children    ;               //  the children 
      /// The algorithm 
      Decays::Trees::Alg         m_alg         ;               // the algorithm 
      /// Oscillation flag 
      Decays::Trees::Oscillation m_oscillation ;               //   oscillation 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class GenInclusive 
     *  Simple sub tree which consists of the node ("mother") and 
     *  subtrees ("children"). Essentially it represent the 
     *  inclusive decays.
     *
     *  The inclusive decays are identified with ellipses: "..." ;
     *
     *  Define the simple regular decay, e.g. 
     *   \f$ B^0 \to \pi^+ \pi^- + ... \f$:
     *  @code
     *
     *   // " B0 -> pi+ pi- ... "
     *
     *   using namespace Decays ; 
     *
     *   typedef Trees::GenExclusive::SubTrees    Children ;
     *
     *   // define daughters:
     *   Children children ;
     *   children.push_back ( Trees::GenExclusive ( "pi+" ) ) ;
     *   children.push_back ( Trees::GenExclusive ( "pi-" ) ) ;
     *
     *   Trees::GenInclusive decay 
     *             ( "B0"    ,     // the parent  
     *               chidren ) ;   // children 
     *
     *  @endcode 
     *
     *  Define the "decay-in-sections", e.g. all following 
     *  decays will be correctly matched with the descriptor 
     *  <c>" B0 --> pi+ pi- pi0 ... "</c>":
     *    - \f$ B^0 \to \left( \rho^0 \to \pi^+ \pi^- \right) 
     *               \left( \pi^0  \to \gamma \gamma  \right) + ...         \f$
     *    - \f$ B^0 \to \left( \rho^0 \to \pi^+ \pi^- + ... \right) 
     *               \left( \pi^0  \to \gamma \gamma  \right)               \f$
     *    - \f$ B^0 \to \left( \rho^+ \to \pi^+ \pi^0 + ...  \right) \pi^-  \f$ 
     *    - \f$ B^0 \to \left( \rho^+ \to \pi^+ \pi^0 \right) \pi^- + ...   \f$ 
     *    - \f$ B^0 \to \left( \rho^- \to \pi^- \pi^0 + ... \right) \pi^+   \f$ 
     *    - \f$ B^0 \to \left( \rho^- \to \pi^- \pi^0   \right) \pi^+ + ... \f$ 
     *    - \f$ B^0 \to \pi^+ \pi^- \pi^0 + ... \f$ 
     *
     *  @code 
     *
     *   // " B0 --> pi+ pi- pi0 ... "
     * 
     *   using namespace Decays ; 
     *
     *   typedef Trees::GenExclusive::TreeList    Children ;
     *
     *   // define daughters:
     *   Children children ;
     *   children.push_back ( Trees::GenExclusive ( "pi+" ) ) ;
     *   children.push_back ( Trees::GenExclusive ( "pi-" ) ) ;
     *   children.push_back ( Trees::GenExclusive ( "pi0" ) ) ;
     *
     *   Trees::GenInclusive decay 
     *             ( "B0"            ,   // parent  
     *               chidren         ,   // children 
     *               Trees::Sections ) ; // matching algorithm 
     *
     *  @endcode 
     *
     *  Define the "decay-with-oscillation". Here the mother particle
     *  is required to oscillate before the decay, e.g. 
     *  \f$ B^0 \to \bar{B}^0 \to \pi^+ \pi^- + ... \f$:
     *
     *  @code
     *
     *   // " [B0]os -> pi+ pi- ... "
     *
     *   using namespace Decays ; 
     *
     *   typedef Trees::GenExclusive::TreeList    Children ;
     *
     *   // define daughters:
     *   Children children ;
     *   children.push_back ( Trees::GenExclusive ( "pi+" ) ) ;
     *   children.push_back ( Trees::GenExclusive ( "pi-" ) ) ;
     *
     *   Trees::GenInclusive decay 
     *             ( "B0"              ,   // parent  
     *               chidren           ,   // children 
     *               Trees::Daughetrs  ,   // matching algorithm
     *               Trees::Oscillated ) ; // oscillation flag 
     *
     *  @endcode 
     *
     *  Define the "decay-without-oscillations":
     *
     *  @code 
     *
     *   // " [B0]nos --> pi+ pi- pi0 ... "
     * 
     *   using namespace Decays ; 
     *
     *   typedef Trees::GenExclusive::TreeList    Children ;
     *
     *   // define daughters:
     *   Children children ;
     *   children.push_back ( Trees::GenExclusive ( "pi+" ) ) ;
     *   children.push_back ( Trees::GenExclusive ( "pi-" ) ) ;
     *   children.push_back ( Trees::GenExclusive ( "pi0" ) ) ;
     *
     *   Trees::GenInclusive decay 
     *             ( "B0"                 ,   // parent  
     *               chidren              ,   // children 
     *               Trees::Sections      ,   // matching algorithm 
     *               Trees::NotOscillated ) ; // oscillation flag 
     *
     *  @endcode 
     *
     *  The <c>algorithm</c> is defined by the length  of the arrow:
     *  
     *    - <c> "->"</c> the decay is defined in terms of direct daughters 
     *    - <c>"-->"</c> the decay is defined in terms of "Sections"
     *
     *  It is rather convinient to create the inclusive decay 
     *  from the corresponding regular decay:
     *
     *  @code 
     *  
     *   using namespace Decays ; 
     *
     *   const Trees::GenExclusive& regular = ... ;
     * 
     *   const Trees::GenInclusive decay ( regular ) ;
     *
     *  @endcode 
     *  
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-06-10
     */
    class GenInclusive : public GenExclusive 
    {
    public:
      // ====================================================================
      /** full constructor from the node (mother), subtrees  & flags
       *  @param mother the mother node 
       *  @param children the list of daughter substrees 
       *  @param alg the matching algorithm 
       *  @param osc  the oscilaltion flag
       */
      GenInclusive
      ( const Decays::iNode& mother                 , 
        const TreeList&      children               ,
        const Alg            alg        = Daughters , 
        const Oscillation    osc        = Undefined ) ;
      /** constructor from the node (mother), subtrees and "final" flag
       *  @param mother the mother node 
       *  @param alg the matching algorithm 
       *  @param osc require the oscillation flag for mother 
       */
      GenInclusive
      ( const Decays::iNode& mother                 , 
        const Alg            alg        = Daughters ,
        const Oscillation    osc        = Undefined ) ;
      /** constructor from the decay & flags 
       *  @param decay the decay 
       *  @param alg the matching algorithm 
       *  @param oscillation check the oscillation flag
       */
      GenInclusive
      ( const Decays::Decay&       decay                   , 
        const Alg                  alg         = Daughters ,
        const Oscillation          oscillation = Undefined ) ;
      /** constructor from exclusive decay
       *  @param mother the mother node 
       */
      GenInclusive ( const GenExclusive& right ) ;
      /// MANDATORY: virtual destructor 
      virtual ~GenInclusive () {}
      // ====================================================================
    public:
      // ====================================================================
      /// MANDATORY: clone method ("virtual constructor")
      virtual  GenInclusive* clone() const { return new GenInclusive ( *this ) ; }
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
      GenInclusive& operator+= ( const Decays::iTree_<PARTICLE>& node ) 
      { addDaughter(  node ) ; return *this ; }
      /// add one more node to the tree 
      GenInclusive& operator+= ( const Decays::iNode& node ) 
      { addDaughter(  node ) ; return *this ; }
      /// add one more node to the tree 
      GenInclusive& operator+= ( const std::string& node ) 
      { addDaughter(  node ) ; return *this ; }
      /// add one more node to the tree 
      GenInclusive& operator+= ( const LHCb::ParticleID& node ) 
      { addDaughter(  node ) ; return *this ; }
      /// add one more node to the tree 
      GenInclusive& operator+= ( const Decays::Decay::Item& node ) 
      { addDaughter ( node ) ; return *this ; }
      // ====================================================================
      /// add one more node to the tree 
      GenInclusive& operator+= ( const LHCb::ParticleProperty* node ) 
      { addDaughter ( node ) ; return *this ; }
      // ====================================================================
    private:
      // ====================================================================
      /// the default constructor is disabled 
      GenInclusive () ; // the default constructor is disabled
      // ====================================================================
    };      
    // ======================================================================
    /** @class GenOptional
     *  Simple sub tree which consists of the node ("mother") and 
     *  subtrees ("children") and optional nodes. 
     *  Essentially it represent the decays with "optional" particles.
     * 
     *  Define the simple regular decay with optional components, e.g.
     *     - \f$ B^0 \to \pi^+ \pi^-       \f$
     *     - \f$ B^0 \to \pi^+ \pi^- \pi^0 \f$
     *
     *  @code
     *
     *   // " B0 -> pi+ pi-  { pi0 } "
     *
     *   using namespace Decays ; 
     *
     *   typedef Trees::GenExclusive::TreeList    Children ;
     *   typedef Trees::GenExclusive::TreeList    Optional ;
     *
     *   // define daughters:
     *   Children children ;
     *   children.push_back ( Trees::GenExclusive ( "pi+" ) ) ;
     *   children.push_back ( Trees::GenExclusive ( "pi-" ) ) ;
     *   
     *   // define optional 
     *   Optional optional ;
     *   optional.push_back ( Trees::GenExclusive ( "pi0" ) ) ;
     *
     *   Trees::GenOptional decay 
     *             ( "B0"     ,     // the parent  
     *               chidren  ,     // children 
     *               optional ) ;   // optional 
     *
     *  @endcode 
     *
     *  The <c>algorithm</c> is defined by the length  of the arrow:
     *  
     *    - <c> "->"</c> the decay is defined in terms of direct daughters 
     *    - <c>"-->"</c> the decay is defined in terms of "Sections"
     *
     *  The most convinient way to create the "optional" decay is from 
     *  the regular decay:
     *  
     *  @code
     *
     *   using namespace Decays ; 
     *
     *   typedef Trees::GenExclusive::TreeList  Optional ;
     *
     *   // define optional 
     *   Optional optional ;
     *   optional.push_back ( Trees::GenExclusive ( "pi0" ) ) ;
     *
     *   const Trees::GenExclusive& regular = ... ; 
     *  
     *   Trees::GenOptional decay 
     *             ( regular  , 
     *               optional ) ;   // optional 
     *
     *  @endcode
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-04-13
     */
    class GenOptional : public GenExclusive
    {
    public:
      // ====================================================================
      /** full constructor from the node (mother) and subtrees
       *  @param mother the mother node 
       *  @param children the list of children 
       *  @param optional the list of optional components 
       *  @param alg the matching algorithm 
       *  @param oscillation check the oscilaltion flag
       */
      GenOptional
      ( const Decays::iNode&       mother                 , 
        const TreeList&            children = TreeList () ,
        const TreeList&            optional = TreeList () ,
        const Alg                  alg      = Daughters   ,
        const Oscillation          osc      = Undefined   ) ;
      // ====================================================================
      /** constructor from decay descriptor, optional and flags 
       *  @param decay the decay descriptor
       *  @param optional the list of optional components 
       *  @param alg the matching algorithm 
       *  @param oscillation check the oscilaltion flag
       */
      GenOptional
      ( const Decays::Decay&       decay                    ,
        const TreeList&            optional   = TreeList () ,
        const Alg                  alg        = Daughters   ,
        const Oscillation          oscillated = Undefined   ) ;
      /// constructor  from GenExclusive & optional stuff
      GenOptional 
      ( const GenExclusive& right                  ,
        const TreeList&     optional = TreeList () ) ;
      /// MANDATORY: virtual destructor 
      virtual ~GenOptional () {}
      // ====================================================================
    public:
      // ====================================================================
      /// MANDATORY: clone method ("virtual constructor")
      virtual  GenOptional* clone() const 
      { return new GenOptional ( *this ) ; }
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
      /// has marked elements  in the tree?
      virtual  bool marked() const ;
      // ====================================================================
    public:
      // ====================================================================
      /// set optional
      void setOptional
      ( const TreeList& optional ) { m_optional = optional ; }
      /// set optional
      void setOptional
      ( const std::vector<std::string>& chidlren ) ;
      /// set children 
      void setOptional
      ( const std::vector<Decays::Decay::Item>& chidlren ) ;
      /// set optional 
      void setOptional 
      ( const std::vector<LHCb::ParticleID>& children ) ;
      /// set optional 
      void setOptional 
      ( const std::vector<const LHCb::ParticleProperty*>& children ) ;
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
      GenOptional& operator+= ( const Decays::iTree_<PARTICLE>& node ) 
      { addDaughter ( node ) ; return *this ; }
      /// add one more node to the tree 
      GenOptional& operator+= ( const Decays::iNode& node ) 
      { addDaughter ( node ) ; return *this ; }
      /// add one more node to the tree 
      GenOptional& operator+= ( const std::string& node ) 
      { addDaughter ( node ) ; return *this ; }
      /// add one more node to the tree 
      GenOptional& operator+= ( const LHCb::ParticleID& node ) 
      { addDaughter ( node ) ; return *this ; }
      /// add one more node to the tree 
      GenOptional& operator+= ( const Decays::Decay::Item& node ) 
      { addDaughter ( node ) ; return *this ; }
      /// add one more node to the tree 
      GenOptional& operator+= ( const LHCb::ParticleProperty* node ) 
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
      GenOptional () ;                   // the default constructor is disabled
      // ======================================================================
    private:
      // ======================================================================
      /// the optional particles in the tree 
      TreeList m_optional ;               // the optional particles in the tree 
      // ======================================================================
    } ; 
    // ========================================================================
    /** @class GenPhotos
     *  Simple sub-tree which can contains an undefined number of "photons"
     *
     *
     *  Define the decay with undefined number of photons, e.g.
     *  \f$ B^0 \to \pi^+ \pi^- n\gamma \f$:
     *  @code 
     *
     *   // " B0 => pi+ pi- "
     *
     *   using namespace Decays ; 
     *
     *   typedef Trees::GenExclusive::SubTrees    Children ;
     *
     *   // define daughters:
     *   Children children ;
     *   children.push_back ( Trees::GenExclusive ( "pi+" ) ) ;
     *   children.push_back ( Trees::GenExclusive ( "pi-" ) ) ;
     *
     *   Trees::GenPhotos decay 
     *             ( "B0"    ,     // the parent  
     *               chidren ) ;   // children 
     *
     *  @endcode 
     *  
     *  The <c>algorithm</c> is defined by the length  of the arrow:
     *  
     *    - <c> "=>"</c> the decay is defined in terms of direct daughters 
     *    - <c>"==>"</c> the decay is defined in terms of "Sections"
     *
     *  The most convinient way to create the Photos-aware decay 
     *  is from the regular one:
     *
     *  @code 
     * 
     *   using namespace Decays ; 
     *
     *   const Trees::GenExclusive& regular = ... ;
     *  
     *   const Trees::GenPhotons decay ( regular ) ;
     *
     *  @endcode 
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-04-21
     */
    class GenPhotos : public GenExclusive
    {
    public:
      // ======================================================================
      /** full constructor from the node (mother) and subtrees
       *  @param mother the mother node 
       *  @param children the list of children
       *  @param alg the matching algorithm 
       *  @param oscillation check the oscilaltion flag
       */
      GenPhotos
      ( const Decays::iNode&     mother                    , 
        const TreeList&          children    = TreeList () ,
        const Decays::Trees::Alg alg         = Daughters   ,
        const Oscillation        oscillation = Undefined   ) ;
      /** constructor from the decay descriptor and flags 
       *  @param decay the decay descriptor 
       *  @param alg the matching algorithm 
       *  @param oscillation check the oscilaltion flag
       */  
      GenPhotos
      ( const Decays::Decay&     decay                  ,
        const Decays::Trees::Alg alg        = Daughters ,
        const Oscillation        oscillated = Undefined ) ;
      /// constructor from GenExclusive 
      GenPhotos ( const GenExclusive& right ) ;
      /// MANDATORY: virtual destructor 
      virtual ~GenPhotos () {}
      // ======================================================================
    public:
      // ======================================================================
      /// MANDATORY: clone method ("virtual constructor")
      virtual  GenPhotos* clone() const { return new GenPhotos ( *this ) ; }
      /// MANDATORY: check the validness 
      virtual bool valid () const 
      { return m_photon.valid() && GenExclusive::valid() ; }
      /// MANDATORY: the proper validation of the tree
      virtual  StatusCode validate 
      ( const LHCb::IParticlePropertySvc* svc ) const ;
      /// MANDATORY: the only one essential method
      virtual bool operator() 
        ( Decays::iTree_<const HepMC::GenParticle*>::argument p ) const ;
      /// MANDATORY: the specific printout 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// add one more node to the tree 
      GenPhotos& operator+= ( const Decays::iTree_<PARTICLE>& node) 
      { addDaughter ( node ) ; return *this ; }
      /// add one more node to the tree 
      GenPhotos& operator+= ( const Decays::iNode& node ) 
      { addDaughter ( node ) ; return *this ; }
      /// add one more node to the tree 
      GenPhotos& operator+= ( const std::string&         node ) 
      { addDaughter ( node ) ; return *this ; }
      /// add one more node to the tree 
      GenPhotos& operator+= ( const LHCb::ParticleID&    node ) 
      { addDaughter ( node ) ; return *this ; }
      /// add one more node to the tree 
      GenPhotos& operator+= ( const Decays::Decay::Item& node ) 
      { addDaughter ( node ) ; return *this ; }
      /// add one more node to the tree 
      GenPhotos& operator+= ( const LHCb::ParticleProperty* node ) 
      { addDaughter ( node ) ; return *this ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      GenPhotos () ; // the default constructor is disabled
      // ======================================================================
    private:
      // ======================================================================
      /// the photon subtree/node ("photon")
      Decays::Nodes::Pid m_photon     ;                        // the sub-trees 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class GenPhotosOptional
     *  Simple sub tree which consists of the node ("mother") and 
     *  subtrees ("children") and optional nodes. Essentially it represent the 
     *  decays with optional particles 
     *
     *  The <c>algorithm</c> is defined by the length  of the arrow:
     *  
     *    - <c> "=>"</c> the decay is defined in terms of direct daughters 
     *    - <c>"==>"</c> the decay is defined in terms of "Sections"
     *
     *  The most convinient way to define "photos-aware" decay with the 
     *  optional components if from the regular decay woth otional 
     *  components: 
     *
     *  @code
     *
     *   using namespace Decays ; 
     *
     *   const Trees::GenOptional& regular = ... ;
     *
     *   const Trees::GenPhotonsOptional decay ( regular ) ;
     *
     *  @endcode
     *       
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-06-10
     */
    class GenPhotosOptional : public GenOptional
    {
    public:
      // ======================================================================
      /** full constructor from the node (mother) and subtrees
       *  @param mother the mother node 
       *  @param children the list of children 
       *  @param optional the list of optional components 
       *  @param alg the matching algorithm 
       *  @param oscillation check the oscilaltion flag
       */
      GenPhotosOptional
      ( const Decays::iNode& mother                 , 
        const TreeList&      children = TreeList () ,
        const TreeList&      optional = TreeList () ,
        const Alg            alg      = Daughters   ,
        const Oscillation    osc      = Undefined   ) ;
      /** constructor from decay descriptor, optional and flags 
       *  @param decay the decay descriptor
       *  @param optional the list of optional components 
       *  @param alg the matching algorithm 
       *  @param oscillation check the oscilaltion flag
       */
      GenPhotosOptional
      ( const Decays::Decay&       decay                    ,
        const TreeList&            optional   = TreeList () ,
        const Alg                  alg        = Daughters   ,
        const Oscillation          oscillated = Undefined   ) ;
      /// constructor from GenOptional 
      GenPhotosOptional ( const GenOptional& right ) ;
      /// constructor from GenExclusive & optional stuff  
      GenPhotosOptional 
      ( const GenExclusive& right                  , 
        const TreeList&     optional = TreeList () ) ;
      // ======================================================================
      /// MANDATORY: virtual destructor 
      virtual ~GenPhotosOptional () {}
      // ======================================================================
    public:
      // ======================================================================
      /// MANDATORY: clone method ("virtual constructor")
      virtual  GenPhotosOptional* clone() const 
      { return new GenPhotosOptional ( *this ) ; }
      /// MANDATORY: the only one essential method
      virtual bool operator() 
        ( Decays::iTree_<PARTICLE>::argument p ) const ;
      /// MANDATORY: the specific printout 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
      /// MANDATORY: check the validness 
      virtual bool valid () const ;
      /// MANDATORY: the proper validation of the tree
      virtual  StatusCode validate 
      ( const LHCb::IParticlePropertySvc* svc ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// add one more node to the tree 
      GenPhotosOptional& operator+= ( const Decays::iTree_<PARTICLE>& node )
      { addDaughter ( node ) ; return *this ; }
      /// add one more node to the tree 
      GenPhotosOptional& operator+= ( const Decays::iNode& node ) 
      { addDaughter ( node ) ; return *this ; }
      /// add one more node to the tree 
      GenPhotosOptional& operator+= ( const std::string&         node ) 
      { addDaughter ( node ) ; return *this ; }
      /// add one more node to the tree 
      GenPhotosOptional& operator+= ( const LHCb::ParticleID&    node ) 
      { addDaughter ( node ) ; return *this ; }
      /// add one more node to the tree 
      GenPhotosOptional& operator+= ( const Decays::Decay::Item& node ) 
      { addDaughter ( node ) ; return *this ; }
      /// add one more node to the tree 
      GenPhotosOptional& operator+= ( const LHCb::ParticleProperty* node ) 
      { addDaughter ( node ) ; return *this ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      GenPhotosOptional () ;             // the default constructor is disabled
      // ======================================================================
    private:
      // ======================================================================
      /// the photon subtree/node ("photon")
      Decays::Nodes::Pid m_photon     ;                        // the sub-trees 
      // ======================================================================
    } ;
    // ========================================================================
    // treat properly the stable trees
    // ========================================================================
    template <> bool Stable_<const HepMC::GenParticle*>::operator() 
      ( Decays::iTree_<const HepMC::GenParticle*>::argument p ) const ;
    // ========================================================================
  } //                                           end of namespace Decays::Trees
  // ==========================================================================
} //                                                    end of namespace Decays  
// ============================================================================



// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_GENDECAYS_H
// ============================================================================
