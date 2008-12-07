// $Id: HltFunctions.cpp,v 1.7 2008-12-07 14:34:29 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// TrackEvent 
// ============================================================================
#include "Event/Track.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/HltFunctions.h"
#include "LoKi/HltBinders.h"
#include "LoKi/Constants.h"
// ============================================================================
// HltBase 
// ============================================================================
#include "HltBase/HltUtils.h"
// ============================================================================
/** @file
 *  implementation file for function from the file LoKi/HltFunctions.h
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2007-08-13
 */
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Tracks::DistanceOfClosestApproach::result_type 
LoKi::Tracks::DistanceOfClosestApproach::operator() 
  ( LoKi::Tracks::DistanceOfClosestApproach::argument a ) const 
{ 
  // unpack argument:
  const LHCb::Track& t1 = a.first  ;
  const LHCb::Track& t2 = a.second ;
  // try to calculate DOCA...
  return HltUtils::closestDistanceMod ( t1 , t2 ) ;    
}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Tracks::DeltaP::result_type 
LoKi::Tracks::DeltaP::operator() 
  ( LoKi::Tracks::DeltaP::argument a ) const 
{ 
  // unpack argument:
  const LHCb::Track& t1 = a.first  ;
  const LHCb::Track& t2 = a.second ;
  //
  if      ( &t1 == &t2 ) { return 0 ; }                        // RETURN
  const double p1 = t1.p() ;
  const double p2 = t2.p() ;
  // trivial checks:
  if      ( p1  ==  p2 ) { return 0 ; }                        // RETURN 
  else if ( 0   ==  p2 ) 
  {
    Warning ( "The second momentum is equal to zero, return -inf") ;
    return LoKi::Constants::NegativeInfinity ;                 // RETURN
  }
  //
  return (p1-p2)/p2 ;                                          // RETURN
}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Tracks::DeltaE::result_type 
LoKi::Tracks::DeltaE::operator() 
  ( LoKi::Tracks::DeltaE::argument t ) const 
{ 
  // get the state:
  const LHCb::State* s  = t.stateAt(LHCb::State::MidHCal) ;
  if ( 0 == s ) 
  { 
    Warning("No state at MidHcal, return +inf") ;
    return LoKi::Constants::PositiveInfinity ;                // RETURN
  }  
  // calculate delta E: 
  const double p  = t .  p () ;
  const double e  = s -> p () ;
  const double de = e * ( sqrt ( 0.60 * 0.60 + 0.70 * 0.70 / e ) ) ;
  //
  return (e-p)/de ;                                            // REUTRN   
}
// ============================================================================
// constructor from the tool 
// ============================================================================
LoKi::Tracks::TrackMatch::TrackMatch
( ITrackBiFunctionTool* tool )
  : LoKi::BasicFunctors<LoKi::TrackTypes::TrackPair>::Function() 
  , m_tool ( tool )
{}
// ============================================================================
// constructor form the tool 
// ============================================================================
LoKi::Tracks::TrackMatch::TrackMatch
( const LoKi::Interface<ITrackBiFunctionTool>& tool )
  : LoKi::BasicFunctors<LoKi::TrackTypes::TrackPair>::Function () 
  , m_tool ( tool )
{}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Tracks::TrackMatch::result_type 
LoKi::Tracks::TrackMatch::operator() 
  ( LoKi::Tracks::TrackMatch::argument a ) const 
{
  // unpack argument:
  const LHCb::Track& t1 = a.first  ;
  const LHCb::Track& t2 = a.second ;
  if ( !m_tool.validPointer() )
  {
    Error ( "Tool is invalid, return +inf" ) ;
    return LoKi::Constants::PositiveInfinity ;               // RETURN
  }
  //
  double chi2 = m_tool -> function ( t1 , t2 ) ;
  // 
  return chi2 ;  
}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Tracks::RadialIP::result_type 
LoKi::Tracks::RadialIP::operator() 
  ( LoKi::Tracks::RadialIP::argument a ) const 
{
  // unpack argument:
  const LHCb::Track&     t = a.first  ;
  const LHCb::RecVertex& v = a.second ;
  // 
  return HltUtils::rImpactParameter ( v , t ) ;
}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Tracks::ImpactParameter::result_type 
LoKi::Tracks::ImpactParameter::operator() 
  ( LoKi::Tracks::ImpactParameter::argument a ) const 
{
  // unpack argument:
  const LHCb::Track&     t = a.first  ;
  const LHCb::RecVertex& v = a.second ;
  // 
  return HltUtils::impactParameter ( v , t ) ;
}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Tracks::MatchIDsFraction::result_type 
LoKi::Tracks::MatchIDsFraction::operator() 
  ( LoKi::Tracks::MatchIDsFraction::argument a ) const 
{ 
  // unpack argument:
  const LHCb::Track& t1 = a.first  ;
  const LHCb::Track& t2 = a.second ;
  //
  return HltUtils::matchIDsFraction ( t1 , t2 ) ;
}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Tracks::DeltaAngle::result_type 
LoKi::Tracks::DeltaAngle::operator() 
  ( LoKi::Tracks::DeltaAngle::argument a ) const 
{ 
  // unpack argument:
  const LHCb::Track& t1 = a.first  ;
  const LHCb::Track& t2 = a.second ;
  //
  return HltUtils::deltaAngle ( t1 , t2 ) ;
}
// ============================================================================
// The END 
// ============================================================================
