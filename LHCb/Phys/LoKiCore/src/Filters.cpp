// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ToStream.h"
// ============================================================================
// local
// ============================================================================
#include "LoKi/BasicFunctors.h"
#include "LoKi/Primitives.h"
#include "LoKi/Filters.h"
#include "LoKi/Scalers.h"
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
 *  @author Vanya Belyaev Ivan.BElyaev@cern.ch
 *  @date 2010-11-17 
 */
// ============================================================================
template <>
LoKi::Functors::Dump_<double>::result_type 
LoKi::Functors::Dump_<double>::operator() 
  ( LoKi::Functors::Dump_<double>::argument a ) const 
{
  m_stream << m_dump.open  () ;
  //
  if ( a.size() <= m_dump.nMax() ) 
  {
    Gaudi::Utils::toStream 
      ( a.begin() , a.end() , m_stream , "[ " , " ]" , " , " ) ;
  }
  else 
  {
    Gaudi::Utils::toStream 
      ( a.begin() , a.begin() + m_dump.nMax() , 
        m_stream , "[ " , " , ... ]" , " , " ) ;
  }
  //
  m_stream << m_dump.close () ;
  return a ;
}
// ============================================================================
// The END 
// ============================================================================
