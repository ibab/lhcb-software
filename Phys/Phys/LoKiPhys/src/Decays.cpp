// $Id: Decays.cpp,v 1.1 2008-06-12 08:25:27 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Decays.h"
#include "LoKi/Sections.h"
#include "LoKi/Child.h"
#include "LoKi/PhysAlgs.h"
// ============================================================================
/** @file
 *  Implementation file for LoKi MC-tree-functors 
 *  @author Vanya BELYAEV Ivan.BELYAEV@nikhef.nl
 *  @date   2008-05-25
 */
// ============================================================================

namespace 
{
  // ==========================================================================
  /** make the decay sections 
   *  @author Vanya BELYAEV Ivan.BELYAEV@nikhef.nl
   *  @date   2008-05-25
   */
  inline void makeSections 
  ( const LHCb::Particle*          p     , 
    const LoKi::Decays::Trees::Alg alg   , 
    LoKi::Decays::Sections&        sect  ) 
  {
    if ( 0 == p ) { return ; }
    //
    if ( LoKi::Decays::Trees::Sections == alg ) 
    { LoKi::Decays::sections ( p , sect ) ; }
    else 
    {
      LoKi::Decays::Section s ;
      LoKi::Decays::section ( p , s ) ;      
      sect.push_back ( s ) ;    
    }
  }
  // ==========================================================================
  struct NotPid : public std::unary_function<const LHCb::Particle*,bool>
  {
    /// constructor 
    NotPid ( const LoKi::Decays::Nodes::Pid& pid ) : m_pid ( pid ) {}
    // the major method: find "not a pid" 
    inline bool operator () ( const LHCb::Particle* p ) const 
    { return 0 != p && !m_pid.check ( p->particleID() ) ; }
  private:
    // ========================================================================
    /// the pid-checker 
    LoKi::Decays::Nodes::Pid m_pid ; //the pid-checker 
    // ========================================================================    
  } ;
  // ==========================================================================
  /** scan the sequence of particles to check that it containes ONLY 
   *  the partiles of the given PID 
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2008-05-26
   */
  template <class TYPE>
  inline bool onlyPid 
  (  TYPE begin , 
     TYPE end   , 
     const LoKi::Decays::Nodes::Pid& pid ) 
  {
    return end == std::find_if ( begin , end , NotPid ( pid ) ) ; 
  }
  // ==========================================================================
  /// the helper object tfor comparison 
  typedef LoKi::Decays::Trees::CheckTree<const LHCb::Particle*> Equal ;  
  // ==========================================================================
}
// ============================================================================
// EXCLUSIVE 
// ============================================================================
/*  constructor from the node (mother), subtrees and "final" flag
 *  @param mother the mother node 
 *  @param children the list of daughter substrees 
 *  @param alg the matching algorithm 
 */
// ============================================================================
LoKi::Decays::Trees::Exclusive::Exclusive
( const LoKi::Decays::iNode&                      mother   , 
  const LoKi::Decays::Trees::Exclusive::SubTrees& children ,
  const LoKi::Decays::Trees::Alg                  alg      )
  : LoKi::Decays::iTree_<const LHCb::Particle*> () 
  , m_mother     ( mother     )
  , m_children   ( children   ) 
  , m_alg        ( alg        )
{}
// ============================================================================
/*  constructor from the node (mother), subtrees and "final" flag
 *  @param mother the mother node 
 *  @param alg the matching algorithm 
 */
// ============================================================================
LoKi::Decays::Trees::Exclusive::Exclusive
( const LoKi::Decays::iNode&                        mother    , 
  const LoKi::Decays::Trees::Alg                    alg       ) 
  : LoKi::Decays::iTree_<const LHCb::Particle*> () 
  , m_mother     ( mother     )
  , m_children   (            ) 
  , m_alg        ( alg        )
{}
// ============================================================================
// MANDATORY: check the validity 
// ============================================================================
bool LoKi::Decays::Trees::Exclusive::valid() const 
{
  return m_mother.valid() && LoKi::Decays::valid ( childBegin() , childEnd() ) ;
}
// ============================================================================
// MANDATORY: validate the tree 
// ============================================================================
StatusCode  LoKi::Decays::Trees::Exclusive::validate 
( IParticlePropertySvc* svc ) const 
{
  StatusCode sc = m_mother.validate ( svc ) ;
  if ( sc.isFailure() ) { return sc ; }
  return LoKi::Decays::validate ( childBegin() , childEnd() , svc ) ;
}
// ============================================================================
// reset the collection cache
// ============================================================================
void LoKi::Decays::Trees::Exclusive::reset() const { i_reset () ; }
// ============================================================================
// collect the marked elements 
// ============================================================================
size_t LoKi::Decays::Trees::Exclusive::collect 
( LoKi::Decays::iTree_<const LHCb::Particle*>::Collection& output ) const 
{
  size_t size = 0 ;
  for ( SubTrees::const_iterator ichild = childBegin() ; 
        childEnd() != ichild ; ++ichild ) { size += ichild->collect ( output ) ; }  
  return size ;
}
// ============================================================================
// the only one essential method 
// ============================================================================
bool LoKi::Decays::Trees::Exclusive::operator() 
  ( LoKi::Decays::iTree_<const LHCb::Particle*>::argument p ) const
{
  // reset the cache (ALWAYS!!!)
  i_reset () ;
  // check the validness and oscillation criteria 
  if ( !ok ( p )                      ) { return false ; }        // RETURN 
  // check the mother node
  if ( !mother ( p -> particleID () ) ) { return false ; }        // RETURN 
  // perform the real matching:
  // (1) get the proper decay sections:
  LoKi::Decays::Sections sections ;
  makeSections ( p , alg() , sections ) ;
  // (2) loop over all sections
  for (  LoKi::Decays::Sections::iterator isect = sections.begin() ; 
         sections.end() != isect ; ++isect )
  {
    // (3) try to match the section 
    // skip the combinations  which does not match at all 
    if ( nChildren() != isect->size() ) { continue ; }
    // (4) sort the section 
    std::sort ( isect->begin() , isect->end() ) ;
    // make all possible permutations of the section  
    while ( std::next_permutation ( isect->begin() , isect->end() ) ) 
    {
      // (5) match all fields:
      if ( std::equal ( childBegin() , childEnd() , isect->begin() , Equal() ) ) 
      { return true ; }                                            // RETURN 
    } // next permutation
  } // next section
  // no match 
  return false ;                                                   // RETURN 
}
// ============================================================================
// MANDATORY: the specific printout 
// ============================================================================
std::ostream& 
LoKi::Decays::Trees::Exclusive::fillStream ( std::ostream& s ) const 
{
  //
  if ( 0 == nChildren() ) { return s << mother() ; }
  //
  s << " (" << mother() ;
  //
  switch ( this->alg() ) 
  {
  case Sections : s << "-->" ; break ;
  default       : s << "->"  ; break ;
  }
  //
  for ( SubTrees::const_iterator itree = childBegin() ; 
        childEnd() != itree ; ++itree ) { s << ( *itree ) ; }
  //
  return s << ") " ;
} 
// ============================================================================
// add one more node to the tree 
// ============================================================================
void LoKi::Decays::Trees::Exclusive::addDaughter 
( const LoKi::Decays::iTree_<const LHCb::Particle*>& tree ) 
{ m_children.push_back ( tree ) ; }
// ============================================================================
// add one more node to the tree 
// ============================================================================
void LoKi::Decays::Trees::Exclusive::addDaughter 
( const LoKi::Decays::iNode& node ){ addDaughter ( Exclusive ( node ) ) ; }
// ============================================================================
// add one more node to the tree 
// ============================================================================
LoKi::Decays::Trees::Exclusive&
LoKi::Decays::Trees::Exclusive::operator+=
( const LoKi::Decays::iTree_<const LHCb::Particle*>& tree ) 
{
  addDaughter ( tree ) ;
  return *this ;
}
// ============================================================================
// add one more node to the tree 
// ============================================================================
LoKi::Decays::Trees::Exclusive&
LoKi::Decays::Trees::Exclusive::operator+=
( const LoKi::Decays::iNode& node ) 
{
  addDaughter ( node ) ;
  return *this ;
}

// ============================================================================
// INCLUSIVE 
// ============================================================================
/*  constructor from the node (mother), subtrees and "final" flag
 *  @param mother the mother node 
 *  @param children the list of daughter substrees 
 *  @param alg the matching algorithm 
 */
// ============================================================================
LoKi::Decays::Trees::Inclusive::Inclusive
( const LoKi::Decays::iNode&                      mother   , 
  const LoKi::Decays::Trees::Exclusive::SubTrees& children ,
  const LoKi::Decays::Trees::Alg                  alg      ) 
  : LoKi::Decays::Trees::Exclusive ( mother , children , alg ) 
{}
// ============================================================================
/*  constructor from the node (mother), subtrees and "final" flag
 *  @param mother the mother node 
 *  @param alg the matching algorithm 
 */
// ============================================================================
LoKi::Decays::Trees::Inclusive::Inclusive
( const LoKi::Decays::iNode&                        mother , 
  const LoKi::Decays::Trees::Alg                    alg    ) 
  : LoKi::Decays::Trees::Exclusive( mother , alg ) 
{}
// ============================================================================
// MANDATORY: check the validity of the tree 
// ============================================================================
bool LoKi::Decays::Trees::Inclusive::valid() const 
{ return Exclusive::valid() && 0 != nChildren() ; }
// ============================================================================
// the only one essential method 
// ============================================================================
bool LoKi::Decays::Trees::Inclusive::operator() 
  ( LoKi::Decays::iTree_<const LHCb::Particle*>::argument p ) const
{
  // reset the cache (ALWAYS!!!)
  i_reset () ;
  // check the validness and oscillation criteria 
  if ( !ok ( p )                      ) { return false ; }        // RETURN 
  // check the mother node
  if ( !mother ( p -> particleID () ) ) { return false ; }        // RETURN 
  // perform the real matching:
  
  // (0) treat specifically the special case 
  
  // Only one inclusive particle and look through the whole tree
  if ( 1 == nChildren () )
  {
    LHCb::Particle::ConstVector d ;
    LoKi::Child::daughters ( p , d ) ;
    // in tree 
    if ( LoKi::Decays::Trees::Sections == alg() ) 
    {
      return LoKi::PhysAlgs::found ( d.begin() , d.end() , *childBegin() ) ;
    }
    // direct daughters :
    return d.end () != std::find_if ( d.begin() , d.end() , *childBegin() ) ;
  }
  
  // (1) get the proper decay sections:
  LoKi::Decays::Sections sections ;
  makeSections ( p , alg() , sections ) ;
  
  // (2) loop over all sections
  for (  LoKi::Decays::Sections::iterator isect = sections.begin() ; 
         sections.end() != isect ; ++isect )
  {
    // (3) try to match the section 
    // skip the combinations  which does not match at all 
    if ( nChildren() > isect->size() ) { continue ; }
    // (4) sort the section 
    std::sort ( isect->begin() , isect->end() ) ;
    // make all possible permutations of the section  
    while ( std::next_permutation ( isect->begin() , isect->end() ) ) 
    {
      // (5) match all fields:
      if ( std::equal ( childBegin() , childEnd() , isect->begin() , Equal() ) ) 
      { return true ; }                                            // RETURN 
    } // next permutation
  } // next section
  // no match 
  return false ;                                                   // RETURN 
}
// ============================================================================
// MANDATORY: the specific printout 
// ============================================================================
std::ostream& 
LoKi::Decays::Trees::Inclusive::fillStream ( std::ostream& s ) const 
{
  //
  s << " (" << mother () ;
  //
  switch ( this->alg() ) 
  {
  case Sections : s << "-->" ; break ;
  default       : s << "->"  ; break ;
  }
  //
  for ( SubTrees::const_iterator itree = childBegin() ; 
        childEnd() != itree ; ++itree ) { s << ( *itree ) ; }
  //
  return s << " ... ) " ;
}
// ============================================================================
// add one more node to the tree 
// ============================================================================
LoKi::Decays::Trees::Inclusive&
LoKi::Decays::Trees::Inclusive::operator+=
( const LoKi::Decays::iTree_<PARTICLE>& tree ) 
{
  addDaughter ( tree ) ;
  return *this ;
}
// ============================================================================
// add one more node to the tree 
// ============================================================================
LoKi::Decays::Trees::Inclusive&
LoKi::Decays::Trees::Inclusive::operator+=
( const LoKi::Decays::iNode& node ) 
{
  addDaughter ( node ) ;
  return *this ;
}
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
