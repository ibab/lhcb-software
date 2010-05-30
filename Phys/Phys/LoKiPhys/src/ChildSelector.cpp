// $Id: ChildSelector.cpp,v 1.1 2010-05-30 11:04:23 ibelyaev Exp $
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
#include "LoKi/BasicFunctors.h"
#include "LoKi/ChildSelector.h"
#include "LoKi/Trees.h"
#include "LoKi/Services.h"
#include "LoKi/PhysExtract.h"
#include "LoKi/Child.h"
// ============================================================================
/** @file 
 *  implementation file for class LoKi::ChildSelector 
 *  @see LoKi::ChidlSelector 
 *  @date 2010-05-29 
 *  @author Vanya Belyaev Ivan.Belyaev@nikhef.nl
 */
// ============================================================================
namespace 
{ 
  // ==========================================================================
  /// invalid decay
  const Decays::Trees::Types_<const LHCb::Particle*>::Invalid       s_INVALID        ;
  /// "None"-selector
  const LoKi::BasicFunctors<const LHCb::Particle*>::BooleanConstant s_NONE ( false ) ;
  /// "Factory"
  const std::string  s_FACTORY = "LoKi::Decay" ;
  // ==========================================================================
}
// ============================================================================
/*  Trivial accessor to the daughter particles for the given particle.
 *  @param  particle (const) pointer to mother particle 
 *  @param  seelctor the selector 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2010-05-29
 */
// ============================================================================
const LHCb::Particle* LoKi::Child::child 
( const LHCb::Particle*        particle , 
  const LoKi::Child::Selector& selector ) 
{
  if ( 0 == particle ) { return 0 ; }
  return selector.child ( particle ) ;
}
// ============================================================================
// constructor from the index
// ============================================================================
LoKi::Child::Selector::Selector 
( const unsigned short i  ) 
  : LoKi::AuxFunBase () 
  , m_indices ( 1 , i      ) 
  , m_finder  ( s_INVALID  )
  , m_setCut  ( false      ) 
  , m_cut     ( s_NONE     ) 
{}
// ============================================================================
// constructor from the index
// ============================================================================
LoKi::Child::Selector::Selector 
( const unsigned short i1 , 
  const unsigned short i2 ) 
  : LoKi::AuxFunBase () 
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
( const unsigned short i1 , 
  const unsigned short i2 ,
  const unsigned short i3 ) 
  : LoKi::AuxFunBase () 
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
( const unsigned short i1 , 
  const unsigned short i2 ,
  const unsigned short i3 , 
  const unsigned short i4 )  
  : LoKi::AuxFunBase () 
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
( const std::vector<std::size_t>& indices ) 
  : LoKi::AuxFunBase () 
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
  : LoKi::AuxFunBase () 
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
// constructor from decay tree 
// ============================================================================
LoKi::Child::Selector::Selector
( const Decays::IDecay::Finder& child ) 
  : LoKi::AuxFunBase () 
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
  : LoKi::AuxFunBase () 
  , m_indices (           ) 
  , m_finder  ( s_INVALID )
  , m_setCut  ( true      ) 
  , m_cut     ( cut       ) 
{}
// ============================================================================
// constructor from decay desctriptor 
// ============================================================================
LoKi::Child::Selector::Selector ( const std::string& child ) 
  : LoKi::AuxFunBase () 
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
  : LoKi::AuxFunBase () 
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
  { return s << " " << Gaudi::Utils::toString ( m_indices ) ; }
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
std::size_t LoKi::Child::Selector::children 
( const LHCb::Particle*        head      , 
  LHCb::Particle::ConstVector& daughters ) const 
{
  if ( 0 == head ) 
  {
    Error ( "children: LHCb::Particle points to NULL" ) ;
    return 0 ;                                                  // RETURN 
  }
  //
  Assert ( valid () , "Selector is invalid!" ) ;
  //
  if      ( !m_indices.empty() ) 
  { 
    const LHCb::Particle* c = LoKi::Child::child ( head , m_indices ) ; 
    if ( 0 == c ) 
    { 
      Warning ("children: no chilndre are selected (1);") ;
      return 0 ;                                                // RETURN 
    }
    daughters.push_back ( c ) ;
    return daughters.size() ;                                   // RETURN 
  }
  else if ( m_setCut           ) 
  {
    LoKi::Extract::particles ( head , std::back_inserter( daughters ) , m_cut ) ;
    return daughters.size() ;                                   // RETURN
  }
  else 
  {
    LHCb::Particle::ConstVector input ( 1 , head ) ;
    return m_finder.findDecay ( input , daughters ) ;            // RETURN 
  }
  //
  return 0 ;                                                     // RETURN 
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
  LHCb::Particle::ConstVector result ;
  this->children ( head , result ) ;
  //
  if ( result.empty() )
  {
    Error ("child: No valid child is found, retutn NULL" ) ;
    return 0 ;
  }
  //
  if ( 1 < result.size() )
  { Warning ("child: too many children are found, return the first" ) ; }
  //
  return result[0] ;
}

 

// ============================================================================
// The END 
// ============================================================================
