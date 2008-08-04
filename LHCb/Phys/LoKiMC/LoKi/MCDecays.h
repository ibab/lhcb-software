// $Id: MCDecays.h,v 1.3 2008-08-04 15:38:53 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_MCDECAYS_H
#define LOKI_MCDECAYS_H 1
// ============================================================================
// Include files
// ============================================================================
// Event
// ============================================================================
#include "Event/MCParticle.h"
// ============================================================================
// LoKiDecay
// ============================================================================
#include "LoKi/Nodes.h"
#include "LoKi/Trees.h"
// ============================================================================
/** @file
 *  Implementation file for LoKi MC-specific decay/tree-functors
 *  @author Vanya BELYAEV Ivan.BELYAEV@nikhef.nl
 *  @date   2008-05-25
 */
// ============================================================================
namespace LoKi
{
  namespace Decays
  {
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
       *   using namespace LoKi::Decays ;
       *   typedef LoKi::Decays::Trees::MCExclusive::SubTrees Children ;
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
       *   using namespace LoKi::Decays ;
       *   typedef LoKi::Decays::Trees::MCExclusive::SubTrees Children ;
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
      class MCExclusive: public LoKi::Decays::iTree_<const LHCb::MCParticle*>
      {
      protected:
        typedef const LHCb::MCParticle*                   PARTICLE ;
      public:
        // ====================================================================
        /// the actual type of the ocntainer of children trees
        typedef LoKi::Decays::Trees::_Tree_<PARTICLE>::SubTrees SubTrees ;
        // ====================================================================
      public:
        // ====================================================================
        /** constructor from the node (mother), subtrees and "final" flag
         *  @param mother the mother node
         *  @param children the list of daughter substrees
         *  @param alg the matching algorithm
         *  @param decayOnly the flag to search only through decay products
         *  @param oscillated check the oscilaltion flag
         */
        MCExclusive
        ( const LoKi::Decays::iNode& mother                 ,
          const SubTrees&            children               ,
          const Alg                  alg        = Daughters ,
          const bool                 decayOnly  = true      ,
          const Oscillation          oscillated = Undefined ) ;
        /** constructor from the node (mother), subtrees and "final" flag
         *  @param mother the mother node
         *  @param alg the matching algorithm
         *  @param oscillated require the oscillation flag for mother
         */
        MCExclusive
        ( const LoKi::Decays::iNode& mother                 ,
          const Alg                  alg        = Daughters ,
          const bool                 decayOnly  = true      ,
          const Oscillation          oscillated = Undefined ) ;
        /** constructor from the node (mother), subtrees and "final" flag
         *  @param mother the mother node
         *  @param children the list of daughter substrees
         *  @param alg the matching algorithm
         *  @param decayOnly the flag to search only through decay products
         *  @param oscillated check the oscilaltion flag
         */
        MCExclusive
        ( const std::string&         mother                 ,
          const SubTrees&            children               ,
          const Alg                  alg        = Daughters ,
          const bool                 decayOnly  = true      ,
          const Oscillation          oscillated = Undefined ) ;
        /** constructor from the node (mother), subtrees and "final" flag
         *  @param mother the mother node
         *  @param alg the matching algorithm
         *  @param osccillated require the oscillation flag for mother
         */
        MCExclusive
        ( const std::string&         mother                 ,
          const Alg                  alg        = Daughters ,
          const bool                 decayOnly  = true      ,
          const Oscillation          oscillated = Undefined ) ;
        /** constructor from the node (mother), subtrees and "final" flag
         *  @param mother the mother node
         *  @param children the list of daughter substrees
         *  @param alg the matching algorithm
         *  @param decayOnly the flag to search only through decay products
         *  @param oscillated check the oscilaltion flag
         */
        MCExclusive
        ( const LHCb::ParticleID&    mother                 ,
          const SubTrees&            children               ,
          const Alg                  alg        = Daughters ,
          const bool                 decayOnly  = true      ,
          const Oscillation          oscillated = Undefined ) ;
        /** constructor from the node (mother), subtrees and "final" flag
         *  @param mother the mother node
         *  @param alg the matching algorithm
         *  @param osc require the oscillation flag for mother
         */
        MCExclusive
        ( const LHCb::ParticleID&    mother                 ,
          const Alg                  alg        = Daughters ,
          const bool                 decayOnly  = true      ,
          const Oscillation          oscillated = Undefined ) ;
        /** constructor from the decay
         *  @param decay
         *  @param alg the matching algorithm
         *  @param osc require the oscillation flag for mother
         */
        MCExclusive
        ( const LHCb::Decay&         decay                  ,
          const Alg                  alg        = Daughters ,
          const bool                 decayOnly  = true      ,
          const Oscillation          oscillated = Undefined ) ;
        /// copy constrcutor 
        MCExclusive ( const MCExclusive& right ) ;
        /// MANDATORY: virtual destructor
        virtual ~MCExclusive () {}
        // ====================================================================
      public:
        // ====================================================================
        /// MANDATORY: clone method ("virtual constructor")
        virtual  MCExclusive* clone() const { return new MCExclusive ( *this ) ; }
        /// MANDATORY: the only one essential method:
        virtual bool operator()
          ( LoKi::Decays::iTree_<PARTICLE>::argument p ) const ;
        /// MANDATORY: check the validity
        virtual bool valid() const ;
        /// MANDATORY: validate the tree
        virtual StatusCode validate ( IParticlePropertySvc* svc ) const ;
        /// MANDATORY: reset the tree
        virtual void reset () const ;
        /// collect the marked elements
        virtual size_t collect
        ( LoKi::Decays::iTree_<PARTICLE>::Collection& output ) const ;
        /// MANDATORY: the specific printout
        virtual  std::ostream& fillStream( std::ostream& s ) const ;
        // ====================================================================
      public:
        // ====================================================================
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
          if ( 0 == p                        ) { return false ; } // RETURN
          // check mother ID
          if ( !mother ( p -> particleID() ) ) { return false ; } // RETURN
          // check the oscillations:
          switch ( oscillation () )
          {
          case LoKi::Decays::Trees::Oscillated    :
            return  p->hasOscillated () ;                           // RETURN
          case LoKi::Decays::Trees::NotOscillated :
            return !p->hasOscillated () ;                           // RETURN
          default:
            return true ;                                           // RETURN
          }
          //
          return true ;
        }
        // ====================================================================
        /// get the algorithm
        Alg alg() const { return m_alg ; }
        /// set the algorithm
        void setAlg ( const Alg value ) { m_alg = value ; }
        // ====================================================================
      public:
        // ====================================================================
        /// add one more daughter to the decay
        void addDaughter ( const LoKi::Decays::iTree_<PARTICLE>& tree ) ;
        /// add one more daughter to the decay
        void addDaughter ( const LoKi::Decays::iNode& node ) ;
        /// add one more daughter to the decay
        void addDaughter ( const LHCb::Decay::Item&   node ) ;
        /// add one more daughter to the decay
        void addDaughter ( const std::string&         node ) ;
        /// add one more daughter to the decay
        void addDaughter ( const LHCb::ParticleID&    node ) ;
        /// add one more daughter to the decay
        void addDaughter ( const ParticleProperty*    node ) ;
        // ====================================================================
      public:
        // ====================================================================
        /// add one more node to the tree
        MCExclusive& operator+= ( const LoKi::Decays::iTree_<PARTICLE>& node)
        { addDaughter ( node ) ; return *this ; }
        /// add one more node to the tree
        MCExclusive& operator+= ( const LoKi::Decays::iNode& node )
        { addDaughter ( node ) ; return *this ; }
        /// add one more node to the tree
        MCExclusive& operator+= ( const LHCb::Decay::Item&   node )
        { addDaughter ( node ) ; return *this ; }
        /// add one more node to the tree
        MCExclusive& operator+= ( const std::string&         node )
        { addDaughter ( node ) ; return *this ; }
        /// add one more node to the tree
        MCExclusive& operator+= ( const LHCb::ParticleID&    node )
        { addDaughter ( node ) ; return *this ; }
        /// add one more node to the tree
        MCExclusive& operator+= ( const ParticleProperty*    node )
        { addDaughter ( node ) ; return *this ; }
        // ====================================================================
      protected:
        // ====================================================================
        inline const SubTrees& children() const { return m_children ; }
        // ====================================================================
        SubTrees::const_iterator  childBegin () const { return m_children.begin () ; }
        SubTrees::const_iterator  childEnd   () const { return m_children.end   () ; }
        SubTrees::const_reference front      () const { return m_children.front () ; }
        SubTrees::const_reference back       () const { return m_children.back  () ; }        
        // ====================================================================
        size_t nChildren () const { return m_children.size() ; }
        // ====================================================================
        // reset the cache
        inline void i_reset () const
        {
          std::for_each
            ( childBegin() , childEnd() ,
              std::mem_fun_ref (&_Tree_<PARTICLE>::reset) ) ;
        }
        // ====================================================================
        const LoKi::Decays::iNode& mother () const { return m_mother ; }
        // ====================================================================
        inline bool mother ( const LHCb::ParticleID& pid ) const
        { return m_mother.node ( pid ) ;  }
        // ====================================================================
      private:
        // ====================================================================
        /// the default constructor is disabled
        MCExclusive () ; // the default constructor is disabled
        // ====================================================================
      private:
        // ====================================================================
        /// The mother
        LoKi::Decays::Node m_mother   ; // the mother
        /// The children
        SubTrees           m_children ; // the children
        // ====================================================================
        /// The algorithm
        LoKi::Decays::Trees::Alg m_alg        ; // the algorithm
        /// flag for "decay-only"
        bool                     m_decayOnly  ; // decay-only
        /// flag for oscillated
        Oscillation              m_oscillation ; // oscillated
        // ====================================================================
      } ;
      // ======================================================================
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
       *   using namespace LoKi::Decays ;
       *   typedef LoKi::Decays::Trees::MCExclusive::SubTrees Children ;
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
       *   using namespace LoKi::Decays ;
       *   typedef LoKi::Decays::Trees::MCExclusive::SubTrees Children ;
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
       *   using namespace LoKi::Decays ;
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
      class MCInclusive : public MCExclusive
      {
      public:
        // ====================================================================
        /** constructor from the node (mother), subtrees and "final" flag
         *  @param mother the mother node
         *  @param children the list of daughter substrees
         *  @param alg the matching algorithm
         *  @param decayOnly the flag to search only through decay products
         *  @param oscillated check the oscilaltion flag
         */
        MCInclusive
        ( const LoKi::Decays::iNode& mother                 ,
          const SubTrees&            children               ,
          const Alg                  alg        = Daughters ,
          const bool                 decayOnly  = true      ,
          const Oscillation          oscillated = Undefined ) ;
        /** constructor from the node (mother), subtrees and "final" flag
         *  @param mother the mother node
         *  @param alg the matching algorithm
         *  @param decayOnly the flag to search only through decay products
         *  @param oscillated check the oscilaltion flag 
        */
        MCInclusive
        ( const LoKi::Decays::iNode& mother                 ,
          const Alg                  alg        = Daughters ,
          const bool                 decayOnly  = true      ,
          const Oscillation          oscillated = Undefined ) ;
        /** constructor from the node (mother), subtrees and "final" flag
         *  @param mother the mother node
         *  @param children the list of daughter substrees
         *  @param alg the matching algorithm
         *  @param decayOnly the flag to search only through decay products
         *  @param oscillated check the oscilaltion flag
         */
        MCInclusive
        ( const std::string&         mother                 ,
          const SubTrees&            children               ,
          const Alg                  alg        = Daughters ,
          const bool                 decayOnly  = true      ,
          const Oscillation          oscillated = Undefined ) ;
        /** constructor from the node (mother), subtrees and "final" flag
         *  @param mother the mother node
         *  @param alg the matching algorithm
         *  @param decayOnly the flag to search only through decay products
         *  @param oscillated check the oscilaltion flag
         */
        MCInclusive
        ( const std::string&         mother                 ,
          const Alg                  alg        = Daughters ,
          const bool                 decayOnly  = true      ,
          const Oscillation          oscillated = Undefined ) ;
        /** constructor from the node (mother), subtrees and "final" flag
         *  @param mother the mother node
         *  @param children the list of daughter substrees
         *  @param alg the matching algorithm
         *  @param decayOnly the flag to search only through decay products
         *  @param oscillated check the oscilaltion flag
         */
        MCInclusive
        ( const LHCb::ParticleID&    mother                 ,
          const SubTrees&            children               ,
          const Alg                  alg        = Daughters ,
          const bool                 decayOnly  = true      ,
          const Oscillation          oscillated = Undefined ) ;
        /** constructor from the node (mother), subtrees and "final" flag
         *  @param mother the mother node
         *  @param alg the matching algorithm
         *  @param decayOnly the flag to search only through decay products
         *  @param oscillated check the oscilaltion flag
         */
        MCInclusive
        ( const LHCb::ParticleID&    mother                 ,
          const Alg                  alg        = Daughters ,
          const bool                 decayOnly  = true      ,
          const Oscillation          oscillated = Undefined ) ;
        /** constructor from the decay 
         *  @param decay the decay 
         *  @param alg the matching algorithm
         *  @param decayOnly the flag to search only through decay products
         *  @param oscillated check the oscilaltion flag
         */
        MCInclusive
        ( const LHCb::Decay&         decay                  ,
          const Alg                  alg        = Daughters ,
          const bool                 decayOnly  = true      ,
          const Oscillation          oscillated = Undefined ) ;
        /// constructor from the exclusive 
        MCInclusive ( const MCExclusive& exclusive ) ;
        // ====================================================================
        /// copy constructor 
        MCInclusive ( const MCInclusive& right ) ;
        // ====================================================================
        /// MANDATORY: virtual destructor
        virtual ~MCInclusive () {}
        // ====================================================================
      public:
        // ====================================================================
        /// MANDATORY: clone method ("virtual constructor")
        virtual  MCInclusive* clone() const { return new MCInclusive ( *this ) ; }
        /// MANDATORY: the only one essential method:
        virtual bool operator()
          ( LoKi::Decays::iTree_<PARTICLE>::argument p ) const ;
        /// MANDATORY: check the validity of the tree
        virtual bool valid() const ;
        /// MANDATORY: the specific printout
        virtual  std::ostream& fillStream( std::ostream& s ) const ;
        // ====================================================================
      public:
        // ====================================================================
        /// add one more node to the tree
        MCInclusive& operator+= ( const LoKi::Decays::iTree_<PARTICLE>& tree ) ;
        /// add one more node to the tree
        MCInclusive& operator+= ( const LoKi::Decays::iNode& node ) ;
        /// add one more node to the tree
        MCInclusive& operator+= ( const LHCb::Decay::Item&   node )
        { addDaughter ( node ) ; return *this ; }
        /// add one more node to the tree
        MCInclusive& operator+= ( const std::string&         node )
        { addDaughter ( node ) ; return *this ; }
        /// add one more node to the tree
        MCInclusive& operator+= ( const LHCb::ParticleID&    node )
        { addDaughter ( node ) ; return *this ; }
        /// add one more node to the tree
        MCInclusive& operator+= ( const ParticleProperty*    node )
        { addDaughter ( node ) ; return *this ; }
        // ====================================================================
      private:
        // ====================================================================
        /// the default constructor is disabled
        MCInclusive () ; // the default constructor is disabled
        // ====================================================================
      };
      // ======================================================================
      /** @class MCOptional
       *  Simple sub tree which consists of the node ("mother") and
       *  subtrees ("children") and optional nodes. Essentially it represent the
       *  decays with optional particles
       *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
       *  @date 2008-04-13
       */
      class MCOptional : public MCExclusive
      {
      public:
        // ====================================================================
        /// constructor from the node (mother) and subtrees
        MCOptional
        ( const LoKi::Decays::iNode& mother                 ,
          const SubTrees&            children               ,
          const SubTrees&            optional               ,
          const Alg                  alg        = Daughters ,
          const bool                 decayOnly  = true      ,
          const Oscillation          oscillated = Undefined ) ;
        // ====================================================================
        /// constructor from the node (mother) and subtrees
        MCOptional
        ( const LoKi::Decays::iNode&                           mother   ,
          const SubTrees&                                      children ,
          const LoKi::Decays::iTree_<const LHCb::MCParticle*>& optional ,
          const Alg                             alg        = Daughters ,
          const bool                            decayOnly  = true      ,
          const Oscillation                     oscillated = Undefined ) ;
        /// constructor from the node (mother) and subtrees
        MCOptional
        ( const LoKi::Decays::iNode& mother                             ,
          const SubTrees&            children               ,
          const LoKi::Decays::iNode& optional               ,
          const Alg                  alg        = Daughters ,
          const bool                 decayOnly  = true      ,
          const Oscillation          oscillated = Undefined ) ;
        /// constructor from the node (mother) and subtrees
        MCOptional
        ( const std::string&         mother                 ,
          const SubTrees&            children               ,
          const SubTrees&            optional               ,
          const Alg                  alg        = Daughters ,
          const bool                 decayOnly  = true      ,
          const Oscillation          oscillated = Undefined ) ;
        /// constructor from the node (mother) and subtrees
        MCOptional
        ( const std::string&                                   mother   ,
          const SubTrees&                                      children ,
          const LoKi::Decays::iTree_<const LHCb::MCParticle*>& optional ,
          const Alg                             alg        = Daughters ,
          const bool                            decayOnly  = true      ,
          const Oscillation                     oscillated = Undefined ) ;
        /// constructor from the node (mother) and subtrees
        MCOptional
        ( const std::string&         mother                             ,
          const SubTrees&            children               ,
          const LoKi::Decays::iNode& optional               ,
          const Alg                  alg        = Daughters ,
          const bool                 decayOnly  = true      ,
          const Oscillation          oscillated = Undefined ) ;
        /// constructor form the decay 
        MCOptional 
        ( const MCExclusive& right    , 
          const SubTrees&    optional ) ;
        /// constructor form the decay 
        MCOptional 
        ( const MCExclusive& right                 , 
          const std::vector<std::string>& optional ) ;
        /// constructor form the decay 
        MCOptional ( const MCExclusive& right ) ;
        /// MANDATORY: virtual destructor
        virtual ~MCOptional () {}
        // ====================================================================
      public:
        // ====================================================================
        /// MANDATORY: clone method ("virtual constructor")
        virtual  MCOptional* clone() const
        { return new MCOptional ( *this ) ; }
        /// MANDATORY: the only one essential method
        virtual bool operator()
          ( LoKi::Decays::iTree_<PARTICLE>::argument p ) const ;
        /// MANDATORY: check the validity of the tree
        virtual bool valid() const ;
        /// MANDATORY: the specific printout
        virtual  std::ostream& fillStream( std::ostream& s ) const ;
        /// MANDATORY: the proper validation of the tree
        virtual  StatusCode validate ( IParticlePropertySvc* svc ) const ;
        // ====================================================================
      public:
        // ====================================================================
        /// add one more daughter to the decay
        void addOptional ( const LoKi::Decays::iTree_<PARTICLE>& tree ) ;
        /// add one more daughter to the decay
        void addOptional ( const LoKi::Decays::iNode& node ) ;
        /// add one more daughter to the decay
        void addOptional ( const LHCb::Decay::Item&   node ) ;
        /// add one more daughter to the decay
        void addOptional ( const std::string&         node ) ;
        /// add one more daughter to the decay
        void addOptional ( const LHCb::ParticleID&    node ) ;
        /// add one more daughter to the decay
        void addOptional ( const ParticleProperty*    node ) ;
        // ====================================================================
      public:
        // ====================================================================
        void setOptional ( const SubTrees& optional ) { m_optional = optional ; }
        void setOptional ( const std::vector<std::string>&      optional ) ;
        void setOptional ( const std::vector<LHCb::ParticleID>& optional ) ;
        // ====================================================================
      public:
        // ====================================================================
        /// add one more node to the tree
        MCOptional& operator+= ( const LoKi::Decays::iTree_<PARTICLE>& node ) 
        { addDaughter ( node ) ; return *this ; }
        /// add one more node to the tree
        MCOptional& operator+= ( const LoKi::Decays::iNode& node ) 
        { addDaughter ( node ) ; return *this ; }
        /// add one more node to the tree
        MCOptional& operator+= ( const LHCb::Decay::Item&   node )
        { addDaughter ( node ) ; return *this ; }
        /// add one more node to the tree
        MCOptional& operator+= ( const std::string&         node )
        { addDaughter ( node ) ; return *this ; }
        /// add one more node to the tree
        MCOptional& operator+= ( const LHCb::ParticleID&    node )
        { addDaughter ( node ) ; return *this ; }
        /// add one more node to the tree
        MCOptional& operator+= ( const ParticleProperty*    node )
        { addDaughter ( node ) ; return *this ; }
        // ====================================================================
      protected:
        // ====================================================================
        SubTrees::const_iterator optBegin () const { return m_optional.begin () ; }
        SubTrees::const_iterator optEnd   () const { return m_optional.end   () ; }
        size_t                   optSize  () const { return m_optional.size  () ; }
        size_t nOptional() const { return optSize() ; }
        // ====================================================================
      private:
        // ====================================================================
        /// the default constructor is disabled
        MCOptional () ; // the default constructor is disabled
        // ====================================================================
      private:
        // ====================================================================
        /// the optional particles in the tree
        SubTrees m_optional ;
        // ====================================================================
      } ;
      // ======================================================================
      /** @class Photos
       *  Simple sub-tree which can contains an undefined number of "photons"
       *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
       *  @date 2008-04-21
       */
      class Photos : public LoKi::Decays::Trees::MCExclusive
      {
      public:
        // ====================================================================
        /// constructor from the node (mother) and subtrees
        Photos
        ( const LoKi::Decays::iNode&     mother                 ,
          const SubTrees&                children               ,
          const LoKi::Decays::Trees::Alg alg        = Daughters ,
          const bool                     decayOnly  = true      ,
          const Oscillation              oscillated = Undefined ) ;
        /// constructor from the node (mother) and subtrees
        Photos
        ( const std::string&             mother                 ,
          const SubTrees&                children               ,
          const LoKi::Decays::Trees::Alg alg        = Daughters ,
          const bool                     decayOnly  = true      ,
          const Oscillation              oscillated = Undefined ) ;
        /// constructor from the node (mother) and subtrees
        Photos
        ( const LHCb::ParticleID&        mother                 ,
          const SubTrees&                children               ,
          const LoKi::Decays::Trees::Alg alg        = Daughters ,
          const bool                     decayOnly  = true      ,
          const Oscillation              oscillated = Undefined ) ;
        /// constructor from the decay                        
        Photos
        ( const LHCb::Decay&             decay                  ,
          const LoKi::Decays::Trees::Alg alg        = Daughters ,
          const bool                     decayOnly  = true      ,
          const Oscillation              oscillated = Undefined ) ;
        /// constructor from MC-exclusive )                   
        Photos ( const MCExclusive& decay ) ;
        /// MANDATORY: virtual destructor
        virtual ~Photos () {}
        // ====================================================================
      public:
        // ====================================================================
        /// MANDATORY: clone method ("virtual constructor")
        virtual  Photos* clone() const { return new Photos ( *this ) ; }
        /// MANDATORY: check the validness
        virtual bool valid () const
        { return m_photon.valid() && MCExclusive::valid() ; }
        /// MANDATORY: the proper validation of the tree
        virtual  StatusCode validate ( IParticlePropertySvc* svc ) const ;
        /// MANDATORY: the only one essential method
        virtual bool operator()
          ( LoKi::Decays::iTree_<const LHCb::MCParticle*>::argument p ) const ;
        /// MANDATORY: the specific printout
        virtual  std::ostream& fillStream( std::ostream& s ) const ;
        // ====================================================================
      public:
        // ====================================================================
        /// add one more node to the tree
        Photos& operator+= ( const LoKi::Decays::iTree_<PARTICLE>& node ) 
        { addDaughter ( node ) ; return *this ; }          
        /// add one more node to the tree
        Photos& operator+= ( const LoKi::Decays::iNode& node ) 
        { addDaughter ( node ) ; return *this ; }          
        /// add one more node to the tree
        Photos& operator+= ( const LHCb::Decay::Item&   node )
        { addDaughter ( node ) ; return *this ; }
        /// add one more node to the tree
        Photos& operator+= ( const std::string&         node )
        { addDaughter ( node ) ; return *this ; }
        /// add one more node to the tree
        Photos& operator+= ( const LHCb::ParticleID&    node )
        { addDaughter ( node ) ; return *this ; }
        /// add one more node to the tree
        Photos& operator+= ( const ParticleProperty*    node )
        { addDaughter ( node ) ; return *this ; }
        // ====================================================================
      private:
        // ====================================================================
        /// the default constructor is disabled
        Photos () ; // the default constructor is disabled
        // ====================================================================
      private:
        // ====================================================================
        /// the photon subtree/node ("photon")
        LoKi::Decays::Nodes::Pid m_photon     ; // the sub-trees
        // ====================================================================
      } ;
      // ======================================================================
      /** @class PhotosOptional
       *  Simple sub tree which consists of the node ("mother") and
       *  subtrees ("children") and optional nodes. Essentially it represent the
       *  decays with optional particles
       *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
       *  @date 2008-04-13
       */
      class PhotosOptional : public MCOptional
      {
      public:
        // ====================================================================
        /// constructor from the node (mother) and subtrees
        PhotosOptional
        ( const LoKi::Decays::iNode& mother                 ,
          const SubTrees&            children               ,
          const SubTrees&            optional               ,
          const Alg                  alg        = Daughters ,
          const bool                 decayOnly  = true      ,
          const Oscillation          oscillated = Undefined ) ;
        /// constructor from the node (mother) and subtrees
        PhotosOptional
        ( const LoKi::Decays::iNode& mother                 ,
          const SubTrees&            children               ,
          const LoKi::Decays::iNode& optional               ,
          const Alg                  alg        = Daughters ,
          const bool                 decayOnly  = true      ,
          const Oscillation          oscillated = Undefined ) ;
        /// constructor from the node (mother) and subtrees
        PhotosOptional
        ( const LoKi::Decays::iNode&                           mother   ,
          const SubTrees&                                      children ,
          const LoKi::Decays::iTree_<const LHCb::MCParticle*>& optional ,
          const Alg                  alg        = Daughters ,
          const bool                 decayOnly  = true      ,
          const Oscillation          oscillated = Undefined ) ;
        /// constructor from MCOptional
        PhotosOptional ( const MCOptional& right ) ;
        /// constructor from MCOptional
        PhotosOptional 
        ( const MCExclusive& right    ,
          const SubTrees&    optional ) ;          
        /// constructor from MCOptional
        PhotosOptional 
        ( const MCExclusive&              right    ,
          const std::vector<std::string>& optional ) ;          
        /// MANDATORY: virtual destructor
        virtual ~PhotosOptional () {}
        // ====================================================================
      public:
        // ====================================================================
        /// MANDATORY: clone method ("virtual constructor")
        virtual  PhotosOptional* clone() const
        { return new PhotosOptional ( *this ) ; }
        /// MANDATORY: the only one essential method
        virtual bool operator()
          ( LoKi::Decays::iTree_<PARTICLE>::argument p ) const ;
        /// MANDATORY: the specific printout
        virtual  std::ostream& fillStream( std::ostream& s ) const ;
        // ====================================================================
        /// MANDATORY: check the validness
        virtual bool valid () const ;
        /// MANDATORY: the proper validation of the tree
        virtual  StatusCode validate ( IParticlePropertySvc* svc ) const ;
      public:
        // ====================================================================
        /// add one more node to the tree
        PhotosOptional& operator+= ( const LoKi::Decays::iTree_<PARTICLE>& node ) 
        { addDaughter ( node ) ; return *this ; }
        /// add one more node to the tree
        PhotosOptional& operator+= ( const LoKi::Decays::iNode& node ) 
        { addDaughter ( node ) ; return *this ; }
        /// add one more node to the tree
        PhotosOptional& operator+= ( const LHCb::Decay::Item&   node )
        { addDaughter ( node ) ; return *this ; }
        /// add one more node to the tree
        PhotosOptional& operator+= ( const std::string&         node )
        { addDaughter ( node ) ; return *this ; }
        /// add one more node to the tree
        PhotosOptional& operator+= ( const LHCb::ParticleID&    node )
        { addDaughter ( node ) ; return *this ; }
        /// add one more node to the tree
        PhotosOptional& operator+= ( const ParticleProperty*    node )
        { addDaughter ( node ) ; return *this ; }
        // ====================================================================
      private:
        // ====================================================================
        /// the default constructor is disabled
        PhotosOptional () ; // the default constructor is disabled
        // ====================================================================
      private:
        // ====================================================================
        /// the photon subtree/node ("photon")
        LoKi::Decays::Nodes::Pid m_photon     ; // the sub-trees
        // ====================================================================
      } ;
    } // end of namespace LoKi::Decays::Trees
    // ========================================================================
  } // end of namespace DaVinci::Graphs
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
/** operator for easy construction of trees 
 *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
 *  @date 2008-07-07
 */
LoKi::Decays::Trees::And_<const LHCb::MCParticle*>
operator&& 
( const LoKi::Decays::iTree_<const LHCb::MCParticle*>& t , 
  const LoKi::Decays::iNode&                           n ) ;
// ============================================================================
/** operator for easy construction of trees 
 *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
 *  @date 2008-07-07
 */
LoKi::Decays::Trees::And_<const LHCb::MCParticle*>
operator&& 
( const LoKi::Decays::iTree_<const LHCb::MCParticle*>& t , 
  const std::string&                                   n ) ;
// ============================================================================
/** operator for the easy construction of trees 
 *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
 *  @date 2008-07-07
 */
LoKi::Decays::Trees::And_<const LHCb::MCParticle*>
operator&& 
( const LoKi::Decays::iTree_<const LHCb::MCParticle*>& t , 
  const LHCb::ParticleID&                              n ) ;
// ============================================================================
/** operator for easy construction of trees 
 *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
 *  @date 2008-07-07
 */
inline LoKi::Decays::Trees::And_<const LHCb::MCParticle*>
operator&& 
( const LoKi::Decays::iNode&                           n ,
  const LoKi::Decays::iTree_<const LHCb::MCParticle*>& t ) { return t && n ; }
// ============================================================================
/** operator for easy construction of trees 
 *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
 *  @date 2008-07-07
 */
inline LoKi::Decays::Trees::And_<const LHCb::MCParticle*>
operator&& 
( const std::string&                                   n ,
  const LoKi::Decays::iTree_<const LHCb::MCParticle*>& t ) { return t && n ; }
// ============================================================================
/** operator for the easy construction of trees 
 *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
 *  @date 2008-07-07
 */
inline LoKi::Decays::Trees::And_<const LHCb::MCParticle*>
operator&& 
( const LHCb::ParticleID&                              n ,
  const LoKi::Decays::iTree_<const LHCb::MCParticle*>& t ) { return t && n ; }
// ============================================================================
/** operator for easy construction of trees 
 *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
 *  @date 2008-07-07
 */
LoKi::Decays::Trees::Or_<const LHCb::MCParticle*>
operator|| 
( const LoKi::Decays::iTree_<const LHCb::MCParticle*>& t , 
  const LoKi::Decays::iNode&                           n ) ;
// ============================================================================
/** operator for easy construction of trees 
 *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
 *  @date 2008-07-07
 */
LoKi::Decays::Trees::Or_<const LHCb::MCParticle*>
operator||
( const LoKi::Decays::iTree_<const LHCb::MCParticle*>& t , 
  const std::string&                                   n ) ;
// ============================================================================
/** operator for the easy construction of trees 
 *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
 *  @date 2008-07-07
 */
LoKi::Decays::Trees::Or_<const LHCb::MCParticle*>
operator||
( const LoKi::Decays::iTree_<const LHCb::MCParticle*>& t , 
  const LHCb::ParticleID&                              n ) ;
// ============================================================================
/** operator for easy construction of trees 
 *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
 *  @date 2008-07-07
 */
inline LoKi::Decays::Trees::Or_<const LHCb::MCParticle*>
operator||
( const LoKi::Decays::iNode&                           n ,
  const LoKi::Decays::iTree_<const LHCb::MCParticle*>& t ) { return t || n ; }
// ============================================================================
/** operator for easy construction of trees 
 *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
 *  @date 2008-07-07
 */
inline LoKi::Decays::Trees::Or_<const LHCb::MCParticle*>
operator||
( const std::string&                                   n ,
  const LoKi::Decays::iTree_<const LHCb::MCParticle*>& t ) { return t || n ; }
// ============================================================================
/** operator for the easy construction of trees 
 *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
 *  @date 2008-07-07
 */
inline LoKi::Decays::Trees::Or_<const LHCb::MCParticle*>
operator|| 
( const LHCb::ParticleID&                              n ,
  const LoKi::Decays::iTree_<const LHCb::MCParticle*>& t ) { return t || n ; }
// ============================================================================


// ============================================================================
// The END
// ============================================================================
#endif // LOKI_MCDECAYS_H
// ============================================================================
