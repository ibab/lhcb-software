// $Id: RecVertices.cpp,v 1.4 2007-12-09 19:14:13 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// Event 
// ============================================================================
#include "Event/RecVertex.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/RecVertices.h"
// ============================================================================
// HltBase 
// ============================================================================
#include "HltBase/HltUtils.h"
// ============================================================================
/** @file
 *  Implementation file for namespace LoKi::RecVertices
 *  @date 2007-08-13 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 */
// ============================================================================
/*  constructor from "info"
 *  @param key info index/mark/key
 */
// ============================================================================
// constructor from the key 
// ============================================================================
LoKi::RecVertices::HasInfo:: HasInfo 
( const int key ) 
  : LoKi::ExtraInfo::CheckInfo<LHCb::RecVertex> ( key ) 
{}
// ============================================================================
// copy constructor 
// ============================================================================
LoKi::RecVertices::HasInfo::HasInfo
( const LoKi::RecVertices::HasInfo& right ) 
  : LoKi::AuxFunBase                            ( right ) 
  , LoKi::ExtraInfo::CheckInfo<LHCb::RecVertex> ( right ) 
{}
// ============================================================================
// the specific printout 
// ============================================================================
std::ostream& 
LoKi::RecVertices::HasInfo::fillStream( std::ostream& s ) const 
{ return s << "RVHASINFO(" << index() << ")" ; }
// ============================================================================
/* constructor from "info"
 *  @param key info index/mark/key
 *  @param def default value for missing key/invalid object 
 */
LoKi::RecVertices::Info::Info
( const int    key , const double def )
  : LoKi::ExtraInfo::GetInfo<LHCb::RecVertex> ( key , def ) 
{}
// ============================================================================
// copy constructor 
// ============================================================================
LoKi::RecVertices::Info::Info
( const LoKi::RecVertices::Info& right ) 
  : LoKi::AuxFunBase                          ( right ) 
  , LoKi::ExtraInfo::GetInfo<LHCb::RecVertex> ( right ) 
{}
// ============================================================================
// the specific printout 
// ============================================================================
std::ostream& LoKi::RecVertices::Info::fillStream( std::ostream& s ) const 
{ return s << "RVINFO(" << index() << "," << value() << ")" ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::RecVertices::MinPt::result_type 
LoKi::RecVertices::MinPt::operator() 
  ( LoKi::RecVertices::MinPt::argument a ) const 
{
  const LHCb::RecVertex& rv = a ;
  return HltUtils::minPT ( rv ) ;                              // RETURN
}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::RecVertices::MaxPt::result_type 
LoKi::RecVertices::MaxPt::operator() 
  ( LoKi::RecVertices::MaxPt::argument a ) const 
{
  const LHCb::RecVertex& rv = a ;
  return HltUtils::maxPT ( rv ) ;                              // RETURN
}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::RecVertices::DeltaZ::result_type 
LoKi::RecVertices::DeltaZ::operator() 
  ( LoKi::RecVertices::DeltaZ::argument a ) const 
{ 
  // unpack argument:
  const LHCb::RecVertex& v1 = a.first  ;
  const LHCb::RecVertex& v2 = a.second ;
  //
  return v1.position().z() - v2.position().z() ;
}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::RecVertices::FC::result_type 
LoKi::RecVertices::FC::operator() 
  ( LoKi::RecVertices::FC::argument a ) const 
{ 
  // unpack argument:
  const LHCb::RecVertex& v1 = a.first  ;
  const LHCb::RecVertex& v2 = a.second ;
  //
  return HltUtils::FC( v1 , v2 ) ;
}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::RecVertices::FC2::result_type 
LoKi::RecVertices::FC2::operator() 
  ( LoKi::RecVertices::FC2::argument a ) const 
{ 
  // unpack argument:
  const LHCb::RecVertex& v1 = a.first  ;
  const LHCb::RecVertex& v2 = a.second ;
  //
  return HltUtils::FC2( v1 , v2 ) ;
}
// ============================================================================
/*  constructor from the function, key and update-flag
 *  @param index the key in LHCb::Track::extraInfo table 
 *  @param fun functionto be evaluated for missing keys 
 *  @param update the flag to allow the insert of mnissing information
 */
// ============================================================================
LoKi::RecVertices::SmartInfo::SmartInfo
( const int                                             index  , 
  const LoKi::BasicFunctors<LHCb::RecVertex>::Function& fun    , 
  const bool                                            update ) 
  : LoKi::ExtraInfo::GetSmartInfo<LHCb::RecVertex> ( index , fun , update )
{}
// ============================================================================
// the specific printout 
// ============================================================================
std::ostream& 
LoKi::RecVertices::SmartInfo::fillStream( std::ostream& s ) const 
{ 
  s << "RVSINFO(" << fun() << "," << index() << "," ;
  if ( update() ) { s << "True"  ; }
  else            { s << "False" ; }
  return s << ")" ;  
}
// ============================================================================
// The END 
// ============================================================================
