// $Id: RoutingBits.cpp,v 1.1 2010-05-17 16:01:39 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <algorithm>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/SmartIF.h"
// ============================================================================
// DAQEvent
// ============================================================================
#include "Event/RawEvent.h"
// ============================================================================
// HltInterfaces
// ============================================================================
#include "Kernel/ReadRoutingBits.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/ILoKiSvc.h"
#include "LoKi/RoutingBits.h"
// ============================================================================
/** @file
 *  set of helper function to deal with "hlt-routing-bits"
 *  @see Hlt::firedRoutingBits 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2010-05-17
 */
// ============================================================================
// constructor from the bit 
// ============================================================================
LoKi::HLT::RoutingBits::RoutingBits 
( const unsigned short bit ) 
  : LoKi::Functor<void,bool>() 
  , m_fired   () 
  , m_bits    ( bit ) 
{}
// ============================================================================
// constructor from the bis 
// ============================================================================
LoKi::HLT::RoutingBits::RoutingBits 
( const unsigned short bit1 , 
  const unsigned short bit2 ) 
  : LoKi::Functor<void,bool>() 
  , m_fired   () 
  , m_bits    () 
{
  m_bits.push_back ( bit1 ) ;
  m_bits.push_back ( bit2 ) ;
  std::sort ( m_fired.begin() , m_fired.end() ) ;  
}
// ============================================================================
// constructor from the bis 
// ============================================================================
LoKi::HLT::RoutingBits::RoutingBits 
( const unsigned short bit1 , 
  const unsigned short bit2 , 
  const unsigned short bit3 ) 
  : LoKi::Functor<void,bool>() 
  , m_fired   () 
  , m_bits    () 
{
  m_bits.push_back ( bit1 ) ;
  m_bits.push_back ( bit2 ) ;
  m_bits.push_back ( bit3 ) ;
  std::sort ( m_fired.begin() , m_fired.end() ) ;  
}
// ============================================================================
// constructor from the bis 
// ============================================================================
LoKi::HLT::RoutingBits::RoutingBits 
( const unsigned short bit1 , 
  const unsigned short bit2 , 
  const unsigned short bit3 , 
  const unsigned short bit4 ) 
  : LoKi::Functor<void,bool>() 
  , m_fired   () 
  , m_bits    () 
{
  m_bits.push_back ( bit1 ) ;
  m_bits.push_back ( bit2 ) ;
  m_bits.push_back ( bit3 ) ;
  m_bits.push_back ( bit4 ) ;
  std::sort ( m_fired.begin() , m_fired.end() ) ;  
}
// ============================================================================
// constructor from the bis 
// ============================================================================
LoKi::HLT::RoutingBits::RoutingBits 
( const std::vector<unsigned int>&  bits ) 
  : LoKi::Functor<void,bool>() 
  , m_fired   () 
  , m_bits    ( bits ) 
{
  std::sort ( m_fired.begin() , m_fired.end() ) ;  
}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::HLT::RoutingBits::~RoutingBits() {}
// ============================================================================
// MANDATORY: clone method ("virtual contructor")
// ============================================================================
LoKi::HLT::RoutingBits* LoKi::HLT::RoutingBits::clone () const 
{ return new RoutingBits ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::HLT::RoutingBits::result_type 
LoKi::HLT::RoutingBits::operator() 
  ( /* LoKi::HLT::RoutingBit::argument a */ ) const
{
  if ( !sameEvent() || 0 >= event() || m_fired.empty() ) { getFired() ; }
  //
  for ( std::vector<unsigned int>::const_iterator ibit = m_bits.begin() ; 
        m_bits.end() != ibit ; ++ibit ) 
  {
    const bool found = std::binary_search 
      ( m_fired.begin() , m_fired.end  () , *ibit ) ; 
    if ( found ) { return true ; } 
  }
  //
  return false ;  
}
// ============================================================================
// get the fired bits 
// ============================================================================
std::size_t LoKi::HLT::RoutingBits::getFired() const 
{
  LoKi::ILoKiSvc*   loki   = lokiSvc()  ;
  Assert ( 0 != loki   , "Uanble to get LoKi  Service" ) ;  
  SmartIF<IDataProviderSvc> evtSvc  ( loki ) ;
  Assert ( !(!evtSvc)  , "Uanble to get Event Service" ) ;
  SmartDataPtr<LHCb::RawEvent> raw
    ( evtSvc , LHCb::RawEventLocation::Default ) ;
  Assert ( !(!raw)     , "Unable to get Raw Event!" ) ;
  //
  m_fired = Hlt::firedRoutingBits ( raw ) ;
  std::sort ( m_fired.begin() , m_fired.end() ) ;
  //
  return m_fired.size() ;
}
// ============================================================================
// OPTIONAL : the nice printout 
// ============================================================================
std::ostream& LoKi::HLT::RoutingBits::fillStream ( std::ostream& s ) const 
{
  s << "routingBits ( " ;
  //
  switch ( m_bits .size() ) 
  {
  case 1 : 
    return  s << m_bits[0] << " ) " ;
  case 2 : 
    return  s << m_bits[0] << "," 
              << m_bits[1] << " ) " ;
  case 3 : 
    return  s << m_bits[0] << "," 
              << m_bits[1] << "," 
              << m_bits[2] << " ) " ;
  case 4 : 
    return  s << m_bits[0] << "," 
              << m_bits[1] << "," 
              << m_bits[2] << "," 
              << m_bits[3] << " ) " ;
  default: 
    break ;
  }
  Gaudi::Utils::toStream ( m_bits , s ) ;
  return s << " ) " ; 
}
// ============================================================================




// ============================================================================
// The END 
// ============================================================================
