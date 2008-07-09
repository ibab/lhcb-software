// $Id: Decays.h,v 1.2 2008-07-09 16:56:34 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_DECAYS_H 
#define LOKI_DECAYS_H 1
// ============================================================================
// Include files
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
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
  // ==========================================================================
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
      class Exclusive: public LoKi::Decays::iTree_<const LHCb::Particle*>
      {
      protected:
        typedef const LHCb::Particle*                   PARTICLE ;
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
         */
        Exclusive
        ( const LoKi::Decays::iNode& mother                 , 
          const SubTrees&            children               ,
          const Alg                  alg        = Daughters ) ;
        /** constructor from the node (mother), subtrees and "final" flag
         *  @param mother the mother node 
         *  @param alg the matching algorithm 
         */
        Exclusive
        ( const LoKi::Decays::iNode& mother                 , 
          const Alg                  alg        = Daughters ) ;
        /** constructor from the node (mother), subtrees and "final" flag
         *  @param mother the mother node 
         *  @param alg the matching algorithm 
         */
        Exclusive
        ( const std::string&         mother                 , 
          const SubTrees&            children               ,
          const Alg                  alg        = Daughters ) ;
        /** constructor from the node (mother), subtrees and "final" flag
         *  @param mother the mother node 
         *  @param alg the matching algorithm 
         */
        Exclusive
        ( const std::string&         mother                 , 
          const Alg                  alg        = Daughters ) ;
        /** constructor from the node (mother), subtrees and "final" flag
         *  @param mother the mother node 
         *  @param children the list of daughter substrees 
         *  @param alg the matching algorithm 
         */
        Exclusive
        ( const LHCb::ParticleID&    mother                 , 
          const SubTrees&            children               ,
          const Alg                  alg        = Daughters ) ;
        /** constructor from the node (mother), subtrees and "final" flag
         *  @param mother the mother node 
         *  @param alg the matching algorithm 
         */
        Exclusive
        ( const LHCb::ParticleID&    mother                 , 
          const Alg                  alg        = Daughters ) ;
        /** constructor from the decay 
         *  @param decay  the decay       
         *  @param alg the matching algorithm 
         */
        Exclusive
        ( const LHCb::Decay&         decay                  , 
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
        void setChildren ( const SubTrees& children ) { m_children = children ; }
        /// set children 
        void setChildren ( const std::vector<std::string>& children ) ;
        /// set children 
        void setChildren ( const std::vector<LHCb::Decay::Item>& children ) ;
        /// set children 
        void setChildren ( const std::vector<LHCb::ParticleID>& children ) ;
        /// set children 
        void setChildren ( const std::vector<const ParticleProperty*>& children ) ;
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
        Exclusive& operator+= ( const LoKi::Decays::iTree_<PARTICLE>& node )
        { addDaughter ( node ) ; return *this ; }
        /// add one more node to the tree 
        Exclusive& operator+= ( const LoKi::Decays::iNode& node ) 
        { addDaughter ( node ) ; return *this ; }
        /// add one more node to the tree 
        Exclusive& operator+= ( const LHCb::Decay::Item&   node ) 
        { addDaughter ( node ) ; return *this ; }
        /// add one more node to the tree 
        Exclusive& operator+= ( const std::string&         node ) 
        { addDaughter ( node ) ; return *this ; }
        /// add one more node to the tree 
        Exclusive& operator+= ( const LHCb::ParticleID&    node ) 
        { addDaughter ( node ) ; return *this ; }
        /// add one more node to the tree 
        Exclusive& operator+= ( const ParticleProperty*    node ) 
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
        Exclusive () ; // the default constructor is disabled
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
        // ====================================================================
      } ;
      // ======================================================================
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
        // ====================================================================
        /** constructor from the node (mother), subtrees and "final" flag
         *  @param mother the mother node 
         *  @param children the list of daughter substrees 
         *  @param alg the matching algorithm 
         */
        Inclusive
        ( const LoKi::Decays::iNode& mother                 , 
          const SubTrees&            children               ,
          const Alg                  alg        = Daughters ) ;
        /** constructor from the node (mother), subtrees and "final" flag
         *  @param mother the mother node 
         *  @param alg the matching algorithm 
         */
        Inclusive
        ( const LoKi::Decays::iNode& mother                 , 
          const Alg                  alg        = Daughters ) ;
        // ====================================================================
        /** constructor from the node (mother), subtrees and "final" flag
         *  @param mother the mother node 
         *  @param children the list of daughter substrees 
         *  @param alg the matching algorithm 
         */
        Inclusive
        ( const std::string&         mother                 , 
          const SubTrees&            children               ,
          const Alg                  alg        = Daughters ) ;
        /** constructor from the node (mother), subtrees and "final" flag
         *  @param mother the mother node 
         *  @param alg the matching algorithm 
         */
        Inclusive
        ( const std::string&         mother                 , 
          const Alg                  alg        = Daughters ) ;
        /** constructor from the node (mother), subtrees and "final" flag
         *  @param mother the mother node 
         *  @param children the list of daughter substrees 
         *  @param alg the matching algorithm 
         */
        Inclusive
        ( const LHCb::ParticleID&    mother                 , 
          const SubTrees&            children               ,
          const Alg                  alg        = Daughters ) ;
        /** constructor from the node (mother), subtrees and "final" flag
         *  @param mother the mother node 
         *  @param alg the matching algorithm 
         */
        Inclusive
        ( const LHCb::ParticleID&    mother                 , 
          const Alg                  alg        = Daughters ) ;
        /** constructor from the decay 
         *  @param decay the decay        
         *  @param alg the matching algorithm 
         */
        Inclusive
        ( const LHCb::Decay&         decay                  , 
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
          ( LoKi::Decays::iTree_<PARTICLE>::argument p ) const ;
        /// MANDATORY: check the validity of the tree 
        virtual bool valid() const ;
        /// MANDATORY: the specific printout 
        virtual  std::ostream& fillStream( std::ostream& s ) const ;
        // ====================================================================
      public:
        // ====================================================================
        /// add one more node to the tree 
        Inclusive& operator+= ( const LoKi::Decays::iTree_<PARTICLE>& tree ) ;
        /// add one more node to the tree 
        Inclusive& operator+= ( const LoKi::Decays::iNode& node ) ;
        /// add one more node to the tree 
        Inclusive& operator+= ( const LHCb::Decay::Item&   node ) 
        { addDaughter ( node ) ; return *this ; }
        /// add one more node to the tree 
        Inclusive& operator+= ( const std::string&         node ) 
        { addDaughter ( node ) ; return *this ; }
        /// add one more node to the tree 
        Inclusive& operator+= ( const LHCb::ParticleID&    node ) 
        { addDaughter ( node ) ; return *this ; }
        /// add one more node to the tree 
        Inclusive& operator+= ( const ParticleProperty*    node ) 
        { addDaughter ( node ) ; return *this ; }
        // ====================================================================
      private:
        // ====================================================================
        /// the default constructor is disabled 
        Inclusive () ; // the default constructor is disabled
        // ====================================================================
      };
      // ======================================================================
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
        // ====================================================================
        /// constructor from the node (mother) and subtrees 
        Optional
        ( const LoKi::Decays::iNode& mother                  , 
          const SubTrees&            children = SubTrees  () ,
          const SubTrees&            optional = SubTrees  () ,
          const Alg                  alg      = Daughters    ) ;
        // ====================================================================
        /// constructor from the node (mother) and subtrees 
        Optional
        ( const std::string&         mother                 , 
          const SubTrees&            children = SubTrees () ,
          const SubTrees&            optional = SubTrees () ,
          const Alg                  alg      = Daughters   ) ;
        // ====================================================================
        /// constructor from the node (mother) and subtrees 
        Optional
        ( const LHCb::ParticleID&    mother                 , 
          const SubTrees&            children = SubTrees () ,
          const SubTrees&            optional = SubTrees () ,
          const Alg                  alg      = Daughters   ) ;
        // ====================================================================
        /// constructor  from the decay & optional
        Optional 
        ( const LHCb::Decay&  decay                  ,
          const SubTrees&     optional = SubTrees () ,
          const Alg           alg      = Daughters   ) ;
        /// constructor  from
        Optional 
        ( const Exclusive&    right                  ,
          const SubTrees&     optional = SubTrees () ) ;
        /// MANDATORY: virtual destructor 
        virtual ~Optional () {}
        // ====================================================================
      public:
        // ====================================================================
        /// MANDATORY: clone method ("virtual constructor")
        virtual  Optional* clone() const { return new Optional ( *this ) ; }
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
        /// set optional
        void setOptional ( const SubTrees& optional ) { m_optional = optional ; }
        /// set optional
        void setOptional ( const std::vector<std::string>&       optional ) ;
        /// set children 
        void setOptional ( const std::vector<LHCb::Decay::Item>& optional ) ;
        /// set optional 
        void setOptional ( const std::vector<LHCb::ParticleID>&  optional ) ;
        /// set optional 
        void setOptional ( const std::vector<const ParticleProperty*>& optional ) ;
        // ====================================================================
      public:
        // ====================================================================
        /// add one more daughter to the decay 
        void addOptional ( const LoKi::Decays::iTree_<PARTICLE>& tree ) ;
        /// add one more daughter to the decay 
        void addOptional ( const LoKi::Decays::iNode& node ) ; 
        /// add one more daughter to the decay 
        void addOptional ( const std::string& node ) ; 
        /// add one more daughter to the decay 
        void addOptional ( const LHCb::ParticleID& node ) ; 
        /// add one more daughter to the decay 
        void addOptional ( const LHCb::Decay::Item& node ) ; 
        /// add one more daughter to the decay 
        void addOptional ( const ParticleProperty* node ) ; 
        // ====================================================================
      public:
        // ====================================================================
        /// add one more node to the tree 
        Optional& operator+= ( const LoKi::Decays::iTree_<PARTICLE>& node ) 
        { addDaughter ( node ) ; return *this ; }
        /// add one more node to the tree 
        Optional& operator+= ( const LoKi::Decays::iNode& node ) 
        { addDaughter ( node ) ; return *this ; }
        /// add one more node to the tree 
        Optional& operator+= ( const std::string& node ) 
        { addDaughter ( node ) ; return *this ; }
        /// add one more node to the tree 
        Optional& operator+= ( const LHCb::ParticleID& node ) 
        { addDaughter ( node ) ; return *this ; }
        /// add one more node to the tree 
        Optional& operator+= ( const LHCb::Decay::Item& node ) 
        { addDaughter ( node ) ; return *this ; }
        /// add one more node to the tree 
        Optional& operator+= ( const ParticleProperty* node ) 
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
        Optional () ; // the default constructor is disabled
        // ====================================================================
      private:
        // ====================================================================
        /// the optional particles in the tree 
        SubTrees m_optional ;
        // ====================================================================
      } ;
      // ======================================================================
    } // end of namespace LoKi::Decays::Trees 
  } // end of namespace LoKi::Decays
} // end of namespace LoKi 
// ============================================================================
/** operator for easy construction of trees 
 *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
 *  @date 2008-07-07
 */
LoKi::Decays::Trees::And_<const LHCb::Particle*>
operator&& 
( const LoKi::Decays::iTree_<const LHCb::Particle*>& t , 
  const LoKi::Decays::iNode&                         n ) ;
// ============================================================================
/** operator for easy construction of trees 
 *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
 *  @date 2008-07-07
 */
LoKi::Decays::Trees::And_<const LHCb::Particle*>
operator&& 
( const LoKi::Decays::iTree_<const LHCb::Particle*>& t , 
  const LHCb::Decay::Item&                           n ) ;
// ============================================================================
/** operator for easy construction of trees 
 *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
 *  @date 2008-07-07
 */
LoKi::Decays::Trees::And_<const LHCb::Particle*>
operator&& 
( const LoKi::Decays::iTree_<const LHCb::Particle*>& t , 
  const std::string&                                 n ) ;
// ============================================================================
/** operator for the easy construction of trees 
 *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
 *  @date 2008-07-07
 */
LoKi::Decays::Trees::And_<const LHCb::Particle*>
operator&& 
( const LoKi::Decays::iTree_<const LHCb::Particle*>& t , 
  const LHCb::ParticleID&                            n ) ;
// ============================================================================
/** operator for easy construction of trees 
 *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
 *  @date 2008-07-07
 */
inline LoKi::Decays::Trees::And_<const LHCb::Particle*>
operator&& 
( const LoKi::Decays::iNode&                         n ,
  const LoKi::Decays::iTree_<const LHCb::Particle*>& t ) { return t && n ; }
// ============================================================================
/** operator for easy construction of trees 
 *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
 *  @date 2008-07-07
 */
inline LoKi::Decays::Trees::And_<const LHCb::Particle*>
operator&& 
( const LHCb::Decay::Item&                           n ,
  const LoKi::Decays::iTree_<const LHCb::Particle*>& t ) { return t && n ; }
// ============================================================================
/** operator for easy construction of trees 
 *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
 *  @date 2008-07-07
 */
inline LoKi::Decays::Trees::And_<const LHCb::Particle*>
operator&& 
( const std::string&                                 n ,
  const LoKi::Decays::iTree_<const LHCb::Particle*>& t ) { return t && n ; }
// ============================================================================
/** operator for the easy construction of trees 
 *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
 *  @date 2008-07-07
 */
inline LoKi::Decays::Trees::And_<const LHCb::Particle*>
operator&& 
( const LHCb::ParticleID&                            n ,
  const LoKi::Decays::iTree_<const LHCb::Particle*>& t ) { return t && n ; }
// ============================================================================
/** operator for easy construction of trees 
 *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
 *  @date 2008-07-07
 */
LoKi::Decays::Trees::Or_<const LHCb::Particle*>
operator|| 
( const LoKi::Decays::iTree_<const LHCb::Particle*>& t , 
  const LoKi::Decays::iNode&                         n ) ;
// ============================================================================
/** operator for easy construction of trees 
 *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
 *  @date 2008-07-07
 */
LoKi::Decays::Trees::Or_<const LHCb::Particle*>
operator||
( const LoKi::Decays::iTree_<const LHCb::Particle*>& t , 
  const LHCb::Decay::Item&                           n ) ;
// ============================================================================
/** operator for easy construction of trees 
 *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
 *  @date 2008-07-07
 */
LoKi::Decays::Trees::Or_<const LHCb::Particle*>
operator||
( const LoKi::Decays::iTree_<const LHCb::Particle*>& t , 
  const std::string&                                 n ) ;
// ============================================================================
/** operator for the easy construction of trees 
 *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
 *  @date 2008-07-07
 */
LoKi::Decays::Trees::Or_<const LHCb::Particle*>
operator||
( const LoKi::Decays::iTree_<const LHCb::Particle*>& t , 
  const LHCb::ParticleID&                            n ) ;
// ============================================================================
/** operator for easy construction of trees 
 *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
 *  @date 2008-07-07
 */
inline LoKi::Decays::Trees::Or_<const LHCb::Particle*>
operator||
( const LoKi::Decays::iNode&                         n ,
  const LoKi::Decays::iTree_<const LHCb::Particle*>& t ) { return t || n ; }
// ============================================================================
/** operator for easy construction of trees 
 *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
 *  @date 2008-07-07
 */
inline LoKi::Decays::Trees::Or_<const LHCb::Particle*>
operator||
( const LHCb::Decay::Item&                           n ,
  const LoKi::Decays::iTree_<const LHCb::Particle*>& t ) { return t || n ; }
// ============================================================================
/** operator for easy construction of trees 
 *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
 *  @date 2008-07-07
 */
inline LoKi::Decays::Trees::Or_<const LHCb::Particle*>
operator||
( const std::string&                                 n ,
  const LoKi::Decays::iTree_<const LHCb::Particle*>& t ) { return t || n ; }
// ============================================================================
/** operator for the easy construction of trees 
 *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
 *  @date 2008-07-07
 */
inline LoKi::Decays::Trees::Or_<const LHCb::Particle*>
operator|| 
( const LHCb::ParticleID&                          n ,
  const LoKi::Decays::iTree_<const LHCb::Particle*>& t ) { return t || n ; }
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_DECAYS_H
// ============================================================================
