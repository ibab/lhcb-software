// $Id: Trees.h,v 1.2 2008-07-09 16:01:00 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_DECAYS_TREES_H 
#define LOKI_DECAYS_TREES_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL
// ============================================================================
#include <algorithm>
// ============================================================================
// LHCbKernel
// ============================================================================
#include "Kernel/ParticleID.h" 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/iTree.h"
#include "LoKi/Nodes.h"
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
      /** the type of maching algorithm:
       *   - <c>Daughters</c> match within (all) direct daughetrs 
       *   - <c>Sections</c>  match within all sections of the decay graph 
       *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
       *  @date 2008-04-30
       */       
      enum Alg {
        Daughters         = 0 , // match only for the direct daughters 
        Sections              , // match in all graph sections 
      } ;
      // ======================================================================
      /** @enum Oscillation 
       *  simple e-num to distinguish oscillated and non-oscillated particles 
       *  @author Vanya BELYAEV Ivan.BELYAEV@nikhef.nl
       *  @see LHCb:MCParticle
       *  @see LHCb:MCParticle::hasOscillated 
       *  @date   2008-05-28
       */
      enum Oscillation {
        /// no need to check the oscillation flag
        Undefined = 0 , //   non-defined 
        /// require the positive oscilaltion flag 
        Oscillated    , //    
        /// require the negative oscilaltion flag 
        NotOscillated   // 
      } ;     
      // ======================================================================
      /** @class Marked_ 
       *  Simple "marked" sub-tree
       *  @attention the class caches the event-data!!
       *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
       *  @date 2008-04-30
       */
      template <class PARTICLE>
      class Marked_ : public LoKi::Decays::iTree_<PARTICLE>
      {
      public:
        // ====================================================================
        /// constructor from the tree 
        Marked_ ( LoKi::Decays::iTree_<PARTICLE>& tree ) ;
        /// copy constructor (ignore the marked particle)
        Marked_ ( const Marked_& right ) ;
        /// MANDATORY: virtual destructor 
        virtual ~Marked_ () {}
        /// MANDATORY: clone method ("virtual constructor")
        virtual  Marked_* clone() const { return new Marked_(*this) ; }
        /// MANDATORY: the only one essential method:
        virtual bool operator() 
          ( typename  LoKi::Decays::iTree_<PARTICLE>::argument p ) const  ;
        /// MANDATORY: the specific printout 
        virtual std::ostream& fillStream ( std::ostream& s ) const ;
        /// Check the validity 
        virtual bool valid() const ;
        /// validate it
        virtual StatusCode validate ( IParticlePropertySvc* svc ) const ;
        /// reset the collection cache 
        virtual void reset () const ; 
        /// collect the marked elements 
        virtual size_t collect 
        ( typename LoKi::Decays::iTree_<PARTICLE>::Collection& output ) const ;
        // ====================================================================
      private:
        // ====================================================================
        /// the default constructor is disabled 
        Marked_ () ; // no default constructor 
        // ====================================================================
      private:
        // ====================================================================
        /// the actual tree 
        LoKi::Decays::Tree_<PARTICLE> m_tree ;      // the actual tree 
        // ====================================================================
        /// the particle itself  
        mutable PARTICLE  m_particle ;              // the particle itself 
        // ====================================================================
      } ; 
      // ======================================================================
      /** @class Any_
       *  the most trivial pseudo-tree, which matches to *any* tree
       *  @author Vanya BELYAEV Ivan.BElyaev@nikhef.nl
       *  @date 2008-04-30
       */
      template <class PARTICLE>
      class Any_ : public LoKi::Decays::iTree_<PARTICLE>
      {
      private:
        // ====================================================================
        /// get the actual argument type form the base 
        typedef typename  LoKi::Decays::iTree_<PARTICLE>::argument   argument   ;
        /// get the actual collection type form the base 
        typedef typename  LoKi::Decays::iTree_<PARTICLE>::Collection Collection ;
        // ====================================================================
      public:
        // ====================================================================
        /// MANDATORY: virtual destructor 
        virtual ~Any_() {}
        /// MANDATORY: clone method ("virtual constructor")
        virtual  Any_* clone() const { return new Any_(*this) ; }
        /// MANDATORY: the only one essential method:
        virtual bool operator() ( argument /* p */ ) const { return true ; }
        /// MANDATORY: the printout 
        virtual std::ostream& fillStream ( std::ostream& s ) const 
        { return s << " XXX "; }
        /// Check the validity 
        virtual bool valid() const { return true ; }
        /// validate it
        virtual StatusCode validate ( IParticlePropertySvc* /* svc */ ) const 
        { return StatusCode( StatusCode::SUCCESS , true ) ; }
        /// reset the collection cache 
        virtual void reset () const {}
        /// collect the marked elements 
        virtual size_t collect ( Collection& ouput ) const { return 0 ; }
        // ====================================================================
      } ;
      // ======================================================================      
      /** @class _Tree_
       *  Helper class needed to hold temporary Tree-objects 
       *  and for the proper permutations 
       *  @author Vanya BELYAEV Ivan.Belyaev@nikhed.nl
       *  @date 2008-05-08
       */
      template <class PARTICLE> 
      struct _Tree_ : public std::unary_function<PARTICLE,bool> 
      {
      public:
        // ====================================================================
        /// the actual type for the vector of trees 
        typedef std::vector<_Tree_>  SubTrees ;
        // ====================================================================
      public:
        // ====================================================================
        /// the default constructor   (Assing invalid ID)
        _Tree_ () 
          : m_tree ( Any_<PARTICLE>() ) , m_id ( 0 )  {}
        /// the constructor from the tree (Assign unique ID) 
        _Tree_ ( const LoKi::Decays::iTree_<PARTICLE>& tree ) 
          : m_tree ( tree        ) , m_id ( 0 )  { m_id = getID() ; }
        /// the constructor from the tree (Assign unique ID)
        _Tree_ ( const LoKi::Decays::Tree_<PARTICLE>&  tree ) 
          : m_tree ( tree        ) , m_id ( 0 )  { m_id = getID() ; }
        /// copy constructor  (Copy the unique ID)
        _Tree_ ( const _Tree_& tree ) 
          : m_tree ( tree.m_tree ) , m_id ( tree.m_id ) {}
        // ====================================================================
      public:
        // ====================================================================
        /// the main method 
        inline bool operator () 
          ( typename LoKi::Decays::iTree_<PARTICLE>::argument p ) const 
        { return m_tree.tree ( p ) ; }
        // valid ? 
        inline bool valid () const { return m_tree.tree().valid() ; }
        // validate 
        inline StatusCode validate ( IParticlePropertySvc* svc ) const 
        { 
          StatusCode sc = m_tree.tree().validate ( svc ) ;
          return sc ;
        }
        /// collect the marked elements 
        inline size_t collect 
        ( typename LoKi::Decays::iTree_<PARTICLE>::Collection& o ) const 
        { return m_tree.tree().collect ( o ) ; }
        /// reset the cache 
        inline void reset () const { return m_tree.reset() ; }    
        /// the specific printout 
        inline std::ostream& fillStream( std::ostream& s ) const 
        { return m_tree.fillStream ( s ) ; }
        // ====================================================================
      public:
        // ====================================================================
        /// needed for std::find
        inline bool operator== 
        ( typename LoKi::Decays::iTree_<PARTICLE>::argument p ) const 
        { return m_tree.tree ( p ) ; }
        // ====================================================================
        /// needed for std::find
        inline bool operator!= 
        ( typename LoKi::Decays::iTree_<PARTICLE>::argument p ) const 
        { return !m_tree.tree ( p ) ; }
        // ====================================================================
      public:
        // ====================================================================
        /// assignement operator, Copy the unique ID 
        _Tree_& operator=( const _Tree_& right ) 
        { 
          if ( &right == this ) { return *this ; }
          m_tree = right.m_tree ;
          /// copy the unique ID 
          m_id   = right.m_id ;                     // NB! Copy the unique ID 
          return *this ;
        }
        // ====================================================================
      public:
        // ====================================================================
        /// ordering (nesessary for the proper permutations)
        bool operator< ( const _Tree_& right ) const 
        { return m_id < right.m_id ; }  
        // ====================================================================
      private:
        // ====================================================================
        /// the actual tree 
        LoKi::Decays::Tree_<PARTICLE> m_tree ; // the actual tree for matching
        // ====================================================================        
        /// own unique ID 
        size_t m_id ; // own unique ID 
        // ====================================================================        
      private:
        // ====================================================================
        /// the source of IDs 
        static size_t getID () ; // the source of IDs 
        // ====================================================================
      } ;
      // ======================================================================
      /** @struct CheckTree
       *  Helper struture to use the whole power of STL algorithms 
       *  for decay finder tools 
       *  @author Vanya BELYAEV Ivan.Belyave@nikhef.nl
       *  @date 2008-07-07
       */
      template <class PARTICLE>
      struct CheckTree
      {
        // ====================================================================
        /// compare tree and the argument 
        inline bool operator() 
          ( typename LoKi::Decays::iTree_<PARTICLE>::argument p , 
            const LoKi::Decays::Trees::_Tree_<PARTICLE>&      t ) const 
        { return t ( p ) ; }        
        // ====================================================================
        /// compare tree and the argument 
        inline bool operator() 
          ( const LoKi::Decays::Trees::_Tree_<PARTICLE>&      t ,
            typename LoKi::Decays::iTree_<PARTICLE>::argument p ) const 
        { return t ( p ) ; }        
        // ====================================================================
      } ;
      // ======================================================================
      /** @class  Op_ 
       *  Simple sub-tree which matches "operation"
       *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
       *  @date 2008-04-13
       */
      template <class PARTICLE> 
      class Op_ : public LoKi::Decays::iTree_<PARTICLE> 
      {
      public:
        // ====================================================================
        /// the actual type for subtrees 
        typedef typename _Tree_<PARTICLE>::SubTrees  SubTrees       ;
        typedef typename SubTrees::iterator          iterator       ;
        typedef typename SubTrees::const_iterator    const_iterator ;
        // ====================================================================
      public:
        // ====================================================================
        /// constructor from two sub-trees 
        Op_ ( const LoKi::Decays::iTree_<PARTICLE>& n1 , 
              const LoKi::Decays::iTree_<PARTICLE>& n2 ) ;
        /// constructor from list of sub-trees 
        Op_ ( const SubTrees& trees ) ;
        /// MANDATORY: virtual destrcutor 
        virtual ~Op_() {}
        // ====================================================================
      public:
        /// MANDATORY: check the validness 
        virtual bool valid() const ;
        /// MANDATORY: the proper validation of the tree
        virtual  StatusCode validate ( IParticlePropertySvc* svc ) const ;
        /// MANDATORY: reset method 
        virtual void reset() const { return i_reset() ; }
        /// MANDATORY: collect all marked elements 
        virtual size_t collect 
        ( typename LoKi::Decays::iTree_<PARTICLE>::Collection& ) const ;
        // ====================================================================
      protected:
        // ====================================================================
        inline const SubTrees& trees () const { return m_trees ;  }        
        inline       SubTrees& trees ()       { return m_trees ;  }
        // ====================================================================
        inline       iterator begin ()       { return m_trees.begin () ; }
        inline const_iterator begin () const { return m_trees.begin () ; }
        inline       iterator end   ()       { return m_trees.end   () ; }
        inline const_iterator end   () const { return m_trees.end   () ; }          
        // ====================================================================
        void push_back ( const LoKi::Decays::iTree_<PARTICLE>& tree ) 
        { m_trees.push_back ( tree ) ;  }      
        // ====================================================================        
      protected:
        // ====================================================================
        /// inline form of reset 
        inline void i_reset() const ;
        // ====================================================================
      private:
        // ====================================================================
        /// the default constructor is disabled
        Op_ () ; // the default constructor is disabled
        // ====================================================================
      private:
        // ====================================================================
        mutable SubTrees m_trees ;
        // ====================================================================
      } ;
      // ======================================================================
      /** @class  Or_ 
       *  Simple sub-tree which matches .OR. for other subtrees
       *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
       *  @date 2008-04-13
       */
      template <class PARTICLE> 
      class Or_ : public LoKi::Decays::Trees::Op_<PARTICLE> 
      {
      public:
        // ====================================================================
        /// constructor from two sub-trees 
        Or_ ( const LoKi::Decays::iTree_<PARTICLE>& n1 , 
              const LoKi::Decays::iTree_<PARTICLE>& n2 ) ;
        /// constructor from list of sub-trees 
        Or_ ( const typename Op_<PARTICLE>::SubTrees& trees ) ;
        /// MANDATORY: virtual destrcutor 
        virtual ~Or_() {}
        // ====================================================================
      public:
        // ====================================================================
        /// MANDATORY: clone method ("virtual constructor")
        virtual  Or_* clone() const { return new Or_ ( *this ) ; }
        /// MANDATORY: the only one essential method
        virtual bool operator() 
          ( typename LoKi::Decays::iTree_<PARTICLE>::argument p ) const ;
        /// MANDATORY: the specific printout 
        virtual  std::ostream& fillStream( std::ostream& s ) const ;
        // ====================================================================
      public:
        // ====================================================================
        Or_& operator|= ( const LoKi::Decays::iTree_<PARTICLE>& tree ) 
        { this->push_back( tree ) ; return *this ; }
        Or_& operator|= ( const LoKi::Decays:: Tree_<PARTICLE>& tree ) 
        { this->push_back( tree ) ; return *this ; }
        Or_& operator|= ( const Or_& tree ) ;
        // ====================================================================
      private:
        // ====================================================================
        /// the default constructor is disabled
        Or_ () ; // the default constructor is disabled
        // ====================================================================
      } ;
      // ======================================================================
      /** @class  And_ 
       *  Simple sub-tree which matches .AND. for other subtrees
       *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
       *  @date 2008-04-13
       */
      template <class PARTICLE>
      class And_ : public LoKi::Decays::Trees::Op_<PARTICLE>
      {
      public:
        /// constructor from two sub-trees 
        And_ ( const LoKi::Decays::iTree_<PARTICLE>& n1 , 
               const LoKi::Decays::iTree_<PARTICLE>& n2 ) ;
        /// constructor from list of sub-trees 
        And_ ( const typename Op_<PARTICLE>::SubTrees& trees ) ;
        /// MANDATORY: virtual destrcutor 
        virtual ~And_ () {}
        // ====================================================================
      public:
        // ====================================================================
        /// MANDATORY: clone method ("virtual constructor")
        virtual  And_* clone() const { return new And_ ( *this ) ; }
        /// MANDATORY: the only one essential method
        virtual bool operator() 
          ( typename LoKi::Decays::iTree_<PARTICLE>::argument p ) const ;
        /// MANDATORY: the specific printout 
        virtual  std::ostream& fillStream( std::ostream& s ) const ;
      public:
        // ====================================================================
        And_& operator&= ( const LoKi::Decays::iTree_<PARTICLE>& tree ) 
        { this->push_back( tree ) ; return *this ; }
        And_& operator&= ( const LoKi::Decays:: Tree_<PARTICLE>& tree ) 
        { this->push_back( tree ) ; return *this ; }
        And_& operator&= ( const And_& tree ) ;
        // ====================================================================
      private:
        // ====================================================================
        /// the default constructor is disabled
        And_ () ; // the default constructor is disabled
        // ====================================================================
      private:
        // ====================================================================
        typename LoKi::Decays::Trees::Op_<PARTICLE>::SubTrees m_trees ;
        // ====================================================================
      } ;
      // ======================================================================
      /** @class  Not_
       *  Simple sub-tree which matches .NOT. for subtree
       *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
       *  @date 2008-04-13
       */
      template <class PARTICLE>
      class Not_ : public LoKi::Decays::iTree_<PARTICLE> 
      {
      public:
        // ====================================================================
        /// constructor from the node 
        Not_ ( const LoKi::Decays::iTree_<PARTICLE>& tree ) ;
        /// MANDATORY: virtual destrcutor 
        virtual ~Not_ () {}
        // ====================================================================
      public:
        // ====================================================================
        // MANDATORY: clone method ("virtual constructor")
        virtual  Not_* clone() const { return new Not_ ( *this ) ; }
        /// MANDATORY: the only one essential method
        virtual bool operator() 
          ( typename LoKi::Decays::iTree_<PARTICLE>::argument p ) const ;
        /// MANDATORY: the specific printout 
        virtual  std::ostream& fillStream( std::ostream& s ) const ;
        /// MANDATORY: check the validness 
        virtual bool valid () const ;
        /// MANDATORY: the proper validation of the tree
        virtual  StatusCode validate ( IParticlePropertySvc* svc ) const ;
        /// reset the cache 
        virtual void reset() ; 
        /// MANDATORY: collect all marked elements 
        virtual size_t collect 
        ( typename LoKi::Decays::iTree_<PARTICLE>::Collection& ) const ;
        // ====================================================================
      private:
        // ====================================================================
        /// the default constructor is disabled
        Not_ () ; // the default constructor is disabled
        // ====================================================================
      private:
        // ====================================================================
        /// the tree itself 
        LoKi::Decays::Tree_<PARTICLE> m_tree ; // the tree itself 
        // ====================================================================
      } ;
      // ======================================================================      
    } // end of namespace LoKi::Decays::Trees 
    // ========================================================================
  } // end of namespace DaVinci::Graphs
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
/** Create the "AND" of two trees 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-04-12
 */
template <class PARTICLE>
inline LoKi::Decays::Trees::And_<PARTICLE> 
operator&&
( const LoKi::Decays::iTree_<PARTICLE>& o1 , 
  const LoKi::Decays::iTree_<PARTICLE>& o2 ) 
{ 
  return LoKi::Decays::Trees::And_<PARTICLE> ( o1 , o2 ) ;
}
// ============================================================================
/** Create the "AND" of two trees 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-04-12
 */
template <class PARTICLE>
inline LoKi::Decays::Trees::And_<PARTICLE> 
operator&&
( const LoKi::Decays::Trees::And_<PARTICLE>& o1 , 
  const LoKi::Decays::iTree_<PARTICLE>&      o2 ) 
{ 
  LoKi::Decays::Trees::And_<PARTICLE> result ( o1 ) ; result &= o2 ;
  return result ;
}
// ============================================================================
/** Create the "AND" of two trees 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-04-12
 */
template <class PARTICLE>
inline LoKi::Decays::Trees::And_<PARTICLE> 
operator&&
( const LoKi::Decays::iTree_<PARTICLE>&      o1 , 
  const LoKi::Decays::Trees::And_<PARTICLE>& o2 )
{ 
  LoKi::Decays::Trees::And_<PARTICLE> result ( o1 ) ; result &= o2 ;
  return result ;
}
// ============================================================================
/** Create the "Or" of two trees 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-04-12
 */
template <class PARTICLE> 
inline LoKi::Decays::Trees::Or_<PARTICLE>
operator||
( const LoKi::Decays::iTree_<PARTICLE>& o1 , 
  const LoKi::Decays::iTree_<PARTICLE>& o2 ) 
{ 
  return LoKi::Decays::Trees::Or_<PARTICLE> ( o1 , o2 ) ; 
}
// ============================================================================
/** Create the "OR" of two trees 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-04-12
 */
template <class PARTICLE>
inline LoKi::Decays::Trees::Or_<PARTICLE> 
operator||
( const LoKi::Decays::Trees::Or_<PARTICLE>& o1 , 
  const LoKi::Decays::iTree_<PARTICLE>&     o2 ) 
{ 
  LoKi::Decays::Trees::Or_<PARTICLE> result ( o1 ) ; result |= o2 ;
  return result ;
}
// ============================================================================
/** Create the "OR" of two trees 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-04-12
 */
template <class PARTICLE>
inline LoKi::Decays::Trees::Or_<PARTICLE> 
operator||
( const LoKi::Decays::iTree_<PARTICLE>&     o1 , 
  const LoKi::Decays::Trees::Or_<PARTICLE>& o2 )
{ 
  LoKi::Decays::Trees::Or_<PARTICLE> result ( o1 ) ; result |= o2 ;
  return result ;
}
// ============================================================================
/** Create the "Not" for the node
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-04-12
 */
template <class PARTICLE>
inline LoKi::Decays::Trees::Not_<PARTICLE> 
operator! ( const LoKi::Decays::iTree_<PARTICLE>& o ) 
{ return LoKi::Decays::Trees::Not_<PARTICLE> ( o ) ; }
// ============================================================================
/** The standard output stream operator for the sub-tree
 *  @param s the reference to the outptu stream 
 *  @param o the decay tree objects 
 *  @return the reference to the output stream
 */
template <class PARTICLE>
inline std::ostream& operator<<
  ( std::ostream&                                s , 
    const LoKi::Decays::Trees::_Tree_<PARTICLE>& o ) 
{ return o.fillStream ( s ) ; }
// ============================================================================
/** "right" for of equality operator 
 *  (needed for std::find)
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-07-07
 */
template <class PARTICLE>
inline bool operator== 
( typename LoKi::Decays::iTree_<PARTICLE>::argument p , 
  const    LoKi::Decays::iTree_<PARTICLE>&          t )
{ return t == p ; }
// ====================================================================


// ============================================================================
/** sequncer operator  (the same as "OR" here)
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-04-12
 */
template <class PARTICLE> 
inline LoKi::Decays::Trees::Or_<PARTICLE>
operator,
  ( const LoKi::Decays::iTree_<PARTICLE>& o1 , 
    const LoKi::Decays::iTree_<PARTICLE>& o2 ) 
{ return LoKi::Decays::Trees::Or_<PARTICLE> ( o1 , o2 ) ; }
// ============================================================================




// ============================================================================
#include "LoKi/Trees.icpp"
// ============================================================================
// The END
// ============================================================================
#endif // LOKI_DECAYS_TREES_H
// ============================================================================
