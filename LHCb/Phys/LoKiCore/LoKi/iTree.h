// $Id: iTree.h,v 1.11 2009-05-23 15:54:19 ibelyaev Exp $
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
#include <iosfwd>
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
namespace LHCb { class IParticlePropertySvc ; }
// ============================================================================
namespace Decays
{  
  // ==========================================================================
  /** @class iTree
   *  The abstract class which represents the sub-tree of the decay tree
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date   2008-04-12
   */
  class iTree
  {
  public:
    // ======================================================================
    /// check the validity of the tree 
    virtual bool valid () const = 0 ;
    // ======================================================================
    /** validate the decay tree 
     *  @param svc pointer to Particle Property Service
     *  @return StatusCode 
     */
    virtual StatusCode validate 
    ( const LHCb::IParticlePropertySvc* svc ) const = 0 ;
    // ======================================================================
    /** has marked elements in the tree ? 
     *  method to check if some elements of the decay tree are marked 
     *  @code 
     *
     *  typedef iTree_<XXX> iTree ;
     *
     *  const iTree& tree = ... ;
     *  if ( tree.marked() ) { ... } /
     *  @endcode
     *  It is esepecially suited for usage with "collect"
     *  
     *  @code 
     *
     *  typedef iTree_<XXX> iTree ;
     *
     *  Collectiion collection  = ... ;
     *
     *  XXX B= ... ; 
     *
     *  const iTree& tree = ... ;
     *
     *  if ( tree ( B ) && tree.marked() ) 
     *   {
     *     tree.collect ( collection ) ;
     *     tree.reset() ;
     *   } 
     *
     *  @endcode
     */
    virtual bool marked() const = 0 ;
    // ========================================================================
    /// reset the collection cache 
    virtual void reset() const = 0 ;
    // ======================================================================
    /// clone method ("virtual constructor")
    virtual  iTree* clone() const = 0 ;
    // ======================================================================
    /** printout of the stream
     *  @param s the reference to the output stream 
     *  @return the reference to the output stream 
     */
    virtual std::ostream& fillStream ( std::ostream& s ) const = 0 ;
    /// conversion to the string 
    virtual std::string toString() const ;
    // ======================================================================
    /// virtual destructor 
    virtual ~iTree () ;
    // ======================================================================
  } ;
  // ========================================================================
  /** @class iTree_ Decays/Tree.h
   *  The abstract class which represents the sub-tree of the decay tree
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date   2008-04-12
   */
  template <class PARTICLE>
  class iTree_ : public iTree
  {
  public:
    // ======================================================================
    /// the actual type of the argument 
    typedef typename boost::call_traits<PARTICLE>::param_type argument   ;
    typedef std::vector<PARTICLE>                             Collection ;
    // the actual type of particle
    typedef PARTICLE                                          particle   ;
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
    /// clone method ("virtual constructor")
    virtual  iTree_* clone() const  = 0 ;
    // ======================================================================
    /** collect all marked elements of the decay tree 
     * 
     *  @code 
     *
     *  typedef iTree_<XXX> iTree ;
     *
     *  Collectiion collection  = ... ;
     *
     *  XXX B= ... ; 
     *
     *  const iTree& tree = ... ;
     *
     *  if ( tree ( B ) && tree.marked() ) 
     *   {
     *     tree.collect ( collection ) ;
     *   } 
     *
     *  @endcode
     */
    virtual size_t collect ( Collection& ) const = 0 ;
    // ======================================================================
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
  class Tree_ : public Decays::iTree_<PARTICLE>
  {
  public:
    // ======================================================================
    /// constructor from the tree 
    Tree_ ( const Decays::iTree_<PARTICLE>& tree ) ;
    // ======================================================================
    /// copy constructor 
    Tree_ ( const Tree_& tree ) ;
    /// MANDATORY: virtual destructor 
    virtual ~Tree_ () { delete m_tree ; }
    /// MANDATORY: clone method ("virtual constructor")
    virtual  Tree_* clone () const { return new Tree_(*this) ; }
    /// MANDATORY: the only one essential method
    virtual  bool operator() 
      ( typename  Decays::iTree_<PARTICLE>:: argument p ) const 
    { return tree ( p ) ; }
    /// MANDATORY: the specific printout 
    virtual  std::ostream& fillStream( std::ostream& s ) const 
    { return m_tree -> fillStream ( s ) ; }
    /// MANDATORY: check the validity of the node 
    virtual bool valid() const { return m_tree->valid() ; }
    /// MANDATORY: the proper validation of the tree
    virtual  StatusCode validate 
    ( const LHCb::IParticlePropertySvc* svc ) const 
    { return m_tree->validate ( svc ) ; }
    /// collect the marked elements 
    virtual size_t collect 
    ( typename Decays::iTree_<PARTICLE>::Collection& output ) const 
    { return m_tree->collect ( output ) ; }
    /// reset the collection cache 
    virtual void reset() const { return m_tree->reset () ; }
    /// has marked elements in the tree? 
    virtual bool marked () const { return m_tree->marked() ; }
    // ======================================================================
  public:
    // ======================================================================
    Tree_& operator&= ( const Decays::iTree_<PARTICLE>& tree );
    Tree_& operator|= ( const Decays::iTree_<PARTICLE>& tree );    
    // ======================================================================
  public:
    // ======================================================================
    /// get the underlying tree 
    inline const Decays::iTree_<PARTICLE>& tree () const { return *m_tree ; }
    // ======================================================================
  public:
    // ======================================================================
    bool tree ( typename Decays::iTree_<PARTICLE>:: argument p ) const
    { return (*m_tree) ( p ) ; }
    // ======================================================================
  public:
    // ======================================================================
    bool eval ( typename Decays::iTree_<PARTICLE>:: argument p ) const
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
    Decays::iTree_<PARTICLE>* m_tree ; // the tree itself
    // ======================================================================
  } ;    
  // ========================================================================
} // end of namespace Decays
// ============================================================================
// constructor from the tree 
// ============================================================================
template <class PARTICLE>
Decays::Tree_<PARTICLE>::Tree_ 
( const Decays::iTree_<PARTICLE>& tree ) 
  : Decays::iTree_<PARTICLE> () 
  , m_tree (0) 
{
  typedef Decays::Tree_<PARTICLE> Self ;  
  const Self* self = dynamic_cast<const Self*>( &tree ) ;
  if ( 0 !=   self ) { m_tree = self  -> m_tree-> clone () ; }
  if ( 0 == m_tree ) { m_tree = tree.             clone () ; }
}
// ============================================================================
// copy constructor 
// ============================================================================
template <class PARTICLE>
Decays::Tree_<PARTICLE>::Tree_ 
( const Decays::Tree_<PARTICLE>& tree )
  : Decays::iTree_<PARTICLE> ( tree ) 
  , m_tree ( tree.m_tree -> clone() ) 
{}             
// ============================================================================
// assignement operator 
// ============================================================================
template <class PARTICLE>
Decays::Tree_<PARTICLE>& 
Decays::Tree_<PARTICLE>::operator=
( const Decays::Tree_<PARTICLE>& right ) // assignement 
{
  if ( &right == this ) { return *this ; } 
  Decays::iTree_<PARTICLE>* tree = right.m_tree->clone() ; 
  delete m_tree ; 
  m_tree = tree ;
  return *this ;
}
// ============================================================================
// assignement operator 
// ============================================================================
template <class PARTICLE>
Decays::Tree_<PARTICLE>& 
Decays::Tree_<PARTICLE>::operator=
( const Decays::iTree_<PARTICLE>& right ) // assignement 
{
  if ( &right == this ) { return *this ; } 
  typedef Decays::Tree_<PARTICLE> Self ;
  Decays::iTree_<PARTICLE>* tree = 0 ;
  const Self* self = dynamic_cast<const Self*>( &right ) ;  
  if ( 0 != self ) { tree = self->m_tree-> clone () ; }
  if ( 0 == tree ) { tree = right.         clone () ; }
  //
  delete m_tree ; 
  m_tree = tree ;
  //
  return *this ;
}
// ============================================================================
/** The standard output stream operator for the sub-tree
 *  @param s the reference to the output stream 
 *  @param o the decay tree objects 
 *  @return the reference to the output stream
 */
std::ostream& operator<<( std::ostream& s , const Decays::iTree& o ) ;
// ============================================================================
// The END
// ============================================================================
#endif // LOKI_ITREE_H
// ============================================================================
