// $Id: $
// ============================================================================
// Include files 
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/Dump.h"
// ============================================================================
/** @file 
 *  Implementation fiel for class LoKi::Dump
 *  @see LoKi::Dump
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
 *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
 *  @date 2010-11-18
 *  
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
// ============================================================================
LoKi::Dump::Dump
( const std::string& open  ,
  const std::string& close )
  : m_open  ( open ) 
  , m_close ( close ) 
{}
// ============================================================================
LoKi::Dump::~Dump(){}
// ============================================================================
// The END 
// ============================================================================
