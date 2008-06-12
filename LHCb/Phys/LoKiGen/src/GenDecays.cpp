// $Id: GenDecays.cpp,v 1.1 2008-06-12 08:19:05 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/GenDecays.h"
#include "LoKi/GenSections.h"
#include "LoKi/GenChild.h"
#include "LoKi/GenAlgs.h"
#include "LoKi/Trees.h"
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
( const LoKi::Decays::iNode&                         mother     , 
  const LoKi::Decays::Trees::GenExclusive::SubTrees& children   ,
  const LoKi::Decays::Trees::Alg                     alg        )
  : LoKi::Decays::iTree_<const HepMC::GenParticle*> () 
  , m_mother     ( mother     )
  , m_children   ( children   ) 
  , m_alg        ( alg        )
{}
// ============================================================================
/*  constructor from the node (mother), subtrees and "final" flag
 *  @param mother the mother node 
 *  @param alg the matching algorithm 
 *  @param osc require the oscillation flag for mother 
 */
// ============================================================================
LoKi::Decays::Trees::GenExclusive::GenExclusive
( const LoKi::Decays::iNode&                        mother , 
  const LoKi::Decays::Trees::Alg                    alg    ) 
  : LoKi::Decays::iTree_<const HepMC::GenParticle*> () 
  , m_mother     ( mother     )
  , m_children   (            ) 
  , m_alg        ( alg        )
{}
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
  if ( !ok ( p )                      ) { return false ; }        // RETURN 
  // check the mother node
  if ( !mother ( p -> pdg_id () )     ) { return false ; }        // RETURN 
  // perform the real matching:
  // (1) get the proper decay sections:
  LoKi::Decays::GenSections sections ;
  makeSections ( p , alg () , sections ) ;
  // (2) loop over all sections
  for (  LoKi::Decays::GenSections::iterator isect = sections.begin() ; 
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
LoKi::Decays::Trees::GenExclusive::fillStream ( std::ostream& s ) const 
{
  //
  if ( 0 == nChildren () ) { return s << mother() ; }
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
( const LoKi::Decays::iNode& node )
{ addDaughter ( GenExclusive ( node ) ) ; }
// ============================================================================
// add one more node to the tree 
// ============================================================================
LoKi::Decays::Trees::GenExclusive&
LoKi::Decays::Trees::GenExclusive::operator+=
( const LoKi::Decays::iTree_<const HepMC::GenParticle*>& tree ) 
{
  addDaughter ( tree ) ;
  return *this ;
}
// ============================================================================
// add one more node to the tree 
// ============================================================================
LoKi::Decays::Trees::GenExclusive&
LoKi::Decays::Trees::GenExclusive::operator+=
( const LoKi::Decays::iNode& node ) 
{
  addDaughter ( node ) ;
  return *this ;
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
  if ( !ok ( p )                  ) { return false ; }        // RETURN 
  // check the mother node
  if ( !mother ( p -> pdg_id () ) ) { return false ; }        // RETURN 
  // perform the real matching:
  
  // (0) treat specifically the special case 
  
  // Only one inclusive particle and look through the whole tree
  if ( 1 == nChildren () )
  {
    std::vector<const HepMC::GenParticle*> d ;
    LoKi::GenChild::daughters ( p , d ) ;
    // in tree 
    if ( LoKi::Decays::Trees::Sections == alg() ) 
    {
      return LoKi::GenAlgs::found ( d.begin() , d.end() , *childBegin() ) ;
    }
    // direct daughters :
    return d.end () != std::find_if ( d.begin() , d.end() , *childBegin() ) ;
  }
  
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
LoKi::Decays::Trees::GenInclusive::fillStream ( std::ostream& s ) const 
{
  //
  s << " (" <<        mother ()            ; 
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
LoKi::Decays::Trees::GenInclusive&
LoKi::Decays::Trees::GenInclusive::operator+=
( const LoKi::Decays::iTree_<PARTICLE>& tree ) 
{
  addDaughter ( tree ) ;
  return *this ;
}
// ============================================================================
// add one more node to the tree 
// ============================================================================
LoKi::Decays::Trees::GenInclusive&
LoKi::Decays::Trees::GenInclusive::operator+=
( const LoKi::Decays::iNode& node ) 
{
  addDaughter ( node ) ;
  return *this ;
}
// ============================================================================


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
  s << " (" << mother() ;
  //
  switch ( this->alg() ) 
  {
  case Sections : s << "-->" ; break ;
  default       : s << "->"  ; break ;
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
  if ( !ok ( p )                  ) { return false ; }        // RETURN 
  // check the mother node
  if ( !mother ( p -> pdg_id () ) ) { return false ; }        // RETURN 
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
    if ( isect->size() < nChildren ()                ) { continue ; } // CONITNUE
    if ( isect->size() > nChildren () + nOptional () ) { continue ; } // CONTINUE
    // (4) sort the section 
    std::sort ( isect->begin() , isect->end() ) ;
    // (5) make all possible permutations:
    while ( std::next_permutation ( isect->begin() , isect->end() ) ) 
    {
      // (6) match all declared mandatory fields:
      if ( std::equal ( childBegin() , childEnd() , isect->begin() , Equal () ) )
      {
        // the temporary iterator 
        LoKi::Decays::GenSection::iterator aux = isect->begin() + nChildren () ;
        // create the local container for permutations 
        SubTrees opt ( m_optional ) ;
        std::sort ( opt.begin () , opt.end() ) ; // sort it!!!
        // start all possible permutations of the optional stuff 
        while ( std::next_permutation ( opt.begin() , opt.end () ) ) 
        {
          if ( std::equal ( aux , isect->end() , opt.begin() , Equal () ) ) 
          { return true ; }                                       // RETURN  
        } // the next permutation for the optional stuff 
      } // match mandatory stuff 
    } // next permutation
  } // next section
  // no match 
  return false ; // RETURN 
}
// ============================================================================
// add one more node to the tree 
// ============================================================================
LoKi::Decays::Trees::GenOptional&
LoKi::Decays::Trees::GenOptional::operator+=
( const LoKi::Decays::iTree_<const HepMC::GenParticle*>& tree ) 
{
  addDaughter ( tree ) ;
  return *this ;
}
// ============================================================================
// add one more node to the tree 
// ============================================================================
LoKi::Decays::Trees::GenOptional&
LoKi::Decays::Trees::GenOptional::operator+=
( const LoKi::Decays::iNode& node ) 
{
  addDaughter ( node ) ;
  return *this ;
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
  s << " (" << mother () ; 
  //
  switch ( this->alg() ) 
  {
  case Sections : s << "==>" ; break ;
  default       : s << "=>"  ; break ;
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
  if ( !ok ( p )                  ) { return false ; }        // RETURN 
  // check the mother node
  if ( !mother ( p -> pdg_id () ) ) { return false ; }        // RETURN 
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
    // (4) sort the section 
    std::sort ( isect->begin() , isect->end() ) ;
    // (5) make all possible permutations:
    while ( std::next_permutation ( isect->begin() , isect->end() ) ) 
    {
      // (6) match all declared mandatory fields:
      if ( std::equal ( childBegin() , childEnd() , isect->begin() , Equal() )
           && // the rest is only photons 
           onlyPid (  isect->begin () + nChildren () , 
                      isect->end   () , m_photon     ) ) { return true ; }
    } // next permutation
  } // next section
  // no match 
  return false ; // RETURN 
}
// ============================================================================
// add one more node to the tree 
// ============================================================================
LoKi::Decays::Trees::GenPhotos&
LoKi::Decays::Trees::GenPhotos::operator+=
( const LoKi::Decays::iTree_<const HepMC::GenParticle*>& tree ) 
{
  addDaughter ( tree ) ;
  return *this ;
}
// ============================================================================
// add one more node to the tree 
// ============================================================================
LoKi::Decays::Trees::GenPhotos&
LoKi::Decays::Trees::GenPhotos::operator+=
( const LoKi::Decays::iNode& node ) 
{
  addDaughter ( node ) ;
  return *this ;
}
// ============================================================================



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
  s << " (" << mother () ; 
  //
  switch ( this->alg() ) 
  {
  case Sections : s << "==>" ; break ;
  default       : s << "=>"  ; break ;
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
  if ( !ok ( p )                  ) { return false ; }        // RETURN 
  // check the mother node
  if ( !mother ( p -> pdg_id () ) ) { return false ; }        // RETURN 
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
    // (4) sort the section 
    std::sort ( isect->begin() , isect->end() ) ;
    // (5) make all possible permutations:
    while ( std::next_permutation ( isect->begin() , isect->end() ) ) 
    {      
      // (6) match all declared mandatory fields:
      if ( std::equal ( childBegin() , childEnd() , isect->begin() , Equal() ) )
      {
        // the temporary iterator 
        LoKi::Decays::GenSection::iterator aux = isect->begin() + nChildren() ;
        // nothing to match? 
        if ( aux == isect->end() ) { return true ; } // RETURN
        // create the local container for permutations 
        SubTrees opt ( optBegin() , optEnd() ) ;
        std::sort ( opt.begin () , opt.end() ) ; // sort it!!!
        // make the nesessary permutations 
        while ( std::next_permutation ( opt.begin() , opt.end() ) ) 
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
      } // match the mandatory stuff
    } // next permutation for mandatory stuff 
  } // next section
  // no match 
  return false ; // RETURN 
}
// ============================================================================
// add one more node to the tree 
// ============================================================================
LoKi::Decays::Trees::GenPhotosOptional&
LoKi::Decays::Trees::GenPhotosOptional::operator+=
( const LoKi::Decays::iTree_<const HepMC::GenParticle*>& tree ) 
{
  addDaughter ( tree ) ;
  return *this ;
}
// ============================================================================
// add one more node to the tree 
// ============================================================================
LoKi::Decays::Trees::GenPhotosOptional&
LoKi::Decays::Trees::GenPhotosOptional::operator+=
( const LoKi::Decays::iNode& node ) 
{
  addDaughter ( node ) ;
  return *this ;
}
// ============================================================================





// ============================================================================
// The END 
// ============================================================================
