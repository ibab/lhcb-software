// $Id: $
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ToStream.h"
// ============================================================================
// local
// ============================================================================
#include "LoKi/Filters.h"
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
 *  By usage of this code one clearly states the disagreement 
 *  with the campain of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *
 *  @author Vanya Belyaev Ivan.BElyaev@cern.ch
 *  @date 2010-11-17 
 *
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
// ============================================================================
template <>
LoKi::Functors::Dump_<double>::result_type 
LoKi::Functors::Dump_<double>::operator() 
  ( LoKi::Functors::Dump_<double>::argument a ) const 
{
  Gaudi::Utils::toStream ( a , m_stream ) ;
  m_stream << std::endl ;
  return a ;
}
// ============================================================================
// The END 
// ============================================================================
