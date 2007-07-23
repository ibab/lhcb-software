// $Id: Particles13.cpp,v 1.7 2007-07-23 17:35:54 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <algorithm>
// ============================================================================
// Event 
// ============================================================================
#include "Event/Track.h"
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/Constants.h"
#include "LoKi/Print.h"
// ============================================================================
// LoKiPhys
// ============================================================================
#include "LoKi/PhysExtract.h"
#include "LoKi/Particles13.h"
// ============================================================================
/** @file
 *
 *  Implementation file for functions from namespace  LoKi::Particles
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-02-22
 */
// ============================================================================


// ============================================================================
/// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::TrackChi2::result_type
LoKi::Particles::TrackChi2::operator() 
  ( LoKi::Particles::TrackChi2::argument p ) const 
{
  //
  if ( 0 == p ) 
  {
    Error ( " Argument is invalid! return 'InvalidChi2'" ) ;
    return LoKi::Constants::InvalidChi2 ;               // RETURN 
  }
  //
  const LHCb::Track* track = m_track ( p ) ;
  //
  if ( 0 == track ) 
  {
    Error ( " Track    is invalid! return 'InvalidChi2'" ) ;
    return LoKi::Constants::InvalidChi2 ;               // RETURN 
  }
  //
  return track->chi2 () ;                               // RETURN 
} ;
// ============================================================================
/// OPTIONAL:  the specific printout 
// ============================================================================
std::ostream& 
LoKi::Particles::TrackChi2::fillStream ( std::ostream& s ) const 
{ return s << "TRCHI2" ; }
// ============================================================================

// ============================================================================
/// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::TrackChi2PerDoF::result_type
LoKi::Particles::TrackChi2PerDoF::operator() 
  ( LoKi::Particles::TrackChi2PerDoF::argument p ) const 
{
  //
  if ( 0 == p ) 
  {
    Error ( " Argument is invalid! return 'InvalidChi2'" ) ;
    return LoKi::Constants::InvalidChi2 ;               // RETURN 
  }
  //
  const LHCb::Track* track = m_track ( p ) ;
  //
  if ( 0 == track ) 
  {
    Error ( " Track    is invalid! return 'InvalidChi2'" ) ;
    return LoKi::Constants::InvalidChi2 ;               // RETURN 
  }
  //
  return track->chi2PerDoF () ;                               // RETURN 
} ;
// ============================================================================
/// OPTIONAL:  the specific printout 
// ============================================================================
std::ostream& 
LoKi::Particles::TrackChi2PerDoF::fillStream ( std::ostream& s ) const 
{ return s << "TRCHI2DOF" ; }
// ============================================================================

// ============================================================================
LoKi::Particles::TrackHasState::TrackHasState
( const LHCb::State::Location location ) 
  : LoKi::Predicate<const LHCb::Particle*>()
  , m_location ( location ) 
{};
// ============================================================================
LoKi::Particles::TrackHasState::TrackHasState
( const LoKi::Particles::TrackHasState& right ) 
  : LoKi::AuxFunBase                      ( right )
  , LoKi::Predicate<const LHCb::Particle*>( right )
  , m_location                            ( right.m_location ) 
{};
// ============================================================================

// ============================================================================
/// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::TrackHasState::result_type
LoKi::Particles::TrackHasState::operator() 
  ( LoKi::Particles::TrackHasState::argument p ) const 
{
  //
  if ( 0 == p ) 
  {
    Error ( " Argument is invalid! return false " ) ;
    return false ;                                         // RETURN 
  }
  //
  const LHCb::Track* track = m_track ( p ) ;
  //
  if ( 0 == track ) 
  {
    Error ( " Track    is invalid! return false " ) ;
    return false ;                                          // RETURN 
  }
  //
  return track->hasStateAt ( m_location ) ;                // RETURN 
} ;
// ============================================================================
/// OPTIONAL:  the specific printout 
// ============================================================================
std::ostream& 
LoKi::Particles::TrackHasState::fillStream ( std::ostream& s ) const 
{ 
  s << "HASSTATE['" ;
  switch ( m_location) 
  {
  case LHCb::State::LocationUnknown  : s << "LocationUnknown"  ; break ;
  case LHCb::State::ClosestToBeam    : s << "ClosestToBeam"    ; break ;
  case LHCb::State::FirstMeasurement : s << "FirstMeasurement" ; break ;
  case LHCb::State::EndVelo          : s << "EndVelo"          ; break ;
  case LHCb::State::AtTT             : s << "AtTT"             ; break ;
  case LHCb::State::AtT              : s << "AtT"              ; break ;
  case LHCb::State::BegRich1         : s << "BegRich1"         ; break ;
  case LHCb::State::EndRich1         : s << "EndRich1"         ; break ;
  case LHCb::State::BegRich2         : s << "BegRich2"         ; break ;
  case LHCb::State::EndRich2         : s << "EndRich2"         ; break ;
  case LHCb::State::Spd              : s << "Spd"              ; break ;
  case LHCb::State::Prs              : s << "Prs"              ; break ;
  case LHCb::State::BegECal          : s << "BegECal"          ; break ;
  case LHCb::State::ECalShowerMax    : s << "ECalShowerMax"    ; break ;
  case LHCb::State::EndECal          : s << "EndECal"          ; break ;
  case LHCb::State::BegHCal          : s << "BegHCal"          ; break ;
  case LHCb::State::MidHCal          : s << "MidHCal"          ; break ;
  case LHCb::State::EndHCal          : s << "EndHCal"          ; break ;
  case LHCb::State::Muon             : s << "Muon"             ; break ;
  default : s << "?" <<m_location << "?" ; break ;  
  }
  return s << "']";
} ;
// ============================================================================

// ============================================================================
/// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::TrackType::result_type
LoKi::Particles::TrackType::operator() 
  ( LoKi::Particles::TrackType::argument p ) const 
{
  //
  if ( 0 == p ) 
  {
    Error ( " Argument is invalid! return -1000 " ) ;
    return -1000 ;                                      // RETURN 
  }
  //
  const LHCb::Track* track = m_track ( p ) ;
  //
  if ( 0 == track ) 
  {
    Error ( " Track    is invalid! return -1000 " ) ;
    return -1000 ;                                     // RETURN 
  }
  //
  return track->type() ;                 // RETURN 
} ;
// ============================================================================
/// OPTIONAL:  the specific printout 
// ============================================================================
std::ostream& 
LoKi::Particles::TrackType::fillStream ( std::ostream& s ) const 
{ return s << "TRTYPE" ;} ;
// ============================================================================

// ============================================================================
/// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::TrackHistory::result_type
LoKi::Particles::TrackHistory::operator() 
  ( LoKi::Particles::TrackHistory::argument p ) const 
{
  //
  if ( 0 == p ) 
  {
    Error ( " Argument is invalid! return -1000 " ) ;
    return -1000 ;                                      // RETURN 
  }
  //
  const LHCb::Track* track = m_track ( p ) ;
  //
  if ( 0 == track ) 
  {
    Error ( " Track    is invalid! return -1000 " ) ;
    return -1000 ;                                     // RETURN 
  }
  //
  return track->history() ;                 // RETURN 
} ;
// ============================================================================
/// OPTIONAL:  the specific printout 
// ============================================================================
std::ostream& 
LoKi::Particles::TrackHistory::fillStream ( std::ostream& s ) const 
{ return s << "TRHISTORY" ;} ;
// ============================================================================

// ============================================================================
/// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::TrackHistoryFit::result_type
LoKi::Particles::TrackHistoryFit::operator() 
  ( LoKi::Particles::TrackHistoryFit::argument p ) const 
{
  //
  if ( 0 == p ) 
  {
    Error ( " Argument is invalid! return -1000 " ) ;
    return -1000 ;                                      // RETURN 
  }
  //
  const LHCb::Track* track = m_track ( p ) ;
  //
  if ( 0 == track ) 
  {
    Error ( " Track    is invalid! return -1000 " ) ;
    return -1000 ;                                     // RETURN 
  }
  //
  return track->fitHistory() ;                 // RETURN 
} ;
// ============================================================================
/// OPTIONAL:  the specific printout 
// ============================================================================
std::ostream& 
LoKi::Particles::TrackHistoryFit::fillStream ( std::ostream& s ) const 
{ return s << "TRHISTFIT" ;} ;
// ============================================================================


// ============================================================================
/// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::TrackStatus::result_type
LoKi::Particles::TrackStatus::operator() 
  ( LoKi::Particles::TrackStatus::argument p ) const 
{
  //
  if ( 0 == p ) 
  {
    Error ( " Argument is invalid! return -1000 " ) ;
    return -1000 ;                                      // RETURN 
  }
  //
  const LHCb::Track* track = m_track ( p ) ;
  //
  if ( 0 == track ) 
  {
    Error ( " Track    is invalid! return -1000 " ) ;
    return -1000 ;                                     // RETURN 
  }
  //
  return track->fitStatus() ;                 // RETURN 
} ;
// ============================================================================
/// OPTIONAL:  the specific printout 
// ============================================================================
std::ostream& 
LoKi::Particles::TrackStatus::fillStream ( std::ostream& s ) const 
{ return s << "TRSTATUS" ;} ;
// ============================================================================



// ============================================================================
// The END 
// ============================================================================

