// $Id$
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
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV Ivan.BELYAEV@nikhef.nl
 *  @date   2008-06-10
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
  ( const HepMC::GenParticle* p     , 
    const Decays::Trees::Alg  alg   , 
    Decays::GenSections&      sect  ) 
  {
    if ( 0 == p ) { return ; }
    //
    if ( Decays::Trees::Sections == alg ) 
    { Decays::sections ( p , sect ) ; }
    else 
    {
      Decays::GenSection s ;
      Decays::section ( p , s ) ;      
      sect.push_back ( s ) ;    
    }
  }
  // ==========================================================================
  struct NotPid : public std::unary_function<const HepMC::GenParticle*,bool>
  {
    // ========================================================================
    /// constructor 
    NotPid ( const Decays::Nodes::Pid& pid ) : m_pid ( pid ) {}
    // ========================================================================
    /// the major method: find "not a pid" 
    inline bool operator () ( const HepMC::GenParticle* p ) const 
    { return 0 != p && !m_pid.check ( LHCb::ParticleID ( p->pdg_id() ) ) ; }
    // ========================================================================
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
  /// the helper object for comparison 
  typedef Decays::Trees::CheckTree<const HepMC::GenParticle*> Equal ;  
  // ==========================================================================
  struct _Pid :  public std::unary_function<const HepMC::GenParticle*,bool>
  {
    /// constructor 
    _Pid ( const Decays::Nodes::Pid& pid ) : m_pid ( pid ) {}
    // ========================================================================
    /// the major method: find "not a pid" 
    inline bool operator () ( const HepMC::GenParticle* p ) const 
    { return 0 != p && m_pid.check ( LHCb::ParticleID ( p->pdg_id() ) ) ; }
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
// GENEXCLUSIVE 
// ============================================================================
/*  constructor from the node (mother), subtrees and "final" flag
 *  @param mother the mother node 
 *  @param children the list of daughter substrees 
 *  @param alg the matching algorithm 
 */
// ============================================================================
Decays::Trees::GenExclusive::GenExclusive
( const Decays::iNode&                         mother      , 
  const Decays::Trees::GenExclusive::TreeList& children    ,
  const Decays::Trees::Alg                     alg         ,
  const Decays::Trees::Oscillation             oscillation )
  : Decays::iTree_<const HepMC::GenParticle*> () 
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
Decays::Trees::GenExclusive::GenExclusive
( const Decays::iNode&                        mother      , 
  const Decays::Trees::Alg                    alg         ,
  const Decays::Trees::Oscillation            oscillation )
  : Decays::iTree_<const HepMC::GenParticle*> () 
  , m_mother      ( mother     )
  , m_children    (            ) 
  , m_alg         ( alg        )
  , m_oscillation ( oscillation )
{}
// ============================================================================
/*  constructor from the node (mother), subtrees and "final" flag
 *  @param osc require the oscillation flag for mother 
 *  @param mother the mother node 
 *  @param osc require the oscillation flag for mother 
 */
// ============================================================================
Decays::Trees::GenExclusive::GenExclusive
( const Decays::Trees::Oscillation            oscillation ,
  const Decays::iNode&                        mother      ) 
  : Decays::iTree_<const HepMC::GenParticle*> () 
  , m_mother      ( mother                   )
  , m_children    (                          ) 
  , m_alg         ( Decays::Trees::Daughters )
  , m_oscillation ( oscillation              )
{}
// ============================================================================
/*  constructor from the decay 
 *  @param decay the decay 
 *  @param alg the matching algorithm 
 *  @param oscillation check the oscillation flag
 */
// ============================================================================
Decays::Trees::GenExclusive::GenExclusive
( const Decays::Decay&                        decay       , 
  const Decays::Trees::Alg                    alg         ,
  const Decays::Trees::Oscillation            oscillation )
  : Decays::iTree_<const HepMC::GenParticle*> () 
  , m_mother      ( Decays::Nodes::Pid ( decay.mother() ) )
  , m_children    (            ) 
  , m_alg         ( alg        )
  , m_oscillation ( oscillation )
{
  setChildren ( decay.children() ) ;
}
// ============================================================================
// MANDATORY: check the validity 
// ============================================================================
bool Decays::Trees::GenExclusive::valid() const 
{
  return m_mother.valid() && Decays::valid ( childBegin() , childEnd() ) ;
}
// ============================================================================
// MANDATORY: validate the tree 
// ============================================================================
StatusCode  Decays::Trees::GenExclusive::validate 
( const LHCb::IParticlePropertySvc* svc ) const 
{
  StatusCode sc = m_mother.validate ( svc ) ;
  if ( sc.isFailure() ) { return sc ; }
  return Decays::validate ( childBegin() , childEnd() , svc ) ;
}
// ============================================================================
// reset the collection cache
// ============================================================================
void Decays::Trees::GenExclusive::reset() const { i_reset () ; }
// ============================================================================
// collect the marked elements 
// ============================================================================
size_t Decays::Trees::GenExclusive::collect 
( Decays::iTree_<const HepMC::GenParticle*>::Collection& output ) const 
{
  size_t size = 0 ;
  for ( TreeList::const_iterator ichild = childBegin() ; 
        childEnd() != ichild ; ++ichild ) { size += ichild->collect ( output ) ; }  
  return size ;
}
// ============================================================================
// has marked elements in the tree ?
// ============================================================================
bool Decays::Trees::GenExclusive::marked () const 
{
  for ( TreeList::const_iterator ichild = childBegin() ; 
        childEnd() != ichild ; ++ichild ) 
  { if ( ichild->marked () ) { return true ; } }  
  return false ;
}
// ============================================================================
// operator() implementation for GenExclusive and derived classes
// ============================================================================
bool Decays::Trees::GenExclusive::operator()
  ( Decays::iTree_<const HepMC::GenParticle*>::argument p ) const
{
  if ( p_match(p) ) {
    return true ;
  } else {
    reset() ; // cache should be empty when not matching
    return false ;
  }
}
// ============================================================================
// the only one essential method 
// ============================================================================
bool Decays::Trees::GenExclusive::p_match
  ( Decays::iTree_<const HepMC::GenParticle*>::argument p ) const
{
  // check the validness and oscillation criteria 
  const HepMC::GenParticle* good = ok ( p ) ;
  if ( !good                          ) { return false ; }        // RETURN 
  // match children? 
  if ( 0 == nChildren()               ) { return true  ; }        // RETURN
  //
  // perform the real matching:
  //
  // (1) get the proper decay sections:
  Decays::GenSections sections ;
  makeSections ( good , alg () , sections ) ;
  //
  // (2) loop over all sections
  for (  Decays::GenSections::iterator isect = sections.begin() ; 
         sections.end() != isect ; ++isect )
  {
    //
    // (3) try to match the section 
    // skip the combinations  which does not match at all 
    if ( nChildren() != isect->size() ) { continue ; }
    //
    // (4) sort the section 
    std::stable_sort ( isect->begin() , isect->end() ) ;
    do // btw - it is my first "do ... while ( .. )" loop..        // ATTENTION 
    {
      //
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
Decays::Trees::GenExclusive::fillStream ( std::ostream& s ) const 
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
  case Sections : s << "-->" ; break ;
  default       : s << " ->" ; break ;
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
void Decays::Trees::GenExclusive::addDaughter 
( const Decays::iTree_<const HepMC::GenParticle*>& tree ) 
{ m_children += tree  ; }
// ============================================================================
// add one more node to the tree 
// ============================================================================
void Decays::Trees::GenExclusive::addDaughter 
( const std::string& node )
{ addDaughter ( Decays::Nodes::Pid ( node ) ) ; }
// ============================================================================
// add one more node to the tree 
// ============================================================================
void Decays::Trees::GenExclusive::addDaughter 
( const LHCb::ParticleID& node )
{ addDaughter ( Decays::Nodes::Pid ( node ) ) ; }
// ============================================================================
// add one more node to the tree 
// ============================================================================
void Decays::Trees::GenExclusive::addDaughter 
( const Decays::Decay::Item& node )
{ addDaughter ( Decays::Nodes::Pid ( node ) ) ; }
// ============================================================================
// add one more node to the tree 
// ============================================================================
void Decays::Trees::GenExclusive::addDaughter 
( const LHCb::ParticleProperty* node )
{ addDaughter ( Decays::Nodes::Pid ( node ) ) ; }
// ============================================================================
// add one more node to the tree 
// ============================================================================
void Decays::Trees::GenExclusive::addDaughter 
( const Decays::iNode& node )
{ addDaughter ( GenExclusive ( node ) ) ; }
// ============================================================================
// set children 
// ============================================================================
void Decays::Trees::GenExclusive::setChildren 
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
void Decays::Trees::GenExclusive::setChildren 
( const std::vector<Decays::Decay::Item>& children ) 
{
  m_children.clear() ;
  for ( std::vector<Decays::Decay::Item>::const_iterator ichild = 
          children.begin() ; children.end() != ichild ; ++ichild ) 
  { addDaughter ( *ichild ) ; }  
}
// ============================================================================
// set children 
// ============================================================================
void Decays::Trees::GenExclusive::setChildren 
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
void Decays::Trees::GenExclusive::setChildren 
( const std::vector<const LHCb::ParticleProperty*>& children ) 
{
  m_children.clear() ;
  for ( std::vector<const LHCb::ParticleProperty*>::const_iterator ichild = 
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
Decays::Trees::GenExclusive::ok ( const HepMC::GenParticle* p ) const 
{
    if ( 0 == p          ) { return 0 ; }                      // RETURN 
    //
    // check mother 
    LHCb::ParticleID pid ( p->pdg_id() ) ;    
    //
    // chargeL 
    const int q3 = pid.threeCharge() ;
    //
    const HepMC::GenParticle* osc1 = 0 ;
    if ( 0 == q3 ) { osc1 = LoKi::GenParticles::oscillated1 ( p ) ; }
    const HepMC::GenParticle* osc2 = 0 ;
    if ( 0 == q3 ) { osc2 = LoKi::GenParticles::oscillated2 ( p ) ; }
    //
    // spaghetti 
    // explicitly requiested oscillation 
    if      ( Decays::Trees::   Oscillated == oscillation () ) 
    {
      if ( !mother ( pid ) ) { return 0 ; }                      // RETURN 
      return osc2 ;
    }
    // explicitly requiested non-oscillation 
    else if ( Decays::Trees::NotOscillated == oscillation () ) 
    {
      //
      if ( 0 != osc1 || 0 != osc1 ) { return 0 ; }
      if ( !mother ( pid )        ) { return 0 ; }               // RETURN 
      //
      return p ;  
    }
    //  oscillation is irrelevant here 
    else if ( 0 != osc2 ) 
    {
      if  ( !mother ( pid ) && !mother ( LHCb::ParticleID ( osc2-> pdg_id() ) ) ) { return 0 ; }
      return osc2 ; 
    }
    else if ( 0 != osc1 ) 
    {
      if  ( !mother ( pid ) && !mother ( LHCb::ParticleID ( osc1-> pdg_id() ) ) ) { return 0 ; }
      return    p ; 
    }
    //
    // the regular case:
    if ( !mother ( pid )        ) { return 0 ; }               // RETURN 
    //
    return p ;
}
// ============================================================================
// GENINCLUSIVE 
// ============================================================================
/*  full constructor from the node (mother), subtrees & flags 
 *  @param mother the mother node 
 *  @param children the list of daughter substrees 
 *  @param alg the matching algorithm 
 */
// ============================================================================
Decays::Trees::GenInclusive::GenInclusive
( const Decays::iNode&                         mother   , 
  const Decays::Trees::GenExclusive::TreeList& children ,
  const Decays::Trees::Alg                     alg      ,
  const Decays::Trees::Oscillation             osc      )
  : Decays::Trees::GenExclusive ( mother   , 
                                  children , 
                                  alg      , 
                                  osc      ) 
{}
// ============================================================================
/*  constructor from the node (mother), subtrees and "final" flag
 *  @param mother the mother node 
 *  @param alg the matching algorithm 
 */
// ============================================================================
Decays::Trees::GenInclusive::GenInclusive
( const Decays::iNode&                        mother , 
  const Decays::Trees::Alg                    alg    ,
  const Decays::Trees::Oscillation            osc    )
  : Decays::Trees::GenExclusive ( mother  , 
                                  alg     , 
                                  osc     ) 
{}
// ============================================================================
/*  constructor from the decay & flags 
 *  @param decay the decay 
 *  @param alg the matching algorithm 
 *  @param oscillation check the oscillation flag
 */
Decays::Trees::GenInclusive::GenInclusive
( const Decays::Decay&                        decay  , 
  const Decays::Trees::Alg                    alg    ,
  const Decays::Trees::Oscillation            osc    )
  : Decays::Trees::GenExclusive ( decay   , 
                                  alg     , 
                                  osc     ) 
{}
// ============================================================================
// constructor  from GenExclusive 
// ============================================================================
Decays::Trees::GenInclusive::GenInclusive
( const Decays::Trees::GenExclusive& right ) 
  : Decays::Trees::GenExclusive ( right ) 
{}
// ============================================================================
// MANDATORY: check the validity of the tree 
// ============================================================================
bool Decays::Trees::GenInclusive::valid() const 
{ return GenExclusive::valid() && 0 != nChildren() ; }
// ============================================================================
// the only one essential method 
// ============================================================================
bool Decays::Trees::GenInclusive::p_match
  ( Decays::iTree_<const HepMC::GenParticle*>::argument p ) const
{
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
    if ( Decays::Trees::Sections == alg() ) 
    { return LoKi::GenAlgs::found ( d.begin() , d.end() , front () ) ; }
    // direct daughters :
    return d.end () != std::find_if ( d.begin() , d.end() , front () ) ;
  }
  // (1) get the proper decay sections:
  Decays::GenSections sections ;
  makeSections ( good , alg() , sections ) ;
  // (2) loop over all sections
  for (  Decays::GenSections::iterator isect = sections.begin() ; 
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
             children ().trees() ) ) { return true ; }            // RETURN 
      continue ;                                               // CONTINUE ;  
    }
    
    // never go further, unfortunately the most generic algorithm
    // it terribly slow... O(N!)
    
    // (4) sort the section 
    //    std::stable_sort ( isect->begin() , isect->end() ) ;
    //    do  // loop over all permutations  
    //    {
    //      // (5) match all fields:
    //      if ( std::equal ( childBegin() , childEnd() , isect->begin() , Equal() ) ) 
    //      { return true ; }                                            // RETURN 
    //    } // make all possible permutations of the section  
    //    while ( std::next_permutation ( isect->begin() , isect->end() ) ) ;
  } // next section
  // no match 
  return false ;                                                   // RETURN 
}
// ============================================================================
// MANDATORY: the specific printout 
// ============================================================================
std::ostream& 
Decays::Trees::GenInclusive::fillStream ( std::ostream& s ) const 
{
  //
  switch ( oscillation () ) 
  {
  case Decays::Trees::Oscillated    : 
    s << " ( [" << mother () << "]os "  ; break ;  // BREAK 
  case Decays::Trees::NotOscillated : 
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
  for ( TreeList::const_iterator itree = childBegin() ; 
        childEnd() != itree ; ++itree ) { s << ( *itree ) ; }
  //
  return s << " ... ) " ;
}
// ============================================================================
// GENOPTIONAL
// ============================================================================
/*  full constructor from the node (mother) and subtrees
 *  @param mother the mother node 
 *  @param children the list of children 
 *  @param optional the list of optional components 
 *  @param alg the matching algorithm 
 *  @param oscillation check the oscilaltion flag
 */
// ============================================================================
Decays::Trees::GenOptional::GenOptional
( const Decays::iNode&                         mother   , 
  const Decays::Trees::GenExclusive::TreeList& children ,
  const Decays::Trees::GenExclusive::TreeList& optional ,
  const Decays::Trees::Alg                     alg      ,
  const Decays::Trees::Oscillation             osc      )
  : Decays::Trees::GenExclusive ( mother   , 
                                  children , 
                                  alg      ,  
                                  osc      )
  , m_optional ( optional ) 
{}
// ============================================================================
/*  constructor from decay descriptor, optional and flags 
 *  @param decay the decay descriptor
 *  @param optional the list of optional components 
 *  @param alg the matching algorithm 
 *  @param oscillation check the oscilaltion flag
 */
// ============================================================================
Decays::Trees::GenOptional::GenOptional
( const Decays::Decay&                         decay    ,
  const Decays::Trees::GenExclusive::TreeList& optional ,
  const Decays::Trees::Alg                     alg      ,
  const Decays::Trees::Oscillation             osc      )
  : Decays::Trees::GenExclusive ( decay , 
                                  alg   , 
                                  osc   ) 
  , m_optional ( optional ) 
{}
// ============================================================================
Decays::Trees::GenOptional::GenOptional
( const Decays::Trees::GenOptional::GenExclusive& right    , 
  const Decays::Trees::GenExclusive::TreeList&    optional ) 
  : Decays::Trees::GenExclusive ( right ) 
  , m_optional ( optional ) 
{}
// ============================================================================
// MANDATORY: check the validity of the tree 
// ============================================================================
bool Decays::Trees::GenOptional::valid() const 
{
  return GenExclusive::valid() && 0 != nChildren() && 0 != nOptional() && 
    Decays::valid ( optBegin( ), optEnd() ) ;
}
// ============================================================================
// MANDATORY: the proper validation of the tree
// ============================================================================
StatusCode Decays::Trees::GenOptional::validate 
( const LHCb::IParticlePropertySvc* svc ) const 
{ 
  StatusCode sc = GenExclusive::validate ( svc ) ;
  return sc.isFailure() ? sc :
    Decays::validate ( optBegin() , optEnd() , svc ) ;
}
// ============================================================================
// has marked elements in the tree ?
// ============================================================================
bool Decays::Trees::GenOptional::marked () const 
{
  for ( TreeList::const_iterator iopt = optBegin() ; 
        optEnd() != iopt ; ++iopt ) { if ( iopt->marked () ) { return true ; } }  
  return Decays::Trees::GenExclusive::marked() ;
}
// ============================================================================
// MANDATORY: the specific printout 
// ============================================================================
std::ostream& Decays::Trees::GenOptional::fillStream 
( std::ostream& s ) const 
{
  //
  switch ( oscillation () ) 
  {
  case Decays::Trees::Oscillated    : 
    s << " ( [" << mother () << "]os "  ; break ;  // BREAK 
  case Decays::Trees::NotOscillated : 
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
bool Decays::Trees::GenOptional::p_match
  ( Decays::iTree_<const HepMC::GenParticle*>::argument p ) const 
{
  // check the validness and oscillation criteria 
  const HepMC::GenParticle* good = ok ( p ) ;
  if ( !good                      ) { return false ; }        // RETURN 
  // perform the real matching:
  // (1) get the proper decay sections:
  Decays::GenSections sections ;
  makeSections ( good , alg() , sections ) ;
  // create local constainer for permutations 
  TreeList opt ( m_optional ) ;
  // (2) loop over all sections
  for (  Decays::GenSections::iterator isect = sections.begin() ; 
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
           children().trees() ) ) { continue ; }                        // CONTINUE 
    // (6) make all possible permutations:
    do // loop over all permutations 
    {
      // (7) match all declared mandatory fields:
      if ( std::equal ( childBegin() , childEnd() , isect->begin() , Equal () ) )
      {
        // the temporary iterator 
        Decays::GenSection::iterator aux = isect->begin() + nChildren () ;
        // create the local container for permutations 
        // TreeList opt ( m_optional ) ;
        std::for_each ( opt.begin () , 
                        opt.end   () , 
                        std::mem_fun_ref(&TreeList::_Tree_::reset) ) ;
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
void Decays::Trees::GenOptional::addOptional 
( const Decays::iTree_<const HepMC::GenParticle*>& tree ) 
{ m_optional += tree  ; }
// ============================================================================
// add one more daughter to the decay 
// ============================================================================
void Decays::Trees::GenOptional::addOptional 
( const Decays::iNode& node ) 
{ addOptional ( GenExclusive ( node ) ) ; }
// ============================================================================
// add one more daughter to the decay 
// ============================================================================
void Decays::Trees::GenOptional::addOptional 
( const std::string& node ) 
{ addOptional ( Decays::Nodes::Pid ( node ) ) ; }
// ============================================================================
// add one more daughter to the decay 
// ============================================================================
void Decays::Trees::GenOptional::addOptional 
( const LHCb::ParticleID& node ) 
{ addOptional ( Decays::Nodes::Pid ( node ) ) ; }
// ============================================================================
// add one more daughter to the decay 
// ============================================================================
void Decays::Trees::GenOptional::addOptional 
( const Decays::Decay::Item& node ) 
{ addOptional ( Decays::Nodes::Pid ( node ) ) ; }
// ============================================================================
// add one more daughter to the decay 
// ============================================================================
void Decays::Trees::GenOptional::addOptional 
( const LHCb::ParticleProperty* node ) 
{ addOptional ( Decays::Nodes::Pid ( node ) ) ; }
// ============================================================================
// set optional
// ============================================================================
void Decays::Trees::GenOptional::setOptional
( const std::vector<const LHCb::ParticleProperty*>& optional ) 
{
  m_optional.clear() ;
  for ( std::vector<const LHCb::ParticleProperty*>::const_iterator iopt = 
          optional.begin() ; optional.end() != iopt; ++iopt ) 
  { addOptional ( *iopt ) ; }  
}
// ============================================================================
// set optional
// ============================================================================
void Decays::Trees::GenOptional::setOptional
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
void Decays::Trees::GenOptional::setOptional
( const std::vector<Decays::Decay::Item>& optional ) 
{
  m_optional.clear() ;
  for ( std::vector<Decays::Decay::Item>::const_iterator iopt = 
          optional.begin() ; optional.end() != iopt; ++iopt ) 
  { addOptional ( *iopt ) ; }  
}
// ============================================================================
// set optional
// ============================================================================
void Decays::Trees::GenOptional::setOptional
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
/*  full constructor from the node (mother) and subtrees
 *  @param mother the mother node 
 *  @param children the list of children
 *  @param alg the matching algorithm 
 *  @param oscillation check the oscilaltion flag
 */
// ============================================================================
Decays::Trees::GenPhotos::GenPhotos
( const Decays::iNode&                      mother   , 
  const Decays::Trees::GenPhotos::TreeList& children ,
  const Decays::Trees::Alg                  alg      ,
  const Decays::Trees::Oscillation          osc      )
  : Decays::Trees::GenExclusive ( mother   , 
                                  children , 
                                  alg      , 
                                  osc      ) 
  , m_photon    ( s_GAMMA )
{}
// ============================================================================
/*  constructor from the decay descriptor and flags 
 *  @param decay the decay descriptor 
 *  @param alg the matching algorithm 
 *  @param oscillation check the oscilaltion flag
 */
// ============================================================================
Decays::Trees::GenPhotos::GenPhotos
( const Decays::Decay&              decay    ,  
  const Decays::Trees::Alg          alg      ,
  const Decays::Trees::Oscillation  osc      )
  : Decays::Trees::GenExclusive ( decay , alg , osc ) 
  , m_photon    ( s_GAMMA )
{}
// ============================================================================
// constructor from the tree 
// ============================================================================
Decays::Trees::GenPhotos::GenPhotos
( const Decays::Trees::GenPhotos::GenExclusive& right ) 
  : Decays::Trees::GenExclusive ( right ) 
  , m_photon    ( s_GAMMA )
{}
// ============================================================================
// MANDATORY: the proper validation of the tree
// ============================================================================
StatusCode Decays::Trees::GenPhotos::validate 
( const LHCb::IParticlePropertySvc* svc ) const 
{
  StatusCode sc = m_photon.validate ( svc ) ;
  return sc.isFailure() ? sc : GenExclusive::validate ( svc ) ;  // RETURN 
}
// ============================================================================
// MANDATORY: the specific printout 
// ============================================================================
std::ostream& Decays::Trees::GenPhotos::fillStream ( std::ostream& s ) const 
{
  //
  switch ( oscillation () ) 
  {
  case Decays::Trees::Oscillated    : 
    s << " ( [" << mother () << "]os "  ; break ;  // BREAK 
  case Decays::Trees::NotOscillated : 
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
  for ( TreeList::const_iterator itree = childBegin() ; 
        childEnd() != itree ; ++itree ) { s << ( *itree ) ; }
  //
  return s << ") " ;
} 
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
bool Decays::Trees::GenPhotos::p_match
  ( Decays::iTree_<const HepMC::GenParticle*>::argument p ) const 
{
  // check the validness and oscillation criteria 
  const HepMC::GenParticle* good = ok ( p ) ;
  if ( !good ) { return false ; }        // RETURN 
  // perform the real matching:
  // (1) get the proper decay sections:
  Decays::GenSections sections ;
  makeSections ( good , alg() , sections ) ;
  // (2) loop over all sections
  for (  Decays::GenSections::iterator isect = sections.begin() ; 
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
           children() .trees() ) ) { continue ; }
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
/*  full constructor from the node (mother) and subtrees
 *  @param mother the mother node 
 *  @param children the list of children 
 *  @param optional the list of optional components 
 *  @param alg the matching algorithm 
 *  @param oscillation check the oscilaltion flag
 */
// ============================================================================
Decays::Trees::GenPhotosOptional::GenPhotosOptional
( const Decays::iNode&                         mother   , 
  const Decays::Trees::GenExclusive::TreeList& children ,
  const Decays::Trees::GenExclusive::TreeList& optional ,
  const Decays::Trees::Alg                     alg      ,
  const Decays::Trees::Oscillation             osc      )
  : Decays::Trees::GenOptional ( mother   , 
                                 children , 
                                 optional , 
                                 alg      , 
                                 osc      )
  , m_photon ( s_GAMMA )  
{}
// ============================================================================
/*  constructor from decay descriptor, optional and flags 
 *  @param decay the decay descriptor
 *  @param optional the list of optional components 
 *  @param alg the matching algorithm 
 *  @param oscillation check the oscilaltion flag
 */
// ============================================================================
Decays::Trees::GenPhotosOptional::GenPhotosOptional
( const Decays::Decay&                         decay    , 
  const Decays::Trees::GenExclusive::TreeList& optional ,
  const Decays::Trees::Alg                     alg      ,
  const Decays::Trees::Oscillation             osc      )
  : Decays::Trees::GenOptional ( decay    , 
                                 optional , 
                                 alg      , 
                                 osc      )
  , m_photon ( s_GAMMA )  
{}
// ============================================================================
// constructor from the tree 
// ============================================================================
Decays::Trees::GenPhotosOptional::GenPhotosOptional
( const Decays::Trees::GenOptional& right ) 
  : Decays::Trees::GenOptional ( right )
  , m_photon ( s_GAMMA )  
{}
// ============================================================================
Decays::Trees::GenPhotosOptional::GenPhotosOptional
( const Decays::Trees::GenExclusive&           right    ,
  const Decays::Trees::GenExclusive::TreeList& optional )
  : Decays::Trees::GenOptional ( right , optional )
  , m_photon ( s_GAMMA )  
{}
// ============================================================================
// MANDATORY: check the validity of the tree 
// ============================================================================
bool Decays::Trees::GenPhotosOptional::valid() const 
{ return GenOptional::valid() && m_photon.valid() ; }
// ============================================================================
// MANDATORY: the proper validation of the tree
// ============================================================================
StatusCode Decays::Trees::GenPhotosOptional::validate 
( const LHCb::IParticlePropertySvc* svc ) const 
{ 
  StatusCode sc = m_photon.validate ( svc ) ;
  return sc.isFailure() ? sc : GenOptional::validate ( svc ) ;
}
// ============================================================================
// MANDATORY: the specific printout 
// ============================================================================
std::ostream& Decays::Trees::GenPhotosOptional::fillStream 
( std::ostream& s ) const 
{
  //
  switch ( oscillation () ) 
  {
  case Decays::Trees::Oscillated    : 
    s << " ( [" << mother () << "]os "  ; break ;  // BREAK 
  case Decays::Trees::NotOscillated : 
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
bool Decays::Trees::GenPhotosOptional::p_match
  ( Decays::iTree_<const HepMC::GenParticle*>::argument p ) const 
{
  // check the validness and oscillation criteria 
  const HepMC::GenParticle* good = ok ( p ) ;
  if ( !good ) { return false ; }        // RETURN 
  // perform the real matching:
  // (1) get the proper decay sections:
  Decays::GenSections sections ;
  makeSections ( p , alg() , sections ) ;
  // create the local container for permutations
  TreeList opt ( optBegin() , optEnd() ) ;
  // (2) loop over all sections
  for (  Decays::GenSections::iterator isect = sections.begin() ; 
         sections.end() != isect ; ++isect )
  {
    // (3) try to match the section 
    // skip the combinations  which does not match at all 
    if ( nChildren() > isect->size() ) { continue ; }
    // (4) check "inclusive"
    if ( !LoKi::Algs::found_N 
         ( isect->begin (), 
           isect->end   (), 
           children().trees() )   ) { continue ; }                  // CONTINUE
    // (5) sort the section 
    std::stable_sort ( isect->begin() , isect->end() ) ;
    // (6) make all possible permutations:
    do
    {      
      // (7) match all declared mandatory fields:
      if ( std::equal ( childBegin() , childEnd() , isect->begin() , Equal() ) )
      {
        // the temporary iterator 
        Decays::GenSection::iterator aux = isect->begin() + nChildren() ;
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
        { 
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
template <> bool Decays::Trees::Stable_<const HepMC::GenParticle*>::operator() 
  ( Decays::iTree_<const HepMC::GenParticle*>::argument p ) const 
{
  //
  if ( 0 == p                                        ) { return false ; }
  if ( !valid()                                      ) { return false ; }
  if ( !m_head ( LHCb::ParticleID ( p->pdg_id () ) ) ) { return false ; }
  //
  HepMC::GenVertex* ev = p -> end_vertex() ;
  if ( 0 == ev         ) { return true ; }                           // RETURN 
  //
  return
    ev->particles_begin ( HepMC::children ) == 
    ev->particles_end   ( HepMC::children ) || 
    0 == std::distance 
    ( ev->particles_begin ( HepMC::children ) , 
      ev->particles_end   ( HepMC::children ) ) ;
}



// ============================================================================
// The END 
// ============================================================================
