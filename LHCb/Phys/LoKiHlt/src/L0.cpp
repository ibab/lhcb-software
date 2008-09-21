// $Id: L0.cpp,v 1.1.1.1 2008-09-21 14:41:20 ibelyaev Exp $
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ToStream.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/L0.h"
// ============================================================================
/** @file 
 *  Implementation file for namespace LoKi::L0
 *  @date 2008-09-19 
 *  @author Vanya BELYAEV Ivan.Belyaev@niklhef.nl 
 */
// ============================================================================
// constructor from bx-id L
// ============================================================================
LoKi::L0::SumEt::SumEt
( const int bx )
  : LoKi::BasicFunctors<const LHCb::L0DUReport*>::Function () 
  , m_bx ( bx ) 
{}
// ============================================================================
// OPTIONAL: the nice printout 
// ============================================================================
std::ostream& LoKi::L0::SumEt::fillStream ( std::ostream& s ) const 
{
  if ( 0 == m_bx ) { return s << "L0_SUMET0" ; }
  return s << "L0_SUMET(" << m_bx << ")" ;
}
// ============================================================================

// ============================================================================
// constructor from data name 
// ============================================================================
LoKi::L0::DataValue::DataValue
( const std::string& name  )
  : LoKi::BasicFunctors<const LHCb::L0DUReport*>::Function () 
  , m_name ( name ) 
{}
// ============================================================================
// OPTIONAL: the nice printout 
// ============================================================================
std::ostream& LoKi::L0::DataValue::fillStream ( std::ostream& s ) const 
{ return s << "L0_DATA('" << name () << "')" ; }
// ============================================================================


// ============================================================================
// constructor from data name 
// ============================================================================
LoKi::L0::DataDigit::DataDigit
( const std::string& name  )
  : LoKi::L0::DataValue ( name ) 
{}
// ============================================================================
// OPTIONAL: the nice printout 
// ============================================================================
std::ostream& LoKi::L0::DataDigit::fillStream ( std::ostream& s ) const 
{ return s << "L0_DIGIT('" << name () << "')" ; }
// ============================================================================



// ============================================================================
// channel decision by channel number 
// ============================================================================
LoKi::L0::ChannelDecision::ChannelDecision
( const unsigned int channel , 
  const int          bx      ) 
  : LoKi::BasicFunctors<const LHCb::L0DUReport*>::Predicate () 
  , m_mode ( ByNumber ) 
  , m_channel ( channel ) 
  , m_names   () 
  , m_bx      ( bx ) 
{}
// ============================================================================
// channel decision by channel name 
// ============================================================================
LoKi::L0::ChannelDecision::ChannelDecision
( const std::string& channel , 
  const int          bx      ) 
  : LoKi::BasicFunctors<const LHCb::L0DUReport*>::Predicate () 
  , m_mode ( ByName ) 
  , m_channel ( 0          ) 
  , m_names   ( 1 ,channel ) 
  , m_bx      ( bx         ) 
{}
// ============================================================================
// channel decision by channel names ("OR")
// ============================================================================
LoKi::L0::ChannelDecision::ChannelDecision
( const std::vector<std::string>& channels ,
  const int                       bx       ) 
  : LoKi::BasicFunctors<const LHCb::L0DUReport*>::Predicate () 
  , m_mode ( ByName ) 
  , m_channel ( 0        ) 
  , m_names   ( channels ) 
  , m_bx      ( bx       ) 
{}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::L0::ChannelDecision::result_type 
LoKi::L0::ChannelDecision::operator() 
  ( LoKi::L0::ChannelDecision::argument a ) const 
{
  if ( ByNumber == mode () ) 
  { return a -> channelDecision ( index () , bx ()  ) ;}           // RETURN 
  // loop over the names (OR)
  const Names& _names = names() ;
  for ( Names::const_iterator iname = _names.begin() ;
        _names.end() != iname ; ++iname ) 
  {
    if ( a -> channelDecisionByName ( *iname , bx ()  ) ) { return true ; } 
  }
  //
  return false ;
}
// ============================================================================
// OPTIONAL: the nice printout 
// ============================================================================
std::ostream& LoKi::L0::ChannelDecision::fillStream ( std::ostream& s ) const 
{
  // ==========================================================================
  if ( ByNumber == mode () ) 
  { return s << "L0_CHANNEL(" << index()<< "," << bx() << ")" ; }
  // ==========================================================================
  if ( 1 == names().size() ) 
  { return s << "L0_CHANNEL('" << names().front() << "'," << bx () << ")" ; }
  // ==========================================================================
  return 
    s << "L0_CHANNEL(" 
      << Gaudi::Utils::toString ( names ()  ) << "," << bx () << ")" ; 
  // ==========================================================================
}

// ============================================================================
// channel decision by channel number 
// ============================================================================
LoKi::L0::ChannelPreDecision::ChannelPreDecision
( const unsigned int channel , 
  const int          bx      ) 
  : LoKi::L0::ChannelDecision ( channel , bx ) 
{}
// ============================================================================
// channel decision by channel name 
// ============================================================================
LoKi::L0::ChannelPreDecision::ChannelPreDecision
( const std::string& channel , 
  const int          bx      ) 
  : LoKi::L0::ChannelDecision ( channel , bx ) 
{}
// ============================================================================
// channel decision by channel names ("OR")
// ============================================================================
LoKi::L0::ChannelPreDecision::ChannelPreDecision
( const std::vector<std::string>& channels ,
  const int                       bx       ) 
  : LoKi::L0::ChannelDecision ( channels , bx ) 
{}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::L0::ChannelPreDecision::result_type 
LoKi::L0::ChannelPreDecision::operator() 
  ( LoKi::L0::ChannelPreDecision::argument a ) const 
{
  if ( ByNumber == mode () ) 
  { return a -> channelPreDecision ( index ()  , bx () ) ;}           // RETURN 
  // loop over the names (OR)
  const Names& _names = names() ;
  for ( Names::const_iterator iname = _names.begin() ;
        _names.end() != iname ; ++iname ) 
  {
    if ( a -> channelPreDecisionByName ( *iname , bx () ) ) { return true ; } 
  }
  //
  return false ;
}
// ============================================================================
// OPTIONAL: the nice printout 
// ============================================================================
std::ostream& LoKi::L0::ChannelPreDecision::fillStream ( std::ostream& s ) const 
{
  // ==========================================================================
  if ( ByNumber == mode () ) 
  { return s << "L0_CHPREDECISION(" << index ()  << "," << bx () << ")" ; }
  // ==========================================================================
  if ( 1 == names().size() ) 
  { return s << "L0_CHPREDECISION('" << names().front() << "'," << bx () << ")" ; }
  // ==========================================================================
  return 
    s << "L0_CHDECISION(" 
      << Gaudi::Utils::toString ( names() ) << "," << bx () << ")" ; 
  // ==========================================================================
}


// ============================================================================
// channel decision by channel number 
// ============================================================================
LoKi::L0::ConditionValue::ConditionValue
( const unsigned int channel , 
  const int          bx      ) 
  : LoKi::L0::ChannelDecision ( channel , bx ) 
{}
// ============================================================================
// channel decision by channel name 
// ============================================================================
LoKi::L0::ConditionValue::ConditionValue
( const std::string& channel , 
  const int          bx      ) 
  : LoKi::L0::ChannelDecision ( channel , bx ) 
{}
// ============================================================================
// channel decision by channel names ("OR")
// ============================================================================
LoKi::L0::ConditionValue::ConditionValue
( const std::vector<std::string>& channels ,
  const int                       bx       ) 
  : LoKi::L0::ChannelDecision ( channels , bx ) 
{}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::L0::ConditionValue::result_type 
LoKi::L0::ConditionValue::operator() 
  ( LoKi::L0::ConditionValue::argument a ) const 
{
  if ( ByNumber == mode () ) 
  { return a -> conditionValue ( index ()  , bx () ) ;}           // RETURN 
  // loop over the names (OR)
  const Names& _names = names() ;
  for ( Names::const_iterator iname = _names.begin() ;
        _names.end() != iname ; ++iname ) 
  {
    if ( a -> conditionValueByName ( *iname , bx () ) ) { return true ; } 
  }
  //
  return false ;
}
// ============================================================================
// OPTIONAL: the nice printout 
// ============================================================================
std::ostream& LoKi::L0::ConditionValue::fillStream ( std::ostream& s ) const 
{
  // ==========================================================================
  if ( ByNumber == mode () ) 
  { return s << "L0_CONDITION(" << index ()  << "," << bx () << ")" ; }
  // ==========================================================================
  if ( 1 == names().size() ) 
  { return s << "L0_CONDITION('" << names().front() << "'," << bx () << ")" ; }
  // ==========================================================================
  return 
    s << "L0_CONDITION(" 
      << Gaudi::Utils::toString ( names() ) << "," << bx () << ")" ; 
  // ==========================================================================
}



// ============================================================================
// channel decision by channel number 
// ============================================================================
LoKi::L0::TriggerDecision::TriggerDecision
( const unsigned int channel , 
  const int          bx      ) 
  : LoKi::L0::ChannelDecision ( channel , bx ) 
{}
// ============================================================================
// channel decision by channel name 
// ============================================================================
LoKi::L0::TriggerDecision::TriggerDecision
( const std::string& channel , 
  const int          bx      ) 
  : LoKi::L0::ChannelDecision ( channel , bx ) 
{}
// ============================================================================
// channel decision by channel names ("OR")
// ============================================================================
LoKi::L0::TriggerDecision::TriggerDecision
( const std::vector<std::string>& channels ,
  const int                       bx       ) 
  : LoKi::L0::ChannelDecision ( channels , bx ) 
{}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::L0::TriggerDecision::result_type 
LoKi::L0::TriggerDecision::operator() 
  ( LoKi::L0::TriggerDecision::argument a ) const 
{
  if ( ByNumber == mode () ) 
  { return a -> triggerDecision ( index ()  , bx () ) ;}           // RETURN 
  // loop over the names (OR)
  const Names& _names = names() ;
  for ( Names::const_iterator iname = _names.begin() ;
        _names.end() != iname ; ++iname ) 
  {
    if ( a -> triggerDecisionByName ( *iname , bx () ) ) { return true ; } 
  }
  //
  return false ;
}
// ============================================================================
// OPTIONAL: the nice printout 
// ============================================================================
std::ostream& LoKi::L0::TriggerDecision::fillStream ( std::ostream& s ) const 
{
  // ==========================================================================
  if ( ByNumber == mode () ) 
  { return s << "L0_TRIGGER(" << index ()  << "," << bx () << ")" ; }
  // ==========================================================================
  if ( 1 == names().size() ) 
  { return s << "L0_TRIGGER('" << names().front() << "'," << bx ()<< ")" ; }
  // ==========================================================================
  return 
    s << "L0_TRIGGER(" 
      << Gaudi::Utils::toString ( names() ) << "," << bx () << ")" ; 
  // ==========================================================================
}


// ============================================================================
// OPTIONAL: the nice printout 
// ============================================================================
std::ostream& LoKi::L0::SumDecision::fillStream ( std::ostream& s ) const 
{ return s << "L0_SUMDECISION" ; }
// ============================================================================
// OPTIONAL: the nice printout 
// ============================================================================
std::ostream& LoKi::L0::Decision::fillStream ( std::ostream& s ) const 
{ return s << "L0_DECISION" ; }
// ============================================================================
// OPTIONAL: the nice printout 
// ============================================================================
std::ostream& LoKi::L0::ForceBit::fillStream ( std::ostream& s ) const 
{ return s << "L0_FORCEBIT" ; }
// ============================================================================
// OPTIONAL: the nice printout 
// ============================================================================
std::ostream& LoKi::L0::Tck::fillStream ( std::ostream& s ) const 
{ return s << "L0_TCK" ; }
// ============================================================================
// OPTIONAL: the nice printout 
// ============================================================================
std::ostream& LoKi::L0::TimingBit::fillStream ( std::ostream& s ) const 
{ return s << "L0_TIMINGBIT" ; }
// ============================================================================



// ============================================================================
// The END 
// ============================================================================
