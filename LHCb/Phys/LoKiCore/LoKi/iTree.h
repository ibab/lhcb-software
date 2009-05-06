// $Id: iTree.h,v 1.7 2009-05-06 20:20:01 ibelyaev Exp $
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
    /// reset the collection cache 
    virtual void reset() const = 0 ;
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
    /// get the underlying tree 
    const Decays::iTree_<PARTICLE>& tree () const { return *m_tree ; }
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
 *  @param s the reference to the outptu stream 
 *  @param o the decay tree objects 
 *  @return the reference to the outptu stream
 */
template <class PARTICLE>
inline std::ostream& operator<<
  ( std::ostream&                   s , 
    const Decays::iTree_<PARTICLE>& o ) { return o.fillStream ( s ) ; }
// ============================================================================
namespace Decays 
{
  // ==========================================================================
  namespace Trees 
  {
    // ========================================================================
    /** the type of maching algorithm:
     *
     *   - <c>Daughters</c> match within (all) direct daughetrs 
     *   - <c>Sections</c>  match within all sections of the decay graph 
     *
     *  For C++ classes the nmatchiung algorithm is defined through 
     *  the length of the arrow:
     *   
     *   - <c>Daughters</c> are defined through "short" arrow:
     *   - <c>Sections</c> are defined through "long" arrow:
     *
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
     *
     *   - <c>Undefined</c> : no indication are requred 
     *   - <c>Oscillated</c>: 
     *        <c>[ A ]os -> X Y Z </c>, for (MC- and HepMC-decays only)  
     *   - <c>NotOscillated</c>: 
     *        <c>[ A ]nos -> X Y Z </c>, for (MC- and HepMC-decays only)  
     *
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
    // ========================================================================
    /** @enum Arrow 
     *  simple enum to mark varios type of arrows for decay descriptors 
     *  - "->"  simple decay match 
     *  - "-->" match decay in sections 
     *  - "->"  simple decay match ignoring photons  
     *  - "-->" match decay in sections innoring photons 
     *  - "-x>"  simple decay match, ignoring secondary interactions  
     *  - "-->" match decay in sections, ignoring secondary interactions  
     *  - "->"  simple decay match ignoring photons and secondary interactions   
     *  - "-->" match decay in sections innoring photons and secondary interactions
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2009-05-06 
     */     
    enum Arrow {
      /// 
      Single       ,   // single arrow        "->" 
      LongSingle   ,   // long single arrow  "-->" 
      Double       ,   // dobule arrow        "=>" 
      LongDouble   ,   // long double arrow  "==>"
      SingleX      ,   // single arrow       "-x>" 
      LongSingleX  ,   // long single arrow "--x>" 
      DoubleX      ,   // dobule arrow       "=x>" 
      LongDoubleX  ,   // long double arrow "==x>"
      // =====================================================================
    } ;
    // ======================================================================
    /** simple function to extract "decay-only" flag from the arrow 
     *  @param   arrow the arrow
     *  @return  "decay-only" flag 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2009-05-06 
     */     
    bool decayOnly ( const Arrow& arrow ) ;
    // ========================================================================
    /** simple function to extract "algorithm" flag from the arrow 
     *  @param   arrow the arrow
     *  @return  "algorithm" flag 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2009-05-06 
     */     
    Alg  algorithm ( const Arrow& arrow ) ;
    // ========================================================================
    /** simple function to extract "photos" flag from the arrow 
     *  @param   arrow the arrow
     *  @return  "photos" flag  
     *  @see Decays::Trees:Arrow 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2009-05-06 
     */     
    bool photos    ( const Arrow& arrow ) ; 
    /** get the string representation of the arrow 
     *  @param    arr the arrow
     *  @return  strnig representation of the arrow
     *  @see Decays::Trees:Arrow 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2009-05-06 
     */     
    std::string arrow ( const Arrow& arr ) ;
    // ========================================================================
    /** valid arrow? 
     *  @param   arrow the arrow
     *  @return  true for valid arrow 
     *  @see Decays::Trees:Arrow 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2009-05-06 
     */     
    bool valid ( const Arrow& arrow ) ;
    // ========================================================================
    /** valid arrow? 
     *  @param   arrow the arrow
     *  @return  true for valid arrow 
     *  @see Decays::Trees:Arrow 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2009-05-06 
     */     
    bool valid ( const std::string& arrow ) ;
    // ========================================================================
  } // end of namespace Decays::Trees 
  // ==========================================================================
} // end of namespace Decays
// ============================================================================
// The END
// ============================================================================
#endif // LOKI_ITREE_H
// ============================================================================
