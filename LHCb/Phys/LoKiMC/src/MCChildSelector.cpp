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
#include "LoKi/iTree.h"
#include "LoKi/DecayFinder.h"
#include "LoKi/ILoKiSvc.h"
#include "LoKi/BasicFunctors.h"
#include "LoKi/MCChildSelector.h"
#include "LoKi/Trees.h"
#include "LoKi/Services.h"
#include "LoKi/MCChild.h"
#include "LoKi/MCExtract.h"
#include "LoKi/MCAlgs.h"
#include "LoKi/MCParticles1.h"
// ============================================================================
/** @file 
 *  implementation file for class LoKi::MCChild::Selector 
 *  @see LoKi::Child::Selector 
 *  @date 2010-05-31 
 *  @author Vanya Belyaev Ivan.Belyaev@nikhef.nl
 */
// ============================================================================
namespace 
{ 
  // ==========================================================================
  /// invalid decay
  const Decays::Trees::Types_<const LHCb::MCParticle*>::Invalid       s_INVALID =
        Decays::Trees::Types_<const LHCb::MCParticle*>::Invalid();
  /// "None"-selector
  const LoKi::BasicFunctors<const LHCb::MCParticle*>::BooleanConstant s_NONE ( false ) ;
  /// "Factory"
  const std::string  s_FACTORY = "LoKi::MCDecay" ;
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
unsigned int LoKi::MCChild::children 
( const LHCb::MCParticle*        particle , 
  const LoKi::MCChild::Selector& selector , 
  LHCb::MCParticle::ConstVector& result   ) 
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
LHCb::MCParticle::ConstVector
LoKi::MCChild::children 
( const LHCb::MCParticle*        particle , 
  const LoKi::MCChild::Selector& selector ) 
{
  LHCb::MCParticle::ConstVector result ;
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
const LHCb::MCParticle* 
LoKi::MCChild::child 
( const LHCb::MCParticle*        particle , 
  const LoKi::MCChild::Selector& selector ) 
{
  if ( 0 == particle ) { return 0 ; }                                // RETURN 
  return selector.child ( particle ) ; 
}
// ============================================================================

// ============================================================================
// constructor from the index
// ============================================================================
LoKi::MCChild::Selector::Selector 
( const unsigned int   i  ) 
  : LoKi::AuxFunBase ( std::tie ( i ) ) 
  , m_indices   ( 1 , i      ) 
  , m_finder    ( s_INVALID  )
  , m_decay     (            ) 
  , m_setCut    ( false      ) 
  , m_cut       ( s_NONE     ) 
  , m_decayOnly ( false      )
  , m_factory   ( s_FACTORY  )
{}
// ============================================================================
// constructor from the index
// ============================================================================
LoKi::MCChild::Selector::Selector 
( const unsigned int   i1 , 
  const unsigned int   i2 ) 
  : LoKi::AuxFunBase ( std::tie ( i1 , i2 ) ) 
  , m_indices   (           ) 
  , m_finder    ( s_INVALID )
  , m_decay     (           ) 
  , m_setCut    ( false     ) 
  , m_cut       ( s_NONE    ) 
  , m_decayOnly ( false     ) 
  , m_factory   ( s_FACTORY  )
{
  m_indices.push_back ( i1 ) ;
  m_indices.push_back ( i2 ) ;
}
// ============================================================================
// constructor from the index
// ============================================================================
LoKi::MCChild::Selector::Selector 
( const unsigned int   i1 , 
  const unsigned int   i2 ,
  const unsigned int   i3 ) 
  : LoKi::AuxFunBase ( std::tie ( i1 , i2 , i3 ) )  
  , m_indices   (            ) 
  , m_finder    ( s_INVALID  )
  , m_decay     (            ) 
  , m_setCut    ( false      ) 
  , m_cut       ( s_NONE     ) 
  , m_decayOnly ( false      ) 
  , m_factory   ( s_FACTORY  )
{
  m_indices.push_back ( i1 ) ;
  m_indices.push_back ( i2 ) ;
  m_indices.push_back ( i3 ) ;
}
// ============================================================================
// constructor from the index
// ============================================================================
LoKi::MCChild::Selector::Selector 
( const unsigned int   i1 , 
  const unsigned int   i2 ,
  const unsigned int   i3 , 
  const unsigned int   i4 )  
  : LoKi::AuxFunBase ( std::tie ( i1 ,i2 , i3 , i4 ) ) 
  , m_indices   (           ) 
  , m_finder    ( s_INVALID )
  , m_decay     (           ) 
  , m_setCut    ( false     ) 
  , m_cut       ( s_NONE    ) 
  , m_decayOnly ( false      ) 
  , m_factory   ( s_FACTORY  )
{
  m_indices.push_back ( i1 ) ;
  m_indices.push_back ( i2 ) ;
  m_indices.push_back ( i3 ) ;
  m_indices.push_back ( i4 ) ;
}
// ============================================================================
// constructor from the index
// ============================================================================
LoKi::MCChild::Selector::Selector 
( const std::vector<unsigned int>& indices ) 
  : LoKi::AuxFunBase ( std::tie ( indices ) )  
  , m_indices   ( indices    ) 
  , m_finder    ( s_INVALID  )
  , m_setCut    ( false      ) 
  , m_cut       ( s_NONE     ) 
  , m_decayOnly ( false      ) 
  , m_factory   ( s_FACTORY  )
{}
// ============================================================================
// constructor from decay tree 
// ============================================================================
LoKi::MCChild::Selector::Selector
( const Decays::IMCDecay::iTree& child ) 
  : LoKi::AuxFunBase ( std::tie ( child ) ) 
  , m_indices   (        ) 
  , m_finder    ( child  )
  , m_decay     (        ) 
  , m_setCut    ( false  ) 
  , m_cut       ( s_NONE ) 
  , m_decayOnly ( false  ) 
  , m_factory   ( s_FACTORY  )
{
  if ( gaudi() ) 
  {
    StatusCode sc = validate() ; 
    Assert ( sc.isSuccess() , "Unable to validate decay tree" ) ;
  }
}
// ============================================================================
// constructor from decay onde  
// ============================================================================
LoKi::MCChild::Selector::Selector 
( const Decays::iNode& node      , 
  const bool           decayOnly ) 
  : LoKi::AuxFunBase ( std::tie ( node , decayOnly ) ) 
  , m_indices   (           ) 
  , m_finder    ( s_INVALID )
  , m_decay     (           ) 
  , m_setCut    ( false     ) 
  , m_cut       ( s_NONE    ) 
  , m_decayOnly ( decayOnly ) 
  , m_factory   ( s_FACTORY )
{
  //
  m_cut    = LoKi::MCParticles::DecNode ( node ) ;
  m_setCut = true ;
}
// ============================================================================
// constructor from decay tree 
// ============================================================================
LoKi::MCChild::Selector::Selector
( const Decays::IMCDecay::Finder& child ) 
  : LoKi::AuxFunBase ( std::tie ( child ) ) 
  , m_indices   (           )   
  , m_finder    ( child     )
  , m_decay     (           ) 
  , m_setCut    ( false     ) 
  , m_cut       ( s_NONE    ) 
  , m_decayOnly ( false     ) 
  , m_factory   ( s_FACTORY )
{
  if ( gaudi() ) 
  {
    StatusCode sc = validate() ; 
    Assert ( sc.isSuccess() , "Unable to validate decay finder" ) ;
  }
}
// ============================================================================
// constructor from the cut 
// ============================================================================
LoKi::MCChild::Selector::Selector 
( const LoKi::MCTypes::MCCuts& cut       ,
  const bool                   decayOnly ) 
  : LoKi::AuxFunBase ( std::tie ( cut , decayOnly ) ) 
  , m_indices   (           ) 
  , m_finder    ( s_INVALID )
  , m_decay     (           ) 
  , m_setCut    ( true      ) 
  , m_cut       ( cut       ) 
  , m_decayOnly ( decayOnly ) 
  , m_factory   ( s_FACTORY )
{}
// ============================================================================
// constructor from decay desctriptor 
// ============================================================================
LoKi::MCChild::Selector::Selector ( const std::string& child ) 
  : LoKi::AuxFunBase ( std::tie ( child ) ) 
  , m_indices   (           ) 
  , m_finder    ( s_INVALID )
  , m_decay     ( child     ) 
  , m_setCut    ( false     ) 
  , m_cut       ( s_NONE    ) 
  , m_decayOnly ( false     ) 
  , m_factory   ( s_FACTORY )
{
  if ( gaudi() ) 
  {
    StatusCode sc = validate() ; 
    Assert ( sc.isSuccess() , "Unable to decode '" + child + "'" ) ;
  }
}
// ============================================================================
// constructor from decay desctriptor & factory 
// ============================================================================
LoKi::MCChild::Selector::Selector 
( const std::string& child   , 
  const std::string& factory ) 
  : LoKi::AuxFunBase ( std::tie ( child , factory ) ) 
  , m_indices   (           ) 
  , m_finder    ( s_INVALID )
  , m_decay     ( child     ) 
  , m_setCut    ( false     ) 
  , m_cut       ( s_NONE    ) 
  , m_decayOnly ( false     ) 
  , m_factory   ( factory   )
{
  if ( gaudi() ) 
  {
    StatusCode sc = validate() ; 
    Assert ( sc.isSuccess() , "Unable to decode '" + child + "'" ) ;
  }
}
// ============================================================================
// destructor  
// ============================================================================
LoKi::MCChild::Selector::~Selector () {}
// ============================================================================
// build the decay tree form the descriptor 
// ============================================================================
StatusCode LoKi::MCChild::Selector::buildTree
( const std::string& descriptor , 
  const std::string& factory    ) const
{
  LoKi::ILoKiSvc* ls = lokiSvc() ;
  SmartIF<IToolSvc> toolSvc ( ls ) ;
  if ( !toolSvc ) { return Error ( "getTree: unable to locate tool service" ) ; }
  
  Decays::IMCDecay* tool = 0 ;
  StatusCode sc = toolSvc -> retrieveTool ( factory , tool ) ;
  if ( sc.isFailure() )
  { return Error ( "getTree: Unable to retrieve '" + factory + "'" , sc ) ; }
  if ( 0 == tool      ) 
  { return Error ( "getTree: Decays::IMCDecay* points to NULL" ) ; }
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
bool LoKi::MCChild::Selector::valid () const 
{ return !m_indices.empty() || m_finder.valid () || m_setCut ; }
// ============================================================================
// try to validate it!
// ============================================================================
StatusCode LoKi::MCChild::Selector::validate 
( const LHCb::IParticlePropertySvc* svc ) const 
{
  if ( !m_indices.empty() || m_setCut ) { return StatusCode::SUCCESS ; }
  //
  if ( !valid() && m_indices.empty() && !m_setCut && !m_decay.empty() ) 
  {
    StatusCode sc = buildTree ( m_decay , m_factory ) ;
    Assert ( sc.isSuccess() , "Unable to decode '" + m_decay + "'" ) ;  
  }
  //
  if ( 0 == svc ) 
  {
    LoKi::ILoKiSvc*  ls  = lokiSvc () ;
    SmartIF<LHCb::IParticlePropertySvc> pp ( ls ) ;
    svc = pp ;
  }
  //
  if ( 0 == svc ) 
  {
    const LoKi::Services& services = LoKi::Services::instance() ;
    svc = services.ppSvc() ;
  }
  //
  if ( 0 == svc ) 
  { return Error ("Unable to access ParticlePropertySvc" ) ; } 
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
std::ostream& LoKi::MCChild::Selector::fillStream ( std::ostream& s ) const 
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
  if ( m_setCut ) 
  { 
    s << " " << m_cut  ; 
    if ( !m_decayOnly ) { s << " , False" ; }
    return s << " " ;                                                // RETURN 
  }
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
unsigned int LoKi::MCChild::Selector::children 
( const LHCb::MCParticle*        head      , 
  LHCb::MCParticle::ConstVector& daughters ) const 
{
  if ( 0 == head ) 
  {
    Error ( "children: LHCb::MCParticle* points to NULL" ) ;
    return 0 ;                                                   // RETURN 
  }
  //
  if ( !valid() ) 
  {
    StatusCode sc = validate() ;
    Assert ( sc.isSuccess() , "Unable to validate child selector" );
  }
  //
  if      ( !m_indices.empty() ) 
  { 
    const LHCb::MCParticle* c = LoKi::MCChild::child ( head , m_indices ) ; 
    if ( 0 == c ) { return 0 ; }                                // RETURN 
    //
    daughters.push_back ( c ) ;
    return daughters.size() ;                                   // RETURN 
  }
  else if ( m_setCut           ) 
  {
    LoKi::Extract::mcParticles 
      ( head                            , 
        std::back_inserter( daughters ) , m_cut , m_decayOnly ) ;
    return daughters.size() ;                                   // RETURN
  }
  else 
  {
    LHCb::MCParticle::ConstVector input ( 1 , head ) ;
    return m_finder.findDecay ( input , daughters ) ;            // RETURN 
  }
}
// ============================================================================
/*  get the child according to the selector 
 *  @param  head (INPUT) the head fo the decay tree 
 *  @return child particle 
 */
// ============================================================================
const LHCb::MCParticle*
LoKi::MCChild::Selector::child 
( const LHCb::MCParticle* head ) const 
{
  //
  if ( 0 == head ) 
  {
    Error ( "child: LHCb::MCParticle points to NULL" ) ;
    return 0 ;
  }
  //
  if ( !valid() ) 
  {
    StatusCode sc = validate() ;
    Assert ( sc.isSuccess() , "Unable to validate child selector" );
  }
  //
  if ( !m_indices.empty() ) { return LoKi::MCChild::child ( head , m_indices ) ; }
  //
  if ( m_setCut ) { return LoKi::MCAlgs::foundFirst ( head        , 
                                                      m_cut       , 
                                                      m_decayOnly ) ; }
  //
  LHCb::MCParticle::ConstVector daughters ;
  LHCb::MCParticle::ConstVector input ( 1 , head ) ;
  //
  m_finder.findDecay ( input , daughters ) ;
  //
  if (     daughters.empty () ) { return 0 ; }             // REUTRN 
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
