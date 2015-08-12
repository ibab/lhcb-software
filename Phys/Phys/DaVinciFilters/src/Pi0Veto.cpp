// $Id: $
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/SystemOfUnits.h"
// ============================================================================
// LoKiCore
// ============================================================================
#include "LoKi/select.h"
// ============================================================================
// LoKiPhys 
// ============================================================================
#include "LoKi/IHybridFactory.h"
#include "LoKi/Photons.h"
#include "LoKi/Child.h"
#include "LoKi/PhysExtract.h"
// ============================================================================
// (local) DaCinciTools
// ============================================================================
#include "Pi0Veto.h"
// ============================================================================
/** @file Pi0Veto.cpp
 *
 *  Implmenentation of components to deal with pi0-veto
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch 
 *  @date 2011-11-20
 *  
 *                    $Revision$
 *  Last Modification $Date$
 *                 by $Author$
 */
// ============================================================================
//                      Filter 
// ============================================================================
// standard constructor
// ============================================================================
Pi0Veto::Filter::Filter
( const std::string& name , 
  ISvcLocator*       pSvc ) 
  : FilterDesktop ( name , pSvc ) 
//
  , m_massWindow  (  20 * Gaudi::Units::MeV ) 
  , m_massChi2    (  -1                     )  
  , m_pi0Mass     ( 135 * Gaudi::Units::MeV ) 
//
{
  //
  declareProperty 
    ( "MassWindow" , 
      m_massWindow , 
      "Half-width of mass-window around pi0mass for veto" ) 
    -> declareUpdateHandler ( &Filter::updateHandler20 , this ) ;
  //
  declareProperty 
    ( "MassChi2"   , 
      m_massChi2   , 
      "Mass-chi2 for pi0 hypothesis to be vetoed" ) 
    -> declareUpdateHandler ( &Filter::updateHandler21 , this ) ;
  //
  declareProperty 
    ( "Pi0Mass"    , 
      m_pi0Mass    , 
      "Pi0Mass to be used in veto" ) 
    -> declareUpdateHandler ( &Filter::updateHandler22 , this ) ;
  //
  // reset FilterDesktop property to select the basic photons only
  StatusCode sc = setProperty ( "Code" , " \"gamma\" == ID " ) ;
  Assert ( sc.isSuccess() , "Unable to set property 'Code'" , sc ) ;
  //
}
// ============================================================================
// virtual and protected destructor
Pi0Veto::Filter::~Filter (){}
// ============================================================================
void Pi0Veto::Filter::updateHandler20 ( Property& p ) 
{
  // no action if not initialized yet:
  if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
  //
  info () << "The updated property is: " << p << endreq ;
  //
  if ( 0 >= m_massWindow ) 
  { warning() << "The mass-window for veto is disabled " << endreq ; }
}
// ============================================================================  
void Pi0Veto::Filter::updateHandler21 ( Property& p ) 
{
  // no action if not initialized yet:
  if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
  //
  info () << "The updated property is: " << p << endreq ;
  //
  if ( 0 >= m_massChi2 ) 
  { warning() << "The chi2 for veto is disabled " << endreq ; }
}
// ============================================================================  
void Pi0Veto::Filter::updateHandler22 ( Property& p ) 
{
  // no action if not initialized yet:
  if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
  //
  info () << "The updated property is: " << p << endreq ;
  //
  if ( 0 >= m_pi0Mass ) 
  { error () << "Pi0 mass is invalid" << endreq ; }
}
// ============================================================================  
// the standard initialization 
StatusCode Pi0Veto::Filter::initialize() 
{
  StatusCode sc = FilterDesktop::initialize () ;
  if ( sc.isFailure() ) { return sc ; }
  //
  if ( 0 >= m_massWindow ) 
  { Warning ( "Mass-window for pi0-veto is disabled " , StatusCode::SUCCESS ) ; }
  if ( 0 >= m_massChi2   ) 
  { Warning ( "Mass-chi2   for pi0-veto is disabled " , StatusCode::SUCCESS ) ; }
  // 
  if ( 0 >= m_pi0Mass ) 
  { return Warning ( "Invalid pi0-mass is specified ") ; }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================  
/*  the major method for filter input particles 
 *  @param input    (INPUT) the input  container of particles
 *  @param filtered (OUPUT) the output container of particles
 *  @return Status code 
 */
// ============================================================================  
StatusCode Pi0Veto::Filter::filter 
( const LHCb::Particle::ConstVector& input    , 
  LHCb::Particle::ConstVector&       filtered )
{
  // 
  filtered.clear() ;
  //
  LHCb::Particle::ConstVector photons ;
  photons.reserve ( input.size() ) ;
  //
  // Filter particles!! 
  LoKi::select ( input.begin () , 
                 input.end   () , 
                 std::back_inserter ( photons ) , predicate()  ) ;
  //
  // Apply pi0 veto!
  // 
  if      ( massWindow() < 0 && massChi2() < 0 ) { filtered = photons ; }
  else if ( photons.empty()                    ) { filtered = photons ; }
  else 
  {
    LoKi::Photons::pi0Veto 
      ( photons , 
        massWindow () , 
        massChi2   () , 
        filtered      , 
        pi0Mass    () ) ;
  }
  //
  // mark & store filtered particles in DVAlgorithm local container 
  markParticles ( filtered ) ;
  // 
  // some countings 
  StatEntity& cnt = counter ( "efficiency" ) ;
  //
  const size_t size1 = filtered  . size () ;
  const size_t size2 = input     . size () ;
  //
  for ( size_t i1 = 0     ; size1 > i1 ; ++i1 ) { cnt += true  ; }
  for ( size_t i2 = size1 ; size2 > i2 ; ++i2 ) { cnt += false ; }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
//                      Tagger 
// ============================================================================
// standard constructor
// ============================================================================
Pi0Veto::Tagger::Tagger
( const std::string& name , 
  ISvcLocator*       pSvc ) 
  : Pi0Veto::Filter ( name , pSvc ) 
//
  , m_index         ( 0  )
  , m_photonCutCode ( " \"gamma\" == ID " ) 
  , m_photonCut     ( LoKi::BasicFunctors<const LHCb::Particle*>::BooleanConstant( false ) )
  , m_photons       ()
//
{
  //
  declareProperty 
    ( "ExtraInfoIndex"  , 
      m_index           , 
      "Index in ExtraInfo object to store the result" ) 
    -> declareUpdateHandler ( &Tagger::updateHandler30 , this ) ;  
  //
  FilterDesktop* fd = this ;
  declareProperty 
    ( "PhotonCut"       , 
      m_photonCutCode   , 
      "The selection criteria for ``another'' photon" ) 
    -> declareUpdateHandler ( &FilterDesktop::updateHandler1 , fd ) ;
  //
  m_photons.push_back ( "/Event/Phys/StdLooseAllPhotons/Particles" ) ;
  declareProperty 
    ( "Photons"        , 
      m_photons        , 
      "TES-locations for ``other'' phtoons to be tested" ) 
    -> declareUpdateHandler ( &Tagger::updateHandler30 , this ) ;  
  //
}
// ============================================================================
// virtual and protected destructor
Pi0Veto::Tagger::~Tagger (){}
// ============================================================================
void Pi0Veto::Tagger::updateHandler30 ( Property& p ) 
{
  // no action if not initialized yet:
  if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
  //
  info () << "The updated property is: " << p << endreq ;
  //
  if ( LHCb::Particle::LastGlobal >= m_index  ) 
  { error () << "Invalid index for ExtraInfo" << endreq ; }
  //
}
// ============================================================================/
namespace 
{
  // ==========================================================================
  std::string short_name ( const std::string& loc ) 
  {
    std::string::size_type i1 = loc. find ( "/Event/Phys/" ) ;
    std::string::size_type i2 = loc.rfind ( "/Particles"   ) ;
    //
    if      ( std::string::npos == i1 ) { i1 =  0 ; }
    else if ( 0 == i1                 ) { i1 = 12 ; }
    //
    if      ( std::string::npos == i2 ) { i2 = loc.length() ; }
    //
    return loc.substr ( i1 , i2 ) ;
  }
  // ==========================================================================
  std::string long_name ( std::string loc )
  {
    if ( std::string::npos == loc.rfind ("/Particles") ) 
    { loc += "/Particles" ; }
    //
    if      ( 0 == loc.find ("/Event/Phys/" ) )  { return             loc ; }
    else if ( 0 == loc.find ("Phys/"        ) )  { return "/Event/" + loc ; }     
    //
    return "/Event/Phys/" + loc ;
  }
  // ==========================================================================   
}
// ============================================================================
void Pi0Veto::Tagger::updateHandler31 ( Property& p ) 
{
  //
  // no action if not initialized yet:
  if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
  //
  info () << "The updated property is: " << p  << endreq ;
  //
  for ( std::vector<std::string>::iterator iloc = 
          m_photons.begin() ; m_photons.end() != iloc ; ++iloc ) 
  { *iloc = long_name ( *iloc ) ; }
  //
  info () << "The updated property is: " << m_photons << endreq ;
  //
}
// ============================================================================
// the standard initialization 
// ============================================================================
StatusCode Pi0Veto::Tagger::initialize() 
{
  StatusCode sc = FilterDesktop::initialize () ;
  if ( sc.isFailure() ) { return sc ; }
  //
  //
  if ( m_photons.empty() ) 
  { Warning ( "No photon locations are specified" , StatusCode::SUCCESS ) ; }
  //
  for ( std::vector<std::string>::iterator iloc = 
          m_photons.begin() ; m_photons.end() != iloc ; ++iloc ) 
  { *iloc = long_name ( *iloc ) ; }
  //
  if ( LHCb::Particle::LastGlobal >= m_index  ) 
  { return Error ( "Invalid index for ExtraInfo" ) ; }
  ///
  return StatusCode::SUCCESS ;  
}
// ============================================================================  
StatusCode Pi0Veto::Tagger::decodeCode () 
{
  StatusCode sc = FilterDesktop::decodeCode () ;
  if ( sc.isFailure() ) { return sc ; }
  //
  // locate the factory
  LoKi::IHybridFactory* factory_ = tool<LoKi::IHybridFactory> ( factory() , this ) ;
  //
  // use the factory
  sc = factory_ -> get ( m_photonCutCode , 
                         m_photonCut     , 
                         preambulo()     ) ;
  if ( sc.isFailure() ) 
  { return Error ( "Error from LoKi/Bender 'hybrid' factory for PhotonCut='" 
                   + m_photonCutCode + "'" , sc )  ; }
  //
  release ( factory_ ) ;
  //
  return sc ;
}
// ============================================================================
/*  get the photons 
 *  @attention RootInTES is *NOT* used here (on purpose!)
 *  @param other (output) the photons 
 *  @return number of photons 
 */
// ============================================================================
unsigned int 
Pi0Veto::Tagger::getPhotons ( LHCb::Particle::ConstVector& other ) const 
{
  //
  other.clear   (     ) ;
  other.reserve ( 150 ) ;
  //
  for ( std::vector<std::string>::const_iterator iloc = 
          m_photons.begin() ; m_photons.end() != iloc ; ++iloc ) 
  {
    //
    std::string sname = short_name ( *iloc ) ;
    //
    if ( !exist<LHCb::Particle::Range> ( *iloc , false ) ) 
    {
      Warning ( "No particles at \"" + sname + "\"" ) ;
      continue ;
    }
    /// get particles form TES 
    LHCb::Particle::Range input = get<LHCb::Particle::Range> ( *iloc , false ) ;
    
    ///
    std::size_t s1 = other.size() ;
    /// 
    LoKi::Extract::particles ( input.begin () , 
                               input.end   () , 
                               std::back_inserter ( other ) , 
                               photonCut   () ) ;
    //
    counter ( "# particles \"" + sname + "\"") += input.size()      ;
    counter ( "# photons   \"" + sname + "\"") += other.size() - s1 ;
    //
  }
  //
  return other.size() ;
}
// ================================================================================
/*  the major method for gamma tagging of input particles 
 *  @param input    (INPUT) the input  container of particles
 *  @param filtered (OUPUT) the output container of particles
 *  @return Status code 
 */
// ================================================================================
StatusCode Pi0Veto::Tagger::filter 
( const LHCb::Particle::ConstVector& input    , 
  LHCb::Particle::ConstVector&       filtered ) 
{
  //
  // get all photons from the input 
  LHCb::Particle::ConstVector photons ;
  photons.reserve ( 2 * input.size() ) ;
  //
  LoKi::Extract::particles ( input.begin () , 
                             input.end   () , 
                             std::back_inserter ( photons  ) , 
                             predicate   () ) ;
  //
  if ( photons.empty() ) 
  {
    filtered   = input ;
    markParticles ( filtered ) ;
    return StatusCode::SUCCESS ;    
  }
  //
  LHCb::Particle::ConstVector other ;
  getPhotons ( other ) ;
  //
  // start some a bit illegal action:
  //
  StatEntity& cnt = counter ("#tagged") ;
  for (  LHCb::Particle::ConstVector::const_iterator iph = 
           photons.begin() ; photons.end() != iph ; ++iph ) 
  {
    const LHCb::Particle* gamma = *iph ;
    if ( 0 == gamma ) { continue ; }
    //
    LHCb::Particle* _gamma = const_cast<LHCb::Particle*> ( gamma ) ;
    //
    const bool result = LoKi::Photons::pi0Veto ( gamma         , 
                                                 other         , 
                                                 massWindow () , 
                                                 massChi2   () , 
                                                 pi0Mass    () ) ;
    // erase info
    if ( _gamma->hasInfo( m_index ) ) 
    {
      //
      const double old_result = _gamma->info ( m_index , result ) ;
      //
      if      (  result &&  old_result ) {}
      else if ( !result && !old_result ) {}
      else 
      { Warning ( "ExtraInfo to be replaced" , 1 , StatusCode::SUCCESS ) ; }  
      //
      _gamma -> eraseInfo ( m_index ) ; 
    }
    //
    _gamma->addInfo ( m_index , result ) ;
    //
    cnt += result ;
  }
  //
  filtered = input ;
  markParticles ( filtered ) ;
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
//                      Tagger-2gamma 
// ============================================================================
// standard constructor
// ============================================================================
Pi0Veto::Tagger2g::Tagger2g
( const std::string& name , 
  ISvcLocator*       pSvc ) 
  : Pi0Veto::Tagger ( name , pSvc ) 
//
{
  // reset FilterDesktop property to select the basic di-photons 
  StatusCode sc = setProperty ( "Code" , " DECTREE(\" X0 -> gamma  gamma \") " ) ;
  Assert ( sc.isSuccess() , "Unable to set property 'Code'" , sc ) ;
  //
}
// ============================================================================
// virtual and protected destructor
Pi0Veto::Tagger2g::~Tagger2g (){}
// ============================================================================
/*  the major method for gamma tagging of input particles 
 *  @param input    (INPUT) the input  container of particles
 *  @param filtered (OUPUT) the output container of particles
 *  @return Status code 
 */
// ============================================================================
StatusCode Pi0Veto::Tagger2g::filter 
( const LHCb::Particle::ConstVector& input    , 
  LHCb::Particle::ConstVector&       filtered ) 
{
  //
  // get all digammas from the input 
  //
  LHCb::Particle::ConstVector digamma ;
  digamma.reserve ( input.size() ) ;
  //
  LoKi::Extract::particles ( input.begin () , 
                             input.end   () , 
                             std::back_inserter ( digamma ) , 
                             predicate   () ) ;
  //
  if ( digamma.empty() ) 
  {
    filtered   = input ;
    markParticles ( filtered ) ;
    return StatusCode::SUCCESS ;    
  }
  //
  LHCb::Particle::ConstVector other ;
  getPhotons ( other ) ;
  //
  // start some a bit illegal action:
  //
  StatEntity& cnt = counter("#tagged") ;
  for (  LHCb::Particle::ConstVector::const_iterator i2g = 
           digamma.begin() ; digamma.end() != i2g ; ++i2g ) 
  {
    const LHCb::Particle* diphoton = *i2g ;
    if ( 0 == diphoton ) { continue ; }
    //
    const LHCb::Particle* gamma1 = LoKi::Child::child ( diphoton , 1 ) ;
    if ( 0 == gamma1 ) 
    { Error ( "Invalid first  photon" ) ; continue ; }    
    const LHCb::Particle* gamma2 = LoKi::Child::child ( diphoton , 2 ) ;
    if ( 0 == gamma2 ) 
    { Error ( "Invalid second photon" ) ; continue ; }
    //
    LHCb::Particle* _diphoton = const_cast<LHCb::Particle*> ( diphoton ) ;
    //
    const bool result = LoKi::Photons::pi0Veto ( gamma1        ,
                                                 gamma2        , 
                                                 other         , 
                                                 massWindow () , 
                                                 massChi2   () , 
                                                 pi0Mass    () ) ;
    //
    // erase info
    if ( _diphoton -> hasInfo ( index() ) ) 
    {
      //
      const double old_result = _diphoton->info ( index() , result ) ;
      //
      if      (  result &&  old_result ) {}
      else if ( !result && !old_result ) {}
      else 
      { Warning ( "ExtraInfo to be replaced" , 1 , StatusCode::SUCCESS ) ; }  
      //
      _diphoton -> eraseInfo  ( index() ) ; 
    }
    //
    _diphoton->addInfo  ( index (), result ) ;
    //
    cnt += result ;
  }
  //
  filtered = input ;
  markParticles ( filtered ) ;
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================

// ============================================================================
/// the factories  
DECLARE_NAMESPACE_ALGORITHM_FACTORY(Pi0Veto,Filter) 
DECLARE_NAMESPACE_ALGORITHM_FACTORY(Pi0Veto,Tagger) 
DECLARE_NAMESPACE_ALGORITHM_FACTORY(Pi0Veto,Tagger2g) 
// ============================================================================
//                                                                      The END 
// ============================================================================
