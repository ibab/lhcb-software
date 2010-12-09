// $Id$
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <sstream>
// ============================================================================
// Event 
// ============================================================================
#include "Event/Track.h"
// ============================================================================
// Track Interfaces 
// ============================================================================
#include "TrackInterfaces/ITrackSelector.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Constants.h"
#include "LoKi/Tracks.h"
// ============================================================================
/** @file
 *  Implementation file for classes from the namespace LoKi::Tracks
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2007-08-08
 */
// ============================================================================
// MANDATORY: virtual destructor  
// ============================================================================
LoKi::Tracks::Key::~Key(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")  
// ============================================================================
LoKi::Tracks::Key* LoKi::Tracks::Key::clone() const { return new Key(*this) ; }
// ============================================================================
// mandatory: the only one essential method 
// ============================================================================
LoKi::Tracks::Key::result_type
LoKi::Tracks::Key::operator() 
  ( LoKi::Tracks::Key::argument t ) const 
{
  //
  if ( 0 == t ) 
  {
    Error ("LHCb::Track* points to NULL, return -1 ") ;
    return -1 ;
  }
  //
  return t->key() ;
}
// ============================================================================
// OPTIONAL: the nice printout 
// ============================================================================
std::ostream& 
LoKi::Tracks::Key::fillStream ( std::ostream& s ) const
{ return s << "TrKEY" ; }
// ============================================================================

// ============================================================================
// mandatory: the only one essential method 
// ============================================================================
LoKi::Tracks::InTES::result_type
LoKi::Tracks::InTES::operator() 
  ( LoKi::Tracks::InTES::argument t ) const 
{
  if ( 0 == t ) 
  {
    Error ("LHCb::Track* points to NULL, return false") ;
    return false;
  }
  return 0 != t->parent() ;
}
// ============================================================================
// mandatory: the only one essential method 
// ============================================================================
LoKi::Tracks::Charge::result_type
LoKi::Tracks::Charge::operator() 
  ( LoKi::Tracks::Charge::argument t ) const 
{
  if ( 0 == t ) 
  {
    Error ("LHCb::Track* points to NULL, return 'InvalidCharge'") ;
    return LoKi::Constants::InvalidCharge ;
  }
  return t->charge() ;
}
// ============================================================================
// mandatory: the only one essential method 
// ============================================================================
LoKi::Tracks::TransverseMomentum::result_type
LoKi::Tracks::TransverseMomentum::operator() 
  ( LoKi::Tracks::TransverseMomentum::argument t ) const 
{
  if ( 0 == t ) 
  {
    Error ("LHCb::Track* points to NULL, return 'InvalidMomentum'") ;
    return LoKi::Constants::InvalidMomentum ;
  }
  return t->pt() ;
}
// ============================================================================
// mandatory: the only one essential method 
// ============================================================================
LoKi::Tracks::Momentum::result_type
LoKi::Tracks::Momentum::operator() 
  ( LoKi::Tracks::Momentum::argument t ) const 
{
  if ( 0 == t ) 
  {
    Error ("LHCb::Track* points to NULL, return 'InvalidMomentum'") ;
    return LoKi::Constants::InvalidMomentum ;
  }
  return t->p() ;
}




// ============================================================================
// constructor from the flag 
// ============================================================================
LoKi::Tracks::CheckFlag::CheckFlag 
( LHCb::Track::Flags flag ) 
  : LoKi::BasicFunctors<const LHCb::Track*>::Predicate() 
  , m_flag ( flag ) 
{}
// ============================================================================
// OPTIONAL: the nice printout 
// ============================================================================
std::ostream& LoKi::Tracks::CheckFlag::fillStream( std::ostream& s ) const 
{ 
  switch ( m_flag ) 
  {
  case LHCb::Track::Backward    :
    return s << "TrBACKWARD"     ;                     // RETURN 
  case LHCb::Track::Invalid     : 
    return s << "TrINVALID"      ;                     // RETURN 
  case LHCb::Track::Clone       :
    return s << "TrCLONE"        ;                     // RETURN 
  case LHCb::Track::Used        :
    return s << "TrUSED"         ;                     // RETURN 
  case LHCb::Track::IPSelected  :
    return s << "TrIPSELECTED"   ;                     // RETURN 
  case LHCb::Track::PIDSelected :
    return s << "TrPIDSELECTED"  ;                     // RETURN 
  case LHCb::Track::Selected    :
    return s << "TrSELECTED"     ;                     // RETURN  
  case LHCb::Track::L0Candidate :
    return s << "TrL0CANDIDATE"  ;                     // RETURN 
  default :
    return s << "TrISFLAG("  << (int) m_flag << ")" ;  // RETURN 
  }
  return   s << "TrISFLAG("  << (int) m_flag << ")" ;
}
// ============================================================================
// mandatory: the only one essential method 
// ============================================================================
LoKi::Tracks::CheckFlag::result_type
LoKi::Tracks::CheckFlag::operator() 
  ( LoKi::Tracks::CheckFlag::argument t ) const 
{
  //
  if ( 0 == t ) 
  {
    Error ("LHCb::Track* points to NULL, return 'false'") ;
    return false ;
  }
  //
  return t->checkFlag ( m_flag )  ;
}
// ============================================================================
// constructor form the tool 
// ============================================================================
LoKi::Tracks::Selector::Selector 
( ITrackSelector* tool ) 
  : LoKi::BasicFunctors<const LHCb::Track*>::Predicate() 
  , m_tool ( tool ) 
{
  Assert ( m_tool.validPointer() , 
           "LoKi::Tracks::Selector: IParticleSelector* point to NULL" );  
}
// ============================================================================
// constructor form the tool 
// ============================================================================
LoKi::Tracks::Selector::Selector 
( const LoKi::Interface<ITrackSelector>& tool ) 
  : LoKi::BasicFunctors<const LHCb::Track*>::Predicate() 
  , m_tool ( tool ) 
{
  Assert ( m_tool.validPointer() , " IParticleSelector* point to NULL" );  
}
// ============================================================================
// copy constructor 
// ============================================================================
LoKi::Tracks::Selector::Selector 
( const LoKi::Tracks::Selector& right ) 
  : LoKi::AuxFunBase             ( right ) 
  , LoKi::BasicFunctors<const LHCb::Track*>::Predicate ( right ) 
  , m_tool ( right.m_tool ) 
{
  Assert ( m_tool.validPointer() , " IParticleSelector* point to NULL" );  
}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Tracks::Selector::result_type 
LoKi::Tracks::Selector::operator() 
  ( LoKi::Tracks::Selector::argument t ) const 
{
  if ( 0 == t ) 
  {
    Error ("LHCb::Track* points to NULL, return 'false'") ;
    return false ;
  }
  //
  Assert ( m_tool.validPointer() , "IParticleSelector* points to NULL" );  
  // use the tool 
  return m_tool->accept ( *t ) ; 
}
// ============================================================================
/*  constructor from "info"
 *  @param key info index/mark/key
 */
// ============================================================================
// constructor from the key 
// ============================================================================
LoKi::Tracks::HasInfo:: HasInfo 
( const int key ) 
  : LoKi::ExtraInfo::CheckInfo<const LHCb::Track*> ( key ) 
{}
// ============================================================================
// the specific printout 
// ============================================================================
std::ostream& 
LoKi::Tracks::HasInfo::fillStream( std::ostream& s ) const 
{ return s << "TrHASINFO(" << index() << ")" ; }
// ============================================================================
/* constructor from "info"
 *  @param key info index/mark/key
 *  @param def default value for missing key/invalid object 
 */
LoKi::Tracks::Info::Info
( const int    key , const double def )
  : LoKi::ExtraInfo::GetInfo<const LHCb::Track*> ( key , def ) 
{}
// ============================================================================
// the specific printout 
// ============================================================================
std::ostream& LoKi::Tracks::Info::fillStream( std::ostream& s ) const 
{ return s << "TrINFO(" << index() << "," << value() << ")" ; }
// ============================================================================
/*  constructor from the function, key and update-flag
 *  @param index the key in LHCb::Track::extraInfo table 
 *  @param fun functionto be evaluated for missing keys 
 *  @param update the flag to allow the insert of mnissing information
 */
// ============================================================================
LoKi::Tracks::SmartInfo::SmartInfo
( const int                                                index  , 
  const LoKi::BasicFunctors<const LHCb::Track*>::Function& fun    , 
  const bool                                               update ) 
  : LoKi::ExtraInfo::GetSmartInfo<const LHCb::Track*> ( index , fun , update )
{}
// ============================================================================
// the specific printout 
// ============================================================================
std::ostream& LoKi::Tracks::SmartInfo::fillStream( std::ostream& s ) const 
{ 
  s << "TrSINFO(" << func () << "," << index() << "," ;
  if ( update() ) { s << "True"  ; }
  else            { s << "False" ; }
  return s << ")" ;  
}
// ============================================================================
// constructor with the state indicator 
// ============================================================================
LoKi::Tracks::StateZ::StateZ 
( const LHCb::State::Location location )
  : LoKi::BasicFunctors<const LHCb::Track*>::Function () 
  , m_state ( location ) 
  , m_bad   ( LoKi::Constants::InvalidDistance ) 
{}
// ============================================================================
// constructor with the state indicator 
// ============================================================================
LoKi::Tracks::StateZ::StateZ 
( const LHCb::State::Location location , 
  const double                bad      )
  : LoKi::BasicFunctors<const LHCb::Track*>::Function () 
  , m_state ( location ) 
  , m_bad   ( bad      ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Tracks::StateZ::~StateZ (){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Tracks::StateZ*
LoKi::Tracks::StateZ::clone() const 
{ return new LoKi::Tracks::StateZ ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Tracks::StateZ::result_type 
LoKi::Tracks::StateZ::operator() 
  ( LoKi::Tracks::StateZ::argument t ) const 
{
  if ( 0 == t ) 
  {
    Error ("LHCb::Track* points to NULL, return 'bad'") ;
    return m_bad;
  }
  /// get the state:
  const LHCb::State* s = t->stateAt( m_state ) ;
  if ( 0 == s )  
  {
    Error ( "There is no state at " + state() + ", return 'bad' " ) ;
    return m_bad ;
  }
  return s->z () ; 
}
// ============================================================================
// get the string representation of the state 
// ============================================================================
const std::string& LoKi::Tracks::StateZ::state() const 
{
  if ( !m__state.empty() ) { return m__state ; }
  std::ostringstream ss ;
  ss << m_state ;
  m__state = ss.str() ;
  return m__state ;
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::Tracks::StateZ::fillStream ( std::ostream& s ) const 
{
  switch ( m_state ) 
  {
  case LHCb::State::FirstMeasurement : 
    return s << " TrFIRSTHITZ " ;
  default : 
    break ;
  }
  return s << " TrSTATEZ( LHCb.State." << state() << " ) " ;
}

// ============================================================================
// mandatory: the only one essential method 
// ============================================================================
LoKi::Tracks::Chi2::result_type 
LoKi::Tracks::Chi2::operator() 
  ( LoKi::Tracks::Chi2::argument t ) const 
{ 
  if ( 0 == t ) 
  {
    Error ("LHCb::Track* points to NULL, return 'InvaildChi2'") ;
    return LoKi::Constants::InvalidChi2 ;
  }
  return t->chi2() ; 
}
// ============================================================================
// mandatory: the only one essential method 
// ============================================================================
LoKi::Tracks::ProbChi2::result_type 
LoKi::Tracks::ProbChi2::operator() 
  ( LoKi::Tracks::ProbChi2::argument t ) const 
{ 
  if ( 0 == t ) 
  {
    Error ("LHCb::Track* points to NULL, return 'InvaildChi2'") ;
    return LoKi::Constants::InvalidChi2 ;
  }
  //
  return t->probChi2() ; 
}
// ============================================================================
// constructor 
// ============================================================================
LoKi::Tracks::HasStateAt::HasStateAt
( const LHCb::State::Location& loc )
  : LoKi::BasicFunctors<const LHCb::Track*>::Predicate () 
  , m_loc ( loc ) 
{}
// ============================================================================
// mandatory: the only one essential method
// ============================================================================
LoKi::Tracks::HasStateAt::result_type 
LoKi::Tracks::HasStateAt::operator() 
  ( LoKi::Tracks::HasStateAt::argument t ) const 
{ 
  if ( 0 == t ) 
  {
    Error ("LHCb::Track* points to NULL, return 'false'") ;
    return false ;
  }
  //
  return t->hasStateAt ( m_loc ) ; 
}
// ============================================================================
// constructor 
// ============================================================================
LoKi::Tracks::IsOnTrack::IsOnTrack
( const LHCb::LHCbID& id ) 
  : LoKi::BasicFunctors<const LHCb::Track*>::Predicate () 
  , m_id ( id ) 
{}
// ============================================================================
// mandatory: the only one essential method
// ============================================================================
LoKi::Tracks::IsOnTrack::result_type 
LoKi::Tracks::IsOnTrack::operator() 
  ( LoKi::Tracks::HasStateAt::argument t ) const 
{ 
  if ( 0 == t ) 
  {
    Error ("LHCb::Track* points to NULL, return 'false'") ;
    return false ;
  }
  //
  return t->isOnTrack( m_id ) ; 
}
// ============================================================================
// mandatory: the only one essential method
// ============================================================================
LoKi::Tracks::Type::result_type 
LoKi::Tracks::Type::operator() 
  ( LoKi::Tracks::Type::argument t ) const 
{ 
  if ( 0 == t ) 
  {
    Error ("LHCb::Track* points to NULL, return -1 ") ;
    return -1 ;
  }
  //
  return t->type() ; 
}
// ============================================================================


// ============================================================================
// constructor from indices
// ============================================================================
LoKi::Tracks::Cov2::Cov2 
( const unsigned short i , 
  const unsigned short j ) 
  : LoKi::BasicFunctors<const LHCb::Track*>::Function() 
  , m_case ( _First                        ) 
  , m_z    ( -1 * Gaudi::Units::km         ) 
  , m_loc  ( LHCb::State::FirstMeasurement ) 
  , m_i    ( i ) 
  , m_j    ( j ) 
{
  Assert ( m_i < 5 && m_j < 5 , "Invalid indices" ) ;
}
// ============================================================================
// constructor from indices & state location 
// ============================================================================
LoKi::Tracks::Cov2::Cov2 
( const LHCb::State::Location location , 
  const unsigned short        i        , 
  const unsigned short        j        ) 
  : LoKi::BasicFunctors<const LHCb::Track*>::Function() 
  , m_case ( _Location  ) 
  , m_z    ( -1 * Gaudi::Units::km         ) 
  , m_loc  ( location   ) 
  , m_i    ( i ) 
  , m_j    ( j ) 
{
  Assert ( m_i < 5 && m_j < 5 , "Invalid indices" ) ;
}
// ============================================================================
// constructor from indices and Z-position: 
// ============================================================================
LoKi::Tracks::Cov2::Cov2    
( const double                z , 
  const unsigned short        i , 
  const unsigned short        j ) 
  : LoKi::BasicFunctors<const LHCb::Track*>::Function() 
  , m_case ( _Z   ) 
  , m_z    (  z   ) 
  , m_loc  ( LHCb::State::FirstMeasurement ) 
  , m_i    ( i ) 
  , m_j    ( j ) 
{
  Assert ( m_i < 5 && m_j < 5 , "Invalid indices" ) ;
}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Tracks::Cov2::~Cov2(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Tracks::Cov2* LoKi::Tracks::Cov2::clone() const 
{ return new LoKi::Tracks::Cov2 ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Tracks::Cov2::result_type 
LoKi::Tracks::Cov2::operator()
  ( LoKi::Tracks::Cov2::argument t ) const
{
  if ( 0 == t ) 
  {
    Error ( "LHCb::Track* points to NULL, return NegativeInfnity" ) ;
    return LoKi::Constants::NegativeInfinity ;  
  }
  //
  const LHCb::State* state = 0 ;
  switch ( m_case ) 
  {
  case _Z         : 
    state = &(t->closestState ( m_z   ) )  ; break ;
  case _Location  :
    state =   t->stateAt      ( m_loc )    ; break ;
  default :
    state =  &t->firstState () ;
  }
  //
  if ( 0 == state ) 
  {
    Warning("LHCb::State* points to NULL, use 'FirstState'")  ;
    state = &t->firstState() ;    
  }
  //
  if ( 0 == state ) 
  {
    Error  ("LHCb::State* points to NULL, return 'NegativeInfinity'")  ;
    return LoKi::Constants::NegativeInfinity ;  
  }
  //
  return state -> covariance () ( m_i , m_j ) ;
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::Tracks::Cov2::fillStream ( std::ostream& s ) const 
{
  s << "TrCOV2(" ;
  //
  switch ( m_case ) 
  {
  case _Z        : 
    s << m_z << "," ; break ;
  case _Location : 
    s << "LHCb.State." << m_loc << "," ; break ;
  default : 
    s << "" ;
  }
  //
  return s << m_i << "," << m_j << ")" ;
}
// ============================================================================
// The END 
// ============================================================================
