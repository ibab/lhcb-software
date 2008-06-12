// $Id: GenDecays.h,v 1.1 2008-06-12 08:19:05 ibelyaev Exp $
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
// LoKiDecay
// ============================================================================
#include "LoKi/Nodes.h"
#include "LoKi/Trees.h"
// ============================================================================
/** @file
 *  Implementation file for LoKi HepMC-specific decay/tree-functors 
 *  @author Vanya BELYAEV Ivan.BELYAEV@nikhef.nl
 *  @date   2008-06-10
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
      /** @class GenExclusive 
       *  Simple sub tree which consists of the node ("mother") and 
       *  subtrees ("children"). Essentially it represent also the 
       *  "semi-exclusive" decay (as well as any decay with the fixed 
       *  number of components for comparisons).
       *  
       *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
       *  @date 2008-06-10
       */
      class GenExclusive: public LoKi::Decays::iTree_<const HepMC::GenParticle*>
      {
      protected:
        typedef const HepMC::GenParticle*                   PARTICLE ;
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
        GenExclusive
        ( const LoKi::Decays::iNode& mother                 , 
          const SubTrees&            children               ,
          const Alg                  alg        = Daughters ) ;
        /** constructor from the node (mother), subtrees and "final" flag
         *  @param mother the mother node 
         *  @param alg the matching algorithm 
         *  @param osc require the oscillation flag for mother 
         */
        GenExclusive
        ( const LoKi::Decays::iNode& mother                 , 
          const Alg                  alg        = Daughters ) ;
        /// MANDATORY: virtual destructor 
        virtual ~GenExclusive () {}
        // ====================================================================      
      public:
        // ====================================================================
        /// MANDATORY: clone method ("virtual constructor")
        virtual  GenExclusive* clone() const { return new GenExclusive ( *this ) ; }
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
        inline bool ok ( const HepMC::GenParticle* p ) const 
        { return 0 != p ? true : false ; }
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
        GenExclusive& operator+= ( const LoKi::Decays::iTree_<PARTICLE>& tree ) ;
        /// add one more node to the tree 
        GenExclusive& operator+= ( const LoKi::Decays::iNode& node ) ;
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
        inline bool mother ( const int pid ) const 
        { return mother ( LHCb::ParticleID ( pid )  ) ;  }
        // ====================================================================
      private:
        // ====================================================================
        /// the default constructor is disabled 
        GenExclusive () ; // the default constructor is disabled
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
      /** @class GenInclusive 
       *  Simple sub tree which consists of the node ("mother") and 
       *  subtrees ("children"). Essentially it represent the 
       *  inclusive decays.
       *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
       *  @date 2008-06-10
       */
      class GenInclusive : public GenExclusive 
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
        GenInclusive
        ( const LoKi::Decays::iNode& mother                 , 
          const SubTrees&            children               ,
          const Alg                  alg        = Daughters ) ;
        /** constructor from the node (mother), subtrees and "final" flag
         *  @param mother the mother node 
         *  @param alg the matching algorithm 
         *  @param osc require the oscillation flag for mother 
         */
        GenInclusive
        ( const LoKi::Decays::iNode& mother                 , 
          const Alg                  alg        = Daughters ) ;
        /// MANDATORY: virtual destructor 
        virtual ~GenInclusive () {}
        // ====================================================================
      public:
        // ====================================================================
        /// MANDATORY: clone method ("virtual constructor")
        virtual  GenInclusive* clone() const { return new GenInclusive ( *this ) ; }
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
        GenInclusive& operator+= ( const LoKi::Decays::iTree_<PARTICLE>& tree ) ;
        /// add one more node to the tree 
        GenInclusive& operator+= ( const LoKi::Decays::iNode& node ) ;
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
       *  subtrees ("children") and optional nodes. Essentially it represent the 
       *  decays with optional particles 
       *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
       *  @date 2008-04-13
       */
      class GenOptional : public GenExclusive
      {
      public:
        // ====================================================================
        /// constructor from the node (mother) and subtrees 
        GenOptional
        ( const LoKi::Decays::iNode& mother                 , 
          const SubTrees&            children               ,
          const SubTrees&            optional               ,
          const Alg                  alg        = Daughters ) ;
        // ====================================================================
        /// constructor from the node (mother) and subtrees 
        GenOptional
        ( const LoKi::Decays::iNode&                             mother   , 
          const SubTrees&                                        children ,
          const LoKi::Decays::iTree_<const HepMC::GenParticle*>& optional ,
          const Alg                             alg        = Daughters  ) ;
        /// constructor from the node (mother) and subtrees 
        GenOptional
        ( const LoKi::Decays::iNode& mother                             , 
          const SubTrees&            children               ,
          const LoKi::Decays::iNode& optional               ,
          const Alg                  alg        = Daughters ) ;
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
        // ====================================================================        
      public:
        // ====================================================================
        /// add one more node to the tree 
        GenOptional& operator+= ( const LoKi::Decays::iTree_<PARTICLE>& tree ) ;
        /// add one more node to the tree 
        GenOptional& operator+= ( const LoKi::Decays::iNode& node ) ;
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
        GenOptional () ; // the default constructor is disabled
        // ====================================================================
      private:
        // ====================================================================
        /// the optional particles in the tree 
        SubTrees m_optional ;
        // ====================================================================
      } ; 
      // ======================================================================
      /** @class GenPhotos
       *  Simple sub-tree which can contains an undefined number of "photons"
       *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
       *  @date 2008-04-21
       */
      class GenPhotos : public GenExclusive
      {
      public:
        // ====================================================================
        /// constructor from the node (mother) and subtrees 
        GenPhotos
        ( const LoKi::Decays::iNode&     mother                 , 
          const SubTrees&                children               ,
          const LoKi::Decays::Trees::Alg alg        = Daughters ) ;
        /// MANDATORY: virtual destructor 
        virtual ~GenPhotos () {}
        // ====================================================================
      public:
        // ====================================================================
        /// MANDATORY: clone method ("virtual constructor")
        virtual  GenPhotos* clone() const { return new GenPhotos ( *this ) ; }
        /// MANDATORY: check the validness 
        virtual bool valid () const 
        { return m_photon.valid() && GenExclusive::valid() ; }
        /// MANDATORY: the proper validation of the tree
        virtual  StatusCode validate ( IParticlePropertySvc* svc ) const ;
        /// MANDATORY: the only one essential method
        virtual bool operator() 
          ( LoKi::Decays::iTree_<const HepMC::GenParticle*>::argument p ) const ;
        /// MANDATORY: the specific printout 
        virtual  std::ostream& fillStream( std::ostream& s ) const ;
        // ====================================================================
      public:
        // ====================================================================
        /// add one more node to the tree 
        GenPhotos& operator+= ( const LoKi::Decays::iTree_<PARTICLE>& tree ) ;
        /// add one more node to the tree 
        GenPhotos& operator+= ( const LoKi::Decays::iNode& node ) ;
        // ====================================================================
      private:
        // ====================================================================
        /// the default constructor is disabled 
        GenPhotos () ; // the default constructor is disabled
        // ====================================================================
      private:
        // ====================================================================
        /// the photon subtree/node ("photon")
        LoKi::Decays::Nodes::Pid m_photon     ; // the sub-trees 
        // ====================================================================
      } ;
      // ======================================================================
      /** @class GenPhotosOptional
       *  Simple sub tree which consists of the node ("mother") and 
       *  subtrees ("children") and optional nodes. Essentially it represent the 
       *  decays with optional particles 
       *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
       *  @date 2008-06-10
       */
      class GenPhotosOptional : public GenOptional
      {
      public:
        // ====================================================================
        /// constructor from the node (mother) and subtrees 
        GenPhotosOptional
        ( const LoKi::Decays::iNode& mother                 , 
          const SubTrees&            children               ,
          const SubTrees&            optional               ,
          const Alg                  alg        = Daughters ) ;
        /// constructor from the node (mother) and subtrees 
        GenPhotosOptional
        ( const LoKi::Decays::iNode& mother                 , 
          const SubTrees&            children               ,
          const LoKi::Decays::iNode& optional               ,
          const Alg                  alg        = Daughters ) ;
        /// constructor from the node (mother) and subtrees 
        GenPhotosOptional
        ( const LoKi::Decays::iNode&                             mother   , 
          const SubTrees&                                        children ,
          const LoKi::Decays::iTree_<const HepMC::GenParticle*>& optional ,
          const Alg                  alg        = Daughters ) ;
        /// MANDATORY: virtual destructor 
        virtual ~GenPhotosOptional () {}
        // ====================================================================
      public:
        // ====================================================================
        /// MANDATORY: clone method ("virtual constructor")
        virtual  GenPhotosOptional* clone() const 
        { return new GenPhotosOptional ( *this ) ; }
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
        GenPhotosOptional& operator+= ( const LoKi::Decays::iTree_<PARTICLE>& tree ) ;
        /// add one more node to the tree 
        GenPhotosOptional& operator+= ( const LoKi::Decays::iNode& node ) ;
        // ====================================================================
      private:
        // ====================================================================
        /// the default constructor is disabled 
        GenPhotosOptional () ; // the default constructor is disabled
        // ====================================================================
      private:
        // ====================================================================
        /// the photon subtree/node ("photon")
        LoKi::Decays::Nodes::Pid m_photon     ; // the sub-trees 
        // ====================================================================
      } ;
      // ======================================================================
    } // end of namespace LoKi::Decays::Trees
    // ========================================================================
  } // end of namespace LoKi::Decays 
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_GENDECAYS_H
// ============================================================================
