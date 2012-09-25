// $Id$
// ============================================================================
// Include files 
// ============================================================================
// STD & STL
// ============================================================================
#include <algorithm>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ToStream.h"
// ============================================================================
// local
// ============================================================================
#include "LoKi/TrackIDs.h"
// ============================================================================
#ifdef __INTEL_COMPILER
#pragma warning(disable:177) //  variable ... was declared but never referenced
#endif
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
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  By usage of this code one clearly states the disagreement 
 *  with the campain of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
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
  : LoKi::BasicFunctors<const LHCb::Track*>::Function ()
  , m_pmf  ( pmf  ) 
  , m_nick ( ) 
{
  
  if      ( &LHCb::LHCbID::isVelo       == m_pmf ) { m_nick = "isVelo"       ; }
  else if ( &LHCb::LHCbID::isVeloR      == m_pmf ) { m_nick = "isVeloR"      ; }
  else if ( &LHCb::LHCbID::isVeloPhi    == m_pmf ) { m_nick = "isVeloPhi"    ; }
  else if ( &LHCb::LHCbID::isVeloPileUp == m_pmf ) { m_nick = "isVeloPileUp" ; }
  else if ( &LHCb::LHCbID::isVP         == m_pmf ) { m_nick = "isVP"         ; }
  else if ( &LHCb::LHCbID::isVPX        == m_pmf ) { m_nick = "isVPX"        ; }
  else if ( &LHCb::LHCbID::isVPY        == m_pmf ) { m_nick = "isVPY"        ; }
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
  : LoKi::BasicFunctors<const LHCb::Track*>::Function ()
  , m_pmf  ( 0    ) 
  , m_nick ( nick ) 
{
  if      ( "isVelo"         == m_nick ) { m_pmf = &LHCb::LHCbID::isVelo       ; }
  else if ( "isVeloR"        == m_nick ) { m_pmf = &LHCb::LHCbID::isVeloR      ; }
  else if ( "isVeloPhi"      == m_nick ) { m_pmf = &LHCb::LHCbID::isVeloPhi    ; }
  else if ( "isVeloPileUp"   == m_nick ) { m_pmf = &LHCb::LHCbID::isVeloPileUp ; }
  else if ( "isVP"           == m_nick ) { m_pmf = &LHCb::LHCbID::isVP         ; }
  else if ( "isVPX"          == m_nick ) { m_pmf = &LHCb::LHCbID::isVPX        ; }
  else if ( "isVPY"          == m_nick ) { m_pmf = &LHCb::LHCbID::isVPY        ; }
  else if ( "isTT"           == m_nick ) { m_pmf = &LHCb::LHCbID::isTT         ; }
  else if ( "isIT"           == m_nick ) { m_pmf = &LHCb::LHCbID::isIT         ; }
  else if ( "isOT"           == m_nick ) { m_pmf = &LHCb::LHCbID::isOT         ; }
  else if ( "isST"           == m_nick ) { m_pmf = &LHCb::LHCbID::isST         ; }
  else if ( "isRich"         == m_nick ) { m_pmf = &LHCb::LHCbID::isRich       ; }
  else if ( "isCalo"         == m_nick ) { m_pmf = &LHCb::LHCbID::isCalo       ; }
  else if ( "isMuon"         == m_nick ) { m_pmf = &LHCb::LHCbID::isMuon       ; }
  // Backwards compatability
  else if ( "isVeloPix"      == m_nick ) { m_pmf = &LHCb::LHCbID::isVP         ; }
  else if ( "isVeloPixX"     == m_nick ) { m_pmf = &LHCb::LHCbID::isVPX        ; }
  else if ( "isVeloPixY"     == m_nick ) { m_pmf = &LHCb::LHCbID::isVPY        ; }
  
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
  if ( 0 == t ) 
  {
    Error ("LHCb::Track* points to NULL, return -1") ;
    return -1 ;
  }
  //
  const std::vector<LHCb::LHCbID>&  lhcbids = t->lhcbIDs() ;
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
  : LoKi::BasicFunctors<const LHCb::Track*>::Function ()
  , m_pmf   ( pmf   ) 
  , m_uints ( 1 , i )
  , m_imin  ( boost::integer_traits<unsigned int>::const_max ) 
  , m_imax  ( boost::integer_traits<unsigned int>::const_min ) 
  , m_nick  (       )
{
  
  setNick ( m_pmf ) ;
  
  Assert ( 0 != m_pmf , "Invalid LHCb::OTchannelID member function!" ) ; 
}
// ============================================================================
// constructor
// ============================================================================
LoKi::Tracks::CountOTIDs::CountOTIDs
( LoKi::Tracks::CountOTIDs::PMF          pmf , 
  const std::vector<unsigned int>& i   ) 
  : LoKi::BasicFunctors<const LHCb::Track*>::Function ()
  , m_pmf   ( pmf   ) 
  , m_uints ( i     )
  , m_imin  ( boost::integer_traits<unsigned int>::const_max ) 
  , m_imax  ( boost::integer_traits<unsigned int>::const_min ) 
  , m_nick  (       )
{
  
  setNick ( m_pmf ) ;
  
  Assert ( 0 != m_pmf , "Invalid LHCb::OTchannelID member function!" ) ; 
  
  Assert ( !m_uints.empty() , "Empty vector of values is specified!" ) ;
}
// ============================================================================
// constructor
// ============================================================================
LoKi::Tracks::CountOTIDs::CountOTIDs
( const unsigned int            imin ,   
  LoKi::Tracks::CountOTIDs::PMF pmf  , 
  const unsigned int            imax )
  : LoKi::BasicFunctors<const LHCb::Track*>::Function ()
  , m_pmf   ( pmf   ) 
  , m_uints ( )
  , m_imin  ( imin ) 
  , m_imax  ( imax ) 
  , m_nick  (      )
{
  setNick ( m_pmf ) ;
  
  Assert ( 0 != m_pmf    , "Invalid LHCb::OTChannelID member function!" ) ; 
  
  Assert ( imin <= imax  , "Invalid range of values" ) ; 
}
// ============================================================================
// set nick
// ============================================================================
void LoKi::Tracks::CountOTIDs::setNick ( LoKi::Tracks::CountOTIDs::PMF pmf ) 
{ 
  if      ( &LHCb::OTChannelID::geometry                == pmf ) { m_nick = "geometry"                    ; }
  else if ( &LHCb::OTChannelID::sequentialUniqueLayer   == pmf ) { m_nick = "sequentialUniqueLayer"       ; }
  else if ( &LHCb::OTChannelID::sequentialUniqueQuarter == pmf ) { m_nick = "sequentialUniqueQuarter"     ; }
  else if ( &LHCb::OTChannelID::sequentialUniqueModule  == pmf ) { m_nick = "sequentialUniqueModule"      ; }
  else if ( &LHCb::OTChannelID::sequentialUniqueOtis    == pmf ) { m_nick = "sequentialUniqueOtis"        ; }
  else if ( &LHCb::OTChannelID::sequentialUniqueStraw   == pmf ) { m_nick = "sequentialUniqueStraw"       ; }
  else if ( &LHCb::OTChannelID::channelID               == pmf ) { m_nick = "channelID"                   ; }
  else if ( &LHCb::OTChannelID::tdcTime                 == pmf ) { m_nick = "tdcTime"                     ; }
  else if ( &LHCb::OTChannelID::module                  == pmf ) { m_nick = "module"                      ; }
  else if ( &LHCb::OTChannelID::quarter                 == pmf ) { m_nick = "quarter"                     ; }
  else if ( &LHCb::OTChannelID::layer                   == pmf ) { m_nick = "layer"                       ; }
  else if ( &LHCb::OTChannelID::straw                   == pmf ) { m_nick = "straw"                       ; }
  else if ( &LHCb::OTChannelID::station                 == pmf ) { m_nick = "station"                     ; }
  else if ( &LHCb::OTChannelID::uniqueModule            == pmf ) { m_nick = "uniqueModule"                ; }
  else if ( &LHCb::OTChannelID::uniqueQuarter           == pmf ) { m_nick = "uniqueQuarter"               ; }
  else if ( &LHCb::OTChannelID::uniqueLayer             == pmf ) { m_nick = "uniqueLayer"                 ; }
  else if ( &LHCb::OTChannelID::uniqueStraw             == pmf ) { m_nick = "uniqueStraw"                 ; }
  else { Exception ( "Invalid LHCb::OTChannelID member function!") ; }
}
// ============================================================================
// constructor
// ============================================================================
LoKi::Tracks::CountOTIDs::CountOTIDs
( const std::string&               nick , 
  const std::vector<unsigned int>& i    ) 
  : LoKi::BasicFunctors<const LHCb::Track*>::Function ()
  , m_pmf   ( 0     ) 
  , m_uints ( i     )
  , m_imin  ( boost::integer_traits<unsigned int>::const_max ) 
  , m_imax  ( boost::integer_traits<unsigned int>::const_min ) 
  , m_nick  ( nick  )
{
  
  setPmf ( m_nick ) ;
  
  Assert ( 0 != m_pmf , "Invalid LHCb::OTChannelID member function: '" + m_nick + "'" ) ; 
  
  Assert ( !m_uints.empty() , "Empty vector of values is specified!" ) ;
  
}
// ============================================================================
// constructor
// ============================================================================
LoKi::Tracks::CountOTIDs::CountOTIDs
( const std::string&  nick , 
  const unsigned int  i    ) 
  : LoKi::BasicFunctors<const LHCb::Track*>::Function ()
  , m_pmf   ( 0     ) 
  , m_uints ( 1 , i )
  , m_imin  ( boost::integer_traits<unsigned int>::const_max ) 
  , m_imax  ( boost::integer_traits<unsigned int>::const_min ) 
  , m_nick  ( nick  )
{

  setPmf ( m_nick ) ;
  
  Assert ( 0 != m_pmf , "Invalid LHCb::OTChannelID member function: '" + m_nick + "'" ) ; 
  
}

// ============================================================================
// constructor
// ============================================================================
LoKi::Tracks::CountOTIDs::CountOTIDs
( const unsigned int  imin , 
  const std::string&  nick , 
  const unsigned int  imax ) 
  : LoKi::BasicFunctors<const LHCb::Track*>::Function ()
  , m_pmf   ( 0     ) 
  , m_uints ( )
  , m_imin  ( imin  ) 
  , m_imax  ( imax  ) 
  , m_nick  ( nick  )
{
  
  setPmf ( m_nick ) ;
  
  Assert ( 0 != m_pmf   , "Invalid LHCb::OTChannelID member function: '" + m_nick + "'" ) ; 
  
  Assert ( imin <= imax  , "Invalid range of values" ) ; 
  
}
// ============================================================================
// set PMF properly
// ============================================================================
void LoKi::Tracks::CountOTIDs::setPmf ( const std::string& nick ) 
{
  if      ( "geometry"                == nick ) { m_pmf = &LHCb::OTChannelID::geometry                ; }
  else if ( "sequentialUniqueLayer"   == nick ) { m_pmf = &LHCb::OTChannelID::sequentialUniqueLayer   ; }
  else if ( "sequentialUniqueQuarter" == nick ) { m_pmf = &LHCb::OTChannelID::sequentialUniqueQuarter ; }
  else if ( "sequentialUniqueModule"  == nick ) { m_pmf = &LHCb::OTChannelID::sequentialUniqueModule  ; }
  else if ( "sequentialUniqueOtis"    == nick ) { m_pmf = &LHCb::OTChannelID::sequentialUniqueOtis    ; }
  else if ( "sequentialUniqueStraw"   == nick ) { m_pmf = &LHCb::OTChannelID::sequentialUniqueStraw   ; }
  else if ( "channelID"               == nick ) { m_pmf = &LHCb::OTChannelID::channelID               ; }
  else if ( "channel"                 == nick ) { m_pmf = &LHCb::OTChannelID::channelID               ; }
  else if ( "tdcTime"                 == nick ) { m_pmf = &LHCb::OTChannelID::tdcTime                 ; }
  else if ( "module"                  == nick ) { m_pmf = &LHCb::OTChannelID::module                  ; }
  else if ( "layer"                   == nick ) { m_pmf = &LHCb::OTChannelID::layer                   ; }
  else if ( "straw"                   == nick ) { m_pmf = &LHCb::OTChannelID::straw                   ; }
  else if ( "station"                 == nick ) { m_pmf = &LHCb::OTChannelID::station                 ; }
  else if ( "uniqueModule"            == nick ) { m_pmf = &LHCb::OTChannelID::uniqueModule            ; }
  else if ( "uniqueQuarter"           == nick ) { m_pmf = &LHCb::OTChannelID::uniqueQuarter           ; }
  else if ( "uniqueLayer"             == nick ) { m_pmf = &LHCb::OTChannelID::uniqueLayer             ; }
  else if ( "uniqueStraw"             == nick ) { m_pmf = &LHCb::OTChannelID::uniqueStraw             ; }
  else { Exception ( "Invalid LHCb::OTChannelID member function '" + nick + "'") ; }
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
  //
  if ( 0 == t ) 
  {
    Error ("LHCb::Track* points to NULL, return -1") ;
    return -1 ;
  }
  //
  typedef  std::vector<LHCb::LHCbID> IDs ;
  //
  const IDs& lhcbids = t->lhcbIDs() ;
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
  s << " TrOTIDC( " ;
  //
  if      ( m_uints.empty()     ) { s << m_imin << " ,'" << m_nick << "', "<< m_imax ; }
  else if ( 1 == m_uints.size() ) { s << "'" << m_nick << "', " << m_uints.front()     ; }
  else 
  {
    s << "'" << m_nick << "', " ;
    Gaudi::Utils::toStream ( m_uints , s ) ;
  }
  //
  return s << " ) " ;
}



// ============================================================================
// constructor
// ============================================================================
LoKi::Tracks::CountSTIDs::CountSTIDs
( LoKi::Tracks::CountSTIDs::PMF pmf , 
  const unsigned int            i   ) 
  : LoKi::BasicFunctors<const LHCb::Track*>::Function ()
  , m_pmf   ( pmf   ) 
  , m_uints ( 1 , i )
  , m_imin  ( boost::integer_traits<unsigned int>::const_max ) 
  , m_imax  ( boost::integer_traits<unsigned int>::const_min ) 
  , m_nick  (       )
{
  
  setNick ( m_pmf ) ;
  
  Assert ( 0 != m_pmf , "Invalid LHCb::STChannelID member function!" ) ; 
}
// ============================================================================
// constructor
// ============================================================================
LoKi::Tracks::CountSTIDs::CountSTIDs
( LoKi::Tracks::CountSTIDs::PMF    pmf , 
  const std::vector<unsigned int>& i   ) 
  : LoKi::BasicFunctors<const LHCb::Track*>::Function ()
  , m_pmf   ( pmf   ) 
  , m_uints ( i     )
  , m_imin  ( boost::integer_traits<unsigned int>::const_max ) 
  , m_imax  ( boost::integer_traits<unsigned int>::const_min ) 
  , m_nick  (       )
{
  
  setNick ( m_pmf ) ;
  
  Assert ( 0 != m_pmf , "Invalid LHCb::STChannelID member function!" ) ; 
  
  Assert ( !m_uints.empty() , "Empty vector of values is specified!" ) ;
}
// ============================================================================
// constructor
// ============================================================================
LoKi::Tracks::CountSTIDs::CountSTIDs
( const unsigned int            imin ,   
  LoKi::Tracks::CountSTIDs::PMF pmf  , 
  const unsigned int            imax )
  : LoKi::BasicFunctors<const LHCb::Track*>::Function ()
  , m_pmf   ( pmf   ) 
  , m_uints ( )
  , m_imin  ( imin ) 
  , m_imax  ( imax ) 
  , m_nick  (      )
{
  setNick ( m_pmf ) ;
  
  Assert ( 0 != m_pmf    , "Invalid LHCb::STChannelID member function!" ) ; 
  
  Assert ( imin <= imax  , "Invalid range of values" ) ; 
}
// ============================================================================
// set nick
// ============================================================================
void LoKi::Tracks::CountSTIDs::setNick ( LoKi::Tracks::CountSTIDs::PMF pmf ) 
{ 
  if      ( &LHCb::STChannelID::strip           == pmf ) { m_nick = "strip"            ; }
  else if ( &LHCb::STChannelID::sector          == pmf ) { m_nick = "sector"           ; }
  else if ( &LHCb::STChannelID::detRegion       == pmf ) { m_nick = "detRegion"        ; }
  else if ( &LHCb::STChannelID::layer           == pmf ) { m_nick = "layer"            ; }
  else if ( &LHCb::STChannelID::station         == pmf ) { m_nick = "station"          ; }
  else if ( &LHCb::STChannelID::uniqueLayer     == pmf ) { m_nick = "uniqueLayer"      ; }
  else if ( &LHCb::STChannelID::uniqueDetRegion == pmf ) { m_nick = "uniqueDetRegion"  ; }
  else if ( &LHCb::STChannelID::uniqueSector    == pmf ) { m_nick = "uniqueSector"     ; }
  else if ( &LHCb::STChannelID::channelID       == pmf ) { m_nick = "channelID"        ; }
  else { Exception ( "Invalid LHCb::STChannelID member function!") ; }
}

// ============================================================================
// constructor
// ============================================================================
LoKi::Tracks::CountSTIDs::CountSTIDs
( const std::string&                   nick , 
  const std::vector<unsigned int>& i    ) 
  : LoKi::BasicFunctors<const LHCb::Track*>::Function ()
  , m_pmf   ( 0     ) 
  , m_uints ( i     )
  , m_imin  ( boost::integer_traits<unsigned int>::const_max ) 
  , m_imax  ( boost::integer_traits<unsigned int>::const_min ) 
  , m_nick  ( nick  )
{
  
  setPmf ( m_nick ) ;
  
  Assert ( 0 != m_pmf , "Invalid LHCb::STChannelID member function: '" + m_nick + "'" ) ; 
  
  Assert ( !m_uints.empty() , "Empty vector of values is specified!" ) ;
  
}
// ============================================================================
// constructor
// ============================================================================
LoKi::Tracks::CountSTIDs::CountSTIDs
( const std::string&  nick , 
  const unsigned int  i    ) 
  : LoKi::BasicFunctors<const LHCb::Track*>::Function ()
  , m_pmf   ( 0     ) 
  , m_uints ( 1 , i )
  , m_imin  ( boost::integer_traits<unsigned int>::const_max ) 
  , m_imax  ( boost::integer_traits<unsigned int>::const_min ) 
  , m_nick  ( nick  )
{

  setPmf ( m_nick ) ;
  
  Assert ( 0 != m_pmf , "Invalid LHCb::STChannelID member function: '" + m_nick + "'" ) ; 
  
}

// ============================================================================
// constructor
// ============================================================================
LoKi::Tracks::CountSTIDs::CountSTIDs
( const unsigned int  imin , 
  const std::string&  nick , 
  const unsigned int  imax ) 
  : LoKi::BasicFunctors<const LHCb::Track*>::Function ()
  , m_pmf   ( 0     ) 
  , m_uints ( )
  , m_imin  ( imin  ) 
  , m_imax  ( imax  ) 
  , m_nick  ( nick  )
{
  
  setPmf ( m_nick ) ;
  
  Assert ( 0 != m_pmf   , "Invalid LHCb::OTChannelID member function: '" + m_nick + "'" ) ; 
  
  Assert ( imin <= imax  , "Invalid range of values" ) ; 
  
}
// ============================================================================
// set PMF properly
// ============================================================================
void LoKi::Tracks::CountSTIDs::setPmf ( const std::string& nick ) 
{
  if      ( "strip"           == nick ) { m_pmf = &LHCb::STChannelID::strip           ; }
  else if ( "sector"          == nick ) { m_pmf = &LHCb::STChannelID::sector          ; }
  else if ( "detRegion"       == nick ) { m_pmf = &LHCb::STChannelID::detRegion       ; }
  else if ( "region"          == nick ) { m_pmf = &LHCb::STChannelID::detRegion       ; }
  else if ( "station"         == nick ) { m_pmf = &LHCb::STChannelID::station         ; }
  else if ( "uniqueLayer"     == nick ) { m_pmf = &LHCb::STChannelID::uniqueLayer     ; }
  else if ( "uniqueDetRegion" == nick ) { m_pmf = &LHCb::STChannelID::uniqueDetRegion ; }
  else if ( "uniqueRegion"    == nick ) { m_pmf = &LHCb::STChannelID::uniqueDetRegion ; }
  else if ( "uniqueSector"    == nick ) { m_pmf = &LHCb::STChannelID::uniqueSector    ; }
  else if ( "type"            == nick ) { m_pmf = &LHCb::STChannelID::type            ; }
  else if ( "channelID"       == nick ) { m_pmf = &LHCb::STChannelID::channelID       ; }
  else if ( "channel"         == nick ) { m_pmf = &LHCb::STChannelID::channelID       ; }
  else { Exception ( "Invalid LHCb::STChannelID member function '" + nick + "'") ; }
}


// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::Tracks::CountSTIDs::~CountSTIDs(){}
// ============================================================================
// MANDATORY: clone method ('virtual constructor')
// ============================================================================
LoKi::Tracks::CountSTIDs*
LoKi::Tracks::CountSTIDs::clone() const 
{ return new LoKi::Tracks::CountSTIDs ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Tracks::CountSTIDs::result_type 
LoKi::Tracks::CountSTIDs::operator()
  ( LoKi::Tracks::CountSTIDs::argument t ) const 
{
  //
  if ( 0 == t ) 
  {
    Error ("LHCb::Track* points to NULL, return -1") ;
    return -1 ;
  }
  //
  typedef  std::vector<LHCb::LHCbID> IDs ;
  //
  const IDs& lhcbids = t->lhcbIDs() ;
  //
  size_t res = 0 ;
  //
  for ( IDs::const_iterator id = lhcbids.begin() ; lhcbids.end() != id ; ++id ) 
  {
    if ( ! id->isST() ) { continue ; }
    //
    const unsigned int r = (id->stID().*m_pmf) () ;
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
std::ostream& LoKi::Tracks::CountSTIDs::fillStream ( std::ostream& s ) const 
{ 
  s << " TrSTIDC( " ;
  //
  if      ( m_uints.empty()     ) { s << m_imin << " ,'" << m_nick << "', "<< m_imax ; }
  else if ( 1 == m_uints.size() ) { s << "'" << m_nick << "', " << m_uints.front()     ; }
  else 
  {
    s << "'" << m_nick << "', " ;
    Gaudi::Utils::toStream ( m_uints , s ) ;
  }
  //
  return s << " ) " ;
}

// ============================================================================
// constructor
// ============================================================================
LoKi::Tracks::CountVeloIDs::CountVeloIDs
( LoKi::Tracks::CountVeloIDs::PMF2 pmf , 
  const unsigned int               i   ) 
  : LoKi::BasicFunctors<const LHCb::Track*>::Function ()
  , m_pmf1  ( 0     ) 
  , m_pmf2  ( pmf   ) 
  , m_uints ( 1 , i )
  , m_imin  ( boost::integer_traits<unsigned int>::const_max ) 
  , m_imax  ( boost::integer_traits<unsigned int>::const_min ) 
  , m_nick  (       )
{
  
  setNick ( m_pmf2 ) ;
  
  Assert ( 0 != m_pmf2 , "Invalid LHCb::VeloChannelID member function!" ) ; 
}
// ============================================================================
// constructor
// ============================================================================
LoKi::Tracks::CountVeloIDs::CountVeloIDs
( LoKi::Tracks::CountVeloIDs::PMF2 pmf , 
  const std::vector<unsigned int>& i   ) 
  : LoKi::BasicFunctors<const LHCb::Track*>::Function ()
  , m_pmf1  ( 0     ) 
  , m_pmf2  ( pmf   ) 
  , m_uints ( i     )
  , m_imin  ( boost::integer_traits<unsigned int>::const_max ) 
  , m_imax  ( boost::integer_traits<unsigned int>::const_min ) 
  , m_nick  (       )
{
  
  setNick ( m_pmf2 ) ;
  
  Assert ( 0 != m_pmf2 , "Invalid LHCb::VeloChannelID member function!" ) ; 
  
  Assert ( !m_uints.empty() , "Empty vector of values is specified!" ) ;
}
// ============================================================================
// constructor
// ============================================================================
LoKi::Tracks::CountVeloIDs::CountVeloIDs
( const unsigned int               imin ,   
  LoKi::Tracks::CountVeloIDs::PMF2 pmf  , 
  const unsigned int               imax )
  : LoKi::BasicFunctors<const LHCb::Track*>::Function ()
  , m_pmf1  ( 0     ) 
  , m_pmf2  ( pmf   ) 
  , m_uints ( )
  , m_imin  ( imin ) 
  , m_imax  ( imax ) 
  , m_nick  (      )
{
  setNick ( m_pmf2 ) ;
  
  Assert ( 0 != m_pmf2 , "Invalid LHCb::OTChannelID member function!" ) ; 
  
  Assert ( imin <= imax  , "Invalid range of values" ) ; 
}
// ============================================================================
// set nick
// ============================================================================
void LoKi::Tracks::CountVeloIDs::setNick ( LoKi::Tracks::CountVeloIDs::PMF2 pmf ) 
{ 
  if      ( &LHCb::VeloChannelID::channelID == pmf ) { m_nick = "channelID" ; }
  else if ( &LHCb::VeloChannelID::strip     == pmf ) { m_nick = "strip"     ; }
  else if ( &LHCb::VeloChannelID::sensor    == pmf ) { m_nick = "sensor"    ; }
  else { Exception ( "Invalid LHCb::VeloChannelID member function!") ; }
}

// ============================================================================
// constructor
// ============================================================================
LoKi::Tracks::CountVeloIDs::CountVeloIDs
( const std::string&               nick , 
  const std::vector<unsigned int>& i    ) 
  : LoKi::BasicFunctors<const LHCb::Track*>::Function ()
  , m_pmf1  ( 0     ) 
  , m_pmf2  ( 0     ) 
  , m_uints ( i     )
  , m_imin  ( boost::integer_traits<unsigned int>::const_max ) 
  , m_imax  ( boost::integer_traits<unsigned int>::const_min ) 
  , m_nick  ( nick  )
{
  
  setPmf ( m_nick ) ;
  
  Assert ( 0 != m_pmf2 , "Invalid LHCb::VeloChannelID member function: '" + m_nick + "'" ) ; 
  
  Assert ( !m_uints.empty() , "Empty vector of values is specified!" ) ;
  
}
// ============================================================================
// constructor
// ============================================================================
LoKi::Tracks::CountVeloIDs::CountVeloIDs
( const std::string&  nick , 
  const unsigned int  i    ) 
  : LoKi::BasicFunctors<const LHCb::Track*>::Function ()
  , m_pmf1  ( 0     ) 
  , m_pmf2  ( 0     ) 
  , m_uints ( 1 , i )
  , m_imin  ( boost::integer_traits<unsigned int>::const_max ) 
  , m_imax  ( boost::integer_traits<unsigned int>::const_min ) 
  , m_nick  ( nick  )
{

  setPmf ( m_nick ) ;
  
  Assert ( 0 != m_pmf2 , "Invalid LHCb::VeloChannelID member function: '" + m_nick + "'" ) ; 
  
}

// ============================================================================
// constructor
// ============================================================================
LoKi::Tracks::CountVeloIDs::CountVeloIDs
( const unsigned int  imin , 
  const std::string&  nick , 
  const unsigned int  imax ) 
  : LoKi::BasicFunctors<const LHCb::Track*>::Function ()
  , m_pmf1  ( 0     ) 
  , m_pmf2  ( 0     ) 
  , m_uints (       )
  , m_imin  ( imin  ) 
  , m_imax  ( imax  ) 
  , m_nick  ( nick  )
{
  
  setPmf ( m_nick ) ;
  
  Assert ( 0 != m_pmf2  , "Invalid LHCb::VeloChannelID member function: '" + m_nick + "'" ) ; 
  
  Assert ( imin <= imax  , "Invalid range of values" ) ; 
  
}
// ============================================================================
// set PMF properly
// ============================================================================
void LoKi::Tracks::CountVeloIDs::setPmf ( const std::string& nick ) 
{
  if      ( "channelID"  == nick ) { m_pmf2 = &LHCb::VeloChannelID::channelID ; }
  else if ( "channel"    == nick ) { m_pmf2 = &LHCb::VeloChannelID::channelID ; }
  else if ( "strip"      == nick ) { m_pmf2 = &LHCb::VeloChannelID::strip     ; }
  else if ( "sensor"     == nick ) { m_pmf2 = &LHCb::VeloChannelID::sensor    ; }
  else { Exception ( "Invalid LHCb::VeloChannelID member function '" + nick + "'") ; }
}

// ============================================================================
// constructor
// ============================================================================
LoKi::Tracks::CountVeloIDs::CountVeloIDs
( LoKi::Tracks::CountVeloIDs::PMF1 pmf ) 
  : LoKi::BasicFunctors<const LHCb::Track*>::Function ()
  , m_pmf1  ( pmf   ) 
  , m_pmf2  ( 0     ) 
  , m_uints (       )
  , m_imin  ( boost::integer_traits<unsigned int>::const_max ) 
  , m_imax  ( boost::integer_traits<unsigned int>::const_min ) 
  , m_nick  ()
{
 
  if      ( &LHCb::VeloChannelID::isPileUp  == m_pmf1 ) { m_nick = "isPileUp"  ; }
  else if ( &LHCb::VeloChannelID::isRType   == m_pmf1 ) { m_nick = "isRType"   ; }
  else if ( &LHCb::VeloChannelID::isPhiType == m_pmf1 ) { m_nick = "isPhiType" ; }
  else 
  { Exception("Invalid LHCb::VeloChannelID memebr function") ; }
  
  Assert ( 0 != m_pmf1  , "Invalid LHCb::VeloChannelID member function:" ) ;
}
// ============================================================================
// constructor
// ============================================================================
LoKi::Tracks::CountVeloIDs::CountVeloIDs
( const std::string& nick  ) 
  : LoKi::BasicFunctors<const LHCb::Track*>::Function ()
  , m_pmf1  ( 0     ) 
  , m_pmf2  ( 0     ) 
  , m_uints (       )
  , m_imin  ( boost::integer_traits<unsigned int>::const_max ) 
  , m_imax  ( boost::integer_traits<unsigned int>::const_min ) 
  , m_nick  ( nick )
{
  
  if      ( "isPileUp"  == m_nick ) { m_pmf1 = &LHCb::VeloChannelID::isPileUp   ; }
  else if ( "pileUp"    == m_nick ) { m_pmf1 = &LHCb::VeloChannelID::isPileUp   ; }
  else if ( "PileUp"    == m_nick ) { m_pmf1 = &LHCb::VeloChannelID::isPileUp   ; }
  else if ( "isRType"   == m_nick ) { m_pmf1 = &LHCb::VeloChannelID::isRType    ; }
  else if ( "RType"     == m_nick ) { m_pmf1 = &LHCb::VeloChannelID::isRType    ; }
  else if ( "rType"     == m_nick ) { m_pmf1 = &LHCb::VeloChannelID::isRType    ; }
  else if ( "R"         == m_nick ) { m_pmf1 = &LHCb::VeloChannelID::isRType    ; }
  else if ( "r"         == m_nick ) { m_pmf1 = &LHCb::VeloChannelID::isRType    ; }
  else if ( "isPhiType" == m_nick ) { m_pmf1 = &LHCb::VeloChannelID::isPhiType  ; }
  else if ( "PhiType"   == m_nick ) { m_pmf1 = &LHCb::VeloChannelID::isPhiType  ; }
  else if ( "phiType"   == m_nick ) { m_pmf1 = &LHCb::VeloChannelID::isPhiType  ; }
  else if ( "Phi"       == m_nick ) { m_pmf1 = &LHCb::VeloChannelID::isPhiType  ; }
  else if ( "phi"       == m_nick ) { m_pmf1 = &LHCb::VeloChannelID::isPhiType  ; }
  
  Assert ( 0 != m_pmf1  , "Invalid LHCb::VeloChannelID member function '" + m_nick + "'" ) ;

}
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::Tracks::CountVeloIDs::~CountVeloIDs(){}
// ============================================================================
// MANDATORY: clone method ('virtual constructor')
// ============================================================================
LoKi::Tracks::CountVeloIDs*
LoKi::Tracks::CountVeloIDs::clone() const 
{ return new LoKi::Tracks::CountVeloIDs ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Tracks::CountVeloIDs::result_type 
LoKi::Tracks::CountVeloIDs::operator()
  ( LoKi::Tracks::CountVeloIDs::argument t ) const 
{
  //
  //
  if ( 0 == t ) 
  {
    Error ("LHCb::Track* points to NULL, return -1") ;
    return -1 ;
  }
  //
  typedef  std::vector<LHCb::LHCbID> IDs ;
  //
  const IDs& lhcbids = t->lhcbIDs() ;
  //
  size_t res = 0 ;
  //
  for ( IDs::const_iterator id = lhcbids.begin() ; lhcbids.end() != id ; ++id ) 
  {
    if ( ! id->isVelo() ) { continue ; }
    //
    const LHCb::VeloChannelID velo = id->veloID() ;
    //
    if ( 0 != m_pmf1 ) { if ( (velo.*m_pmf1)() ) { ++res ; } }
    else 
    {
      const unsigned int r = (velo.*m_pmf2) () ;
      if      ( m_uints.empty()     ) { if ( m_imin <= r && r <= m_imax ) { ++res ; } }
      else if ( 1 == m_uints.size() ) { if ( m_uints.front() == r       ) { ++res ; } }
      else 
      { 
        if  ( m_uints.end() != std::find ( m_uints.begin() , m_uints.end() , r ) ) { ++res ; } 
      }
    }
  }
  //
  return res;
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::Tracks::CountVeloIDs::fillStream ( std::ostream& s ) const 
{ 
  s << " TrVELOIDC( " ;
  //
  if ( 0 != m_pmf1 ) { return s << "'" << m_nick << "' ) " ; }
  //
  if      ( m_uints.empty()     ) { s << m_imin << " ,'" << m_nick << "', "<< m_imax ; }
  else if ( 1 == m_uints.size() ) { s << "'" << m_nick << "', " << m_uints.front()     ; }
  else 
  {
    s << "'" << m_nick << "', " ;
    Gaudi::Utils::toStream ( m_uints , s ) ;
  }
  //
  return s << " ) " ;
}


// ============================================================================
// The END 
// ============================================================================
