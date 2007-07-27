// $Id: Tracks.cpp,v 1.1.1.1 2007-07-27 15:56:42 ibelyaev Exp $
// ============================================================================
// Include files 
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
#include "LoKi/Tracks.h"
// ============================================================================
/** @file
 *  Implementation file for classes from the namespace LoKi::Tracks
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2007-08-08
 */
// ============================================================================
// constructor from the flag 
// ============================================================================
LoKi::Tracks::CheckFlag::CheckFlag 
( LHCb::Track::Flags flag ) 
  : LoKi::Predicate<LHCb::Track> () 
  , m_flag ( flag ) 
{}
// ============================================================================
// copy constructor 
// ============================================================================
LoKi::Tracks::CheckFlag::CheckFlag 
( const LoKi::Tracks::CheckFlag& right ) 
  : AuxFunBase ( right ) 
  , LoKi::Predicate<LHCb::Track> ( right ) 
  , m_flag ( right.m_flag ) 
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
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Tracks::DistanceOfClosestApproach::result_type 
LoKi::Tracks::DistanceOfClosestApproach::operator() 
  ( LoKi::Tracks::DistanceOfClosestApproach::argument a ) const 
{ 
  const LHCb::Track& t1 = a.first  ;
  const LHCb::Track& t2 = a.second ;
  // try to calculate DOCA...
  Error("(): NOT IMPLEMENTED YET just for tests") ;
  return -10 ;
}
// ============================================================================
// constructor form the tool 
// ============================================================================
LoKi::Tracks::Selector::Selector 
( ITrackSelector* tool ) 
  : LoKi::Predicate<LHCb::Track> () 
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
  : LoKi::Predicate<LHCb::Track> () 
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
  , LoKi::Predicate<LHCb::Track> ( right ) 
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
  Assert ( m_tool.validPointer() , "I ParticleSelector* points to NULL" );  
  // use the tool 
  return m_tool->accept ( t ) ; 
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
  : LoKi::ExtraInfo::CheckInfo<LHCb::Track> ( key ) 
{}
// ============================================================================
// copy constructor 
// ============================================================================
LoKi::Tracks::HasInfo::HasInfo
( const LoKi::Tracks::HasInfo& right ) 
  : LoKi::AuxFunBase                        ( right ) 
  , LoKi::ExtraInfo::CheckInfo<LHCb::Track> ( right ) 
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
  : LoKi::ExtraInfo::GetInfo<LHCb::Track> ( key , def ) 
{}
// ============================================================================
// copy constructor 
// ============================================================================
LoKi::Tracks::Info::Info
( const LoKi::Tracks::Info& right ) 
  : LoKi::AuxFunBase                      ( right ) 
  , LoKi::ExtraInfo::GetInfo<LHCb::Track> ( right ) 
{}
// ============================================================================
// the specific printout 
// ============================================================================
std::ostream& LoKi::Tracks::Info::fillStream( std::ostream& s ) const 
{ return s << "TrINFO(" << index() << "," << value() << ")" ; }
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
