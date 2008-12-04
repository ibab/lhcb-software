// $Id: MCDecays.cpp,v 1.3 2008-12-04 16:42:16 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/MCDecays.h"
#include "LoKi/MCSections.h"
#include "LoKi/MCChild.h"
#include "LoKi/MCAlgs.h"
#include "LoKi/Algs.h"
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
  ( const LHCb::MCParticle*        p     , 
    const Decays::Trees::Alg alg   , 
    const bool                     decay , 
    Decays::MCSections&      sect  ) 
  {
    if ( 0 == p ) { return ; }
    //
    if ( Decays::Trees::Sections == alg ) 
    { Decays::sections ( p , sect , decay ) ; }
    else 
    {
      Decays::MCSection s ;
      Decays::section ( p , s , decay ) ;      
      sect.push_back ( s ) ;    
    }
  }
  // ==========================================================================
  struct NotPid : public std::unary_function<const LHCb::MCParticle*,bool>
  {
    /// constructor 
    NotPid ( const Decays::Nodes::Pid& pid ) : m_pid ( pid ) {}
    // the major method: find "not a pid" 
    inline bool operator () ( const LHCb::MCParticle* p ) const 
    { return 0 != p && !m_pid.check ( p->particleID() ) ; }
  private:
    // ========================================================================
    /// the pid-checker 
    Decays::Nodes::Pid m_pid ; //the pid-checker 
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
     const Decays::Nodes::Pid& pid ) 
  {
    return end == std::find_if ( begin , end , NotPid ( pid ) ) ; 
  }
  // ==========================================================================
  /// the helper object tfor comparison 
  typedef Decays::Trees::CheckTree<const LHCb::MCParticle*> Equal ;  
  // ==========================================================================
  struct _Pid :  public std::unary_function<const LHCb::MCParticle*,bool>
  {
    /// constructor 
    _Pid ( const Decays::Nodes::Pid& pid ) : m_pid ( pid ) {}
    // ========================================================================
    /// the major method: find "not a pid" 
    inline bool operator () ( const LHCb::MCParticle* p ) const 
    { return 0 != p && m_pid.check ( p->particleID () ) ; }
    // ========================================================================
  private:
    // ========================================================================
    /// the pid-checker 
    Decays::Nodes::Pid m_pid ; //the pid-checker 
    // ========================================================================    
  } ;
  // ========================================================================== 
}
// ============================================================================
// MCEXCLUSIVE 
// ============================================================================
/*  constructor from the node (mother), subtrees and "final" flag
 *  @param mother the mother node 
 *  @param children the list of daughter substrees 
 *  @param alg the matching algorithm 
 *  @param decayOnly the flag to search only through decay products 
 *  @param oscillation check the oscilaltion flag
 */
// ============================================================================
Decays::Trees::MCExclusive::MCExclusive
( const Decays::iNode&                        mother     , 
  const Decays::Trees::MCExclusive::SubTrees& children   ,
  const Decays::Trees::Alg                    alg        , 
  const bool                                        decayOnly  , 
  const Decays::Trees::Oscillation            oscillation ) 
  : Decays::iTree_<const LHCb::MCParticle*> () 
  , m_mother      ( mother     )
  , m_children    ( children   ) 
  , m_alg         ( alg        )
  , m_decayOnly   ( decayOnly  )
  , m_oscillation ( oscillation ) 
{}
// ============================================================================
/*  constructor from the node (mother), subtrees and "final" flag
 *  @param mother the mother node 
 *  @param alg the matching algorithm 
 *  @param osc require the oscillation flag for mother 
 */
// ============================================================================
Decays::Trees::MCExclusive::MCExclusive
( const Decays::iNode&                        mother     , 
  const Decays::Trees::Alg                    alg        , 
  const bool                                        decayOnly  , 
  const Decays::Trees::Oscillation            oscillation ) 
  : Decays::iTree_<const LHCb::MCParticle*> () 
  , m_mother      ( mother     )
  , m_children    (            ) 
  , m_alg         ( alg        )
  , m_decayOnly   ( decayOnly  )
  , m_oscillation ( oscillation ) 
{}
// ============================================================================
/*  constructor from the node (mother), subtrees and "final" flag
 *  @param mother the mother node 
 *  @param children the list of daughter substrees 
 *  @param alg the matching algorithm 
 *  @param decayOnly the flag to search only through decay products 
 *  @param oscillation check the oscilaltion flag
 */
// ============================================================================
Decays::Trees::MCExclusive::MCExclusive
( const std::string&                                mother     , 
  const Decays::Trees::MCExclusive::SubTrees& children   ,
  const Decays::Trees::Alg                    alg        , 
  const bool                                        decayOnly  , 
  const Decays::Trees::Oscillation            oscillation ) 
  : Decays::iTree_<const LHCb::MCParticle*> () 
  , m_mother      ( Decays::Nodes::Pid ( mother ) )
  , m_children    ( children   ) 
  , m_alg         ( alg        )
  , m_decayOnly   ( decayOnly  )
  , m_oscillation ( oscillation ) 
{}
// ============================================================================
/*  constructor from the node (mother), subtrees and "final" flag
 *  @param mother the mother node 
 *  @param alg the matching algorithm 
 *  @param osc require the oscillation flag for mother 
 */
// ============================================================================
Decays::Trees::MCExclusive::MCExclusive
( const std::string&                                mother     , 
  const Decays::Trees::Alg                    alg        , 
  const bool                                        decayOnly  , 
  const Decays::Trees::Oscillation            oscillation ) 
  : Decays::iTree_<const LHCb::MCParticle*> () 
  , m_mother      ( Decays::Nodes::Pid ( mother ) )
  , m_children    (            ) 
  , m_alg         ( alg        )
  , m_decayOnly   ( decayOnly  )
  , m_oscillation ( oscillation ) 
{}
// ============================================================================
/*  constructor from the node (mother), subtrees and "final" flag
 *  @param mother the mother node 
 *  @param children the list of daughter substrees 
 *  @param alg the matching algorithm 
 *  @param decayOnly the flag to search only through decay products 
 *  @param oscillation check the oscilaltion flag
 */
// ============================================================================
Decays::Trees::MCExclusive::MCExclusive
( const LHCb::ParticleID&                           mother     , 
  const Decays::Trees::MCExclusive::SubTrees& children   ,
  const Decays::Trees::Alg                    alg        , 
  const bool                                        decayOnly  , 
  const Decays::Trees::Oscillation            oscillation ) 
  : Decays::iTree_<const LHCb::MCParticle*> () 
  , m_mother      ( Decays::Nodes::Pid ( mother ) )
  , m_children    ( children   ) 
  , m_alg         ( alg        )
  , m_decayOnly   ( decayOnly  )
  , m_oscillation ( oscillation ) 
{}
// ============================================================================
/*  constructor from the node (mother), subtrees and "final" flag
 *  @param mother the mother node 
 *  @param alg the matching algorithm 
 *  @param osc require the oscillation flag for mother 
 */
// ============================================================================
Decays::Trees::MCExclusive::MCExclusive
( const LHCb::ParticleID&                           mother     , 
  const Decays::Trees::Alg                    alg        , 
  const bool                                        decayOnly  , 
  const Decays::Trees::Oscillation            oscillation ) 
  : Decays::iTree_<const LHCb::MCParticle*> () 
  , m_mother      ( Decays::Nodes::Pid ( mother ) )
  , m_children    (             ) 
  , m_alg         ( alg         )
  , m_decayOnly   ( decayOnly   )
  , m_oscillation ( oscillation ) 
{}
// ============================================================================
/*  constructor from the decay 
 *  @param decay the decay        
 *  @param alg the matching algorithm 
 *  @param osc require the oscillation flag for mother 
 */
// ============================================================================
Decays::Trees::MCExclusive::MCExclusive
( const Decays::Decay&                                decay      , 
  const Decays::Trees::Alg                    alg        , 
  const bool                                        decayOnly  , 
  const Decays::Trees::Oscillation            oscillation ) 
  : Decays::iTree_<const LHCb::MCParticle*> () 
  , m_mother      ( Decays::Nodes::Pid ( decay.mother() ) )
  , m_children    (             ) 
  , m_alg         ( alg         )
  , m_decayOnly   ( decayOnly   )
  , m_oscillation ( oscillation ) 
{
  const Decays::Decay::Items& items = decay.children() ;
  for ( Decays::Decay::Items::const_iterator item = items.begin() ; 
        items.end() != item ; ++item ) { addDaughter ( *item ) ; } 
}
// ============================================================================
// copy constructor 
// ============================================================================
Decays::Trees::MCExclusive::MCExclusive
( const Decays::Trees::MCExclusive& right ) 
  : Decays::iTree_<const LHCb::MCParticle*> ( right ) 
  , m_mother      ( right.m_mother      )
  , m_children    ( right.m_children    ) 
  , m_alg         ( right.m_alg         )
  , m_decayOnly   ( right.m_decayOnly   )
  , m_oscillation ( right.m_oscillation ) 
{}

// ============================================================================
// MANDATORY: check the validity 
// ============================================================================
bool Decays::Trees::MCExclusive::valid() const 
{
  return m_mother.valid() && Decays::valid ( childBegin() , childEnd() ) ;
}
// ============================================================================
// MANDATORY: validate the tree 
// ============================================================================
StatusCode  Decays::Trees::MCExclusive::validate 
( const LHCb::IParticlePropertySvc* svc ) const 
{
  StatusCode sc = m_mother.validate ( svc ) ;
  if ( sc.isFailure() ) { return sc ; }
  return Decays::validate ( childBegin() , childEnd() , svc ) ;
}
// ============================================================================
// reset the collection cache
// ============================================================================
void Decays::Trees::MCExclusive::reset() const { i_reset () ; }
// ============================================================================
// collect the marked elements 
// ============================================================================
size_t Decays::Trees::MCExclusive::collect 
( Decays::iTree_<const LHCb::MCParticle*>::Collection& output ) const 
{
  size_t size = 0 ;
  for ( SubTrees::const_iterator ichild = childBegin() ; 
        childEnd() != ichild ; ++ichild ) { size += ichild->collect ( output ) ; }  
  return size ;
}
// ============================================================================
// the only one essential method 
// ============================================================================
bool Decays::Trees::MCExclusive::operator() 
  ( Decays::iTree_<const LHCb::MCParticle*>::argument p ) const
{
  // reset the cache (ALWAYS!!!)
  i_reset () ;
  // check the validness and oscillation criteria 
  if ( !ok ( p )                      ) { return false ; }        // RETURN 
  // perform the real matching:
  // match children? 
  if ( 0 == nChildren()               ) { return true  ; }        // RETURN
  // (1) get the proper decay sections:
  Decays::MCSections sections ;
  makeSections ( p , alg() , decayOnly() , sections ) ;
  // (2) loop over all sections
  for (  Decays::MCSections::iterator isect = sections.begin() ; 
         sections.end() != isect ; ++isect )
  {
    // (3) try to match the section 
    // skip the combinations  which does not match at all 
    if ( nChildren() != isect->size() ) { continue ; }
    // (4) sort the section 
    std::stable_sort ( isect->begin() , isect->end() ) ;
    do // make all possible permutations of the section  
    {
      // (5) match all fields:
      if ( std::equal ( childBegin() , childEnd() , isect->begin() , Equal() ) ) 
      { return true ; }                                            // RETURN 
    } // next permutation
    while ( std::next_permutation ( isect->begin() , isect->end() ) ) ;
  } // next section
  // no match 
  return false ;                                                   // RETURN 
}
// ============================================================================
// MANDATORY: the specific printout 
// ============================================================================
std::ostream& 
Decays::Trees::MCExclusive::fillStream ( std::ostream& s ) const 
{
  //
  s << ( ( 0 != nChildren() ) ? " (" : " " ) ;
  //
  switch ( oscillation () ) 
  {
  case Decays::Trees::Oscillated    : 
    s << "[" << mother () << "]os "  ; break ;  // BREAK 
  case Decays::Trees::NotOscillated : 
    s << "[" << mother () << "]nos " ; break ;  // BREAK 
  default:
    s <<        mother ()            ; 
  }
  //
  if ( 0 == nChildren() ) { return s ; }
  //
  switch ( this->alg() ) 
  {
  case Sections : s << ( decayOnly () ? "-->" : "--x>" ) ; break ;
  default       : s << ( decayOnly () ? " ->" : " -x>" ) ; break ;
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
void Decays::Trees::MCExclusive::addDaughter 
( const Decays::iTree_<const LHCb::MCParticle*>& tree ) 
{ m_children.push_back ( tree ) ; }
// ============================================================================
// add one more node to the tree 
// ============================================================================
void Decays::Trees::MCExclusive::addDaughter 
( const Decays::iNode& node ){ addDaughter ( MCExclusive ( node ) ) ; }
// ============================================================================
// add one more node to the tree 
// ============================================================================
void Decays::Trees::MCExclusive::addDaughter 
( const Decays::Decay::Item& node )
{ addDaughter ( Decays::Nodes::Pid ( node ) ) ; }
// ============================================================================
// add one more node to the tree 
// ============================================================================
void Decays::Trees::MCExclusive::addDaughter 
( const std::string& node )
{ addDaughter ( Decays::Nodes::Pid ( node ) ) ; }
// ============================================================================
// add one more node to the tree 
// ============================================================================
void Decays::Trees::MCExclusive::addDaughter 
( const LHCb::ParticleID& node )
{ addDaughter ( Decays::Nodes::Pid ( node ) ) ; }
// ============================================================================
// add one more node to the tree 
// ============================================================================
void Decays::Trees::MCExclusive::addDaughter 
( const LHCb::ParticleProperty* node )
{ addDaughter ( Decays::Nodes::Pid ( node ) ) ; }
// ============================================================================





// ============================================================================
// MCINCLUSIVE 
// ============================================================================
/*  constructor from the node (mother), subtrees and "final" flag
 *  @param mother the mother node 
 *  @param children the list of daughter substrees 
 *  @param alg the matching algorithm 
 *  @param decayOnly the flag to search only through decay products 
 *  @param oscillation check the oscilaltion flag
 */
// ============================================================================
Decays::Trees::MCInclusive::MCInclusive
( const Decays::iNode&                        mother     , 
  const Decays::Trees::MCExclusive::SubTrees& children   ,
  const Decays::Trees::Alg                    alg        , 
  const bool                                        decayOnly  , 
  const Decays::Trees::Oscillation            oscillation ) 
  : Decays::Trees::MCExclusive ( mother , children , 
                                       alg , decayOnly , oscillation ) 
{}
// ============================================================================
/*  constructor from the node (mother), subtrees and "final" flag
 *  @param mother the mother node 
 *  @param alg the matching algorithm 
 *  @param osc require the oscillation flag for mother 
 */
// ============================================================================
Decays::Trees::MCInclusive::MCInclusive
( const Decays::iNode&                        mother     , 
  const Decays::Trees::Alg                    alg        , 
  const bool                                        decayOnly  , 
  const Decays::Trees::Oscillation            oscillation ) 
  : Decays::Trees::MCExclusive( mother , alg , decayOnly , oscillation ) 
{}
// ============================================================================
/*  constructor from the node (mother), subtrees and "final" flag
 *  @param mother the mother node 
 *  @param children the list of daughter substrees 
 *  @param alg the matching algorithm 
 *  @param decayOnly the flag to search only through decay products 
 *  @param oscillation check the oscilaltion flag
 */
// ============================================================================
Decays::Trees::MCInclusive::MCInclusive
( const std::string&                                mother     , 
  const Decays::Trees::MCExclusive::SubTrees& children   ,
  const Decays::Trees::Alg                    alg        , 
  const bool                                        decayOnly  , 
  const Decays::Trees::Oscillation            oscillation ) 
  : Decays::Trees::MCExclusive ( mother , children , 
                                       alg , decayOnly , oscillation ) 
{}
// ============================================================================
/*  constructor from the node (mother), subtrees and "final" flag
 *  @param mother the mother node 
 *  @param alg the matching algorithm 
 *  @param osc require the oscillation flag for mother 
 */
// ============================================================================
Decays::Trees::MCInclusive::MCInclusive
( const std::string&                                mother     , 
  const Decays::Trees::Alg                    alg        , 
  const bool                                        decayOnly  , 
  const Decays::Trees::Oscillation            oscillation ) 
  : Decays::Trees::MCExclusive( mother , alg , decayOnly , oscillation ) 
{}

// ============================================================================
/*  constructor from the node (mother), subtrees and "final" flag
 *  @param mother the mother node 
 *  @param children the list of daughter substrees 
 *  @param alg the matching algorithm 
 *  @param decayOnly the flag to search only through decay products 
 *  @param oscillation check the oscilaltion flag
 */
// ============================================================================
Decays::Trees::MCInclusive::MCInclusive
( const LHCb::ParticleID&                           mother     , 
  const Decays::Trees::MCExclusive::SubTrees& children   ,
  const Decays::Trees::Alg                    alg        , 
  const bool                                        decayOnly  , 
  const Decays::Trees::Oscillation            oscillation ) 
  : Decays::Trees::MCExclusive ( mother , children , 
                                       alg , decayOnly , oscillation ) 
{}
// ============================================================================
/*  constructor from the node (mother), subtrees and "final" flag
 *  @param mother the mother node 
 *  @param alg the matching algorithm 
 *  @param osc require the oscillation flag for mother 
 */
// ============================================================================
Decays::Trees::MCInclusive::MCInclusive
( const LHCb::ParticleID&                           mother     , 
  const Decays::Trees::Alg                    alg        , 
  const bool                                        decayOnly  , 
  const Decays::Trees::Oscillation            oscillation ) 
  : Decays::Trees::MCExclusive( mother , alg , decayOnly , oscillation ) 
{}
// ============================================================================
/*  constructor from the decay 
 *  @param decay the decay        
 *  @param alg the matching algorithm 
 *  @param osc require the oscillation flag for mother 
 */
// ============================================================================
Decays::Trees::MCInclusive::MCInclusive
( const Decays::Decay&                                decay      , 
  const Decays::Trees::Alg                    alg        , 
  const bool                                        decayOnly  , 
  const Decays::Trees::Oscillation            oscillation ) 
  : Decays::Trees::MCExclusive( decay  , alg , decayOnly , oscillation ) 
{}
// ============================================================================
/*  constructor from the tree 
 */
// ============================================================================
Decays::Trees::MCInclusive::MCInclusive
( const Decays::Trees::MCExclusive& right ) 
  : Decays::Trees::MCExclusive( right ) 
{}
// ============================================================================
// copy constrcutor 
// ============================================================================
Decays::Trees::MCInclusive::MCInclusive
( const Decays::Trees::MCInclusive& right ) 
  : Decays::Trees::MCExclusive( right ) 
{}
// ============================================================================


// ============================================================================
// MANDATORY: check the validity of the tree 
// ============================================================================
bool Decays::Trees::MCInclusive::valid() const 
{ return MCExclusive::valid() && 0 != nChildren() ; }
// ============================================================================
// the only one essential method 
// ============================================================================
bool Decays::Trees::MCInclusive::operator() 
  ( Decays::iTree_<const LHCb::MCParticle*>::argument p ) const
{
  // reset the cache (ALWAYS!!!)
  i_reset () ;
  // check the validness and oscillation criteria 
  if ( !ok ( p )                      ) { return false ; }        // RETURN 
  // perform the real matching:
  
  // (0) treat specifically the special case 
  
  // Only one inclusive particle and look through the whole tree
  if ( 1 == nChildren () )
  {
    LHCb::MCParticle::ConstVector d ;
    LoKi::MCChild::daughters ( p , d , decayOnly () ) ;
    // in tree 
    if ( Decays::Trees::Sections == alg() ) 
    {
      return LoKi::MCAlgs::found 
        ( d.begin() , d.end() , *childBegin() , decayOnly() ) ;
    }
    // direct daughters :
    return d.end () != std::find_if ( d.begin() , d.end() , *childBegin() ) ;
  }
  
  // (1) get the proper decay sections:
  Decays::MCSections sections ;
  makeSections ( p , alg() , decayOnly() , sections ) ;
  
  // (2) loop over all sections
  for (  Decays::MCSections::iterator isect = sections.begin() ; 
         sections.end() != isect ; ++isect )
  {
    // (3) try to match the section 
    // skip the combinations  which does not match at all 
    if ( nChildren() > isect->size() ) { continue ; }
    
    // the special case of 2 elements  
    if      ( 2 == nChildren () ) 
    {
      if ( LoKi::Algs::found_2 
           ( isect -> begin () , 
             isect -> end   () , 
             front () , back () ) ) { return true ; }          // RETURN 
      continue ;                                               // CONTINUE 
    }
    else 
    {
      if ( LoKi::Algs::found_N 
           ( isect->begin  () , 
             isect->end    () , 
             children ()      ) ) { return true ; }            // RETURN 
      continue ;                                               // CONTINUE ;  
    }
    
    // never go further, unfortunately the most generic algorithm
    // it terribly slow... O(N!)
    
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
Decays::Trees::MCInclusive::fillStream ( std::ostream& s ) const 
{
  //
  s << " (" ;
  //
  switch ( oscillation () ) 
  {
  case Decays::Trees::Oscillated    : 
    s << "[" << mother () << "]os "  ; break ;  // BREAK 
  case Decays::Trees::NotOscillated : 
    s << "[" << mother () << "]nos " ; break ;  // BREAK 
  default:
    s <<        mother ()            ; 
  }
  //
  switch ( this->alg() ) 
  {
  case Sections : s << ( decayOnly () ? "-->" : "--x>" ) ; break ;
  default       : s << ( decayOnly () ? " ->" : " -x>" ) ; break ;
  }
  //
  for ( SubTrees::const_iterator itree = childBegin() ; 
        childEnd() != itree ; ++itree ) { s << ( *itree ) ; }
  //
  return s << " ... ) " ;
}


// ============================================================================
// MCOPTIONAL
// ============================================================================
Decays::Trees::MCOptional::MCOptional
( const Decays::iNode&                        mother     , 
  const Decays::Trees::MCExclusive::SubTrees& children   ,
  const Decays::Trees::MCExclusive::SubTrees& optional   ,
  const Alg                                         alg        ,
  const bool                                        decayOnly  , 
  const Decays::Trees::Oscillation            oscillation ) 
  : Decays::Trees::MCExclusive ( mother , children , alg , 
                                       decayOnly , oscillation )
  , m_optional ( optional ) 
{}
// ============================================================================
Decays::Trees::MCOptional::MCOptional
( const Decays::iNode&                           mother     , 
  const Decays::Trees::MCExclusive::SubTrees&    children   ,
  const Decays::iTree_<const LHCb::MCParticle*>& optional   ,
  const Alg                                            alg        ,
  const bool                                           decayOnly  , 
  const Decays::Trees::Oscillation               oscillation ) 
  : Decays::Trees::MCExclusive ( mother , children , alg , 
                                       decayOnly , oscillation )
  , m_optional ( 1 , optional ) 
{}
// ============================================================================
Decays::Trees::MCOptional::MCOptional
( const Decays::iNode&                           mother     , 
  const Decays::Trees::MCExclusive::SubTrees&    children   ,
  const Decays::iNode&                           optional   ,
  const Alg                                            alg        ,
  const bool                                           decayOnly  , 
  const Decays::Trees::Oscillation               oscillation ) 
  : Decays::Trees::MCExclusive ( mother , children , alg , 
                                       decayOnly , oscillation )
  , m_optional ( 1 , MCExclusive ( optional ) ) 
{}
// ============================================================================
Decays::Trees::MCOptional::MCOptional
( const std::string&                                mother     , 
  const Decays::Trees::MCExclusive::SubTrees& children   ,
  const Decays::Trees::MCExclusive::SubTrees& optional   ,
  const Alg                                         alg        ,
  const bool                                        decayOnly  , 
  const Decays::Trees::Oscillation            oscillation ) 
  : Decays::Trees::MCExclusive ( mother , children , alg , 
                                       decayOnly , oscillation )
  , m_optional ( optional ) 
{}
// ============================================================================
Decays::Trees::MCOptional::MCOptional
( const std::string&                                   mother     , 
  const Decays::Trees::MCExclusive::SubTrees&    children   ,
  const Decays::iTree_<const LHCb::MCParticle*>& optional   ,
  const Alg                                            alg        ,
  const bool                                           decayOnly  , 
  const Decays::Trees::Oscillation               oscillation ) 
  : Decays::Trees::MCExclusive ( mother , children , alg , 
                                       decayOnly , oscillation )
  , m_optional ( 1 , optional ) 
{}
// ============================================================================
Decays::Trees::MCOptional::MCOptional
( const std::string&                                   mother     , 
  const Decays::Trees::MCExclusive::SubTrees&    children   ,
  const Decays::iNode&                           optional   ,
  const Alg                                            alg        ,
  const bool                                           decayOnly  , 
  const Decays::Trees::Oscillation               oscillation ) 
  : Decays::Trees::MCExclusive ( mother , children , alg , 
                                       decayOnly , oscillation )
  , m_optional ( 1 , MCExclusive ( optional ) ) 
{}
// ============================================================================
Decays::Trees::MCOptional::MCOptional
( const Decays::Trees::MCExclusive&           right    ,
  const Decays::Trees::MCExclusive::SubTrees& optional )
  : Decays::Trees::MCExclusive ( right ) 
  , m_optional ( optional ) 
{}
// ============================================================================
Decays::Trees::MCOptional::MCOptional
( const Decays::Trees::MCExclusive& right    ,
  const std::vector<std::string>&         optional )
  : Decays::Trees::MCExclusive ( right ) 
  , m_optional () 
{ 
  setOptional ( optional ) ; 
}
// ============================================================================
Decays::Trees::MCOptional::MCOptional
( const Decays::Trees::MCExclusive& right ) 
  : Decays::Trees::MCExclusive ( right ) 
  , m_optional () 
{}

// ============================================================================
// MANDATORY: check the validity of the tree 
// ============================================================================
bool Decays::Trees::MCOptional::valid() const 
{
  return MCExclusive::valid() && 0 != nChildren() && 0 != nOptional() && 
    Decays::valid ( optBegin( ), optEnd() ) ;
}
// ============================================================================
// MANDATORY: the proper validation of the tree
// ============================================================================
StatusCode Decays::Trees::MCOptional::validate 
( const LHCb::IParticlePropertySvc* svc ) const 
{ 
  StatusCode sc = MCExclusive::validate ( svc ) ;
  return sc.isFailure() ? sc :
    Decays::validate ( optBegin() , optEnd() , svc ) ;
}
// ============================================================================
// MANDATORY: the specific printout 
// ============================================================================
std::ostream& Decays::Trees::MCOptional::fillStream 
( std::ostream& s ) const 
{
  //
  s << " (" ;
  //
  switch ( oscillation () ) 
  {
  case Decays::Trees::Oscillated    : 
    s << "[" << mother () << "]os "  ; break ;  // BREAK 
  case Decays::Trees::NotOscillated : 
    s << "[" << mother () << "]nos " ; break ;  // BREAK 
  default:
    s <<        mother ()            ; 
  }
  //
  switch ( this->alg() ) 
  {
  case Sections : s << ( decayOnly () ? "-->" : "--x>" ) ; break ;
  default       : s << ( decayOnly () ? " ->" : " -x>" ) ; break ;
  }
  //
  for ( SubTrees::const_iterator itree = childBegin() ;
        childEnd() != itree ; ++itree ) 
  { s << ( *itree ) ; }
  //
  for ( SubTrees::const_iterator itree = optBegin() ; 
        optEnd () != itree ; ++itree ) 
  { s << " {" << ( *itree ) << "} "; }
  //
  return s << ") " ;
} 
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
bool Decays::Trees::MCOptional::operator() 
  ( Decays::iTree_<const LHCb::MCParticle*>::argument p ) const 
{
  // reset the cache (ALWAYS!!!)
  i_reset () ;
  // check the validness and oscillation criteria 
  if ( !ok ( p )                      ) { return false ; }        // RETURN 
  // perform the real matching:
  // (1) get the proper decay sections:
  Decays::MCSections sections ;
  makeSections ( p , alg() , decayOnly() , sections ) ;
  // (2) loop over all sections
  for (  Decays::MCSections::iterator isect = sections.begin() ; 
         sections.end() != isect ; ++isect )
  {
    // (3) try to match the section 
    // skip the combinations  which does not match at all 
    if ( isect->size() < nChildren ()                ) { continue ; } // CONITNUE
    if ( isect->size() > nChildren () + nOptional () ) { continue ; } // CONTINUE
    // (4) sort the section 
    std::stable_sort ( isect->begin() , isect->end() ) ;
    // (5) check "inclusive"
    if ( !LoKi::Algs::found_N 
         ( isect->begin (), 
           isect->end   (), 
           children() )   ) { continue ; }                        // CONTINUE
    // (6) make all possible permutations:
    do // make all possible permutations:
    {
      // (7) match all declared mandatory fields:
      if ( std::equal ( childBegin() , childEnd() , isect->begin() , Equal() ) )
      {
        // the temporary iterator 
        Decays::MCSection::iterator aux = isect->begin() + nChildren() ;
        // create the local container for permutations 
        SubTrees opt ( m_optional ) ;
        std::stable_sort ( opt.begin () , opt.end() ) ; // sort it!!!
        // start all possible permutations of the optional stuff 
        do // start all possible permutations of the optional stuff 
        {
          if ( std::equal ( aux , isect->end() , opt.begin() , Equal() ) ) 
          { return true ; }                                       // RETURN  
        } // next permutation for optional stuff 
        while ( std::next_permutation ( opt.begin() , opt.end () ) ) ;
      } // match mandatory stuff 
    } // next permutation
    while ( std::next_permutation ( isect->begin() , isect->end() ) ) ;
  } // next section
  // no match 
  return false ; // RETURN 
}
// ============================================================================
// add one more daughter to the decay 
// ============================================================================
void Decays::Trees::MCOptional::addOptional 
( const Decays::iTree_<PARTICLE>& tree ) 
{ m_optional.push_back ( tree ) ; }
// ============================================================================
// add one more daughter to the decay 
// ============================================================================
void Decays::Trees::MCOptional::addOptional 
( const Decays::iNode& node ) 
{ m_optional.push_back ( MCExclusive ( node ) ) ; }
// ============================================================================
// add one more daughter to the decay 
// ============================================================================
void Decays::Trees::MCOptional::addOptional 
( const Decays::Decay::Item& node ) 
{ addOptional ( Decays::Nodes::Pid ( node ) ) ; }
// ============================================================================
// add one more daughter to the decay 
// ============================================================================
void Decays::Trees::MCOptional::addOptional 
( const std::string& node ) 
{ addOptional ( Decays::Nodes::Pid ( node ) ) ; }
// ============================================================================
// add one more daughter to the decay 
// ============================================================================
void Decays::Trees::MCOptional::addOptional 
( const LHCb::ParticleID& node ) 
{ addOptional ( Decays::Nodes::Pid ( node ) ) ; }
// ============================================================================
// add one more daughter to the decay 
// ============================================================================
void Decays::Trees::MCOptional::addOptional 
( const LHCb::ParticleProperty* node ) 
{ addOptional ( Decays::Nodes::Pid ( node ) ) ; }
// ============================================================================
void Decays::Trees::MCOptional::setOptional 
( const std::vector<std::string>& optional ) 
{
  m_optional.clear() ;
  for ( std::vector<std::string>::const_iterator item = optional.begin() ;
        optional.end() != item ; ++item ) { addOptional( *item ) ; }
}
// ============================================================================
void Decays::Trees::MCOptional::setOptional 
( const std::vector<LHCb::ParticleID>& optional ) 
{
  m_optional.clear() ;
  for ( std::vector<LHCb::ParticleID>::const_iterator item = optional.begin() ;
        optional.end() != item ; ++item ) { addOptional( *item ) ; }
}




// ============================================================================
// PHOTOS 
// ============================================================================
// constructor from the node (mother) and subtrees 
// ============================================================================
Decays::Trees::Photos::Photos
( const Decays::iNode&                   mother     , 
  const Decays::Trees::Photos::SubTrees& children   ,
  const Decays::Trees::Alg               alg        , 
  const bool                                   decayOnly  ,
  const Decays::Trees::Oscillation       oscillation ) 
  : Decays::Trees::MCExclusive ( mother    , children , alg , 
                                       decayOnly , oscillation ) 
  , m_photon    ( "gamma"   )
{}
// ============================================================================
// constructor from the node (mother) and subtrees 
// ============================================================================
Decays::Trees::Photos::Photos
( const std::string&                           mother     , 
  const Decays::Trees::Photos::SubTrees& children   ,
  const Decays::Trees::Alg               alg        , 
  const bool                                   decayOnly  ,
  const Decays::Trees::Oscillation       oscillation ) 
  : Decays::Trees::MCExclusive ( mother    , children , alg , 
                                       decayOnly , oscillation ) 
  , m_photon    ( "gamma"   )
{}
// ============================================================================
// constructor from the node (mother) and subtrees 
// ============================================================================
Decays::Trees::Photos::Photos
( const LHCb::ParticleID&                      mother     , 
  const Decays::Trees::Photos::SubTrees& children   ,
  const Decays::Trees::Alg               alg        , 
  const bool                                   decayOnly  ,
  const Decays::Trees::Oscillation       oscillation ) 
  : Decays::Trees::MCExclusive ( mother    , children , alg , 
                                       decayOnly , oscillation ) 
    , m_photon    ( "gamma"   )
{}
// ============================================================================
// constructor from the decay
// ============================================================================
Decays::Trees::Photos::Photos
( const Decays::Decay&                   decay      , 
  const Decays::Trees::Alg               alg        , 
  const bool                             decayOnly  ,
  const Decays::Trees::Oscillation       oscillation ) 
  : Decays::Trees::MCExclusive ( decay , alg , decayOnly , oscillation ) 
  , m_photon    ( "gamma"   )
{}
// ============================================================================
// constructor from the decay
// ============================================================================
Decays::Trees::Photos::Photos
( const  Decays::Trees::MCExclusive& right ) 
  : Decays::Trees::MCExclusive  ( right ) 
  , m_photon    ( "gamma"   )
{}





// ============================================================================
// MANDATORY: the proper validation of the tree
// ============================================================================
StatusCode Decays::Trees::Photos::validate 
( const LHCb::IParticlePropertySvc* svc ) const 
{
  StatusCode sc = m_photon.validate ( svc ) ;
  return sc.isFailure() ? sc : MCExclusive::validate ( svc ) ;  // RETURN 
}
// ============================================================================
// MANDATORY: the specific printout 
// ============================================================================
std::ostream& Decays::Trees::Photos::fillStream ( std::ostream& s ) const 
{
  //
  s << " (" ;
  //
  switch ( oscillation () ) 
  {
  case Decays::Trees::Oscillated    : 
    s << "[" << mother () << "]os "  ; break ;  // BREAK 
  case Decays::Trees::NotOscillated : 
    s << "[" << mother () << "]nos " ; break ;  // BREAK 
  default:
    s <<        mother ()            ; 
  }
  //
  switch ( this->alg() ) 
  {
  case Sections : s << ( decayOnly () ? "==>" : "==x>" ) ; break ;
  default       : s << ( decayOnly () ? " =>" : " =x>" ) ; break ;
  }
  //
  for ( SubTrees::const_iterator itree = childBegin() ; 
        childEnd() != itree ; ++itree ) { s << ( *itree ) ; }
  //
  return s << ") " ;
} 
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
bool Decays::Trees::Photos::operator() 
  ( Decays::iTree_<const LHCb::MCParticle*>::argument p ) const 
{
  // reset the cache (ALWAYS!!!)
  i_reset () ;
  // check the validness and oscillation criteria 
  if ( !ok ( p )                      ) { return false ; }        // RETURN 
  // perform the real matching:
  // (1) get the proper decay sections:
  Decays::MCSections sections ;
  makeSections ( p , alg() , decayOnly() , sections ) ;
  // (2) loop over all sections
  for (  Decays::MCSections::iterator isect = sections.begin() ; 
         sections.end() != isect ; ++isect )
  {
    // (3) try to match the section 
    // skip the combinations  which does not match at all 
    if ( nChildren() > isect->size() ) { continue ; }                  // CONTINUE 
    // (4) count photons
    const size_t nGamma = 
      std::count_if ( isect->begin() , isect->end() , _Pid(m_photon) ) ;
    if ( nChildren () + nGamma < isect->size() ) { continue ; }        // CONTINUE 
    // (4) sort the section
    std::stable_sort ( isect->begin() , isect->end() ) ;
    // (6) check "inclusive"
    if ( !LoKi::Algs::found_N 
         ( isect -> begin () , 
           isect -> end   () , 
           children()        ) ) { continue ; }                        // CONTINUE 
    // (5) make all possible permutations:
    do 
    {
      // (6) match all declared mandatory fields:
      if ( std::equal ( childBegin() , childEnd() , isect->begin() , Equal() )
           && // the rest is only photons 
           onlyPid (  isect->begin () + nChildren () , 
                      isect->end   () , m_photon     ) ) { return true ; }
    } // next permutation
    while ( std::next_permutation ( isect->begin() , isect->end() ) ) ;
  } // next section
  // no match 
  return false ; // RETURN 
}


// ============================================================================
// PHOTOSOPTIONAL
// ============================================================================
Decays::Trees::PhotosOptional::PhotosOptional
( const Decays::iNode&                        mother     , 
  const Decays::Trees::MCExclusive::SubTrees& children   ,
  const Decays::Trees::MCExclusive::SubTrees& optional   ,
  const Decays::Trees::Alg                    alg        ,
  const bool                                        decayOnly  , 
  const Decays::Trees::Oscillation            oscillation ) 
  : Decays::Trees::MCOptional ( mother , children , optional , alg , decayOnly , oscillation )
  , m_photon ( "gamma")  
{}
// ============================================================================
Decays::Trees::PhotosOptional::PhotosOptional
( const Decays::iNode&                        mother     , 
  const Decays::Trees::MCExclusive::SubTrees& children   ,
  const Decays::iNode&                        optional   ,
  const Decays::Trees::Alg                    alg        ,
  const bool                                        decayOnly  , 
  const Decays::Trees::Oscillation            oscillation ) 
  : Decays::Trees::MCOptional ( mother , children , optional , alg , decayOnly , oscillation )
    , m_photon ( "gamma")  
{}
// ============================================================================
Decays::Trees::PhotosOptional::PhotosOptional
( const Decays::iNode&                           mother     , 
  const Decays::Trees::MCExclusive::SubTrees&    children   ,
  const Decays::iTree_<const LHCb::MCParticle*>& optional   ,
  const Decays::Trees::Alg                       alg        ,
  const bool                                           decayOnly  , 
  const Decays::Trees::Oscillation               oscillation ) 
  : Decays::Trees::MCOptional ( mother , children , optional , alg , decayOnly , oscillation )
  , m_photon ( "gamma")  
{}
// ============================================================================
Decays::Trees::PhotosOptional::PhotosOptional
( const Decays::Trees::MCOptional& right ) 
  : Decays::Trees::MCOptional ( right )
  , m_photon ( "gamma")  
{}
// ============================================================================
Decays::Trees::PhotosOptional::PhotosOptional
( const Decays::Trees::MCExclusive&           right    ,   
  const Decays::Trees::MCExclusive::SubTrees& optional ) 
  : Decays::Trees::MCOptional ( right , optional )
  , m_photon ( "gamma")  
{}
// ============================================================================
Decays::Trees::PhotosOptional::PhotosOptional
( const Decays::Trees::MCExclusive& right    ,   
  const std::vector<std::string>&         optional ) 
  : Decays::Trees::MCOptional ( right , optional )
    , m_photon ( "gamma")  
{}


// ============================================================================
// MANDATORY: check the validity of the tree 
// ============================================================================
bool Decays::Trees::PhotosOptional::valid() const 
{ return MCOptional::valid() && m_photon.valid() ; }
// ============================================================================
// MANDATORY: the proper validation of the tree
// ============================================================================
StatusCode Decays::Trees::PhotosOptional::validate 
( const LHCb::IParticlePropertySvc* svc ) const 
{ 
  StatusCode sc = m_photon.validate ( svc ) ;
  return sc.isFailure() ? sc : MCOptional::validate ( svc ) ;
}
// ============================================================================
// MANDATORY: the specific printout 
// ============================================================================
std::ostream& Decays::Trees::PhotosOptional::fillStream 
( std::ostream& s ) const 
{
  //
  s << " (";
  //
  switch ( oscillation () ) 
  {
  case Decays::Trees::Oscillated    : 
    s << "[" << mother () << "]os "  ; break ;  // BREAK 
  case Decays::Trees::NotOscillated : 
    s << "[" << mother () << "]nos " ; break ;  // BREAK 
  default:
    s <<        mother ()            ; 
  }
  //
  switch ( this->alg() ) 
  {
  case Sections : s << ( decayOnly () ? "==>" : "==x>" ) ; break ;
  default       : s << ( decayOnly () ? "=>"  : "=x>"  ) ; break ;
  }
  //
  for ( SubTrees::const_iterator itree = childBegin() ; 
        childEnd() != itree ; ++itree ) 
  { s << ( *itree ) ; }
  //
  for ( SubTrees::const_iterator itree = optBegin() ; 
        optEnd () != itree ; ++itree ) 
  { s << " {" << ( *itree ) << "} "; }
  //
  return s << ") " ;
} 
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
bool Decays::Trees::PhotosOptional::operator() 
  ( Decays::iTree_<const LHCb::MCParticle*>::argument p ) const 
{
  // reset the cache (ALWAYS!!!)
  i_reset () ;
  // check the validness and oscillation criteria 
  if ( !ok ( p )                      ) { return false ; }        // RETURN 
  // perform the real matching:
  // (1) get the proper decay sections:
  Decays::MCSections sections ;
  makeSections ( p , alg() , decayOnly() , sections ) ;
  // (2) loop over all sections
  for (  Decays::MCSections::iterator isect = sections.begin() ; 
         sections.end() != isect ; ++isect )
  {
    // (3) try to match the section 
    // skip the combinations  which does not match at all 
    if ( nChildren() > isect->size() ) { continue ; }
    // (4) check "inclusive"
    if ( !LoKi::Algs::found_N 
         ( isect->begin (), 
           isect->end   (), 
           children() )   ) { continue ; }                        // CONTINUE
    // (5) sort the section 
    std::stable_sort ( isect->begin() , isect->end() ) ;
    // (6) make all possible permutations:
    do //  make all possible permutations:
    {      
      // (7) match all declared mandatory fields:
      if ( std::equal ( childBegin() , childEnd() , isect->begin() , Equal() ) )
      {
        // the temporary iterator 
        Decays::MCSection::iterator aux = isect->begin() + nChildren() ;
        // nothing to match? 
        if ( aux == isect->end() ) { return true ; } // RETURN
        // create the local container for permutations 
        SubTrees opt ( optBegin() , optEnd() ) ;
        std::stable_sort ( opt.begin () , opt.end() ) ; // sort it!!!
        // make the nesessary permutations 
        do 
        {                                     // make the nesessary permutations 
          for ( SubTrees::iterator it = opt.begin() ; opt.end() != it ; ++it )
          {
            // select only the subsequence of the right length 
            if ( it - opt.begin() <= isect->end() - aux ) 
            {
              // match the subsequence 
              if ( std::equal ( opt.begin() , it , aux , Equal() ) 
                   && // the rest is only photons 
                   onlyPid ( aux + opt.size() , 
                             isect->end    () , m_photon ) ) { return true ; }
            }
            // try to match the back-end of the sequence (save time?)
            if ( opt.end() - it <= isect->end() - aux ) 
            {
              // match the subsequence 
              if ( std::equal ( it , opt.end() , aux , Equal() ) 
                   && // the rest is only photons 
                   onlyPid ( aux + opt.size() , 
                             isect->end    () , m_photon ) ) { return true ; }
            } // match the optional stuff 
          } // the length of the optional stuff 
        } // permutations over the optional stuff
        while ( std::next_permutation ( opt.begin() , opt.end() ) ) ;
      } // match the mandatory stuff
    } // next permutation for mandatory stuff 
    while ( std::next_permutation ( isect->begin() , isect->end() ) ) ;
  } // next section
  // no match 
  return false ; // RETURN 
}
// ============================================================================
/*  operator for easy construction of trees 
 *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
 *  @date 2008-07-07
 */
// ============================================================================
Decays::Trees::And_<const LHCb::MCParticle*>
operator&& 
( const Decays::iTree_<const LHCb::MCParticle*>& t , 
  const Decays::iNode&                           n ) 
{ return t && Decays::Trees::MCExclusive ( n ) ; }
// ============================================================================
/*  operator for easy construction of trees 
 *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
 *  @date 2008-07-07
 */
// ============================================================================
Decays::Trees::And_<const LHCb::MCParticle*>
operator&& 
( const Decays::iTree_<const LHCb::MCParticle*>& t , 
  const std::string&                                   n ) 
{ return t && Decays::Trees::MCExclusive ( n ) ; }
// ============================================================================
/*  operator for the easy construction of trees 
 *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
 *  @date 2008-07-07
 */
// ============================================================================
Decays::Trees::And_<const LHCb::MCParticle*>
operator&& 
( const Decays::iTree_<const LHCb::MCParticle*>& t , 
  const LHCb::ParticleID&                              n ) 
{ return t && Decays::Trees::MCExclusive ( n ) ; }
// ============================================================================
/*  operator for easy construction of trees 
 *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
 *  @date 2008-07-07
 */
// ============================================================================
Decays::Trees::Or_<const LHCb::MCParticle*>
operator|| 
( const Decays::iTree_<const LHCb::MCParticle*>& t , 
  const Decays::iNode&                           n ) 
{ return t || Decays::Trees::MCExclusive ( n ) ; }
// ============================================================================
/*  operator for easy construction of trees 
 *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
 *  @date 2008-07-07
 */
// ============================================================================
Decays::Trees::Or_<const LHCb::MCParticle*>
operator||
( const Decays::iTree_<const LHCb::MCParticle*>& t , 
  const std::string&                                   n ) 
{ return t || Decays::Trees::MCExclusive ( n ) ; }
// ============================================================================
/*  operator for the easy construction of trees 
 *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
 *  @date 2008-07-07
 */
// ============================================================================
Decays::Trees::Or_<const LHCb::MCParticle*>
operator||
( const Decays::iTree_<const LHCb::MCParticle*>& t , 
  const LHCb::ParticleID&                              n ) 
{ return t || Decays::Trees::MCExclusive ( n ) ; }
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
