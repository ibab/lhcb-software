// $Id: L0.cpp,v 1.3 2009-06-17 14:37:25 ibelyaev Exp $
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
  //
  Assert ( 0 != a , "LHCb::L0DUReport* points to null" ) ;
  //
  // update the system:
  if ( 0 == tckPrev() || tckPrev() != a->tck() ) 
  { 
    clearChannels () ;
    // convert names into conditions 
    const LHCb::L0DUConfig* config = a->configuration() ;
    Assert ( 0 != config , "LHCb::L0DUConfig* points to NULL" );
    const LHCb::L0DUElementaryCondition::Map& m = config->conditions() ;
    for ( Names::const_iterator iname = names().begin() ; names().end() != iname ; ++iname )
    {
      LHCb::L0DUElementaryCondition::Map::const_iterator ifind = m.find (*iname) ;
      if ( m.end() == ifind ) 
      {
        Error ( "Non-existing condition '" + (*iname) + "' has been requested" );
        continue ;
      }
      const LHCb::L0DUElementaryCondition* channel = ifind->second ;
      Assert ( 0 != channel , "LHCb::L0DUElementaryCondition* points to NULL" ) ;
      addChannel ( channel -> id() ) ;
    }
    // store the tck of successful configuration 
    setTckPrev ( a -> tck () ) ;
  }
  
  // sanity check:
  if ( channels().empty() ) 
  { 
    Error ( "empty list of channels, return false" ) ;
    return false ;                                                    // RETURN 
  }
  
  // loop over the defined channels 
  for ( Channels::const_iterator ic = channels().begin() ; 
        channels().end() != ic ; ++ic ) 
  { if ( a -> conditionValue ( *ic , bx() ) ) { return true ; } }    // REUTRN 
  //
  return false ;
}
// ============================================================================
// OPTIONAL: the nice printout 
// ============================================================================
std::ostream& LoKi::L0::ConditionValue::fillStream ( std::ostream& s ) const 
{
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
// channel decision by channel name 
// ============================================================================
LoKi::L0::ChannelDecision::ChannelDecision
( const std::string& channel , 
  const int          bx      ) 
  : LoKi::BasicFunctors<const LHCb::L0DUReport*>::Predicate () 
  , m_names    ( 1 ,channel ) 
  , m_channels () 
  , m_bx       ( bx         ) 
  , m_tckPrev  ( 0          ) 
{}
// ============================================================================
// channel decision by channel names ("OR")
// ============================================================================
LoKi::L0::ChannelDecision::ChannelDecision
( const std::vector<std::string>& channels ,
  const int                       bx       ) 
  : LoKi::BasicFunctors<const LHCb::L0DUReport*>::Predicate () 
  , m_names    ( channels ) 
  , m_channels () 
  , m_bx       ( bx       ) 
  , m_tckPrev  ( 0        ) 
{}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::L0::ChannelDecision::result_type 
LoKi::L0::ChannelDecision::operator() 
  ( LoKi::L0::ChannelDecision::argument a ) const 
{
  //
  Assert ( 0 != a , "LHCb::L0DUReport* points to null" ) ;
  //
  // update the system:
  if ( 0 == tckPrev() || tckPrev() != a->tck() ) 
  { 
    clearChannels () ;
    // convert names into channels 
    const LHCb::L0DUConfig* config = a->configuration() ;
    Assert ( 0 != config , "LHCb::L0DUConfig* points to NULL" );
    const LHCb::L0DUChannel::Map& m = config->channels() ;
    for ( Names::const_iterator iname = names().begin() ; names().end() != iname ; ++iname )
    {
      LHCb::L0DUChannel::Map::const_iterator ifind = m.find (*iname) ;
      if ( m.end() == ifind ) 
      {
        Error ( "Non-existing channel '" + (*iname) + "' has been requested" );
        continue ;
      }
      const LHCb::L0DUChannel* channel = ifind->second ;
      Assert ( 0 != channel , "LHCb::L0DUChannel* points to NULL" ) ;
      addChannel ( channel -> id() ) ;
    }
    // store the tck of successful configuration 
    setTckPrev ( a -> tck () ) ;
  }
  
  // sanity check:
  if ( channels().empty() ) 
  { 
    Error ( "empty list of channels, return false" ) ;
    return false ;                                                    // RETURN 
  }
  
  // loop over the defined channels 
  for ( Channels::const_iterator ic = channels().begin() ; 
        channels().end() != ic ; ++ic ) 
  { if ( a -> channelDecision ( *ic , bx() ) ) { return true ; } }    // REUTRN 
  //
  return false ;
}
// ============================================================================
// OPTIONAL: the nice printout 
// ============================================================================
std::ostream& LoKi::L0::ChannelDecision::fillStream ( std::ostream& s ) const 
{
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
  //
  Assert ( 0 != a , "LHCb::L0DUReport* points to null" ) ;
  //
  // update the system:
  if ( 0 == tckPrev () || tckPrev () != a->tck() ) 
  { 
    clearChannels () ;
    // convert names into channels 
    const LHCb::L0DUConfig* config = a->configuration() ;
    Assert ( 0 != config , "LHCb::L0DUConfig* points to NULL" );
    const LHCb::L0DUChannel::Map& m = config->channels() ;
    for ( Names::const_iterator iname = names().begin() ; names().end() != iname ; ++iname )
    {
      LHCb::L0DUChannel::Map::const_iterator ifind = m.find (*iname) ;
      if ( m.end() == ifind ) 
      {
        Error ( "Non-existing channel '" + (*iname) + "' has been requested" );
        continue ;
      }
      const LHCb::L0DUChannel* channel = ifind->second ;
      Assert ( 0 != channel , "LHCb::L0DUChannel* points to NULL" ) ;
      addChannel ( channel -> id() ) ;
    }    
    // store the tck of successful configuration 
    setTckPrev ( a -> tck () ) ;
  }
  
  // sanity check:
  if ( channels().empty() ) 
  { 
    Error ( "empty list of channels, return false" ) ;
    return false ;                                                    // RETURN 
  }
  
  // loop over the defined channels 
  for ( Channels::const_iterator ic = channels().begin() ; channels().end() != ic ; ++ic ) 
  { if ( a -> channelPreDecision ( *ic , bx() ) ) { return true ; } }    // REUTRN 
  //
  return false ;
}
// ============================================================================
// OPTIONAL: the nice printout 
// ============================================================================
std::ostream& LoKi::L0::ChannelPreDecision::fillStream ( std::ostream& s ) const 
{
  // ==========================================================================
  if ( 1 == names().size() ) 
  { return s << "L0_CHPREDECISION('" << names().front() << "'," << bx () << ")" ; }
  // ==========================================================================
  return 
    s << "L0_CHPREDECISION(" 
      << Gaudi::Utils::toString ( names() ) << "," << bx () << ")" ; 
  // ==========================================================================
}



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
  //
  Assert ( 0 != a , "LHCb::L0DUReport* points to null" ) ;
  //
  // update the system:
  if ( 0 == tckPrev() || tckPrev() != a->tck() ) 
  {
    clearChannels()  ;
    // CHECK THE EXISTENCE OF TRIGGERS 
    const LHCb::L0DUConfig* config = a->configuration() ;
    Assert ( 0 != config , "LHCb::L0DUConfig* points to NULL" );
    const LHCb::L0DUTrigger::Map& m = config->triggers() ;
    for ( Names::const_iterator iname = names().begin() ; 
          names().end() != iname ; ++iname ) 
    {
      LHCb::L0DUTrigger::Map::const_iterator ifind = m.find ( *iname ) ;
      if ( m.end() == ifind ) 
      {
        Error ("Unknown trigger '" + (*iname) + "' has been requested") ;
        continue ;
      }
      const LHCb::L0DUTrigger* trigger = ifind->second ;
      Assert ( 0 != trigger , "LHCb::L0DUTrigger* points to NULL" ) ;
      addChannel ( trigger -> id() ) ;
    }
    // store the tck of successful configuration 
    setTckPrev ( a -> tck () ) ;
  }
  
  // loop over the defined triggers 
  for ( Names::const_iterator ic = names().begin()  ; names().end()!= ic ; ++ic ) 
  { if ( a -> triggerDecisionByName ( *ic , bx() ) ) { return true ; } }    // REUTRN 
  //
  return false ;
}
// ============================================================================
// OPTIONAL: the nice printout 
// ============================================================================
std::ostream& LoKi::L0::TriggerDecision::fillStream ( std::ostream& s ) const 
{
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
// channel decision by channel name 
// ============================================================================
LoKi::L0::ChannelDecisionSubString::ChannelDecisionSubString
( const std::string& channel , 
  const int          bx      ) 
  : LoKi::L0::ChannelDecision ( LoKi::L0::ChannelDecision::Names() , bx ) 
  , m_substr ( channel )
{}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::L0::ChannelDecisionSubString::result_type 
LoKi::L0::ChannelDecisionSubString::operator() 
  ( LoKi::L0::ChannelDecisionSubString::argument a ) const 
{
  //
  Assert ( 0 != a , "LHCb::L0DUReport* points to null" ) ;
  //
  // update the system:
  if ( 0 == tckPrev() || tckPrev() != a->tck() ) 
  { 
    clearChannels () ;
    clearNames    () ;
    // convert names into channels 
    const LHCb::L0DUConfig* config = a->configuration() ;
    Assert ( 0 != config , "LHCb::L0DUConfig* points to NULL" );
    const LHCb::L0DUChannel::Map& m = config->channels() ;
    for ( LHCb::L0DUChannel::Map::const_iterator ic = m.begin() ; 
          m.end() != ic ; ++ic )
    {
      if ( std::string::npos == ic->first.find ( substr() ) ) { continue ; }
      const LHCb::L0DUChannel* channel = ic->second ;
      Assert ( 0 != channel , "LHCb::L0DUChannel* points to NULL" ) ;
      addChannel ( channel -> id() ) ;
      addName    ( ic->first       ) ;
    }
    // store the tck of successful configuration 
    setTckPrev ( a -> tck () ) ;
  }
  
  // sanity check:
  if ( channels().empty() ) 
  { 
    Error ( "empty list of channels, return false" ) ;
    return false ;                                                    // RETURN 
  }
  
  // loop over the defined channels 
  for ( Channels::const_iterator ic = channels().begin() ; 
        channels().end() != ic ; ++ic ) 
  { if ( a -> channelDecision ( *ic , bx() ) ) { return true ; } }    // REUTRN 
  //
  return false ;
}
// ============================================================================
// OPTIONAL: the nice printout 
// ============================================================================
std::ostream& LoKi::L0::ChannelDecisionSubString::fillStream ( std::ostream& s ) const 
{ return s << "L0_CHANNEL_SUBSTR ('" << substr() << "'," << bx () << ")" ; }



// ============================================================================
// channel decision by channel name 
// ============================================================================
LoKi::L0::ChannelDecisionRegex::ChannelDecisionRegex
( const std::string& channel , 
  const int          bx      ) 
  : LoKi::L0::ChannelDecisionSubString( channel , bx ) 
  , m_expression ( channel )
{}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::L0::ChannelDecisionRegex::result_type 
LoKi::L0::ChannelDecisionRegex::operator() 
  ( LoKi::L0::ChannelDecisionRegex::argument a ) const 
{
  //
  Assert ( 0 != a , "LHCb::L0DUReport* points to null" ) ;
  //
  // update the system:
  if ( 0 == tckPrev() || tckPrev() != a->tck() ) 
  { 
    clearChannels () ;
    clearNames    () ;
    // convert names into channels 
    const LHCb::L0DUConfig* config = a->configuration() ;
    Assert ( 0 != config , "LHCb::L0DUConfig* points to NULL" );
    const LHCb::L0DUChannel::Map& m = config->channels() ;
    for ( LHCb::L0DUChannel::Map::const_iterator ic = m.begin() ; 
          m.end() != ic ; ++ic )
    {
      if ( !boost::regex_match ( ic->first , expression() ) ) { continue ; }
      const LHCb::L0DUChannel* channel = ic->second ;
      Assert ( 0 != channel , "LHCb::L0DUChannel* points to NULL" ) ;
      addChannel ( channel -> id() ) ;
      addName    ( ic->first       ) ;
    }
    // store the tck of successful configuration 
    setTckPrev ( a -> tck () ) ;
  }
  
  // sanity check:
  if ( channels().empty() ) 
  { 
    Error ( "empty list of channels, return false" ) ;
    return false ;                                                    // RETURN 
  }
  
  // loop over the defined channels 
  for ( Channels::const_iterator ic = channels().begin() ; 
        channels().end() != ic ; ++ic ) 
  { if ( a -> channelDecision ( *ic , bx() ) ) { return true ; } }    // REUTRN 
  //
  return false ;
}
// ============================================================================
// OPTIONAL: the nice printout 
// ============================================================================
std::ostream& LoKi::L0::ChannelDecisionRegex::fillStream ( std::ostream& s ) const 
{ return s << "L0_CHANNEL_RE ('" << substr() << "'," << bx () << ")" ; }




// ============================================================================
// channel decision by channel name 
// ============================================================================
LoKi::L0::ChannelPreDecisionSubString::ChannelPreDecisionSubString
( const std::string& channel , 
  const int          bx      ) 
  : LoKi::L0::ChannelDecisionSubString ( channel , bx ) 
{}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::L0::ChannelPreDecisionSubString::result_type 
LoKi::L0::ChannelPreDecisionSubString::operator() 
  ( LoKi::L0::ChannelPreDecisionSubString::argument a ) const 
{
  //
  Assert ( 0 != a , "LHCb::L0DUReport* points to null" ) ;
  //
  // update the system:
  if ( 0 == tckPrev() || tckPrev() != a->tck() ) 
  { 
    clearChannels () ;
    clearNames    () ;
    // convert names into channels 
    const LHCb::L0DUConfig* config = a->configuration() ;
    Assert ( 0 != config , "LHCb::L0DUConfig* points to NULL" );
    const LHCb::L0DUChannel::Map& m = config->channels() ;
    for ( LHCb::L0DUChannel::Map::const_iterator ic = m.begin() ; 
          m.end() != ic ; ++ic )
    {
      if ( std::string::npos == ic->first.find ( substr() ) ) { continue ; }
      const LHCb::L0DUChannel* channel = ic->second ;
      Assert ( 0 != channel , "LHCb::L0DUChannel* points to NULL" ) ;
      addChannel ( channel -> id() ) ;
      addName    ( ic -> first     ) ;
    }
    // store the tck of successful configuration 
    setTckPrev ( a -> tck () ) ;
  }
  
  // sanity check:
  if ( channels().empty() ) 
  { 
    Error ( "empty list of channels, return false" ) ;
    return false ;                                                    // RETURN 
  }
  
  // loop over the defined channels 
  for ( Channels::const_iterator ic = channels().begin() ; 
        channels().end() != ic ; ++ic ) 
  { if ( a -> channelPreDecision ( *ic , bx() ) ) { return true ; } } // REUTRN 
  //
  return false ;
}
// ============================================================================
// OPTIONAL: the nice printout 
// ============================================================================
std::ostream& LoKi::L0::ChannelPreDecisionSubString::fillStream ( std::ostream& s ) const 
{ return s << "L0_CHPREDECISION_SUBSTR ('" << substr() << "'," << bx () << ")" ; }


// ============================================================================
// channel decision by channel name 
// ============================================================================
LoKi::L0::ChannelPreDecisionRegex::ChannelPreDecisionRegex
( const std::string& channel , 
  const int          bx      ) 
  : LoKi::L0::ChannelDecisionRegex ( channel , bx ) 
{}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::L0::ChannelPreDecisionRegex::result_type 
LoKi::L0::ChannelPreDecisionRegex::operator() 
  ( LoKi::L0::ChannelPreDecisionRegex::argument a ) const 
{
  //
  Assert ( 0 != a , "LHCb::L0DUReport* points to null" ) ;
  //
  // update the system:
  if ( 0 == tckPrev() || tckPrev() != a->tck() ) 
  { 
    clearChannels () ;
    clearNames    () ;
    // convert names into channels 
    const LHCb::L0DUConfig* config = a->configuration() ;
    Assert ( 0 != config , "LHCb::L0DUConfig* points to NULL" );
    const LHCb::L0DUChannel::Map& m = config->channels() ;
    for ( LHCb::L0DUChannel::Map::const_iterator ic = m.begin() ; 
          m.end() != ic ; ++ic )
    {
      if ( !boost::regex_match ( ic->first , expression() ) ) { continue ; }
      const LHCb::L0DUChannel* channel = ic->second ;
      Assert ( 0 != channel , "LHCb::L0DUChannel* points to NULL" ) ;
      addChannel ( channel -> id() ) ;
      addName    ( ic->first       ) ;
    }
    // store the tck of successful configuration 
    setTckPrev ( a -> tck () ) ;
  }
  
  // sanity check:
  if ( channels().empty() ) 
  { 
    Error ( "empty list of channels, return false" ) ;
    return false ;                                                    // RETURN 
  }
  
  // loop over the defined channels 
  for ( Channels::const_iterator ic = channels().begin() ; 
        channels().end() != ic ; ++ic ) 
  { if ( a -> channelPreDecision ( *ic , bx() ) ) { return true ; } } // REUTRN 
  //
  return false ;
}
// ============================================================================
// OPTIONAL: the nice printout 
// ============================================================================
std::ostream& LoKi::L0::ChannelPreDecisionRegex::fillStream ( std::ostream& s ) const 
{ return s << "L0_CHPREDECISION_RE ('" << substr() << "'," << bx () << ")" ; }


// ============================================================================
// channel decision by channel name 
// ============================================================================
LoKi::L0::TriggerDecisionSubString::TriggerDecisionSubString
( const std::string& channel , 
  const int          bx      ) 
  : LoKi::L0::ChannelDecisionSubString ( channel , bx ) 
{}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::L0::TriggerDecisionSubString::result_type 
LoKi::L0::TriggerDecisionSubString::operator() 
  ( LoKi::L0::TriggerDecisionSubString::argument a ) const 
{
  //
  Assert ( 0 != a , "LHCb::L0DUReport* points to null" ) ;
  //
  // update the system:
  if ( 0 == tckPrev() || tckPrev() != a->tck() ) 
  { 
    clearChannels () ;
    clearNames    () ;
    // convert names into channels 
    const LHCb::L0DUConfig* config = a->configuration() ;
    Assert ( 0 != config , "LHCb::L0DUConfig* points to NULL" );
    const LHCb::L0DUChannel::Map& m = config->channels() ;
    for ( LHCb::L0DUChannel::Map::const_iterator ic = m.begin() ; 
          m.end() != ic ; ++ic )
    {
      if ( std::string::npos == ic->first.find ( substr() ) ) { continue ; }
      const LHCb::L0DUChannel* channel = ic->second ;
      Assert ( 0 != channel , "LHCb::L0DUChannel* points to NULL" ) ;
      addChannel ( channel -> id() ) ;
      addName    ( ic -> first     ) ;
    }
    // store the tck of successful configuration 
    setTckPrev ( a -> tck () ) ;
  }
  
  // sanity check:
  if ( names().empty() ) 
  { 
    Error ( "empty list of triggers for pattern '" + substr() + "', return false" ) ;
    return false ;                                                    // RETURN 
  }

  // loop over the defined triggers 
  for ( Names::const_iterator ic = names().begin()  ; names().end()!= ic ; ++ic ) 
  { if ( a -> triggerDecisionByName ( *ic , bx() ) ) { return true ; } }    // REUTRN 
  //
  return false ;
}
// ============================================================================
// OPTIONAL: the nice printout 
// ============================================================================
std::ostream& LoKi::L0::TriggerDecisionSubString::fillStream ( std::ostream& s ) const 
{ return s << "L0_TRIGGER_SUBSTR ('" << substr() << "'," << bx () << ")" ; }


// ============================================================================
// channel decision by channel name 
// ============================================================================
LoKi::L0::TriggerDecisionRegex::TriggerDecisionRegex
( const std::string& channel , 
  const int          bx      ) 
  : LoKi::L0::ChannelDecisionRegex ( channel , bx ) 
{}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::L0::TriggerDecisionRegex::result_type 
LoKi::L0::TriggerDecisionRegex::operator() 
  ( LoKi::L0::TriggerDecisionRegex::argument a ) const 
{
  //
  Assert ( 0 != a , "LHCb::L0DUReport* points to null" ) ;
  //
  // update the system:
  if ( 0 == tckPrev() || tckPrev() != a->tck() ) 
  { 
    clearChannels () ;
    clearNames    () ;
    // convert names into channels 
    const LHCb::L0DUConfig* config = a->configuration() ;
    Assert ( 0 != config , "LHCb::L0DUConfig* points to NULL" );
    const LHCb::L0DUChannel::Map& m = config->channels() ;
    for ( LHCb::L0DUChannel::Map::const_iterator ic = m.begin() ; 
          m.end() != ic ; ++ic )
    {
      if ( !boost::regex_match ( ic->first , expression()  ) )  { continue ; }
      const LHCb::L0DUChannel* channel = ic->second ;
      Assert ( 0 != channel , "LHCb::L0DUChannel* points to NULL" ) ;
      addChannel ( channel -> id() ) ;
      addName    ( ic -> first     ) ;
    }
    // store the tck of successful configuration 
    setTckPrev ( a -> tck () ) ;
  }
  
  // sanity check:
  if ( names().empty() ) 
  { 
    Error ( "empty list of triggers for pattern '" + substr() + "', return false" ) ;
    return false ;                                                    // RETURN 
  }
  
  // loop over the defined triggers 
  for ( Names::const_iterator ic = names().begin()  ; names().end()!= ic ; ++ic ) 
  { if ( a -> triggerDecisionByName ( *ic , bx() ) ) { return true ; } }    // REUTRN 
  //
  return false ;
}
// ============================================================================
// OPTIONAL: the nice printout 
// ============================================================================
std::ostream& LoKi::L0::TriggerDecisionRegex::fillStream ( std::ostream& s ) const 
{ return s << "L0_TRIGGER_RE ('" << substr() << "'," << bx () << ")" ; }



// ============================================================================
// The END 
// ============================================================================
