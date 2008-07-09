// $Id: GenDecays.cpp,v 1.2 2008-07-09 16:19:16 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Algs.h"
#include "LoKi/GenDecays.h"
#include "LoKi/GenSections.h"
#include "LoKi/GenChild.h"
#include "LoKi/GenAlgs.h"
#include "LoKi/Trees.h"
#include "LoKi/GenOscillated.h"
// ============================================================================
/** @file
 *  Implementation file for LoKi HepMC-tree-functors 
 *  @author Vanya BELYAEV Ivan.BELYAEV@nikhef.nl
 *  @date   2008-06-10
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
  ( const HepMC::GenParticle*      p     , 
    const LoKi::Decays::Trees::Alg alg   , 
    LoKi::Decays::GenSections&     sect  ) 
  {
    if ( 0 == p ) { return ; }
    //
    if ( LoKi::Decays::Trees::Sections == alg ) 
    { LoKi::Decays::sections ( p , sect ) ; }
    else 
    {
      LoKi::Decays::GenSection s ;
      LoKi::Decays::section ( p , s ) ;      
      sect.push_back ( s ) ;    
    }
  }
  // ==========================================================================
  struct NotPid : public std::unary_function<const HepMC::GenParticle*,bool>
  {
    // ========================================================================
    /// constructor 
    NotPid ( const LoKi::Decays::Nodes::Pid& pid ) : m_pid ( pid ) {}
    // ========================================================================
    /// the major method: find "not a pid" 
    inline bool operator () ( const HepMC::GenParticle* p ) const 
    { return 0 != p && !m_pid.check ( LHCb::ParticleID ( p->pdg_id() ) ) ; }
    // ========================================================================
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
  /// the helper object for comparison 
  typedef LoKi::Decays::Trees::CheckTree<const HepMC::GenParticle*> Equal ;  
  // ==========================================================================
  struct _Pid :  public std::unary_function<const HepMC::GenParticle*,bool>
  {
    /// constructor 
    _Pid ( const LoKi::Decays::Nodes::Pid& pid ) : m_pid ( pid ) {}
    // ========================================================================
    /// the major method: find "not a pid" 
    inline bool operator () ( const HepMC::GenParticle* p ) const 
    { return 0 != p && m_pid.check ( LHCb::ParticleID ( p->pdg_id() ) ) ; }
    // ========================================================================
  private:
    // ========================================================================
    /// the pid-checker 
    LoKi::Decays::Nodes::Pid m_pid ; //the pid-checker 
    // ========================================================================    
  } ;
  // ==========================================================================
}
// ============================================================================
// GENEXCLUSIVE 
// ============================================================================
/*  constructor from the node (mother), subtrees and "final" flag
 *  @param mother the mother node 
 *  @param children the list of daughter substrees 
 *  @param alg the matching algorithm 
 */
// ============================================================================
LoKi::Decays::Trees::GenExclusive::GenExclusive
( const LoKi::Decays::iNode&                         mother      , 
  const LoKi::Decays::Trees::GenExclusive::SubTrees& children    ,
  const LoKi::Decays::Trees::Alg                     alg         ,
  const LoKi::Decays::Trees::Oscillation             oscillation )
  : LoKi::Decays::iTree_<const HepMC::GenParticle*> () 
  , m_mother      ( mother      )
  , m_children    ( children    ) 
  , m_alg         ( alg         )
  , m_oscillation ( oscillation )
{}
// ============================================================================
/*  constructor from the node (mother), subtrees and "final" flag
 *  @param mother the mother node 
 *  @param alg the matching algorithm 
 *  @param osc require the oscillation flag for mother 
 */
// ============================================================================
LoKi::Decays::Trees::GenExclusive::GenExclusive
( const LoKi::Decays::iNode&                        mother      , 
  const LoKi::Decays::Trees::Alg                    alg         ,
  const LoKi::Decays::Trees::Oscillation            oscillation )
  : LoKi::Decays::iTree_<const HepMC::GenParticle*> () 
  , m_mother      ( mother     )
  , m_children    (            ) 
  , m_alg         ( alg        )
  , m_oscillation ( oscillation )
{}
// ============================================================================
/*  constructor from the node (mother), subtrees and "final" flag
 *  @param mother the mother node 
 *  @param children the list of daughter substrees 
 *  @param alg the matching algorithm 
 */
// ============================================================================
LoKi::Decays::Trees::GenExclusive::GenExclusive
( const std::string&                                 mother      , 
  const LoKi::Decays::Trees::GenExclusive::SubTrees& children    ,
  const LoKi::Decays::Trees::Alg                     alg         ,
  const LoKi::Decays::Trees::Oscillation             oscillation )
  : LoKi::Decays::iTree_<const HepMC::GenParticle*> () 
  , m_mother     ( LoKi::Decays::Nodes::Pid ( mother ) )
  , m_children   ( children   ) 
  , m_alg        ( alg        )
  , m_oscillation ( oscillation )
{}
// ============================================================================
/*  constructor from the node (mother), subtrees and "final" flag
 *  @param mother the mother node 
 *  @param alg the matching algorithm 
 *  @param osc require the oscillation flag for mother 
 */
// ============================================================================
LoKi::Decays::Trees::GenExclusive::GenExclusive
( const std::string&                                mother      , 
  const LoKi::Decays::Trees::Alg                    alg         ,
  const LoKi::Decays::Trees::Oscillation            oscillation )
  : LoKi::Decays::iTree_<const HepMC::GenParticle*> () 
  , m_mother     ( LoKi::Decays::Nodes::Pid ( mother ) )
  , m_children   (            ) 
  , m_alg        ( alg        )
  , m_oscillation ( oscillation )
{}
// ============================================================================
/*  constructor from the node (mother), subtrees and "final" flag
 *  @param mother the mother node 
 *  @param children the list of daughter substrees 
 *  @param alg the matching algorithm 
 */
// ============================================================================
LoKi::Decays::Trees::GenExclusive::GenExclusive
( const LHCb::ParticleID&                            mother     , 
  const LoKi::Decays::Trees::GenExclusive::SubTrees& children   ,
  const LoKi::Decays::Trees::Alg                     alg        ,
  const LoKi::Decays::Trees::Oscillation             oscillation )
  : LoKi::Decays::iTree_<const HepMC::GenParticle*> () 
  , m_mother     ( LoKi::Decays::Nodes::Pid ( mother ) )
  , m_children   ( children   ) 
  , m_alg        ( alg        )
  , m_oscillation ( oscillation )
{}
// ============================================================================
/*  constructor from the node (mother), subtrees and "final" flag
 *  @param mother the mother node 
 *  @param alg the matching algorithm 
 *  @param osc require the oscillation flag for mother 
 */
// ============================================================================
LoKi::Decays::Trees::GenExclusive::GenExclusive
( const LHCb::ParticleID&                           mother      , 
  const LoKi::Decays::Trees::Alg                    alg         ,
  const LoKi::Decays::Trees::Oscillation            oscillation )
  : LoKi::Decays::iTree_<const HepMC::GenParticle*> () 
  , m_mother      ( LoKi::Decays::Nodes::Pid ( mother ) )
  , m_children    (            ) 
  , m_alg         ( alg        )
  , m_oscillation ( oscillation )
{}
// ============================================================================
/*  constructor from the decay 
 *  @param decay the decay 
 *  @param alg the matching algorithm 
 *  @param oscillation check the oscillation flag
 */
// ============================================================================
LoKi::Decays::Trees::GenExclusive::GenExclusive
( const LHCb::Decay&                                decay       , 
  const LoKi::Decays::Trees::Alg                    alg         ,
  const LoKi::Decays::Trees::Oscillation            oscillation )
  : LoKi::Decays::iTree_<const HepMC::GenParticle*> () 
  , m_mother      ( LoKi::Decays::Nodes::Pid ( decay.mother() ) )
  , m_children    (            ) 
  , m_alg         ( alg        )
  , m_oscillation ( oscillation )
{
  setChildren ( decay.children() ) ;
}
// ============================================================================
// MANDATORY: check the validity 
// ============================================================================
bool LoKi::Decays::Trees::GenExclusive::valid() const 
{
  return m_mother.valid() && LoKi::Decays::valid ( childBegin() , childEnd() ) ;
}
// ============================================================================
// MANDATORY: validate the tree 
// ============================================================================
StatusCode  LoKi::Decays::Trees::GenExclusive::validate 
( IParticlePropertySvc* svc ) const 
{
  StatusCode sc = m_mother.validate ( svc ) ;
  if ( sc.isFailure() ) { return sc ; }
  return LoKi::Decays::validate ( childBegin() , childEnd() , svc ) ;
}
// ============================================================================
// reset the collection cache
// ============================================================================
void LoKi::Decays::Trees::GenExclusive::reset() const { i_reset () ; }
// ============================================================================
// collect the marked elements 
// ============================================================================
size_t LoKi::Decays::Trees::GenExclusive::collect 
( LoKi::Decays::iTree_<const HepMC::GenParticle*>::Collection& output ) const 
{
  size_t size = 0 ;
  for ( SubTrees::const_iterator ichild = childBegin() ; 
        childEnd() != ichild ; ++ichild ) { size += ichild->collect ( output ) ; }  
  return size ;
}
// ============================================================================
// the only one essential method 
// ============================================================================
bool LoKi::Decays::Trees::GenExclusive::operator() 
  ( LoKi::Decays::iTree_<const HepMC::GenParticle*>::argument p ) const
{
  // reset the cache (ALWAYS!!!)
  i_reset () ;
  // check the validness and oscillation criteria 
  const HepMC::GenParticle* good = ok ( p ) ;
  if ( !good                          ) { return false ; }        // RETURN 
  // match children? 
  if ( 0 == nChildren()               ) { return true  ; }        // RETURN
  // perform the real matching:
  // (1) get the proper decay sections:
  LoKi::Decays::GenSections sections ;
  makeSections ( good , alg () , sections ) ;
  // (2) loop over all sections
  for (  LoKi::Decays::GenSections::iterator isect = sections.begin() ; 
         sections.end() != isect ; ++isect )
  {
    // (3) try to match the section 
    // skip the combinations  which does not match at all 
    if ( nChildren() != isect->size() ) { continue ; }
    // (4) sort the section 
    std::stable_sort ( isect->begin() , isect->end() ) ;
    do // btw - it is my first "do ... while ( .. )" loop..        // ATTENTION 
    {
      // (5) match all fields:
      if ( std::equal ( childBegin() , childEnd() , isect->begin() , Equal() ) ) 
      { return true ; }                                               // RETURN 
    } // make all possible permutations of the section  
    while ( std::next_permutation ( isect->begin() , isect->end() ) ) ;
  } // next section
  // no match 
  return false ;                                                      // RETURN 
}
// ============================================================================
// MANDATORY: the specific printout 
// ============================================================================
std::ostream& 
LoKi::Decays::Trees::GenExclusive::fillStream ( std::ostream& s ) const 
{
  //
  s << ( ( 0 != nChildren() ) ? " (" : " " ) ;
  //
  switch ( oscillation () ) 
  {
  case LoKi::Decays::Trees::Oscillated    : 
    s << "[" << mother () << "]os "  ; break ;  // BREAK 
  case LoKi::Decays::Trees::NotOscillated : 
    s << "[" << mother () << "]nos " ; break ;  // BREAK 
  default:
    s <<        mother ()            ; 
  }
  //
  if ( 0 == nChildren() ) { return s ; }
  //
  switch ( this->alg() ) 
  {
  case Sections : s << "-->" ; break ;
  default       : s << " ->" ; break ;
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
void LoKi::Decays::Trees::GenExclusive::addDaughter 
( const LoKi::Decays::iTree_<const HepMC::GenParticle*>& tree ) 
{ m_children.push_back ( tree ) ; }
// ============================================================================
// add one more node to the tree 
// ============================================================================
void LoKi::Decays::Trees::GenExclusive::addDaughter 
( const std::string& node )
{ addDaughter ( LoKi::Decays::Nodes::Pid ( node ) ) ; }
// ============================================================================
// add one more node to the tree 
// ============================================================================
void LoKi::Decays::Trees::GenExclusive::addDaughter 
( const LHCb::ParticleID& node )
{ addDaughter ( LoKi::Decays::Nodes::Pid ( node ) ) ; }
// ============================================================================
// add one more node to the tree 
// ============================================================================
void LoKi::Decays::Trees::GenExclusive::addDaughter 
( const LHCb::Decay::Item& node )
{ addDaughter ( LoKi::Decays::Nodes::Pid ( node ) ) ; }
// ============================================================================
// add one more node to the tree 
// ============================================================================
void LoKi::Decays::Trees::GenExclusive::addDaughter 
( const ParticleProperty* node )
{ addDaughter ( LoKi::Decays::Nodes::Pid ( node ) ) ; }
// ============================================================================
// add one more node to the tree 
// ============================================================================
void LoKi::Decays::Trees::GenExclusive::addDaughter 
( const LoKi::Decays::iNode& node )
{ addDaughter ( GenExclusive ( node ) ) ; }
// ============================================================================
// set children 
// ============================================================================
void LoKi::Decays::Trees::GenExclusive::setChildren 
( const std::vector<std::string>& children ) 
{
  m_children.clear() ;
  for ( std::vector<std::string>::const_iterator ichild = 
          children.begin() ; children.end() != ichild ; ++ichild ) 
  { addDaughter ( *ichild ) ; }  
}
// ============================================================================
// set children 
// ============================================================================
void LoKi::Decays::Trees::GenExclusive::setChildren 
( const std::vector<LHCb::Decay::Item>& children ) 
{
  m_children.clear() ;
  for ( std::vector<LHCb::Decay::Item>::const_iterator ichild = 
          children.begin() ; children.end() != ichild ; ++ichild ) 
  { addDaughter ( *ichild ) ; }  
}
// ============================================================================
// set children 
// ============================================================================
void LoKi::Decays::Trees::GenExclusive::setChildren 
( const std::vector<LHCb::ParticleID>& children ) 
{
  m_children.clear() ;
  for ( std::vector<LHCb::ParticleID>::const_iterator ichild = 
          children.begin() ; children.end() != ichild ; ++ichild ) 
  { addDaughter ( *ichild ) ; }  
}
// ============================================================================
// set children 
// ============================================================================
void LoKi::Decays::Trees::GenExclusive::setChildren 
( const std::vector<const ParticleProperty*>& children ) 
{
  m_children.clear() ;
  for ( std::vector<const ParticleProperty*>::const_iterator ichild = 
          children.begin() ; children.end() != ichild ; ++ichild ) 
  { addDaughter ( *ichild ) ; }  
}
// ============================================================================
/*  check the validness & mother & oscillation 
 *  @param p particle to be checked
 *  @return oscilalted or not
 */
// ============================================================================
const HepMC::GenParticle* 
LoKi::Decays::Trees::GenExclusive::ok ( const HepMC::GenParticle* p ) const 
{
    if ( 0 == p          ) { return 0 ; }                      // RETURN 
    // check mother 
    LHCb::ParticleID pid ( p->pdg_id() ) ;
    if ( !mother ( pid ) ) { return 0 ; }                      // RETURN 
    //
    switch ( oscillation  () ) 
    {
    case LoKi::Decays::Trees::Oscillated    : 
      return 0 == pid.threeCharge() ?    LoKi::GenParticles::oscillated2 ( p )       : 0 ;  
    case LoKi::Decays::Trees::NotOscillated :
      return 0 == pid.threeCharge() && ( LoKi::GenParticles::oscillated1 ( p ) ||
                                         LoKi::GenParticles::oscillated2 ( p ) ) ? 0 : p ;
    default :
      return p ;
    }
    return p ;
}
// ============================================================================
// GENINCLUSIVE 
// ============================================================================
/*  constructor from the node (mother), subtrees and "final" flag
 *  @param mother the mother node 
 *  @param children the list of daughter substrees 
 *  @param alg the matching algorithm 
 */
// ============================================================================
LoKi::Decays::Trees::GenInclusive::GenInclusive
( const LoKi::Decays::iNode&                         mother     , 
  const LoKi::Decays::Trees::GenExclusive::SubTrees& children   ,
  const LoKi::Decays::Trees::Alg                     alg        ) 
  : LoKi::Decays::Trees::GenExclusive ( mother , children , alg ) 
{}
// ============================================================================
/*  constructor from the node (mother), subtrees and "final" flag
 *  @param mother the mother node 
 *  @param alg the matching algorithm 
 */
// ============================================================================
LoKi::Decays::Trees::GenInclusive::GenInclusive
( const LoKi::Decays::iNode&                        mother     , 
  const LoKi::Decays::Trees::Alg                    alg        ) 
  : LoKi::Decays::Trees::GenExclusive( mother , alg ) 
{}
// ============================================================================
/*  constructor from the node (mother), subtrees and "final" flag
 *  @param mother the mother node 
 *  @param children the list of daughter substrees 
 *  @param alg the matching algorithm 
 */
// ============================================================================
LoKi::Decays::Trees::GenInclusive::GenInclusive
( const std::string&                                 mother     , 
  const LoKi::Decays::Trees::GenExclusive::SubTrees& children   ,
  const LoKi::Decays::Trees::Alg                     alg        ) 
  : LoKi::Decays::Trees::GenExclusive ( mother , children , alg ) 
{}
// ============================================================================
/*  constructor from the node (mother), subtrees and "final" flag
 *  @param mother the mother node 
 *  @param alg the matching algorithm 
 */
// ============================================================================
LoKi::Decays::Trees::GenInclusive::GenInclusive
( const std::string&                                mother     , 
  const LoKi::Decays::Trees::Alg                    alg        ) 
  : LoKi::Decays::Trees::GenExclusive( mother , alg ) 
{}
// ============================================================================
/*  constructor from the node (mother), subtrees and "final" flag
 *  @param mother the mother node 
 *  @param children the list of daughter substrees 
 *  @param alg the matching algorithm 
 */
// ============================================================================
LoKi::Decays::Trees::GenInclusive::GenInclusive
( const LHCb::ParticleID&                            mother     , 
  const LoKi::Decays::Trees::GenExclusive::SubTrees& children   ,
  const LoKi::Decays::Trees::Alg                     alg        ) 
  : LoKi::Decays::Trees::GenExclusive ( mother , children , alg ) 
{}
// ============================================================================
/*  constructor from the node (mother), subtrees and "final" flag
 *  @param mother the mother node 
 *  @param alg the matching algorithm 
 */
// ============================================================================
LoKi::Decays::Trees::GenInclusive::GenInclusive
( const LHCb::ParticleID&                           mother     , 
  const LoKi::Decays::Trees::Alg                    alg        ) 
  : LoKi::Decays::Trees::GenExclusive( mother , alg ) 
{}
// ============================================================================
// constructor  from GenExclusive 
// ============================================================================
LoKi::Decays::Trees::GenInclusive::GenInclusive
( const LoKi::Decays::Trees::GenExclusive& right ) 
  : LoKi::Decays::Trees::GenExclusive ( right ) 
{}
// ============================================================================
// MANDATORY: check the validity of the tree 
// ============================================================================
bool LoKi::Decays::Trees::GenInclusive::valid() const 
{ return GenExclusive::valid() && 0 != nChildren() ; }
// ============================================================================
// the only one essential method 
// ============================================================================
bool LoKi::Decays::Trees::GenInclusive::operator() 
  ( LoKi::Decays::iTree_<const HepMC::GenParticle*>::argument p ) const
{
  // reset the cache (ALWAYS!!!)
  i_reset () ;
  // check the validness and oscillation criteria 
  const HepMC::GenParticle* good = ok ( p ) ;
  if ( !good                      ) { return false ; }        // RETURN 
  // perform the real matching:  
  // (0) treat specifically the special case
  // Only one inclusive particle and look through the whole tree
  if ( 1 == nChildren () )
  {
    std::vector<const HepMC::GenParticle*> d ;
    LoKi::GenChild::daughters ( good , d ) ;
    // in tree 
    if ( LoKi::Decays::Trees::Sections == alg() ) 
    { return LoKi::GenAlgs::found ( d.begin() , d.end() , front () ) ; }
    // direct daughters :
    return d.end () != std::find_if ( d.begin() , d.end() , front () ) ;
  }
  // (1) get the proper decay sections:
  LoKi::Decays::GenSections sections ;
  makeSections ( good , alg() , sections ) ;
  // (2) loop over all sections
  for (  LoKi::Decays::GenSections::iterator isect = sections.begin() ; 
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
    std::stable_sort ( isect->begin() , isect->end() ) ;
    do  // loop over all permutations  
    {
      // (5) match all fields:
      if ( std::equal ( childBegin() , childEnd() , isect->begin() , Equal() ) ) 
      { return true ; }                                            // RETURN 
    } // make all possible permutations of the section  
    while ( std::next_permutation ( isect->begin() , isect->end() ) ) ;
  } // next section
  // no match 
  return false ;                                                   // RETURN 
}
// ============================================================================
// MANDATORY: the specific printout 
// ============================================================================
std::ostream& 
LoKi::Decays::Trees::GenInclusive::fillStream ( std::ostream& s ) const 
{
  //
  switch ( oscillation () ) 
  {
  case LoKi::Decays::Trees::Oscillated    : 
    s << " ( [" << mother () << "]os "  ; break ;  // BREAK 
  case LoKi::Decays::Trees::NotOscillated : 
    s << " ( [" << mother () << "]nos " ; break ;  // BREAK 
  default:
    s << " ("   << mother ()            ; 
  } 
  //
  switch ( this->alg() ) 
  {
  case Sections : s << "-->" ; break ;
  default       : s << " ->" ; break ;
  }
  //
  for ( SubTrees::const_iterator itree = childBegin() ; 
        childEnd() != itree ; ++itree ) { s << ( *itree ) ; }
  //
  return s << " ... ) " ;
}
// ============================================================================
// GENOPTIONAL
// ============================================================================
LoKi::Decays::Trees::GenOptional::GenOptional
( const LoKi::Decays::iNode&                         mother     , 
  const LoKi::Decays::Trees::GenExclusive::SubTrees& children   ,
  const LoKi::Decays::Trees::GenExclusive::SubTrees& optional   ,
  const Alg                                          alg        ) 
  : LoKi::Decays::Trees::GenExclusive ( mother , children , alg )
  , m_optional ( optional ) 
{}
// ============================================================================
LoKi::Decays::Trees::GenOptional::GenOptional
( const LoKi::Decays::iNode&                             mother   , 
  const LoKi::Decays::Trees::GenExclusive::SubTrees&     children ,
  const LoKi::Decays::iTree_<const HepMC::GenParticle*>& optional ,
  const Alg                                              alg      ) 
  : LoKi::Decays::Trees::GenExclusive ( mother , children , alg ) 
  , m_optional ( 1 , optional ) 
{}
// ============================================================================
LoKi::Decays::Trees::GenOptional::GenOptional
( const LoKi::Decays::iNode&                         mother   , 
  const LoKi::Decays::Trees::GenExclusive::SubTrees& children ,
  const LoKi::Decays::iNode&                         optional ,
  const LoKi::Decays::Trees::Alg                     alg      ) 
  : LoKi::Decays::Trees::GenExclusive ( mother , children , alg ) 
  , m_optional ( 1 , GenExclusive ( optional ) ) 
{}
// ============================================================================
LoKi::Decays::Trees::GenOptional::GenOptional
( const std::string&                                 mother     , 
  const LoKi::Decays::Trees::GenExclusive::SubTrees& children   ,
  const LoKi::Decays::Trees::GenExclusive::SubTrees& optional   ,
  const Alg                                          alg        ) 
  : LoKi::Decays::Trees::GenExclusive ( mother , children , alg )
  , m_optional ( optional ) 
{}
// ============================================================================
LoKi::Decays::Trees::GenOptional::GenOptional
( const std::string&                                     mother   , 
  const LoKi::Decays::Trees::GenExclusive::SubTrees&     children ,
  const LoKi::Decays::iTree_<const HepMC::GenParticle*>& optional ,
  const Alg                                              alg      ) 
  : LoKi::Decays::Trees::GenExclusive ( mother , children , alg ) 
  , m_optional ( 1 , optional ) 
{}
// ============================================================================
LoKi::Decays::Trees::GenOptional::GenOptional
( const std::string&                                 mother   , 
  const LoKi::Decays::Trees::GenExclusive::SubTrees& children ,
  const LoKi::Decays::iNode&                         optional ,
  const LoKi::Decays::Trees::Alg                     alg      ) 
  : LoKi::Decays::Trees::GenExclusive ( mother , children , alg ) 
  , m_optional ( 1 , GenExclusive ( optional ) ) 
{}
// ============================================================================
LoKi::Decays::Trees::GenOptional::GenOptional
( const LHCb::ParticleID&                            mother     , 
  const LoKi::Decays::Trees::GenExclusive::SubTrees& children   ,
  const LoKi::Decays::Trees::GenExclusive::SubTrees& optional   ,
  const Alg                                          alg        ) 
  : LoKi::Decays::Trees::GenExclusive ( mother , children , alg )
  , m_optional ( optional ) 
{}
// ============================================================================
LoKi::Decays::Trees::GenOptional::GenOptional
( const LHCb::ParticleID&                                mother   , 
  const LoKi::Decays::Trees::GenExclusive::SubTrees&     children ,
  const LoKi::Decays::iTree_<const HepMC::GenParticle*>& optional ,
  const Alg                                              alg      ) 
  : LoKi::Decays::Trees::GenExclusive ( mother , children , alg ) 
  , m_optional ( 1 , optional ) 
{}
// ============================================================================
LoKi::Decays::Trees::GenOptional::GenOptional
( const LHCb::ParticleID&                            mother   , 
  const LoKi::Decays::Trees::GenExclusive::SubTrees& children ,
  const LoKi::Decays::iNode&                         optional ,
  const LoKi::Decays::Trees::Alg                     alg      ) 
  : LoKi::Decays::Trees::GenExclusive ( mother , children , alg ) 
  , m_optional ( 1 , GenExclusive ( optional ) ) 
{}
// ============================================================================
LoKi::Decays::Trees::GenOptional::GenOptional
( const LoKi::Decays::Trees::GenOptional::GenExclusive& right    , 
  const LoKi::Decays::Trees::GenExclusive::SubTrees&    optional ) 
  : LoKi::Decays::Trees::GenExclusive ( right ) 
  , m_optional ( optional ) 
{}
// ============================================================================
LoKi::Decays::Trees::GenOptional::GenOptional
( const LoKi::Decays::Trees::GenOptional::GenExclusive& right    , 
  const LoKi::Decays::iNode&                            optional ) 
  : LoKi::Decays::Trees::GenExclusive ( right ) 
  , m_optional ( 1 , GenExclusive ( optional ) )  
{}
// ============================================================================
// MANDATORY: check the validity of the tree 
// ============================================================================
bool LoKi::Decays::Trees::GenOptional::valid() const 
{
  return GenExclusive::valid() && 0 != nChildren() && 0 != nOptional() && 
    LoKi::Decays::valid ( optBegin( ), optEnd() ) ;
}
// ============================================================================
// MANDATORY: the proper validation of the tree
// ============================================================================
StatusCode LoKi::Decays::Trees::GenOptional::validate 
( IParticlePropertySvc* svc ) const 
{ 
  StatusCode sc = GenExclusive::validate ( svc ) ;
  return sc.isFailure() ? sc :
    LoKi::Decays::validate ( optBegin() , optEnd() , svc ) ;
}
// ============================================================================
// MANDATORY: the specific printout 
// ============================================================================
std::ostream& LoKi::Decays::Trees::GenOptional::fillStream 
( std::ostream& s ) const 
{
  //
  switch ( oscillation () ) 
  {
  case LoKi::Decays::Trees::Oscillated    : 
    s << " ( [" << mother () << "]os "  ; break ;  // BREAK 
  case LoKi::Decays::Trees::NotOscillated : 
    s << " ( [" << mother () << "]nos " ; break ;  // BREAK 
  default:
    s << " ("   << mother ()            ; 
  } 
  //
  switch ( this->alg() ) 
  {
  case Sections : s << "-->" ; break ;
  default       : s << " ->" ; break ;
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
bool LoKi::Decays::Trees::GenOptional::operator() 
  ( LoKi::Decays::iTree_<const HepMC::GenParticle*>::argument p ) const 
{
  // reset the cache (ALWAYS!!!)
  i_reset () ;
  // check the validness and oscillation criteria 
  const HepMC::GenParticle* good = ok ( p ) ;
  if ( !good                      ) { return false ; }        // RETURN 
  // perform the real matching:
  // (1) get the proper decay sections:
  LoKi::Decays::GenSections sections ;
  makeSections ( good , alg() , sections ) ;
  // (2) loop over all sections
  for (  LoKi::Decays::GenSections::iterator isect = sections.begin() ; 
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
    do // loop over all permutations 
    {
      // (7) match all declared mandatory fields:
      if ( std::equal ( childBegin() , childEnd() , isect->begin() , Equal () ) )
      {
        // the temporary iterator 
        LoKi::Decays::GenSection::iterator aux = isect->begin() + nChildren () ;
        // create the local container for permutations 
        SubTrees opt ( m_optional ) ;
        std::stable_sort ( opt.begin () , opt.end() ) ; // sort it!!!
        // start all possible permutations of the optional stuff 
        do  
        {
          if ( std::equal ( aux , isect->end() , opt.begin() , Equal () ) ) 
          { return true ; }                                       // RETURN  
        } // the next permutation for the optional stuff 
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
void LoKi::Decays::Trees::GenOptional::addOptional 
( const LoKi::Decays::iTree_<const HepMC::GenParticle*>& tree ) 
{ m_optional.push_back ( tree ) ; }
// ============================================================================
// add one more daughter to the decay 
// ============================================================================
void LoKi::Decays::Trees::GenOptional::addOptional 
( const LoKi::Decays::iNode& node ) 
{ m_optional.push_back ( GenExclusive ( node ) ) ; }
// ============================================================================
// add one more daughter to the decay 
// ============================================================================
void LoKi::Decays::Trees::GenOptional::addOptional 
( const std::string& node ) 
{ addOptional ( LoKi::Decays::Nodes::Pid ( node ) ) ; }
// ============================================================================
// add one more daughter to the decay 
// ============================================================================
void LoKi::Decays::Trees::GenOptional::addOptional 
( const LHCb::ParticleID& node ) 
{ addOptional ( LoKi::Decays::Nodes::Pid ( node ) ) ; }
// ============================================================================
// add one more daughter to the decay 
// ============================================================================
void LoKi::Decays::Trees::GenOptional::addOptional 
( const LHCb::Decay::Item& node ) 
{ addOptional ( LoKi::Decays::Nodes::Pid ( node ) ) ; }
// ============================================================================
// add one more daughter to the decay 
// ============================================================================
void LoKi::Decays::Trees::GenOptional::addOptional 
( const ParticleProperty* node ) 
{ addOptional ( LoKi::Decays::Nodes::Pid ( node ) ) ; }
// ============================================================================
// set optional
// ============================================================================
void LoKi::Decays::Trees::GenOptional::setOptional
( const std::vector<int>& optional ) 
{
  m_optional.clear() ;
  for ( std::vector<int>::const_iterator iopt = 
          optional.begin() ; optional.end() != iopt; ++iopt ) 
  { addOptional ( LHCb::ParticleID ( *iopt ) ) ; }  
}
// ============================================================================
// set optional
// ============================================================================
void LoKi::Decays::Trees::GenOptional::setOptional
( const std::vector<const ParticleProperty*>& optional ) 
{
  m_optional.clear() ;
  for ( std::vector<const ParticleProperty*>::const_iterator iopt = 
          optional.begin() ; optional.end() != iopt; ++iopt ) 
  { addOptional ( *iopt ) ; }  
}
// ============================================================================
// set optional
// ============================================================================
void LoKi::Decays::Trees::GenOptional::setOptional
( const std::vector<LHCb::ParticleID>& optional ) 
{
  m_optional.clear() ;
  for ( std::vector<LHCb::ParticleID>::const_iterator iopt = 
          optional.begin() ; optional.end() != iopt; ++iopt ) 
  { addOptional ( *iopt ) ; }  
}
// ============================================================================
// set optional
// ============================================================================
void LoKi::Decays::Trees::GenOptional::setOptional
( const std::vector<LHCb::Decay::Item>& optional ) 
{
  m_optional.clear() ;
  for ( std::vector<LHCb::Decay::Item>::const_iterator iopt = 
          optional.begin() ; optional.end() != iopt; ++iopt ) 
  { addOptional ( *iopt ) ; }  
}
// ============================================================================
// set optional
// ============================================================================
void LoKi::Decays::Trees::GenOptional::setOptional
( const std::vector<std::string>& optional ) 
{
  m_optional.clear() ;
  for ( std::vector<std::string>::const_iterator iopt = 
          optional.begin() ; optional.end() != iopt; ++iopt ) 
  { addOptional ( *iopt ) ; }  
}
// ============================================================================
// GENPHOTOS 
// ============================================================================
// constructor from the node (mother) and subtrees 
// ============================================================================
LoKi::Decays::Trees::GenPhotos::GenPhotos
( const LoKi::Decays::iNode&                      mother   , 
  const LoKi::Decays::Trees::GenPhotos::SubTrees& children ,
  const LoKi::Decays::Trees::Alg                  alg      )
  : LoKi::Decays::Trees::GenExclusive ( mother    , children , alg ) 
  , m_photon    ( "gamma"   )
{}
// ============================================================================
// constructor from the node (mother) and subtrees 
// ============================================================================
LoKi::Decays::Trees::GenPhotos::GenPhotos
( const std::string&                              mother   , 
  const LoKi::Decays::Trees::GenPhotos::SubTrees& children ,
  const LoKi::Decays::Trees::Alg                  alg      )
  : LoKi::Decays::Trees::GenExclusive ( mother    , children , alg ) 
  , m_photon    ( "gamma"   )
{}
// ============================================================================
// constructor from the node (mother) and subtrees 
// ============================================================================
LoKi::Decays::Trees::GenPhotos::GenPhotos
( const LHCb::ParticleID&                         mother   , 
  const LoKi::Decays::Trees::GenPhotos::SubTrees& children ,
  const LoKi::Decays::Trees::Alg                  alg      )
  : LoKi::Decays::Trees::GenExclusive ( mother    , children , alg ) 
  , m_photon    ( "gamma"   )
{}
// ============================================================================
LoKi::Decays::Trees::GenPhotos::GenPhotos
( const LoKi::Decays::Trees::GenPhotos::GenExclusive& right ) 
  : LoKi::Decays::Trees::GenExclusive ( right ) 
  , m_photon    ( "gamma"   )
{}
// ============================================================================
// MANDATORY: the proper validation of the tree
// ============================================================================
StatusCode LoKi::Decays::Trees::GenPhotos::validate 
( IParticlePropertySvc* svc ) const 
{
  StatusCode sc = m_photon.validate ( svc ) ;
  return sc.isFailure() ? sc : GenExclusive::validate ( svc ) ;  // RETURN 
}
// ============================================================================
// MANDATORY: the specific printout 
// ============================================================================
std::ostream& LoKi::Decays::Trees::GenPhotos::fillStream ( std::ostream& s ) const 
{
  //
  switch ( oscillation () ) 
  {
  case LoKi::Decays::Trees::Oscillated    : 
    s << " ( [" << mother () << "]os "  ; break ;  // BREAK 
  case LoKi::Decays::Trees::NotOscillated : 
    s << " ( [" << mother () << "]nos " ; break ;  // BREAK 
  default:
    s << " ("   << mother ()            ; 
  } 
  //
  switch ( this->alg() ) 
  {
  case Sections : s << "==>" ; break ;
  default       : s << " =>" ; break ;
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
bool LoKi::Decays::Trees::GenPhotos::operator() 
  ( LoKi::Decays::iTree_<const HepMC::GenParticle*>::argument p ) const 
{
  // reset the cache (ALWAYS!!!)
  i_reset () ;
  // check the validness and oscillation criteria 
  const HepMC::GenParticle* good = ok ( p ) ;
  if ( !good ) { return false ; }        // RETURN 
  // perform the real matching:
  // (1) get the proper decay sections:
  LoKi::Decays::GenSections sections ;
  makeSections ( good , alg() , sections ) ;
  // (2) loop over all sections
  for (  LoKi::Decays::GenSections::iterator isect = sections.begin() ; 
         sections.end() != isect ; ++isect )
  {
    // (3) try to match the section 
    // skip the combinations  which does not match at all 
    if ( nChildren() > isect->size() ) { continue ; }
    // (4) count photons
    const size_t nGamma = 
      std::count_if ( isect->begin() , isect->end() , _Pid(m_photon) ) ;
    if ( nChildren () + nGamma < isect->size() ) { continue ; }
    // (5) sort the section 
    std::stable_sort ( isect->begin() , isect->end() ) ;
    // (6) check "inclusive"
    if ( !LoKi::Algs::found_N 
         ( isect -> begin () , 
           isect -> end   () , 
           children()        ) ) { continue ; }
    // (7) make all possible permutations:
    do 
    {
      // (8) match all declared mandatory fields:
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
// GENPHOTOSOPTIONAL
// ============================================================================
LoKi::Decays::Trees::GenPhotosOptional::GenPhotosOptional
( const LoKi::Decays::iNode&                         mother   , 
  const LoKi::Decays::Trees::GenExclusive::SubTrees& children ,
  const LoKi::Decays::Trees::GenExclusive::SubTrees& optional ,
  const LoKi::Decays::Trees::Alg                     alg      )
  : LoKi::Decays::Trees::GenOptional ( mother , children , optional , alg )
  , m_photon ( "gamma")  
{}
// ============================================================================
LoKi::Decays::Trees::GenPhotosOptional::GenPhotosOptional
( const LoKi::Decays::iNode&                         mother   , 
  const LoKi::Decays::Trees::GenExclusive::SubTrees& children ,
  const LoKi::Decays::iNode&                         optional ,
  const LoKi::Decays::Trees::Alg                     alg      )
  : LoKi::Decays::Trees::GenOptional ( mother , children , optional , alg )
  , m_photon ( "gamma")  
{}
// ============================================================================
LoKi::Decays::Trees::GenPhotosOptional::GenPhotosOptional
( const LoKi::Decays::iNode&                             mother   , 
  const LoKi::Decays::Trees::GenExclusive::SubTrees&     children ,
  const LoKi::Decays::iTree_<const HepMC::GenParticle*>& optional ,
  const LoKi::Decays::Trees::Alg                         alg      )
  : LoKi::Decays::Trees::GenOptional ( mother , children , optional , alg )
  , m_photon ( "gamma")  
{}
// ============================================================================
LoKi::Decays::Trees::GenPhotosOptional::GenPhotosOptional
( const std::string&                                 mother   , 
  const LoKi::Decays::Trees::GenExclusive::SubTrees& children ,
  const LoKi::Decays::Trees::GenExclusive::SubTrees& optional ,
  const LoKi::Decays::Trees::Alg                     alg      )
  : LoKi::Decays::Trees::GenOptional ( mother , children , optional , alg )
  , m_photon ( "gamma")  
{}
// ============================================================================
LoKi::Decays::Trees::GenPhotosOptional::GenPhotosOptional
( const std::string&                                 mother   , 
  const LoKi::Decays::Trees::GenExclusive::SubTrees& children ,
  const LoKi::Decays::iNode&                         optional ,
  const LoKi::Decays::Trees::Alg                     alg      )
  : LoKi::Decays::Trees::GenOptional ( mother , children , optional , alg )
  , m_photon ( "gamma")  
{}
// ============================================================================
LoKi::Decays::Trees::GenPhotosOptional::GenPhotosOptional
( const std::string&                                     mother   , 
  const LoKi::Decays::Trees::GenExclusive::SubTrees&     children ,
  const LoKi::Decays::iTree_<const HepMC::GenParticle*>& optional ,
  const LoKi::Decays::Trees::Alg                         alg      )
  : LoKi::Decays::Trees::GenOptional ( mother , children , optional , alg )
  , m_photon ( "gamma")  
{}
// ============================================================================
LoKi::Decays::Trees::GenPhotosOptional::GenPhotosOptional
( const LHCb::ParticleID&                            mother   , 
  const LoKi::Decays::Trees::GenExclusive::SubTrees& children ,
  const LoKi::Decays::Trees::GenExclusive::SubTrees& optional ,
  const LoKi::Decays::Trees::Alg                     alg      )
  : LoKi::Decays::Trees::GenOptional ( mother , children , optional , alg )
  , m_photon ( "gamma")  
{}
// ============================================================================
LoKi::Decays::Trees::GenPhotosOptional::GenPhotosOptional
( const LHCb::ParticleID&                            mother   , 
  const LoKi::Decays::Trees::GenExclusive::SubTrees& children ,
  const LoKi::Decays::iNode&                         optional ,
  const LoKi::Decays::Trees::Alg                     alg      )
  : LoKi::Decays::Trees::GenOptional ( mother , children , optional , alg )
  , m_photon ( "gamma")  
{}
// ============================================================================
LoKi::Decays::Trees::GenPhotosOptional::GenPhotosOptional
( const LHCb::ParticleID&                                mother   , 
  const LoKi::Decays::Trees::GenExclusive::SubTrees&     children ,
  const LoKi::Decays::iTree_<const HepMC::GenParticle*>& optional ,
  const LoKi::Decays::Trees::Alg                         alg      )
  : LoKi::Decays::Trees::GenOptional ( mother , children , optional , alg )
  , m_photon ( "gamma")  
{}
// ============================================================================
LoKi::Decays::Trees::GenPhotosOptional::GenPhotosOptional
( const LoKi::Decays::Trees::GenOptional& right ) 
  : LoKi::Decays::Trees::GenOptional ( right )
  , m_photon ( "gamma")  
{}
// ============================================================================
LoKi::Decays::Trees::GenPhotosOptional::GenPhotosOptional
( const LoKi::Decays::Trees::GenExclusive&           right    ,
  const LoKi::Decays::Trees::GenExclusive::SubTrees& optional )
  : LoKi::Decays::Trees::GenOptional ( right , optional )
  , m_photon ( "gamma")  
{}
// ============================================================================
LoKi::Decays::Trees::GenPhotosOptional::GenPhotosOptional
( const LoKi::Decays::Trees::GenExclusive& right    ,
  const LoKi::Decays::iNode&               optional )
  : LoKi::Decays::Trees::GenOptional ( right , optional )
  , m_photon ( "gamma")  
{}
// ============================================================================
// MANDATORY: check the validity of the tree 
// ============================================================================
bool LoKi::Decays::Trees::GenPhotosOptional::valid() const 
{ return GenOptional::valid() && m_photon.valid() ; }
// ============================================================================
// MANDATORY: the proper validation of the tree
// ============================================================================
StatusCode LoKi::Decays::Trees::GenPhotosOptional::validate 
( IParticlePropertySvc* svc ) const 
{ 
  StatusCode sc = m_photon.validate ( svc ) ;
  return sc.isFailure() ? sc : GenOptional::validate ( svc ) ;
}
// ============================================================================
// MANDATORY: the specific printout 
// ============================================================================
std::ostream& LoKi::Decays::Trees::GenPhotosOptional::fillStream 
( std::ostream& s ) const 
{
  //
  switch ( oscillation () ) 
  {
  case LoKi::Decays::Trees::Oscillated    : 
    s << " ( [" << mother () << "]os "  ; break ;  // BREAK 
  case LoKi::Decays::Trees::NotOscillated : 
    s << " ( [" << mother () << "]nos " ; break ;  // BREAK 
  default:
    s << " ("   << mother ()            ; 
  } 
  //
  switch ( this->alg() ) 
  {
  case Sections : s << "==>" ; break ;
  default       : s << " =>" ; break ;
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
bool LoKi::Decays::Trees::GenPhotosOptional::operator() 
  ( LoKi::Decays::iTree_<const HepMC::GenParticle*>::argument p ) const 
{
  // reset the cache (ALWAYS!!!)
  i_reset () ;
  // check the validness and oscillation criteria 
  const HepMC::GenParticle* good = ok ( p ) ;
  if ( !good ) { return false ; }        // RETURN 
  // perform the real matching:
  // (1) get the proper decay sections:
  LoKi::Decays::GenSections sections ;
  makeSections ( p , alg() , sections ) ;
  // (2) loop over all sections
  for (  LoKi::Decays::GenSections::iterator isect = sections.begin() ; 
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
    do
    {      
      // (7) match all declared mandatory fields:
      if ( std::equal ( childBegin() , childEnd() , isect->begin() , Equal() ) )
      {
        // the temporary iterator 
        LoKi::Decays::GenSection::iterator aux = isect->begin() + nChildren() ;
        // nothing to match? 
        if ( aux == isect->end() ) { return true ; } // RETURN
        // create the local container for permutations 
        SubTrees opt ( optBegin() , optEnd() ) ;
        std::stable_sort ( opt.begin () , opt.end() ) ; // sort it!!!
        // make the nesessary permutations 
        do 
        { 
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
LoKi::Decays::Trees::And_<const HepMC::GenParticle*>
operator&& 
( const LoKi::Decays::iTree_<const HepMC::GenParticle*>& t , 
  const LoKi::Decays::iNode&                             n ) 
{ return t && LoKi::Decays::Trees::GenExclusive ( n ) ; }
// ============================================================================
/*  operator for easy construction of trees 
 *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
 *  @date 2008-07-07
 */
// ============================================================================
LoKi::Decays::Trees::And_<const HepMC::GenParticle*>
operator&& 
( const LoKi::Decays::iTree_<const HepMC::GenParticle*>& t , 
  const std::string&                                     n ) 
{ return t && LoKi::Decays::Trees::GenExclusive ( n ) ; }
// ============================================================================
/*  operator for the easy construction of trees 
 *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
 *  @date 2008-07-07
 */
// ============================================================================
LoKi::Decays::Trees::And_<const HepMC::GenParticle*>
operator&& 
( const LoKi::Decays::iTree_<const HepMC::GenParticle*>& t , 
  const LHCb::ParticleID&                                n ) 
{ return t && LoKi::Decays::Trees::GenExclusive ( n ) ; }
// ============================================================================
/*  operator for easy construction of trees 
 *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
 *  @date 2008-07-07
 */
// ============================================================================
LoKi::Decays::Trees::Or_<const HepMC::GenParticle*>
operator|| 
( const LoKi::Decays::iTree_<const HepMC::GenParticle*>& t , 
  const LoKi::Decays::iNode&                             n ) 
{ return t || LoKi::Decays::Trees::GenExclusive ( n ) ; }
// ============================================================================
/*  operator for easy construction of trees 
 *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
 *  @date 2008-07-07
 */
// ============================================================================
LoKi::Decays::Trees::Or_<const HepMC::GenParticle*>
operator||
( const LoKi::Decays::iTree_<const HepMC::GenParticle*>& t , 
  const std::string&                                     n ) 
{ return t || LoKi::Decays::Trees::GenExclusive ( n ) ; }
// ============================================================================
/*  operator for the easy construction of trees 
 *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
 *  @date 2008-07-07
 */
// ============================================================================
LoKi::Decays::Trees::Or_<const HepMC::GenParticle*>
operator||
( const LoKi::Decays::iTree_<const HepMC::GenParticle*>& t , 
  const LHCb::ParticleID&                                n ) 
{ return t || LoKi::Decays::Trees::GenExclusive ( n ) ; }
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
