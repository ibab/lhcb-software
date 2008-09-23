// $Id: Odin.cpp,v 1.1 2008-09-23 13:13:35 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ToStream.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Odin.h"
// ============================================================================
/** @file 
 *  Implementation file for classes from namesapce LoKi::Odin
 *  @date 2008-09-17 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 */
// ============================================================================
// the constructor from the start/stop times
// ============================================================================
LoKi::Odin::InTime::InTime 
( const Gaudi::Time& start , 
  const Gaudi::Time& stop  ) 
  : LoKi::BasicFunctors<const LHCb::ODIN*>::Predicate() 
  , m_start ( start ) 
  , m_stop  ( stop  ) 
{}
// ============================================================================
// the constructor from the start&span times
// ============================================================================
LoKi::Odin::InTime::InTime 
( const Gaudi::Time&     start , 
  const Gaudi::TimeSpan& span  ) 
  : LoKi::BasicFunctors<const LHCb::ODIN*>::Predicate() 
  , m_start ( start        ) 
  , m_stop  ( start + span ) 
{}
// ============================================================================
// MANDATORY: The only one essential method:
// ============================================================================
LoKi::Odin::InTime::result_type 
LoKi::Odin::InTime::operator() 
  ( LoKi::Odin::InTime::argument o ) const 
{
  const Gaudi::Time time = o -> eventTime () ;
  //
  return m_start <= time && time < m_stop ;
}
// ============================================================================
// OPTIONAL: the nice printout 
// ============================================================================
std::ostream& LoKi::Odin::InTime::fillStream ( std::ostream& s ) const 
{
  return s << "ODIN_TIME(" 
           << "Gaudi.Time(" << m_start . ns () << ","
           << "Gaudi.Time(" << m_stop  . ns () << ")" ;
}
// ============================================================================


// ============================================================================
// constructor from the run number 
// ============================================================================
LoKi::Odin::RunNumber::RunNumber
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
LoKi::Odin::RunNumber::RunNumber
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
LoKi::Odin::RunNumber::RunNumber
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
LoKi::Odin::RunNumber::result_type 
LoKi::Odin::RunNumber::operator() 
  ( LoKi::Odin::RunNumber::argument o ) const 
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
std::ostream& LoKi::Odin::RunNumber::fillStream ( std::ostream& s ) const 
{
  s << "ODIN_RUNNUMBER(" ;
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
LoKi::Odin::BXId::BXId
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
LoKi::Odin::BXId::BXId
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
LoKi::Odin::BXId::BXId
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
LoKi::Odin::BXId::result_type 
LoKi::Odin::BXId::operator() 
  ( LoKi::Odin::BXId::argument o ) const 
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
std::ostream& LoKi::Odin::BXId::fillStream ( std::ostream& s ) const 
{
  s << "ODIN_BXID(" ;
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
LoKi::Odin::Tck::Tck
( const unsigned int bx   ) 
  : LoKi::BasicFunctors<const LHCb::ODIN*>::Predicate() 
  , m_flag  ( One ) 
  , m_bx    ( bx ) 
  , m_bxs   (     ) 
{}
// ============================================================================
// constructor from the run list 
// ============================================================================
LoKi::Odin::Tck::Tck
( const std::vector<unsigned int>& bxs ) 
  : LoKi::BasicFunctors<const LHCb::ODIN*>::Predicate() 
  , m_flag  ( List ) 
  , m_bx    ( 0    ) 
  , m_bxs   ( bxs  ) 
{}
// ============================================================================
// MANDATORY: The only one essential method:
// ============================================================================
LoKi::Odin::Tck::result_type 
LoKi::Odin::Tck::operator() 
  ( LoKi::Odin::Tck::argument o ) const 
{
  // 
  switch ( m_flag ) 
  {
  case One   : 
    return m_bx == o->triggerConfigurationKey() ;             // RETURN 
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
std::ostream& LoKi::Odin::Tck::fillStream ( std::ostream& s ) const 
{
  s << "ODIN_TCKEYS(" ;
  //
  switch ( m_flag ) 
  {
  case One :
    return s << m_bx << ")" ;                              // RETURN 
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
