// $Id: O1.cpp,v 1.1.1.1 2008-09-21 14:41:20 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ToStream.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/O1.h"
// ============================================================================
/** @file 
 *  Implementation file for classes from namesapce LoKi::O1  
 *  @date 2008-09-17 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 */
// ============================================================================
// the constructor from the start/stop times
// ============================================================================
LoKi::O1::InTime::InTime 
( const Gaudi::Time& start , 
  const Gaudi::Time& stop  ) 
  : LoKi::BasicFunctors<const LHCb::ODIN*>::Predicate() 
  , m_start ( start ) 
  , m_stop  ( stop  ) 
{}
// ============================================================================
// the constructor from the start&span times
// ============================================================================
LoKi::O1::InTime::InTime 
( const Gaudi::Time&     start , 
  const Gaudi::TimeSpan& span  ) 
  : LoKi::BasicFunctors<const LHCb::ODIN*>::Predicate() 
  , m_start ( start        ) 
  , m_stop  ( start + span ) 
{}
// ============================================================================
// MANDATORY: The only one essential method:
// ============================================================================
LoKi::O1::InTime::result_type 
LoKi::O1::InTime::operator() 
  ( LoKi::O1::InTime::argument o ) const 
{
  const Gaudi::Time time = o -> eventTime () ;
  //
  return m_start <= time && time < m_stop ;
}
// ============================================================================
// OPTIONAL: the nice printout 
// ============================================================================
std::ostream& LoKi::O1::InTime::fillStream ( std::ostream& s ) const 
{
  return s << "O1_TIME(" 
           << "Gaudi.Time(" << m_start . ns () << ","
           << "Gaudi.Time(" << m_stop  . ns () << ")" ;
}
// ============================================================================


// ============================================================================
// constructor from the run number 
// ============================================================================
LoKi::O1::RunNumber::RunNumber
( const unsigned int run   ) 
  : LoKi::BasicFunctors<const LHCb::ODIN*>::Predicate() 
  , m_flag  ( One ) 
  , m_run   ( run ) 
  , m_begin ( 0   ) 
  , m_end   ( 0   ) 
  , m_runs  (     ) 
{}
// ============================================================================
// constructor from the run range 
// ============================================================================
LoKi::O1::RunNumber::RunNumber
( const unsigned int begin , 
  const unsigned int end   ) 
  : LoKi::BasicFunctors<const LHCb::ODIN*>::Predicate() 
  , m_flag  ( Range ) 
  , m_run   ( 0     ) 
  , m_begin ( begin ) 
  , m_end   ( end   ) 
  , m_runs  (       ) 
{}
// ============================================================================
// constructor from the run list 
// ============================================================================
LoKi::O1::RunNumber::RunNumber
( const std::vector<unsigned int>& runs ) 
  : LoKi::BasicFunctors<const LHCb::ODIN*>::Predicate() 
  , m_flag  ( List ) 
  , m_run   ( 0    ) 
  , m_begin ( 0    ) 
  , m_end   ( 0    ) 
  , m_runs  ( runs ) 
{}
// ============================================================================
// MANDATORY: The only one essential method:
// ============================================================================
LoKi::O1::RunNumber::result_type 
LoKi::O1::RunNumber::operator() 
  ( LoKi::O1::RunNumber::argument o ) const 
{
  // 
  switch ( m_flag ) 
  {
  case One   : 
    return m_run == o->runNumber() ;                             // RETURN 
  case Range :
    return m_begin <= o->runNumber() && o->runNumber() < m_end ; // RETURN
  case List:
    return m_runs.end() != std::find 
      ( m_runs.begin() , m_runs.end() , o->runNumber() ) ;       // RETURN
  default:
    break ;
  }
  return false ;
}
// ============================================================================
// OPTIONAL: the nice printout 
// ============================================================================
std::ostream& LoKi::O1::RunNumber::fillStream ( std::ostream& s ) const 
{
  s << "O1_RUNNUMBER(" ;
  //
  switch ( m_flag ) 
  {
  case One :
    return s << m_run << ")" ;                              // RETURN 
  case Range :
    return s << m_begin << "," << m_end << ")" ;            // RETURN 
  case List :
    return s << Gaudi::Utils::toString ( m_runs ) << ")" ;  // RETURN
  default: break ;
  }
  return s << ")" ;
}
// ============================================================================

// ============================================================================
// constructor from the run number 
// ============================================================================
LoKi::O1::BXId::BXId
( const unsigned int bx   ) 
  : LoKi::BasicFunctors<const LHCb::ODIN*>::Predicate() 
  , m_flag  ( One ) 
  , m_bx    ( bx ) 
  , m_begin ( 0   ) 
  , m_end   ( 0   ) 
  , m_bxs   (     ) 
{}
// ============================================================================
// constructor from the run range 
// ============================================================================
LoKi::O1::BXId::BXId
( const unsigned int begin , 
  const unsigned int end   ) 
  : LoKi::BasicFunctors<const LHCb::ODIN*>::Predicate() 
  , m_flag  ( Range ) 
  , m_bx    ( 0     ) 
  , m_begin ( begin ) 
  , m_end   ( end   ) 
  , m_bxs   (       ) 
{}
// ============================================================================
// constructor from the run list 
// ============================================================================
LoKi::O1::BXId::BXId
( const std::vector<unsigned int>& bxs ) 
  : LoKi::BasicFunctors<const LHCb::ODIN*>::Predicate() 
  , m_flag  ( List ) 
  , m_bx    ( 0    ) 
  , m_begin ( 0    ) 
  , m_end   ( 0    ) 
  , m_bxs   ( bxs  ) 
{}
// ============================================================================
// MANDATORY: The only one essential method:
// ============================================================================
LoKi::O1::BXId::result_type 
LoKi::O1::BXId::operator() 
  ( LoKi::O1::BXId::argument o ) const 
{
  // 
  switch ( m_flag ) 
  {
  case One   : 
    return m_bx == o->bunchId() ;                             // RETURN 
  case Range :
    return m_begin <= o->bunchId() && o->bunchId() < m_end ;  // RETURN
  case List:
    return m_bxs.end() != std::find 
      ( m_bxs.begin() , m_bxs.end() , o->bunchId() ) ;        // RETURN
  default:
    break ;
  }
  return false ;
}
// ============================================================================
// OPTIONAL: the nice printout 
// ============================================================================
std::ostream& LoKi::O1::BXId::fillStream ( std::ostream& s ) const 
{
  s << "O1_BXID(" ;
  //
  switch ( m_flag ) 
  {
  case One :
    return s << m_bx << ")" ;                              // RETURN 
  case Range :
    return s << m_begin << "," << m_end << ")" ;            // RETURN 
  case List :
    return s << Gaudi::Utils::toString ( m_bxs ) << ")" ;  // RETURN
  default: break ;
  }
  return s << ")" ;
}
// ============================================================================


// ============================================================================
// constructor from the run number 
// ============================================================================
LoKi::O1::Tck::Tck
( const unsigned int bx   ) 
  : LoKi::BasicFunctors<const LHCb::ODIN*>::Predicate() 
  , m_flag  ( One ) 
  , m_bx    ( bx ) 
  , m_begin ( 0   ) 
  , m_end   ( 0   ) 
  , m_bxs   (     ) 
{}
// ============================================================================
// constructor from the run range 
// ============================================================================
LoKi::O1::Tck::Tck
( const unsigned int begin , 
  const unsigned int end   ) 
  : LoKi::BasicFunctors<const LHCb::ODIN*>::Predicate() 
  , m_flag  ( Range ) 
  , m_bx    ( 0     ) 
  , m_begin ( begin ) 
  , m_end   ( end   ) 
  , m_bxs   (       ) 
{}
// ============================================================================
// constructor from the run list 
// ============================================================================
LoKi::O1::Tck::Tck
( const std::vector<unsigned int>& bxs ) 
  : LoKi::BasicFunctors<const LHCb::ODIN*>::Predicate() 
  , m_flag  ( List ) 
  , m_bx    ( 0    ) 
  , m_begin ( 0    ) 
  , m_end   ( 0    ) 
  , m_bxs   ( bxs  ) 
{}
// ============================================================================
// MANDATORY: The only one essential method:
// ============================================================================
LoKi::O1::Tck::result_type 
LoKi::O1::Tck::operator() 
  ( LoKi::O1::Tck::argument o ) const 
{
  // 
  switch ( m_flag ) 
  {
  case One   : 
    return m_bx == o->triggerConfigurationKey() ;             // RETURN 
  case Range :
    return m_begin <= o->triggerConfigurationKey() 
      && o->triggerConfigurationKey() < m_end ;              // RETURN
  case List:
    return m_bxs.end() != std::find 
      ( m_bxs.begin() , m_bxs.end() , 
        o->triggerConfigurationKey() ) ;                    // RETURN
  default:
    break ;
  }
  return false ;
}
// ============================================================================
// OPTIONAL: the nice printout 
// ============================================================================
std::ostream& LoKi::O1::Tck::fillStream ( std::ostream& s ) const 
{
  s << "O1_TCKEYS(" ;
  //
  switch ( m_flag ) 
  {
  case One :
    return s << m_bx << ")" ;                              // RETURN 
  case Range :
    return s << m_begin << "," << m_end << ")" ;            // RETURN 
  case List :
    return s << Gaudi::Utils::toString ( m_bxs ) << ")" ;  // RETURN
  default: break ;
  }
  return s << ")" ;
}
// ============================================================================

  
// ============================================================================
// The END 
// ============================================================================
