// $Id$
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
LoKi::RecVertices::VertexMinPt::result_type 
LoKi::RecVertices::VertexMinPt::operator() 
  ( LoKi::RecVertices::VertexMinPt::argument a ) const 
{
  const LHCb::RecVertex& rv = a ;
  return HltUtils::VertexMinPT ( rv ) ;                              // RETURN
}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::RecVertices::VertexMaxPt::result_type 
LoKi::RecVertices::VertexMaxPt::operator() 
  ( LoKi::RecVertices::VertexMaxPt::argument a ) const 
{
  const LHCb::RecVertex& rv = a ;
  return HltUtils::VertexMaxPT ( rv ) ;                              // RETURN
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
  s << "RVSINFO(" << func () << "," << index() << "," ;
  if ( update() ) { s << "True"  ; }
  else            { s << "False" ; }
  return s << ")" ;  
}
// ============================================================================
// The END 
// ============================================================================
