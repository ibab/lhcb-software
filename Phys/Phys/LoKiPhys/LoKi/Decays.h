// $Id: Decays.h,v 1.1 2008-06-12 08:25:27 ibelyaev Exp $
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
         *  @param osc require the oscillation flag for mother 
         */
        Exclusive
        ( const LoKi::Decays::iNode& mother                 , 
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
        /// check the validness 
        inline bool ok ( const LHCb::Particle* p ) const { return 0 != p ; }
        // ====================================================================
        /// get the algorithm 
        Alg alg() const { return m_alg ; }
        // ====================================================================
      public:
        // ====================================================================        
        /// add one more daughter to the decay 
        void addDaughter ( const LoKi::Decays::iTree_<PARTICLE>& tree ) ;
        /// add one more daughter to the decay 
        void addDaughter ( const LoKi::Decays::iNode& node ) ; 
        // ====================================================================        
      public:
        // ====================================================================
        /// add one more node to the tree 
        Exclusive& operator+= ( const LoKi::Decays::iTree_<PARTICLE>& tree ) ;
        /// add one more node to the tree 
        Exclusive& operator+= ( const LoKi::Decays::iNode& node ) ;
        // ====================================================================
      protected:
        // ====================================================================
        inline const SubTrees& children() const { return m_children ; }
        // ====================================================================
        SubTrees::const_iterator childBegin () const { return m_children.begin () ; }
        SubTrees::const_iterator childEnd   () const { return m_children.end   () ; }
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
       *  inclusive decays.
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
         *  @param osc require the oscillation flag for mother 
         */
        Inclusive
        ( const LoKi::Decays::iNode& mother                 , 
          const Alg                  alg        = Daughters ) ;
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
        // ====================================================================
      private:
        // ====================================================================
        /// the default constructor is disabled 
        Inclusive () ; // the default constructor is disabled
        // ====================================================================
      };



      // ======================================================================
    } // end of namespace LoKi::Decays::Trees 
  } // end of namespace LoKi::Decays
} // end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_DECAYS_H
// ============================================================================
