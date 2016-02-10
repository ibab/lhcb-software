// $Id$
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel 
// ============================================================================
#include "GaudiKernel/ToStream.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/IToolSvc.h"
// ============================================================================
// PartProp
// ============================================================================
#include "Kernel/IParticlePropertySvc.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/ILoKiSvc.h"
#include "LoKi/iTree.h"
#include "LoKi/DecayFinder.h"
#include "LoKi/BasicFunctors.h"
#include "LoKi/ChildSelector.h"
#include "LoKi/Trees.h"
#include "LoKi/Services.h"
#include "LoKi/PhysExtract.h"
#include "LoKi/PhysAlgs.h"
#include "LoKi/Child.h"
#include "LoKi/Particles24.h"
#include "LoKi/ToCpp.h"
// ============================================================================
/** @file 
 *  implementation file for class LoKi::Child::Selector 
 *  @see LoKi::Child::Selector 
 *  @date 2010-05-29 
 *  @author Vanya Belyaev Ivan.Belyaev@nikhef.nl
 */
// ============================================================================
namespace 
{ 
  // ==========================================================================
  // Suppress Intel compiler warnings about missing default constructor
  // In this case the compiler generated constructor is fine, since there are
  // no member data to be initialised
#ifdef __INTEL_COMPILER
#pragma warning(disable:854)
#pragma warning(push)
#endif
  // ==========================================================================
  /// invalid decay
  const Decays::Trees::Types_<const LHCb::Particle*>::Invalid       s_INVALID        ;
  /// "None"-selector
  const LoKi::BasicFunctors<const LHCb::Particle*>::BooleanConstant s_NONE ( false ) ;
  /// "Factory"
  const std::string  s_FACTORY = "LoKi::Decay" ;
  // ==========================================================================
#ifdef __INTEL_COMPILER
#pragma warning(pop) 
#endif    
  // ==========================================================================
}
// ============================================================================
/* accessor to certain children particles for the given particle 
 *  @param  particle (INPUT) pointer to mother particle 
 *  @param  selector (INPUT) the selector 
 *  @param  result   (OUTPUT) the container of found particles
 *  @return number of found particles 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2010-05-29
 */
// ============================================================================
unsigned int LoKi::Child::children 
( const LHCb::Particle*        particle , 
  const LoKi::Child::Selector& selector , 
  LHCb::Particle::ConstVector& result   ) 
{
  if ( 0 == particle ) { return 0 ; }                               // RETURN 
  return selector.children ( particle , result ) ;
}
// ============================================================================
/*  accessor to certain children particles for the givenm particle 
 *  @param  particle (INPUT) pointer to mother particle 
 *  @param  selector (INPUT) the selector 
 *  @return the container of found particles
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2010-05-29
 */
// ============================================================================
LHCb::Particle::ConstVector
LoKi::Child::children 
( const LHCb::Particle*        particle , 
  const LoKi::Child::Selector& selector ) 
{
  LHCb::Particle::ConstVector result ;
  if ( 0 == particle ) { return result ; }                           // RETURN
  //
  children ( particle , selector , result ) ;
  return result ;                                                    
}
// ============================================================================
/* Trivial accessor to the daughter particles for the given particle.
 *  @param  particle (const) pointer to mother particle 
 *  @param  selector the selector 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2010-05-29
 */
// ============================================================================
const LHCb::Particle* 
LoKi::Child::child 
( const LHCb::Particle*        particle , 
  const LoKi::Child::Selector& selector ) 
{
  if ( 0 == particle ) { return 0 ; }                                // RETURN 
  return selector.child ( particle ) ; 
}
// ============================================================================
/* Trivial accessor to the daughter particles for the given particle.
 *  @param  particle (const) pointer to mother particle 
 *  @param  selector the selector 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2016-02-10
 */
// ============================================================================
const LHCb::Particle* 
LoKi::Child::child 
( const LHCb::Particle*        particle , 
  const Decays::iNode&         node     ) 
{ return nullptr == particle ? nullptr : 
    child ( particle , LoKi::Child::Selector ( node ) ) ; }    
// ============================================================================
/*  Trivial accessor to the daughter particles for the given particle.
 *  @param  particle (const) pointer to mother particle 
 *  @param  selector the selector 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2016-02-10
 */
// ============================================================================
const LHCb::Particle* 
LoKi::Child::child 
( const LHCb::Particle*        particle , 
  const Decays::IDecay::iTree& node     ) 
{ return nullptr == particle ? nullptr : 
    child ( particle , LoKi::Child::Selector ( node ) ) ; }    
// ============================================================================
/*  Trivial accessor to the daughter particles for the given particle.
 *  @param  particle (const) pointer to mother particle 
 *  @param  selector the selector 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2016-02-10
 */
// ============================================================================
const LHCb::Particle* 
LoKi::Child::child 
( const LHCb::Particle*         particle , 
  const Decays::IDecay::Finder& node     ) 
{ return nullptr == particle ? nullptr : 
    child ( particle , LoKi::Child::Selector ( node ) ) ; }    
// ============================================================================
/*  Trivial accessor to the daughter particles for the given particle.
 *  @param  particle (const) pointer to mother particle 
 *  @param  selector the selector 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2016-02-10
 */
// ============================================================================
const LHCb::Particle* 
LoKi::Child::child 
( const LHCb::Particle*         particle , 
  const std::string&            node     ) 
{ return nullptr == particle ? nullptr : 
    child ( particle , LoKi::Child::Selector ( node ) ) ; }
// ============================================================================
/* Trivial accessor to the daughter particles for the given particle.
 *  @param  particle (const) pointer to mother particle 
 *  @param  selector the selector 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2016-02-10
 */
// ============================================================================
const LHCb::Particle* 
LoKi::Child::child 
( const LHCb::Particle*         particle , 
  const LoKi::Types::Cuts&      node     ) 
{ return nullptr == particle ? nullptr : 
    child ( particle , LoKi::Child::Selector ( node ) ) ; }    
// ============================================================================
/* accessor to certain children particles for the given particle 
 *  @param  particle (INPUT) pointer to mother particle 
 *  @param  selector (INPUT) the selector 
 *  @return the container of found particles
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2010-05-29
 */
// ============================================================================
LHCb::Particle::ConstVector
LoKi::Child::children 
( const LHCb::Particle*        particle , 
  const Decays::iNode&         node     ) 
{ return nullptr == particle ? LHCb::Particle::ConstVector() 
    : children ( particle , LoKi::Child::Selector ( node ) ) ; }
// ============================================================================
/*  accessor to certain children particles for the given particle 
 *  @param  particle (INPUT) pointer to mother particle 
 *  @param  selector (INPUT) the selector 
 *  @return the container of found particles
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2010-05-29
 */
// ============================================================================
LHCb::Particle::ConstVector
LoKi::Child::children 
( const LHCb::Particle*        particle , 
  const Decays::IDecay::iTree& node     ) 
{ return nullptr == particle ? LHCb::Particle::ConstVector()  
    : children ( particle , LoKi::Child::Selector ( node ) ) ; }
// ============================================================================
/*  accessor to certain children particles for the given particle 
 *  @param  particle (INPUT) pointer to mother particle 
 *  @param  selector (INPUT) the selector 
 *  @return the container of found particles
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2010-05-29
 */
// ============================================================================
LHCb::Particle::ConstVector
LoKi::Child::children 
( const LHCb::Particle*         particle ,  
  const Decays::IDecay::Finder& node     ) 
{ return nullptr == particle ? LHCb::Particle::ConstVector()  
    : children ( particle , LoKi::Child::Selector ( node ) ) ; }
// ============================================================================
/*  accessor to certain children particles for the given particle 
 *  @param  particle (INPUT) pointer to mother particle 
 *  @param  selector (INPUT) the selector 
 *  @return the container of found particles
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2010-05-29
 */
// ============================================================================
LHCb::Particle::ConstVector
LoKi::Child::children 
( const LHCb::Particle*         particle ,  
  const std::string&            node     ) 
{ return nullptr == particle ? LHCb::Particle::ConstVector()  
    : children ( particle , LoKi::Child::Selector ( node ) ) ; }
// ============================================================================
/*  accessor to certain children particles for the given particle 
 *  @param  particle (INPUT) pointer to mother particle 
 *  @param  selector (INPUT) the selector 
 *  @return the container of found particles
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2010-05-29
 */
// ============================================================================
LHCb::Particle::ConstVector
LoKi::Child::children 
( const LHCb::Particle*         particle ,  
  const LoKi::Types::Cuts&      node     ) 
{ return nullptr == particle ? LHCb::Particle::ConstVector()  
    : children ( particle , LoKi::Child::Selector ( node ) ) ; }
// ============================================================================



// ============================================================================
// constructor from the index
// ============================================================================
LoKi::Child::Selector::Selector 
( const unsigned int   i  ) 
  : LoKi::AuxFunBase ( std::tie ( i ) ) 
  , m_indices ( 1 , i      ) 
  , m_finder  ( s_INVALID  )
  , m_setCut  ( false      ) 
  , m_cut     ( s_NONE     ) 
{}
// ============================================================================
// constructor from the index
// ============================================================================
LoKi::Child::Selector::Selector 
( const unsigned int   i1 , 
  const unsigned int   i2 ) 
  : LoKi::AuxFunBase ( std::tie ( i1 ,i2 ) ) 
  , m_indices (           ) 
  , m_finder  ( s_INVALID )
  , m_setCut  ( false     ) 
  , m_cut     ( s_NONE    ) 
{
  m_indices.push_back ( i1 ) ;
  m_indices.push_back ( i2 ) ;
}
// ============================================================================
// constructor from the index
// ============================================================================
LoKi::Child::Selector::Selector 
( const unsigned int   i1 , 
  const unsigned int   i2 ,
  const unsigned int   i3 ) 
  : LoKi::AuxFunBase ( std::tie ( i1 , i2 , i3 ) ) 
  , m_indices (           ) 
  , m_finder  ( s_INVALID )
  , m_setCut  ( false     ) 
  , m_cut     ( s_NONE    ) 
{
  m_indices.push_back ( i1 ) ;
  m_indices.push_back ( i2 ) ;
  m_indices.push_back ( i3 ) ;
}
// ============================================================================
// constructor from the index
// ============================================================================
LoKi::Child::Selector::Selector 
( const unsigned int   i1 , 
  const unsigned int   i2 ,
  const unsigned int   i3 , 
  const unsigned int   i4 )  
  : LoKi::AuxFunBase ( std::tie ( i1 , i2 , i3 , i4 ) ) 
  , m_indices (           ) 
  , m_finder  ( s_INVALID )
  , m_setCut  ( false     ) 
  , m_cut     ( s_NONE    ) 
{
  m_indices.push_back ( i1 ) ;
  m_indices.push_back ( i2 ) ;
  m_indices.push_back ( i3 ) ;
  m_indices.push_back ( i4 ) ;
}
// ============================================================================
// constructor from the index
// ============================================================================
LoKi::Child::Selector::Selector 
( const std::vector<unsigned int>& indices ) 
  : LoKi::AuxFunBase ( std::tie ( indices ) ) 
  , m_indices ( indices   ) 
  , m_finder  ( s_INVALID )
  , m_setCut  ( false     ) 
  , m_cut     ( s_NONE    ) 
{
  Assert ( valid()         , "The child selector is invalid"        ) ;
}
// ============================================================================
// constructor from decay tree 
// ============================================================================
LoKi::Child::Selector::Selector
 ( const Decays::IDecay::iTree& child ) 
   : LoKi::AuxFunBase ( std::tie ( child ) ) 
  , m_indices (        ) 
  , m_finder  ( child  )
  , m_setCut  ( false  ) 
  , m_cut     ( s_NONE ) 
{
  //
  if ( !m_finder ) 
  {
    LoKi::ILoKiSvc*  ls  = lokiSvc () ;
    SmartIF<LHCb::IParticlePropertySvc> pp ( ls ) ;
    const LHCb::IParticlePropertySvc* ppsvc = pp ;
    if ( 0 == ppsvc ) 
    {
      const LoKi::Services& services = LoKi::Services::instance() ;
      ppsvc = services.ppSvc() ;
    }
    StatusCode sc = m_finder.validate ( ppsvc ) ;
    Assert ( sc.isSuccess() , " Unable to validate the decay tree!" ) ;
  }
  //
  Assert ( valid()           , "The child selector is invalid"        ) ;
}
// ============================================================================
// constructor from decay node 
// ============================================================================
LoKi::Child::Selector::Selector ( const Decays::iNode& node ) 
  : LoKi::AuxFunBase ( std::tie ( node ) ) 
  , m_indices (           ) 
  , m_finder  ( s_INVALID )
  , m_setCut  ( false     ) 
  , m_cut     ( s_NONE    ) 
{
  //
  m_cut    = LoKi::Particles::DecNode ( node ) ;
  m_setCut = true ;
  //
  Assert ( valid()       , "The child selector is invalid"        ) ;
}
// ============================================================================
// constructor from decay tree 
// ============================================================================
LoKi::Child::Selector::Selector
( const Decays::IDecay::Finder& child ) 
  : LoKi::AuxFunBase ( std::tie ( child ) ) 
  , m_indices (        ) 
  , m_finder  ( child  )
  , m_setCut  ( false  ) 
  , m_cut     ( s_NONE ) 
{
  //
  if ( !m_finder ) 
  {
    LoKi::ILoKiSvc*  ls  = lokiSvc () ;
    SmartIF<LHCb::IParticlePropertySvc> pp ( ls ) ;
    const LHCb::IParticlePropertySvc* ppsvc = pp ;
    if ( 0 == ppsvc ) 
    {
      const LoKi::Services& services = LoKi::Services::instance() ;
      ppsvc = services.ppSvc() ;
    }
    StatusCode sc = m_finder.validate ( ppsvc ) ;
    Assert ( sc.isSuccess() , " Unable to validate the decay tree!" ) ;
  }
  //
  Assert ( valid()           , "The child selector is invalid"        ) ;
}
// ============================================================================
// constructor from the cut 
// ============================================================================
LoKi::Child::Selector::Selector ( const LoKi::Types::Cuts& cut ) 
  : LoKi::AuxFunBase ( std::tie ( cut ) ) 
  , m_indices (           ) 
  , m_finder  ( s_INVALID )
  , m_setCut  ( true      ) 
  , m_cut     ( cut       ) 
{}
// ============================================================================
// constructor from decay desctriptor 
// ============================================================================
LoKi::Child::Selector::Selector ( const std::string& child ) 
  : LoKi::AuxFunBase ( std::tie ( child ) ) 
  , m_indices (           ) 
  , m_finder  ( s_INVALID )
  , m_setCut  ( false     ) 
  , m_cut     ( s_NONE    ) 
{
  StatusCode sc = buildTree ( child , s_FACTORY ) ;
  Assert ( sc.isSuccess () , 
           "Unable to decode descriptor '" + child + "'" , sc ) ;
  Assert ( valid()           , "The child selector is ibvalid"        ) ;
}
// ============================================================================
// constructor from decay desctriptor & factory 
// ============================================================================
LoKi::Child::Selector::Selector 
( const std::string& child   , 
  const std::string& factory ) 
  : LoKi::AuxFunBase ( std::tie ( child , factory ) ) 
  , m_indices (           ) 
  , m_finder  ( s_INVALID )
  , m_setCut  ( false     ) 
  , m_cut     ( s_NONE    ) 
{
  StatusCode sc = buildTree ( child , factory ) ;
  Assert ( sc.isSuccess () , 
           "Unable to decode descriptor '" + child + "'" , sc ) ;
  Assert ( valid()           , "The child selector is invalid"        ) ;
}
// ============================================================================
// destructor  
// ============================================================================
LoKi::Child::Selector::~Selector () {}
// ============================================================================
// build the decay tree form the descriptor 
// ============================================================================
StatusCode LoKi::Child::Selector::buildTree
( const std::string& descriptor , 
  const std::string& factory    ) 
{
  LoKi::ILoKiSvc* ls = lokiSvc() ;
  SmartIF<IToolSvc> toolSvc ( ls ) ;
  if ( !toolSvc ) { return Error ( "getTree: unable to locate tool service" ) ; }
  
  Decays::IDecay* tool = 0 ;
  StatusCode sc = toolSvc -> retrieveTool ( factory , tool ) ;
  if ( sc.isFailure() )
  { return Error ( "getTree: Unable to retrieve '" + factory + "'" , sc ) ; }
  if ( 0 == tool      ) 
  { return Error ( "getTree: Decays::IDecay* points to NULL" ) ; }
  //
  m_finder = tool -> tree ( descriptor ) ;
  //
  toolSvc -> releaseTool ( tool ) ; // do not need the tool anymore 
  //
  if ( !m_finder           ) 
  { return Error   ( "getTree: the tree is invalid:  '" + descriptor + "'" ) ; }  
  if ( !m_finder.marked () ) 
  { Warning ( "getTree: the tree is unmarked: '" + descriptor + "'" ) ; }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// check the validity the child-selector 
// ============================================================================
bool LoKi::Child::Selector::valid () const 
{ return !m_indices.empty() || m_finder.valid () || m_setCut ; }
// ============================================================================
// try to validate it!
// ============================================================================
StatusCode LoKi::Child::Selector::validate 
( const LHCb::IParticlePropertySvc* svc ) const 
{
  if ( !m_indices.empty() || m_setCut ) { return StatusCode::SUCCESS ; }
  //
  StatusCode sc = m_finder.validate ( svc ) ;
  if ( sc.isFailure() ) 
  { return Error ("Unable to validate the tree/decay descriptor" , sc ) ; }
  // 
  return StatusCode::SUCCESS ;
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::Child::Selector::fillStream ( std::ostream& s ) const 
{
  //
  if ( !m_indices.empty() ) 
  {
    if      ( 1 == m_indices.size() ) 
    { return s <<  " " << m_indices[0] << " " ; }
    else if ( 2 == m_indices.size() ) 
    { return s <<  " " << m_indices[0] 
               << ", " << m_indices[1] << " " ; }
    else if ( 3 == m_indices.size() ) 
    { return s <<  " " << m_indices[0] 
               << ", " << m_indices[1]
               << ", " << m_indices[2] << " " ; }
    else if ( 4 == m_indices.size() ) 
    { return s <<  " " << m_indices[0] 
               << ", " << m_indices[1]
               << ", " << m_indices[2]
               << ", " << m_indices[3] << " " ; }
    return s << " " << Gaudi::Utils::toString ( m_indices ) << " " ; 
  }
  //
  if ( m_setCut           ) { return s << " " << m_cut  << " " ; }
  //
  return s << " " << m_finder.tree() << " " ;
}
// ============================================================================
/*  get the child according to the selector 
 *  @param mother    (INPUT) the head fo the decay tree 
 *  @param daughetrs (UPDATE) 
 *  @return unmber of particles 
 */
// ============================================================================
unsigned int LoKi::Child::Selector::children 
( const LHCb::Particle*        head      , 
  LHCb::Particle::ConstVector& daughters ) const 
{
  if ( 0 == head ) 
  {
    Error ( "children: LHCb::Particle* points to NULL" ) ;
    return 0 ;                                                   // RETURN 
  }
  //
  Assert ( valid () , "Selector is invalid!" ) ;
  //
  if      ( !m_indices.empty() ) 
  { 
    const LHCb::Particle* c = LoKi::Child::child ( head , m_indices ) ; 
    if ( 0 == c ) { return 0 ; }                                // RETURN 
    //
    daughters.push_back ( c ) ;
    return daughters.size() ;                                   // RETURN 
  }
  else if ( m_setCut           ) 
  {
    LoKi::Extract::particles ( head , std::back_inserter( daughters ) , m_cut ) ;
    return daughters.size() ;                                   // RETURN
  }
  //
  LHCb::Particle::ConstVector input ( 1 , head ) ;
  //
  return m_finder.findDecay ( input , daughters ) ;            // RETURN 
}
// ============================================================================
/*  get the child accoeding to the selector 
 *  @param  head (INPUT) the head fo the decay tree 
 *  @return child particle 
 */
// ============================================================================
const LHCb::Particle* LoKi::Child::Selector::child 
( const LHCb::Particle* head ) const 
{
  //
  if ( 0 == head ) 
  {
    Error ( "child: LHCb::Particle points to NULL" ) ;
    return 0 ;
  }
  //
  Assert ( valid () , "Selector is invalid!" ) ;
  //
  if ( !m_indices.empty() ) { return LoKi::Child::child ( head , m_indices ) ; }
  //
  if ( m_setCut ) { return LoKi::PhysAlgs::foundFirst ( head , m_cut ) ; }
  //
  LHCb::Particle::ConstVector daughters ;
  LHCb::Particle::ConstVector input ( 1 , head ) ;
  //
  m_finder.findDecay ( input , daughters ) ;
  //
  if (     daughters.empty () ) { return 0 ; }             // RETURN 
  //
  if ( 1 < daughters.size  () ) 
  { Warning ("child: >1 daughter particles are found, return the first") ; }
  //
  return daughters[0] ;
}
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
