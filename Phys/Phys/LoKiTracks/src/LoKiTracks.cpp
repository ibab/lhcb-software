// $Id$ 
// ============================================================================
// Incldiue files 
// ============================================================================
// Local 
// ============================================================================
#include "LoKi/LoKiTracks.h"
// =======================================================================
/** @file
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
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *
 *  $Revision$
 *  Last Modification $Date$ 
 *                 by $Author$ 
 */
// ============================================================================
// the specific printout
// ============================================================================
template <>
std::ostream& LoKi::Functors::Empty<const LHCb::Track*>::fillStream
( std::ostream& s ) const { return s << "TrEMPTY" ; }
// ============================================================================
// the specific printpout
// ============================================================================
template <>
std::ostream& LoKi::Functors::Size<const LHCb::Track*>::fillStream
( std::ostream& s ) const { return s << "TrSIZE" ; }
// ============================================================================
// the specific printpout
// ============================================================================
template <>
std::ostream& 
LoKi::Constant<const LHCb::Track*,bool>::fillStream( std::ostream& s ) const 
{ return s << ( this->m_value ? "TrALL" : "TrNONE" ) ; }
// ============================================================================
template <>
LoKi::Functors::Dump_<const LHCb::Track*>::result_type 
LoKi::Functors::Dump_<const LHCb::Track*>::operator() 
  ( LoKi::Functors::Dump_<const LHCb::Track*>::argument a ) const 
{
  const unsigned int mx = std::min ( m_dump.nMax() , a.size() ) ;
  //
  m_stream << m_dump.open  () ;
  //
  for ( unsigned int i = 0 ; i < mx ; ++i ) 
  {
    const LHCb::Track* track = a[i] ;
    if (  0 == track ) 
    { m_stream << "<NULL> LHCb::Track" << std::endl ; }
    else 
    { m_stream << ( *track ) ; }
  }
  //
  m_stream << m_dump.close () ;
  //
  return a ; 
}
// ============================================================================
template <>
LoKi::Functors::Dump1_<const LHCb::Track*,bool>::result_type 
LoKi::Functors::Dump1_<const LHCb::Track*,bool>::operator() 
  ( LoKi::Functors::Dump1_<const LHCb::Track*,bool>::argument a ) const 
{

  if ( m_right ) 
  {
    m_stream << m_dump.open  () ;
    //
    if ( 0 == a ) { m_stream << "<NULL> LHCb::Track" << std::endl ; }
    else          { m_stream << (*a) ; }
    //
    m_stream << m_dump.close () ;
    return m_fun ( a ) ;
  }
  //
  result_type result = m_fun ( a ) ;
  //
  m_stream << m_dump.open  () ;
  //
  if ( 0 == a ) { m_stream << "<NULL> LHCb::Track" << std::endl ; }
  else          { m_stream << (*a) ; }
  //
  m_stream << m_dump.close () ;
  //
  return result ;
}
// ============================================================================
template <>
LoKi::Functors::Dump1_<const LHCb::Track*,double>::result_type 
LoKi::Functors::Dump1_<const LHCb::Track*,double>::operator() 
  ( LoKi::Functors::Dump1_<const LHCb::Track*,double>::argument a ) const 
{
  
  if ( m_right ) 
  {
    m_stream << m_dump.open  () ;
    //
    if ( 0 == a ) { m_stream << "<NULL> LHCb::Track" << std::endl ; }
    else          { m_stream << (*a) ; }
    //
    m_stream << m_dump.close () ;
    return m_fun ( a ) ;
  }
  //
  result_type result = m_fun ( a ) ;
  //
  m_stream << m_dump.open  () ;
  //
  if ( 0 == a ) { m_stream << "<NULL> LHCb::Track" << std::endl ; }
  else          { m_stream << (*a) ; }
  //
  m_stream << m_dump.close () ;
  //
  return result ;
}
// ============================================================================
// The END 
// ============================================================================


