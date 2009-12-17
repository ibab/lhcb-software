// $Id: TrackIDs.cpp,v 1.1 2009-12-17 20:18:37 ibelyaev Exp $
// ===========================================================================
// Include files 
// ===========================================================================
// STD & STL
// ===========================================================================
#include <algorithm>
// ===========================================================================
// GaudiKernel
// ===========================================================================
#include "GaudiKernel/ToStream.h"
// ===========================================================================
// local
// ===========================================================================
#include "LoKi/TrackIDs.h"
// ============================================================================
// Boost
// ============================================================================
#include "boost/static_assert.hpp"
#include "boost/integer_traits.hpp"
#include "boost/lambda/bind.hpp"
// ============================================================================
/** @file
 *  Implementation file for class : TrackIDs
 *  Collection of functors that deals with LHCbIDs for Tracks 
 *  (on request from Wouter Hulsbergen)
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2009-12-17
 */
// ============================================================================
namespace 
{
  BOOST_STATIC_ASSERT( boost::integer_traits<unsigned int>::is_integral    && 
                       boost::integer_traits<unsigned int>::is_specialized ) ;                     
}
// ============================================================================
// constructor
// ============================================================================
LoKi::Tracks::CountIDs::CountIDs
( LoKi::Tracks::CountIDs::PMF pmf ) 
  : LoKi::BasicFunctors<LHCb::Track>::Function ()
  , m_pmf  ( pmf  ) 
  , m_nick ( ) 
{
  
  if      ( &LHCb::LHCbID::isVelo       == m_pmf ) { m_nick = "isVelo"       ; }
  else if ( &LHCb::LHCbID::isVeloR      == m_pmf ) { m_nick = "isVeloR"      ; }
  else if ( &LHCb::LHCbID::isVeloPhi    == m_pmf ) { m_nick = "isVeloPhi"    ; }
  else if ( &LHCb::LHCbID::isVeloPileUp == m_pmf ) { m_nick = "isVeloPileUp" ; }
  else if ( &LHCb::LHCbID::isVeloPix    == m_pmf ) { m_nick = "isVeloPix"    ; }
  else if ( &LHCb::LHCbID::isVeloPixX   == m_pmf ) { m_nick = "isVeloPixX"   ; }
  else if ( &LHCb::LHCbID::isVeloPixY   == m_pmf ) { m_nick = "isVeloPixY"   ; }
  else if ( &LHCb::LHCbID::isTT         == m_pmf ) { m_nick = "isTT"         ; }
  else if ( &LHCb::LHCbID::isIT         == m_pmf ) { m_nick = "isIT"         ; }
  else if ( &LHCb::LHCbID::isST         == m_pmf ) { m_nick = "isST"         ; }
  else if ( &LHCb::LHCbID::isOT         == m_pmf ) { m_nick = "isOT"         ; }
  else if ( &LHCb::LHCbID::isRich       == m_pmf ) { m_nick = "isRich"       ; }
  else if ( &LHCb::LHCbID::isCalo       == m_pmf ) { m_nick = "isCalo"       ; }
  else if ( &LHCb::LHCbID::isMuon       == m_pmf ) { m_nick = "isMuon"       ; }
  
  Assert ( 0 != m_pmf , "Invalid LHCb::LHCbID member function!" ) ; 
}
// ============================================================================
// constructor
// ============================================================================
LoKi::Tracks::CountIDs::CountIDs
( const std::string& nick ) 
  : LoKi::BasicFunctors<LHCb::Track>::Function ()
  , m_pmf  ( 0    ) 
  , m_nick ( nick ) 
{
  if      ( "isVelo"         == m_nick ) { m_pmf = &LHCb::LHCbID::isVelo       ; }
  else if ( "isVeloR"        == m_nick ) { m_pmf = &LHCb::LHCbID::isVeloR      ; }
  else if ( "isVeloPhi"      == m_nick ) { m_pmf = &LHCb::LHCbID::isVeloPhi    ; }
  else if ( "isVeloPileUp"   == m_nick ) { m_pmf = &LHCb::LHCbID::isVeloPileUp ; }
  else if ( "isVeloPix"      == m_nick ) { m_pmf = &LHCb::LHCbID::isVeloPix    ; }
  else if ( "isVeloPixX"     == m_nick ) { m_pmf = &LHCb::LHCbID::isVeloPixX   ; }
  else if ( "isVeloPixY"     == m_nick ) { m_pmf = &LHCb::LHCbID::isVeloPixY   ; }
  else if ( "isTT"           == m_nick ) { m_pmf = &LHCb::LHCbID::isTT         ; }
  else if ( "isIT"           == m_nick ) { m_pmf = &LHCb::LHCbID::isIT         ; }
  else if ( "isST"           == m_nick ) { m_pmf = &LHCb::LHCbID::isST         ; }
  else if ( "isRich"         == m_nick ) { m_pmf = &LHCb::LHCbID::isRich       ; }
  else if ( "isCalo"         == m_nick ) { m_pmf = &LHCb::LHCbID::isCalo       ; }
  else if ( "isMuon"         == m_nick ) { m_pmf = &LHCb::LHCbID::isMuon       ; }
  
  if ( 0 == m_pmf ) 
  { Exception ( "Invalid LHCb::LHCbID member function: '" + m_nick + "'" ) ; }
  
}
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::Tracks::CountIDs::~CountIDs(){}
// ============================================================================
// MANDATORY: clone method ("virtual consttructor")
// ============================================================================
LoKi::Tracks::CountIDs*
LoKi::Tracks::CountIDs::clone() const 
{ return  new LoKi::Tracks::CountIDs ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Tracks::CountIDs::result_type 
LoKi::Tracks::CountIDs::operator() 
  ( LoKi::Tracks::CountIDs::argument t ) const 
{
  //
  const std::vector<LHCb::LHCbID>&  lhcbids = t.lhcbIDs() ;
  //
  return 
    std::count_if ( lhcbids.begin() , 
                    lhcbids.end() , 
                    boost::lambda::bind ( m_pmf , boost::lambda::_1 ) ) ;  
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::Tracks::CountIDs::fillStream( std::ostream& s ) const 
{ return s << " TrIDC( '" << m_nick << "' ) " ; }


// ============================================================================
// constructor
// ============================================================================
LoKi::Tracks::CountOTIDs::CountOTIDs
( LoKi::Tracks::CountOTIDs::PMF pmf , 
  const unsigned int            i   ) 
  : LoKi::BasicFunctors<LHCb::Track>::Function ()
  , m_pmf   ( pmf   ) 
  , m_imin  ( boost::integer_traits<unsigned int>::const_max ) 
  , m_imax  ( boost::integer_traits<unsigned int>::const_min ) 
  , m_uints ( 1 , i )
  , m_nick  (       )
{
  
  if      ( &LHCb::OTChannelID::geometry                == m_pmf ) { m_nick = "geometry"                    ; }
  else if ( &LHCb::OTChannelID::sequentialUniqueLayer   == m_pmf ) { m_nick = "sequentialUniqueLayer"       ; }
  else if ( &LHCb::OTChannelID::sequentialUniqueQuarter == m_pmf ) { m_nick = "sequentialUniqueQuarter"     ; }
  else if ( &LHCb::OTChannelID::sequentialUniqueModule  == m_pmf ) { m_nick = "sequentialUniqueModule"      ; }
  else if ( &LHCb::OTChannelID::sequentialUniqueOtis    == m_pmf ) { m_nick = "sequentialUniqueOtis"        ; }
  else if ( &LHCb::OTChannelID::sequentialUniqueStraw   == m_pmf ) { m_nick = "sequentialUniqueStraw"       ; }
  else if ( &LHCb::OTChannelID::channelID               == m_pmf ) { m_nick = "channelID"                   ; }
  else if ( &LHCb::OTChannelID::tdcTime                 == m_pmf ) { m_nick = "tdcTime"                     ; }
  else if ( &LHCb::OTChannelID::module                  == m_pmf ) { m_nick = "module"                      ; }
  else if ( &LHCb::OTChannelID::quarter                 == m_pmf ) { m_nick = "quarter"                     ; }
  else if ( &LHCb::OTChannelID::layer                   == m_pmf ) { m_nick = "layer"                       ; }
  else if ( &LHCb::OTChannelID::straw                   == m_pmf ) { m_nick = "straw"                       ; }
  else if ( &LHCb::OTChannelID::station                 == m_pmf ) { m_nick = "station"                     ; }
  else if ( &LHCb::OTChannelID::uniqueModule            == m_pmf ) { m_nick = "uniqueModule"                ; }
  else if ( &LHCb::OTChannelID::uniqueQuarter           == m_pmf ) { m_nick = "uniqueQuarter"               ; }
  else if ( &LHCb::OTChannelID::uniqueLayer             == m_pmf ) { m_nick = "uniqueLayer"                 ; }
  else if ( &LHCb::OTChannelID::uniqueStraw             == m_pmf ) { m_nick = "uniqueStraw"                 ; }
  
  Assert ( 0 != m_pmf , "Invalid LHCb::OTchannelID member function!" ) ; 
}
// ============================================================================
// constructor
// ============================================================================
LoKi::Tracks::CountOTIDs::CountOTIDs
( LoKi::Tracks::CountOTIDs::PMF          pmf , 
  const LoKi::Tracks::CountOTIDs::UInts& i   ) 
  : LoKi::BasicFunctors<LHCb::Track>::Function ()
  , m_pmf   ( pmf   ) 
  , m_uints ( i     )
  , m_imin  ( boost::integer_traits<unsigned int>::const_max ) 
  , m_imax  ( boost::integer_traits<unsigned int>::const_min ) 
  , m_nick  (       )
{
  
  if      ( &LHCb::OTChannelID::geometry                == m_pmf ) { m_nick = "geometry"                    ; }
  else if ( &LHCb::OTChannelID::sequentialUniqueLayer   == m_pmf ) { m_nick = "sequentialUniqueLayer"       ; }
  else if ( &LHCb::OTChannelID::sequentialUniqueQuarter == m_pmf ) { m_nick = "sequentialUniqueQuarter"     ; }
  else if ( &LHCb::OTChannelID::sequentialUniqueModule  == m_pmf ) { m_nick = "sequentialUniqueModule"      ; }
  else if ( &LHCb::OTChannelID::sequentialUniqueOtis    == m_pmf ) { m_nick = "sequentialUniqueOtis"        ; }
  else if ( &LHCb::OTChannelID::sequentialUniqueStraw   == m_pmf ) { m_nick = "sequentialUniqueStraw"       ; }
  else if ( &LHCb::OTChannelID::channelID               == m_pmf ) { m_nick = "channelID"                   ; }
  else if ( &LHCb::OTChannelID::tdcTime                 == m_pmf ) { m_nick = "tdcTime"                     ; }
  else if ( &LHCb::OTChannelID::module                  == m_pmf ) { m_nick = "module"                      ; }
  else if ( &LHCb::OTChannelID::quarter                 == m_pmf ) { m_nick = "quarter"                     ; }
  else if ( &LHCb::OTChannelID::layer                   == m_pmf ) { m_nick = "layer"                       ; }
  else if ( &LHCb::OTChannelID::straw                   == m_pmf ) { m_nick = "straw"                       ; }
  else if ( &LHCb::OTChannelID::station                 == m_pmf ) { m_nick = "station"                     ; }
  else if ( &LHCb::OTChannelID::uniqueModule            == m_pmf ) { m_nick = "uniqueModule"                ; }
  else if ( &LHCb::OTChannelID::uniqueQuarter           == m_pmf ) { m_nick = "uniqueQuarter"               ; }
  else if ( &LHCb::OTChannelID::uniqueLayer             == m_pmf ) { m_nick = "uniqueLayer"                 ; }
  else if ( &LHCb::OTChannelID::uniqueStraw             == m_pmf ) { m_nick = "uniqueStraw"                 ; }
  
  Assert ( 0 != m_pmf , "Invalid LHCb::OTchannelID member function!" ) ; 

  Assert ( !m_uints.empty() , "Empty vector of values is specified!" ) ;
}
// ============================================================================
// constructor
// ============================================================================
LoKi::Tracks::CountOTIDs::CountOTIDs
( LoKi::Tracks::CountOTIDs::PMF pmf  , 
  const unsigned int            imin ,   
  const unsigned int            imax )
 : LoKi::BasicFunctors<LHCb::Track>::Function ()
 , m_pmf   ( pmf   ) 
 , m_uints ( )
 , m_imin  ( imin ) 
 , m_imax  ( imax ) 
 , m_nick  (      )
{
  
  if      ( &LHCb::OTChannelID::geometry                == m_pmf ) { m_nick = "geometry"                    ; }
  else if ( &LHCb::OTChannelID::sequentialUniqueLayer   == m_pmf ) { m_nick = "sequentialUniqueLayer"       ; }
  else if ( &LHCb::OTChannelID::sequentialUniqueQuarter == m_pmf ) { m_nick = "sequentialUniqueQuarter"     ; }
  else if ( &LHCb::OTChannelID::sequentialUniqueModule  == m_pmf ) { m_nick = "sequentialUniqueModule"      ; }
  else if ( &LHCb::OTChannelID::sequentialUniqueOtis    == m_pmf ) { m_nick = "sequentialUniqueOtis"        ; }
  else if ( &LHCb::OTChannelID::sequentialUniqueStraw   == m_pmf ) { m_nick = "sequentialUniqueStraw"       ; }
  else if ( &LHCb::OTChannelID::channelID               == m_pmf ) { m_nick = "channelID"                   ; }
  else if ( &LHCb::OTChannelID::tdcTime                 == m_pmf ) { m_nick = "tdcTime"                     ; }
  else if ( &LHCb::OTChannelID::module                  == m_pmf ) { m_nick = "module"                      ; }
  else if ( &LHCb::OTChannelID::quarter                 == m_pmf ) { m_nick = "quarter"                     ; }
  else if ( &LHCb::OTChannelID::layer                   == m_pmf ) { m_nick = "layer"                       ; }
  else if ( &LHCb::OTChannelID::straw                   == m_pmf ) { m_nick = "straw"                       ; }
  else if ( &LHCb::OTChannelID::station                 == m_pmf ) { m_nick = "station"                     ; }
  else if ( &LHCb::OTChannelID::uniqueModule            == m_pmf ) { m_nick = "uniqueModule"                ; }
  else if ( &LHCb::OTChannelID::uniqueQuarter           == m_pmf ) { m_nick = "uniqueQuarter"               ; }
  else if ( &LHCb::OTChannelID::uniqueLayer             == m_pmf ) { m_nick = "uniqueLayer"                 ; }
  else if ( &LHCb::OTChannelID::uniqueStraw             == m_pmf ) { m_nick = "uniqueStraw"                 ; }
  
  Assert ( 0 != m_pmf    , "Invalid LHCb::OTChannelID member function!" ) ; 
  Assert ( imin <= imax  , "Invalid range of values" ) ; 
}


// ============================================================================
// constructor
// ============================================================================
LoKi::Tracks::CountOTIDs::CountOTIDs
( const std::string&                   nick , 
  const LoKi::Tracks::CountOTIDs::UInts& i    ) 
  : LoKi::BasicFunctors<LHCb::Track>::Function ()
  , m_pmf   ( 0     ) 
  , m_uints ( i     )
  , m_imin  ( boost::integer_traits<unsigned int>::const_max ) 
  , m_imax  ( boost::integer_traits<unsigned int>::const_min ) 
  , m_nick  ( nick  )
{
  
  if      ( "geometry"                == nick ) { m_pmf = &LHCb::OTChannelID::geometry                ; }
  else if ( "sequentialUniqueLayer"   == nick ) { m_pmf = &LHCb::OTChannelID::sequentialUniqueLayer   ; }
  else if ( "sequentialUniqueQuarter" == nick ) { m_pmf = &LHCb::OTChannelID::sequentialUniqueQuarter ; }
  else if ( "sequentialUniqueModule"  == nick ) { m_pmf = &LHCb::OTChannelID::sequentialUniqueModule  ; }
  else if ( "sequentialUniqueOtis"    == nick ) { m_pmf = &LHCb::OTChannelID::sequentialUniqueOtis    ; }
  else if ( "sequentialUniqueStraw"   == nick ) { m_pmf = &LHCb::OTChannelID::sequentialUniqueStraw   ; }
  else if ( "channelID"               == nick ) { m_pmf = &LHCb::OTChannelID::channelID               ; }
  else if ( "tdcTime"                 == nick ) { m_pmf = &LHCb::OTChannelID::tdcTime                 ; }
  else if ( "module"                  == nick ) { m_pmf = &LHCb::OTChannelID::module                  ; }
  else if ( "layer"                   == nick ) { m_pmf = &LHCb::OTChannelID::layer                   ; }
  else if ( "straw"                   == nick ) { m_pmf = &LHCb::OTChannelID::straw                   ; }
  else if ( "station"                 == nick ) { m_pmf = &LHCb::OTChannelID::station                 ; }
  else if ( "uniqueModule"            == nick ) { m_pmf = &LHCb::OTChannelID::uniqueModule            ; }
  else if ( "uniqueQuarter"           == nick ) { m_pmf = &LHCb::OTChannelID::uniqueQuarter           ; }
  else if ( "uniqueLayer"             == nick ) { m_pmf = &LHCb::OTChannelID::uniqueLayer             ; }
  else if ( "uniqueStraw"             == nick ) { m_pmf = &LHCb::OTChannelID::uniqueStraw             ; }
  
  if ( 0 == m_pmf ) 
  { Exception ( "Invalid LHCb::OTChannelID member function: '" + m_nick + "'" ) ; }

  Assert ( !m_uints.empty() , "Empty vector of values is specified!" ) ;
  
}


// ============================================================================
// constructor
// ============================================================================
LoKi::Tracks::CountOTIDs::CountOTIDs
( const std::string&  nick , 
  const unsigned int  i    ) 
  : LoKi::BasicFunctors<LHCb::Track>::Function ()
  , m_pmf   ( 0     ) 
  , m_uints ( 1 , i )
  , m_imin  ( boost::integer_traits<unsigned int>::const_max ) 
  , m_imax  ( boost::integer_traits<unsigned int>::const_min ) 
  , m_nick  ( nick  )
{
  
  if      ( "geometry"                == nick ) { m_pmf = &LHCb::OTChannelID::geometry                ; }
  else if ( "sequentialUniqueLayer"   == nick ) { m_pmf = &LHCb::OTChannelID::sequentialUniqueLayer   ; }
  else if ( "sequentialUniqueQuarter" == nick ) { m_pmf = &LHCb::OTChannelID::sequentialUniqueQuarter ; }
  else if ( "sequentialUniqueModule"  == nick ) { m_pmf = &LHCb::OTChannelID::sequentialUniqueModule  ; }
  else if ( "sequentialUniqueOtis"    == nick ) { m_pmf = &LHCb::OTChannelID::sequentialUniqueOtis    ; }
  else if ( "sequentialUniqueStraw"   == nick ) { m_pmf = &LHCb::OTChannelID::sequentialUniqueStraw   ; }
  else if ( "channelID"               == nick ) { m_pmf = &LHCb::OTChannelID::channelID               ; }
  else if ( "tdcTime"                 == nick ) { m_pmf = &LHCb::OTChannelID::tdcTime                 ; }
  else if ( "module"                  == nick ) { m_pmf = &LHCb::OTChannelID::module                  ; }
  else if ( "layer"                   == nick ) { m_pmf = &LHCb::OTChannelID::layer                   ; }
  else if ( "straw"                   == nick ) { m_pmf = &LHCb::OTChannelID::straw                   ; }
  else if ( "station"                 == nick ) { m_pmf = &LHCb::OTChannelID::station                 ; }
  else if ( "uniqueModule"            == nick ) { m_pmf = &LHCb::OTChannelID::uniqueModule            ; }
  else if ( "uniqueQuarter"           == nick ) { m_pmf = &LHCb::OTChannelID::uniqueQuarter           ; }
  else if ( "uniqueLayer"             == nick ) { m_pmf = &LHCb::OTChannelID::uniqueLayer             ; }
  else if ( "uniqueStraw"             == nick ) { m_pmf = &LHCb::OTChannelID::uniqueStraw             ; }
  
  if ( 0 == m_pmf ) 
  { Exception ( "Invalid LHCb::OTChannelID member function: '" + m_nick + "'" ) ; }

}

// ============================================================================
// constructor
// ============================================================================
LoKi::Tracks::CountOTIDs::CountOTIDs
( const std::string&  nick , 
  const unsigned int  imin , 
  const unsigned int  imax ) 
  : LoKi::BasicFunctors<LHCb::Track>::Function ()
  , m_pmf   ( 0     ) 
  , m_uints ( )
  , m_imin  ( imin  ) 
  , m_imax  ( imax  ) 
  , m_nick  ( nick  )
{
  
  if      ( "geometry"                == nick ) { m_pmf = &LHCb::OTChannelID::geometry                ; }
  else if ( "sequentialUniqueLayer"   == nick ) { m_pmf = &LHCb::OTChannelID::sequentialUniqueLayer   ; }
  else if ( "sequentialUniqueQuarter" == nick ) { m_pmf = &LHCb::OTChannelID::sequentialUniqueQuarter ; }
  else if ( "sequentialUniqueModule"  == nick ) { m_pmf = &LHCb::OTChannelID::sequentialUniqueModule  ; }
  else if ( "sequentialUniqueOtis"    == nick ) { m_pmf = &LHCb::OTChannelID::sequentialUniqueOtis    ; }
  else if ( "sequentialUniqueStraw"   == nick ) { m_pmf = &LHCb::OTChannelID::sequentialUniqueStraw   ; }
  else if ( "channelID"               == nick ) { m_pmf = &LHCb::OTChannelID::channelID               ; }
  else if ( "tdcTime"                 == nick ) { m_pmf = &LHCb::OTChannelID::tdcTime                 ; }
  else if ( "module"                  == nick ) { m_pmf = &LHCb::OTChannelID::module                  ; }
  else if ( "layer"                   == nick ) { m_pmf = &LHCb::OTChannelID::layer                   ; }
  else if ( "straw"                   == nick ) { m_pmf = &LHCb::OTChannelID::straw                   ; }
  else if ( "station"                 == nick ) { m_pmf = &LHCb::OTChannelID::station                 ; }
  else if ( "uniqueModule"            == nick ) { m_pmf = &LHCb::OTChannelID::uniqueModule            ; }
  else if ( "uniqueQuarter"           == nick ) { m_pmf = &LHCb::OTChannelID::uniqueQuarter           ; }
  else if ( "uniqueLayer"             == nick ) { m_pmf = &LHCb::OTChannelID::uniqueLayer             ; }
  else if ( "uniqueStraw"             == nick ) { m_pmf = &LHCb::OTChannelID::uniqueStraw             ; }
  
  if ( 0 == m_pmf ) 
  { Exception ( "Invalid LHCb::OTChannelID member function: '" + m_nick + "'" ) ; }
  
  Assert ( imin <= imax  , "Invalid range of values" ) ; 
  
}
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::Tracks::CountOTIDs::~CountOTIDs(){}
// ============================================================================
// MANDATORY: clone method ('virtual constructor')
// ============================================================================
LoKi::Tracks::CountOTIDs*
LoKi::Tracks::CountOTIDs::clone() const 
{ return new LoKi::Tracks::CountOTIDs ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Tracks::CountOTIDs::result_type 
LoKi::Tracks::CountOTIDs::operator()
  ( LoKi::Tracks::CountOTIDs::argument t ) const 
{
  //
  typedef  std::vector<LHCb::LHCbID> IDs ;
  //
  const IDs& lhcbids = t.lhcbIDs() ;
  //
  size_t res = 0 ;
  //
  for ( IDs::const_iterator id = lhcbids.begin() ; lhcbids.end() != id ; ++id ) 
  {
    if ( ! id->isOT() ) { continue ; }
    //
    const unsigned int r = (id->otID().*m_pmf) () ;
    if      ( m_uints.empty()     ) { if ( m_imin <= r && r <= m_imax ) { ++res ; } }
    else if ( 1 == m_uints.size() ) { if ( m_uints.front() == r       ) { ++res ; } }
    else 
    { 
      if  ( m_uints.end() != std::find ( m_uints.begin() , m_uints.end() , r ) ) { ++res ; } 
    }
  }
  //
  return res;
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::Tracks::CountOTIDs::fillStream( std::ostream& s ) const 
{ 
  s << " TrOTIDC( '" << m_nick << "' , " ;
  //
  if      ( m_uints.empty()     ) { s << m_imin << " , " << m_imax ; }
  else if ( 1 == m_uints.size() ) { s << m_uints.front()     ; }
  else { Gaudi::Utils::toStream ( m_uints , s ) ; }
  //
  return s << " ) " ;
}






// ============================================================================
// The END 
// ============================================================================
