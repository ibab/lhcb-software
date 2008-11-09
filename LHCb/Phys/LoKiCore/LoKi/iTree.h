// $Id: iTree.h,v 1.3 2008-11-09 17:10:06 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_ITREE_H 
#define LOKI_ITREE_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <functional>
#include <string>
#include <vector>
#include <iostream>
#include <typeinfo>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/StatusCode.h"
// ============================================================================
// Boost 
// ============================================================================
#include "boost/call_traits.hpp"
// ============================================================================
class IParticlePropertySvc ;
// ============================================================================
namespace LoKi
{
  namespace Decays
  {    
    // ========================================================================
    /** @class iTree_ Decays/Tree.h
     *  The abstract class which represents the sub-tree of the decay tree
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-04-12
     */
    template <class PARTICLE>
    class iTree_ : public std::unary_function<PARTICLE,bool>
    {
    public:
      // ======================================================================
      /// the actual type of the argument 
      typedef typename boost::call_traits<PARTICLE>::param_type argument   ;
      typedef std::vector<PARTICLE>                             Collection ;
      // ======================================================================
    public:
      // ======================================================================
      /** the basic operation: comparison of the sub-tree with the 
       *  actual decay 
       *  @param   p the particle to be compared 
       *  @return  true if the actual decay tree of the particle is 
       *           described by the tree objects
       */
      virtual bool operator()  ( argument p ) const = 0 ;
      // ======================================================================
      /** printout of the stream
       *  @param s the reference to the output stream 
       *  @return the reference to the output stream 
       */
      virtual std::ostream& fillStream ( std::ostream& s ) const = 0 ;
      // ======================================================================
      /// clone method ("virtual constructor")
      virtual  iTree_* clone() const  = 0 ;
      // ======================================================================
      /// check the validity of the tree 
      virtual bool valid() const = 0 ;
      // ======================================================================
      /** validate the decay tree 
       *  @param svc pointer to Particle Property Service
       *  @return StatusCode 
       */
      virtual StatusCode validate ( IParticlePropertySvc* svc ) const = 0 ;
      // ======================================================================
      /// collect all marked elements 
      virtual size_t collect ( Collection& ) const = 0 ;
      /// reset the collection cache 
      virtual void reset() const = 0 ;
      /// virtual destructor 
      virtual ~iTree_ () {} ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Tree
     *  Simple holder of the decay tree 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl.
     *  @date 2008-04-13
     */
    template <class PARTICLE>
    class Tree_ : public LoKi::Decays::iTree_<PARTICLE>
    {
    public:
      // ======================================================================
      /// constructor from the tree 
      Tree_ ( const LoKi::Decays::iTree_<PARTICLE>& tree ) ;
      // ======================================================================
      /// copy constructor 
      Tree_ ( const Tree_& tree ) ;
      /// MANDATORY: virtual destructor 
      virtual ~Tree_ () { delete m_tree ; }
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Tree_* clone () const { return new Tree_(*this) ; }
      /// MANDATORY: the only one essential method
      virtual  bool operator() 
        (  typename  LoKi::Decays::iTree_<PARTICLE>:: argument p ) const 
      { return tree ( p ) ; }
      /// MANDATORY: the specific printout 
      virtual  std::ostream& fillStream( std::ostream& s ) const 
      { return m_tree -> fillStream ( s ) ; }
      /// MANDATORY: check the validity of the node 
      virtual bool valid() const { return m_tree->valid() ; }
      /// MANDATORY: the proper validation of the tree
      virtual  StatusCode validate ( IParticlePropertySvc* svc ) const 
      { return m_tree->validate ( svc ) ; }
      /// collect the marked elements 
      virtual size_t collect 
      ( typename  LoKi::Decays::iTree_<PARTICLE>::Collection& output ) const 
      { return m_tree->collect ( output ) ; }
      /// reset the collection cache 
      virtual void reset() const { return m_tree->reset () ; }
      // ======================================================================
    public:
      // ======================================================================
      /// get the underlying tree 
      const LoKi::Decays::iTree_<PARTICLE>& tree () const { return *m_tree ; }
      /// cast to the underlying teree     
      operator const LoKi::Decays::iTree_<PARTICLE>& () const { return tree() ; }
      // ======================================================================
    public:
      // ======================================================================
      bool tree ( typename  LoKi::Decays::iTree_<PARTICLE>:: argument p ) const
      { return (*m_tree) ( p ) ; }
      // ======================================================================
    public:
      // ======================================================================
      /// assignement operator 
      Tree_& operator=( const  Tree_&           right ) ; // assignement 
      /// pseudo-assignement operator 
      Tree_& operator=( const iTree_<PARTICLE>& right ) ; // assignement 
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is private obe 
      Tree_ () ; // no  default constructor 
      // ======================================================================
    private:
      // ======================================================================
      /// the tree itself 
      LoKi::Decays::iTree_<PARTICLE>* m_tree ; // the tree itself
      // ======================================================================
    } ;    
    // ========================================================================
  } // end of namespace LoKi::Decays
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
// constructor from the tree 
// ============================================================================
template <class PARTICLE>
LoKi::Decays::Tree_<PARTICLE>::Tree_ 
( const LoKi::Decays::iTree_<PARTICLE>& tree ) 
  : LoKi::Decays::iTree_<PARTICLE> () 
  , m_tree (0) 
{
  if ( typeid ( LoKi::Decays::Tree_<PARTICLE> ) == typeid ( tree ) ) 
  {
    const Tree_& _tree = dynamic_cast<const Tree_&>( tree ) ;
    m_tree = _tree.m_tree->clone() ;
  }
  else { m_tree = tree.clone() ; }
}
// ============================================================================
// copy constructor 
// ============================================================================
template <class PARTICLE>
LoKi::Decays::Tree_<PARTICLE>::Tree_ 
( const LoKi::Decays::Tree_<PARTICLE>& tree ) 
  : LoKi::Decays::iTree_<PARTICLE> ( tree ) 
  , m_tree ( tree.m_tree -> clone() ) 
{}             
// ============================================================================
// assignement operator 
// ============================================================================
template <class PARTICLE>
LoKi::Decays::Tree_<PARTICLE>& 
LoKi::Decays::Tree_<PARTICLE>::operator=
( const LoKi::Decays::Tree_<PARTICLE>& right ) // assignement 
{
  if ( &right == this ) { return *this ; } 
  LoKi::Decays::iTree_<PARTICLE>* tree = right.m_tree->clone() ; 
  delete m_tree ; 
  m_tree = tree ;
  return *this ;
}
// ============================================================================
// assignement operator 
// ============================================================================
template <class PARTICLE>
LoKi::Decays::Tree_<PARTICLE>& 
LoKi::Decays::Tree_<PARTICLE>::operator=
( const LoKi::Decays::iTree_<PARTICLE>& right ) // assignement 
{
  if ( &right == this ) { return *this ; } 
  LoKi::Decays::iTree_<PARTICLE>* tree = 0 ;
  typedef LoKi::Decays::Tree_<PARTICLE> Self ;
  //
  if ( typeid ( Self ) == typeid ( right ) ) 
  { 
    const Self* self = dynamic_cast<const Self*>( &right ) ;  
    tree = self->m_tree->clone() ; 
  }
  else { tree = right.clone()         ; }
  //
  delete m_tree ; 
  m_tree = tree ;
  //
  return *this ;
}
// ============================================================================



// ============================================================================
/** The standard output stream operator for the sub-tree
 *  @param s the reference to the outptu stream 
 *  @param o the decay tree objects 
 *  @return the reference to the outptu stream
 */
template <class PARTICLE>
inline std::ostream& operator<<
  ( std::ostream&                         s , 
    const LoKi::Decays::iTree_<PARTICLE>& o ) { return o.fillStream ( s ) ; }
// ============================================================================



// ============================================================================
// The END
// ============================================================================
#endif // LOKI_ITREE_H
// ============================================================================
