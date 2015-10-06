// $Id$
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
#include "LoKi/GetN.h"
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
  /// the name of photon for PHOTOS 
  const std::string s_GAMMA = "gamma" ; 
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
    /// the major method: find "a pid" 
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
  /// the actual matching function
  bool match 
  ( Decays::MCSection::iterator                          first , 
    Decays::MCSection::iterator                          last  , 
    Decays::Trees::MCExclusive::TreeList::const_iterator begin ,
    Decays::Trees::MCExclusive::TreeList::const_iterator end   ) 
  {
    //
    if ( std::distance ( first , last ) != std::distance ( begin , end ) ) 
    { return false ; }
    //
    //    (1) sort the sections 
    std::stable_sort ( first , last ) ;
    do // (2) make all possible permutations of the section  
    {  // (3) match all fields:
      if ( std::equal ( begin , end , first , Equal () ) ) { return true ; } 
    } //  (4) next permutation
    while ( std::next_permutation ( first , last ) ) ;
    //
    return false ;
  }
  // ==========================================================================
} //                                                 end of anonymous namespace 
// ============================================================================
// MCEXCLUSIVE 
// ============================================================================
/*  full constructor from the node (mother), subtrees and "final" flag
 *  @param mother the mother node 
 *  @param children the list of daughter substrees 
 *  @param alg the matching algorithm 
 *  @param decayOnly the flag to search only through decay products 
 *  @param oscillation check the oscilaltion flag
 */
// ============================================================================
Decays::Trees::MCExclusive::MCExclusive
( const Decays::iNode&                        mother     , 
  const Decays::Trees::MCExclusive::TreeList& children   ,
  const Decays::Trees::Alg                    alg        , 
  const bool                                  decayOnly  , 
  const Decays::Trees::Oscillation            oscillation ) 
  : Decays::iTree_<const LHCb::MCParticle*> () 
  , m_mother      ( mother     )
  , m_children    ( children   ) 
  , m_alg         ( alg        )
  , m_decayOnly   ( decayOnly  )
  , m_oscillation ( oscillation ) 
{}
// ============================================================================
/*  constructor from the node (mother) and flags
 *  @param mother the mother node
 *  @param alg the matching algorithm
 *  @param decayOnly the flag to search only through decay products
 *  @param oscillated check the oscilaltion flag
 */
// ============================================================================
Decays::Trees::MCExclusive::MCExclusive
( const Decays::iNode&                        mother     , 
  const Decays::Trees::Alg                    alg        , 
  const bool                                  decayOnly  , 
  const Decays::Trees::Oscillation            oscillation ) 
  : Decays::iTree_<const LHCb::MCParticle*> () 
  , m_mother      ( mother     )
  , m_children    (            ) 
  , m_alg         ( alg        )
  , m_decayOnly   ( decayOnly  )
  , m_oscillation ( oscillation ) 
{}
// ============================================================================
/*  constructor from the node (mother) and flags
 *  @param oscillated check the oscilaltion flag
 *  @param mother the mother node
 */
// ============================================================================
Decays::Trees::MCExclusive::MCExclusive
( const Decays::Trees::Oscillation            oscillation ,
  const Decays::iNode&                        mother      ) 
  : Decays::iTree_<const LHCb::MCParticle*> () 
  , m_mother      ( mother                   )
  , m_children    (                          ) 
  , m_alg         ( Decays::Trees::Daughters )
  , m_decayOnly   ( true                     )
  , m_oscillation ( oscillation              ) 
{}
// ============================================================================
/*  constructor from the decay and flags 
 *  @param decay the decay descriptor
 *  @param alg the matching algorithm
 *  @param decayOnly the flag to search only through decay products
 *  @param oscillated check the oscilaltion flag
 */
// ============================================================================
Decays::Trees::MCExclusive::MCExclusive
( const Decays::Decay&              decay       , 
  const Decays::Trees::Alg          alg         , 
  const bool                        decayOnly   , 
  const Decays::Trees::Oscillation  oscillation ) 
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
// MANDATORY: check the validity 
// ============================================================================
bool Decays::Trees::MCExclusive::valid() const 
{ return m_mother.valid() && Decays::valid ( childBegin() , childEnd() ) ; }
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
  for ( TreeList::const_iterator ichild = childBegin() ; 
        childEnd() != ichild ; ++ichild ) { size += ichild->collect ( output ) ; }  
  return size ;
}
// ============================================================================
// has marked decays in tree ?
// ============================================================================
bool Decays::Trees::MCExclusive::marked() const 
{
  for ( TreeList::const_iterator ichild = childBegin() ; 
        childEnd() != ichild ; ++ichild ) 
  { if ( ichild->marked() ) { return true ; } }
  return false ;
}
// ============================================================================
// implementation of operator() for MCExclusive and derived classes
// ============================================================================
bool Decays::Trees::MCExclusive::operator() 
  ( Decays::iTree_<const LHCb::MCParticle*>::argument p ) const
{
  if ( p_match(p) ) {
    return true ;
  } else {
    reset() ; // cache should be empty when not matching
    return false ;
  }
}
// ============================================================================
// MANDATORY: actual implementation of operator()
// ============================================================================
bool Decays::Trees::MCExclusive::p_match
  ( Decays::iTree_<const LHCb::MCParticle*>::argument p ) const
{
  // check the validness and oscillation criteria 
  if ( !ok ( p )                      ) { return false ; }        // RETURN 
  // perform the real matching:
  // match children? 
  if ( 0 == nChildren()               ) { return true  ; }        // RETURN
  //   (1) get the proper decay sections:
  Decays::MCSections sections ;
  makeSections ( p , alg() , decayOnly() , sections ) ;
  //   (2) loop over all sections
  for (  Decays::MCSections::iterator isect = sections.begin() ; 
         sections.end() != isect ; ++isect )
  {
    // (3) try to match the section 
    if ( match ( isect -> begin () , 
                 isect -> end   () ,
                 childBegin     () , 
                 childEnd       () ) ) { return true ; } // RETURN
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
  for ( TreeList::const_iterator itree = childBegin() ; 
        childEnd() != itree ; ++itree ) { s << ( *itree ) ; }
  //
  return s << ") " ;
} 
// ============================================================================
// add one more node to the tree 
// ============================================================================
void Decays::Trees::MCExclusive::addDaughter 
( const Decays::iTree_<const LHCb::MCParticle*>& tree ) 
{ m_children += tree ; }
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
/*  full constructor from the node (mother), subtrees and "final" flag
 *  @param mother the mother node 
 *  @param children the list of daughter substrees 
 *  @param alg the matching algorithm 
 *  @param decayOnly the flag to search only through decay products 
 *  @param oscillation check the oscilaltion flag
 */
// ============================================================================
Decays::Trees::MCInclusive::MCInclusive
( const Decays::iNode&                        mother      , 
  const Decays::Trees::MCExclusive::TreeList& children    ,
  const Decays::Trees::Alg                    alg         , 
  const bool                                  decayOnly   , 
  const Decays::Trees::Oscillation            oscillation ) 
  : Decays::Trees::MCExclusive ( mother      , 
                                 children    , 
                                 alg         , 
                                 decayOnly   , 
                                 oscillation ) 
{}
// ============================================================================
/*  constructor from the node (mother) & flags
 *  @param mother the mother node
 *  @param alg the matching algorithm
 *  @param decayOnly the flag to search only through decay products
 *  @param oscillated check the oscilaltion flag
 */
// ============================================================================
Decays::Trees::MCInclusive::MCInclusive
( const Decays::iNode&             mother      , 
  const Decays::Trees::Alg         alg         , 
  const bool                       decayOnly   , 
  const Decays::Trees::Oscillation oscillation ) 
  : Decays::Trees::MCExclusive ( mother      , 
                                 alg         , 
                                 decayOnly   , 
                                 oscillation ) 
{}
// ============================================================================
/*  constructor from the decay descriptor and flags 
 *  @param decay the decay 
 *  @param alg the matching algorithm
 *  @param decayOnly the flag to search only through decay products
 *  @param oscillated check the oscilaltion flag
 */
// ============================================================================
Decays::Trees::MCInclusive::MCInclusive
( const Decays::Decay&             decay       , 
  const Decays::Trees::Alg         alg         , 
  const bool                       decayOnly   , 
  const Decays::Trees::Oscillation oscillation ) 
  : Decays::Trees::MCExclusive ( decay       , 
                                 alg         , 
                                 decayOnly   , 
                                 oscillation ) 
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

// ============================================================================
// MANDATORY: check the validity of the tree 
// ============================================================================
bool Decays::Trees::MCInclusive::valid() const 
{ return MCExclusive::valid() && 0 != nChildren() ; }
// ============================================================================
// the only one essential method 
// ============================================================================
bool Decays::Trees::MCInclusive::p_match
  ( Decays::iTree_<const LHCb::MCParticle*>::argument p ) const
{
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
             children ().trees() ) ) { return true ; }            //   RETURN 
      continue ;                                                  // CONTINUE  
    }
    
    // never go further, unfortunately the most generic algorithm
    // it terribly slow... O(N!)
    
    //    // (4) sort the section 
    //    std::sort ( isect->begin() , isect->end() ) ;
    //    // make all possible permutations of the section  
    //    while ( std::next_permutation ( isect->begin() , isect->end() ) ) 
    //    {
    //      // (5) match all fields:
    //      if ( std::equal ( childBegin() , childEnd() , isect->begin() , Equal() ) ) 
    //      { return true ; }                                            // RETURN 
    //    } // next permutation
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
  for ( TreeList::const_iterator itree = childBegin() ; 
        childEnd() != itree ; ++itree ) { s << ( *itree ) ; }
  //
  return s << " ... ) " ;
}


// ============================================================================
// MCOPTIONAL
// ============================================================================
/*  full constructor from the node (mother) and subtrees
 *  @param mother the mother node 
 *  @param children the list of children 
 *  @param optional the list of optional components 
 *  @param alg the matching algorithm 
 *  @param decayOnly the flag to search only through decay products 
 *  @param oscillation check the oscilaltion flag
 */
// ============================================================================
Decays::Trees::MCOptional::MCOptional
( const Decays::iNode&                        mother     , 
  const Decays::Trees::MCExclusive::TreeList& children   ,
  const Decays::Trees::MCExclusive::TreeList& optional   ,
  const Decays::Trees::Alg                    alg        ,
  const bool                                  decayOnly  , 
  const Decays::Trees::Oscillation            oscillation ) 
  : Decays::Trees::MCExclusive ( mother      , 
                                 children    , 
                                 alg         , 
                                 decayOnly   , 
                                 oscillation )
  , m_optional ( optional ) 
{}
// ============================================================================
/*  constructor from decay descriptor, optional and flags 
 *  @param decay the decay descriptor
 *  @param optional the list of optional components 
 *  @param alg the matching algorithm 
 *  @param decayOnly the flag to search only through decay products 
 *  @param oscillation check the oscilaltion flag
 */
// ============================================================================
Decays::Trees::MCOptional::MCOptional
( const Decays::Decay&                        decay      , 
  const Decays::Trees::MCExclusive::TreeList& optional   ,
  const Decays::Trees::Alg                    alg        ,
  const bool                                  decayOnly  , 
  const Decays::Trees::Oscillation            oscillation ) 
  : Decays::Trees::MCExclusive ( decay       , 
                                 alg         , 
                                 decayOnly   , 
                                 oscillation )
  , m_optional ( optional ) 
{}
// ============================================================================
// constructor form the constructed tree 
// ============================================================================
Decays::Trees::MCOptional::MCOptional
( const Decays::Trees::MCExclusive&           right    ,
  const Decays::Trees::MCExclusive::TreeList& optional )
  : Decays::Trees::MCExclusive ( right ) 
  , m_optional ( optional ) 
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
// has marked decays in tree ?
// ============================================================================
bool Decays::Trees::MCOptional::marked() const 
{
  for ( TreeList::const_iterator iopt = optBegin() ; 
        optEnd() != iopt ; ++iopt ) 
  { if ( iopt->marked() ) { return true ; } }
  // check the base class:
  return Decays::Trees::MCExclusive::marked() ;
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
  for ( TreeList::const_iterator itree = childBegin() ;
        childEnd() != itree ; ++itree ) 
  { s << ( *itree ) ; }
  //
  for ( TreeList::const_iterator itree = optBegin() ; 
        optEnd () != itree ; ++itree ) 
  { s << " {" << ( *itree ) << "} "; }
  //
  return s << ") " ;
} 
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
bool Decays::Trees::MCOptional::p_match
  ( Decays::iTree_<const LHCb::MCParticle*>::argument p ) const 
{
  // check the validness and oscillation criteria 
  if ( !ok ( p )                      ) { return false ; }        // RETURN 
  // perform the real matching:
  // (1) get the proper decay sections:
  Decays::MCSections sections ;
  makeSections ( p , alg() , decayOnly() , sections ) ;
  // (2) loop over all sections
  // copy list of optional component.
  TreeList opt ( m_optional ) ;
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
           children().trees() )   ) { continue ; }                    // CONTINUE
    // (6) make all possible permutations:
    do // make all possible permutations:
    {
      // (7) match all declared mandatory fields:
      if ( std::equal ( childBegin() , childEnd() , isect->begin() , Equal() ) )
      {
        // the temporary iterator 
        Decays::MCSection::iterator aux = isect->begin() + nChildren() ;
        // create the local container for permutations 
        // TreeList opt ( m_optional ) ;
        std::for_each ( opt.begin () , 
                        opt.end   () , 
                        std::mem_fun_ref(&TreeList::_Tree_::reset) ) ;
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
{ m_optional += tree ; }
// ============================================================================
// add one more daughter to the decay 
// ============================================================================
void Decays::Trees::MCOptional::addOptional 
( const Decays::iNode& node ) 
{ addOptional ( MCExclusive ( node ) ) ; }
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
/*  full constructor from the node (mother) and subtrees
 *  @param mother the mother node 
 *  @param children the list of children
 *  @param alg the matching algorithm 
 *  @param decayOnly the flag to search only through decay products 
 *  @param oscillation check the oscilaltion flag
 */
// ============================================================================
Decays::Trees::Photos::Photos
( const Decays::iNode&                   mother     , 
  const Decays::Trees::Photos::TreeList& children   ,
  const Decays::Trees::Alg               alg        , 
  const bool                             decayOnly  ,
  const Decays::Trees::Oscillation       oscillation ) 
  : Decays::Trees::MCExclusive ( mother      , 
                                 children    , 
                                 alg         , 
                                 decayOnly   , 
                                 oscillation ) 
    , m_photon    ( s_GAMMA )
{}
// ============================================================================
/** constructor from the decay descriptor and flags 
 *  @param decay the decay descriptor 
 *  @param alg the matching algorithm 
 *  @param decayOnly the flag to search only through decay products 
 *  @param oscillation check the oscilaltion flag
 */
// ============================================================================
Decays::Trees::Photos::Photos
( const Decays::Decay&                   decay      , 
  const Decays::Trees::Alg               alg        , 
  const bool                             decayOnly  ,
  const Decays::Trees::Oscillation       oscillation ) 
  : Decays::Trees::MCExclusive ( decay       , 
                                 alg         , 
                                 decayOnly   , 
                                 oscillation ) 
    , m_photon    ( s_GAMMA )
{}
// ============================================================================
// constructor from the tree 
// ============================================================================
Decays::Trees::Photos::Photos
( const  Decays::Trees::MCExclusive& right ) 
  : Decays::Trees::MCExclusive  ( right ) 
  , m_photon    ( s_GAMMA )
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
  for ( TreeList::const_iterator itree = childBegin() ; 
        childEnd() != itree ; ++itree ) { s << ( *itree ) ; }
  //
  return s << ") " ;
} 
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
bool Decays::Trees::Photos::p_match
  ( Decays::iTree_<const LHCb::MCParticle*>::argument p ) const 
{
  // check the validness and oscillation criteria 
  if ( !ok ( p )                      ) { return false ; }        // RETURN 
  // perform the real matching:
  // (1) get the proper decay sections:
  Decays::MCSections sections ;
  makeSections ( p , alg() , decayOnly() , sections ) ;
  // (2) loop over all sections
  //
  const _Pid   gamma     ( m_photon ) ;
  const NotPid not_gamma ( m_photon ) ;
  // 
  for (  Decays::MCSections::iterator isect = sections.begin() ; 
         sections.end() != isect ; ++isect )
  {
    // (3) try to match the section 
    // skip the combinations  which does not match at all 
    if ( nChildren() > isect->size() ) { continue ; }                  // CONTINUE 
    // (4) count photons
    const size_t nGamma = 
      std::count_if ( isect->begin () , isect->end () , gamma ) ;
    if ( nChildren () + nGamma < isect->size() ) { continue ; }        // CONTINUE 
    //
    // (5) check "inclusive"
    if ( !LoKi::Algs::found_N 
         ( isect -> begin () , 
           isect -> end   () , 
           children().trees() ) ) { continue ; }                     // CONTINUE 
    //
    //  (6) start some branches: 
    //
    // A. simplest cases: 
    //             a. no photons at all 
    //             b. no room for photons
    if      ( 0             == nGamma              || 
              isect->size() == nChildren()          ) 
    {
      if ( match ( isect -> begin () , 
                   isect -> end   () , 
                   childBegin     () , 
                   childEnd       () ) ) { return true ; }              // RETURN 
      //
      continue ;                                                        // CONTINUE 
    }
    //
    // B. check the "non-photon" part
    //  
    Decays::MCSection::iterator iphys = 
      std::partition  ( isect->begin() , isect->end() , not_gamma ) ;
    //
    const std::size_t nPhys = iphys - isect->begin() ;
    //      a. no room for photons 
    if ( nPhys == nChildren() ) 
    {
      if ( match ( isect -> begin () , 
                   iphys             , 
                   childBegin     () , 
                   childEnd       () ) ) { return true ; }              // RETURN 
      //
      continue ;                                                        // CONTINUE 
    }
    else if ( nPhys > nChildren() ) { continue ; }                      // CONTINUE  
    // 
    // C. general case:
    //
    const std::size_t  nPhotons  = nChildren() - nPhys ;
    LoKi::GetN_<Decays::MCSection,std::less<const LHCb::MCParticle*> > 
      getN ( nPhotons        , 
             iphys           , 
             isect -> end () ) ;
    //
    Decays::MCSection sect ;
    sect.reserve ( nChildren() ) ;
    while ( nPhotons == getN.next ( std::back_inserter ( sect ) ) )
    {
      sect.insert ( sect.end() , isect->begin() , iphys ) ;
      //
      if ( match ( sect.begin () , sect.end () , 
                   childBegin () , childEnd () ) ) { return true ; }
      sect.clear   () ;
      sect.reserve ( nChildren() ) ;
    }
  } // go to the next section
  // 
  // no match 
  return false ; // RETURN 
}
// ============================================================================
//     // (4) sort the section
//     std::stable_sort ( isect->begin() , isect->end() ) ;
//     // (6) check "inclusive"
//     if ( !LoKi::Algs::found_N 
//          ( isect -> begin () , 
//            isect -> end   () , 
//            children().trees() ) ) { continue ; }                     // CONTINUE 
//     // (5) make all possible permutations:
//     do 
//     {
//       // (6) match all declared mandatory fields:
//       if ( std::equal ( childBegin() , childEnd() , isect->begin() , Equal() )
//            && // the rest is only photons 
//            onlyPid (  isect->begin () + nChildren () , 
//                       isect->end   () , m_photon     ) ) { return true ; }
//     } // next permutation
//     while ( std::next_permutation ( isect->begin() , isect->end() ) ) ;
//   } // next section
//   // no match 
//   return false ; // RETURN 
// }
// // ============================================================================




// ============================================================================
// PHOTOSOPTIONAL
// ============================================================================
/*  full constructor from the node (mother) and subtrees
 *  @param mother the mother node 
 *  @param children the list of children 
 *  @param optional the list of optional components 
 *  @param alg the matching algorithm 
 *  @param decayOnly the flag to search only through decay products 
 *  @param oscillation check the oscilaltion flag
 */
// ============================================================================
Decays::Trees::PhotosOptional::PhotosOptional
( const Decays::iNode&                        mother     , 
  const Decays::Trees::MCExclusive::TreeList& children   ,
  const Decays::Trees::MCExclusive::TreeList& optional   ,
  const Decays::Trees::Alg                    alg        ,
  const bool                                  decayOnly  , 
  const Decays::Trees::Oscillation            oscillation ) 
  : Decays::Trees::MCOptional ( mother      , 
                                children    , 
                                optional    , 
                                alg         , 
                                decayOnly   , 
                                oscillation )
  , m_photon ( s_GAMMA )  
{}
// ============================================================================
/*  constructor from decay descriptor, optional and flags 
 *  @param decay the decay descriptor
 *  @param optional the list of optional components 
 *  @param alg the matching algorithm 
 *  @param decayOnly the flag to search only through decay products 
 *  @param oscillation check the oscilaltion flag
 */
// ============================================================================
Decays::Trees::PhotosOptional::PhotosOptional
( const Decays::Decay&                        decay      ,
  const Decays::Trees::MCExclusive::TreeList& optional   ,
  const Decays::Trees::Alg                    alg        ,
  const bool                                  decayOnly  ,
  const Decays::Trees::Oscillation            oscillated ) 
  : Decays::Trees::MCOptional ( decay      , 
                                optional   , 
                                alg        , 
                                decayOnly  , 
                                oscillated )
  , m_photon ( s_GAMMA )  
{}
// ============================================================================
Decays::Trees::PhotosOptional::PhotosOptional
( const Decays::Trees::MCOptional& right ) 
  : Decays::Trees::MCOptional ( right )
  , m_photon ( s_GAMMA )  
{}
// ============================================================================
Decays::Trees::PhotosOptional::PhotosOptional
( const Decays::Trees::MCExclusive&           right    ,   
  const Decays::Trees::MCExclusive::TreeList& optional ) 
  : Decays::Trees::MCOptional ( right , optional )
  , m_photon ( s_GAMMA )  
{}
// ============================================================================


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
  for ( TreeList::const_iterator itree = childBegin() ; 
        childEnd() != itree ; ++itree ) 
  { s << ( *itree ) ; }
  //
  for ( TreeList::const_iterator itree = optBegin() ; 
        optEnd () != itree ; ++itree ) 
  { s << " {" << ( *itree ) << "} "; }
  //
  return s << ") " ;
} 
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
bool Decays::Trees::PhotosOptional::p_match
  ( Decays::iTree_<const LHCb::MCParticle*>::argument p ) const 
{
  // check the validness and oscillation criteria 
  if ( !ok ( p )                      ) { return false ; }        // RETURN 
  // perform the real matching:
  // (1) get the proper decay sections:
  Decays::MCSections sections ;
  makeSections ( p , alg() , decayOnly() , sections ) ;
  // (2) loop over all sections
  // local list of optional stuff 
  TreeList opt ( optBegin() , optEnd() ) ;
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
           children().trees() ) ) { continue ; }                    // CONTINUE
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
        // TreeList opt ( optBegin() , optEnd() ) ;
        std::for_each ( opt.begin () , 
                        opt.end   () , 
                        std::mem_fun_ref(&TreeList::_Tree_::reset) ) ;
        std::stable_sort ( opt.begin () , opt.end() ) ; // sort it!!!
        // make the nesessary permutations 
        do 
        {                                     // make the nesessary permutations 
          for ( TreeList::iterator it = opt.begin() ; opt.end() != it ; ++it )
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

// ============================================================================
// treat properly the stable trees
// ============================================================================
template <> bool Decays::Trees::Stable_<const LHCb::MCParticle*>::operator() 
  ( Decays::iTree_<const LHCb::MCParticle*>::argument p ) const 
{
  //
  if ( 0 == p                      ) { return false ; }
  if ( !valid()                    ) { return false ; }
  if ( !m_head ( p->particleID() ) ) { return false ; }
  //
  typedef SmartRefVector<LHCb::MCVertex> MCVs ;
  const MCVs& vertices = p->endVertices() ;
  if ( vertices.empty() ) { return true ; }   // RETURN 
  //
  for ( MCVs::const_iterator imcv = vertices.begin() ; 
        vertices.end() != imcv ; ++imcv ) 
  {
    const LHCb::MCVertex* mcv = *imcv ;
    if ( 0 == mcv ) { continue ; }
    //
    switch ( mcv->type() ) 
    {
    case LHCb::MCVertex::DecayVertex        : return false ;
    case LHCb::MCVertex::OscillatedAndDecay : return false ;
    default                                 : continue     ;
    }
    //
  }  
  return true ; // true ? 
}






// ============================================================================
// The END 
// ============================================================================
