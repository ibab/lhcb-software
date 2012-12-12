// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <iostream>
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/BasicFunctors.h"
#include "LoKi/Dumper.h"
// ============================================================================
/** @file 
 *  Implementation file for classes form namesapce LoKi::Functors 
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya Belyaev Ivan.BElyaev@itep.ru
 *  @date 2012-01-28 
 */
// ============================================================================
template <>
LoKi::Functors::Dump1_<double,bool>::result_type 
LoKi::Functors::Dump1_<double,bool>::operator() 
  ( LoKi::Functors::Dump1_<double,bool>::argument a ) const 
{
  if ( m_right ) 
  {
    m_stream << m_dump.open  () ;
    m_stream << a ;
    m_stream << m_dump.close () ;
    return m_fun.fun ( a ) ;
  }
  //
  result_type result = m_fun.fun ( a ) ;
  //
  m_stream << m_dump.open  () ;
  m_stream << a ;
  m_stream << m_dump.close () ;
  //
  return result ;
}
// ============================================================================
template <>
LoKi::Functors::Dump1_<double,double>::result_type 
LoKi::Functors::Dump1_<double,double>::operator() 
  ( LoKi::Functors::Dump1_<double,double>::argument a ) const 
{
  if ( m_right ) 
  {
    m_stream << m_dump.open  () ;
    m_stream << a ;
    m_stream << m_dump.close () ;
    return m_fun.fun ( a ) ;
  }
  //
  result_type result = m_fun.fun ( a ) ;
  //
  m_stream << m_dump.open  () ;
  m_stream << a ;
  m_stream << m_dump.close () ;
  //
  return result ;
}
// ============================================================================
// The END 
// ============================================================================
