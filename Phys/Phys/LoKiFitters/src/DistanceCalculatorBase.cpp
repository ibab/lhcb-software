// $Id: DistanceCalculatorBase.cpp,v 1.2 2008-07-15 10:12:21 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// local
// ============================================================================
#include "DistanceCalculatorBase.h"
// ============================================================================
/** @file 
 *  The implementation of the class LoKi::DistanceCalculator 
 *
 *  This file is a part of 
 *  <a href="http://cern.ch/lhcb-comp/Analysis/LoKi/index.html">LoKi project:</a>
 *  ``C++ ToolKit for Smart and Friendly Physics Analysis''
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *   By usage of this code one clearly states the disagreement 
 *    with the campain of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-03-05
 *  
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 * 
 */
// ============================================================================
/** Standard constructor
 *  @param type tool type(?)
 *  @param name tool instance name 
 *  @param parent the pointer to the parent
 */
// ============================================================================
LoKi::DistanceCalculatorBase::DistanceCalculatorBase
( const std::string& type   , // tool type (?)
  const std::string& name   , // tool instance name 
  const IInterface*  parent ) // the parent 
  : GaudiTool ( type , name , parent ) 
/// number of prints 
  , m_prints              ( 10 )
{
  // ==========================================================================
  declareProperty 
    ( "MaxPrints"        , 
      m_prints           , 
      "Maximal number of prints "        ) ;
  // ==========================================================================
}
// ============================================================================
// virtual and protected desctrustor 
// ============================================================================
LoKi::DistanceCalculatorBase::~DistanceCalculatorBase(){}
// ============================================================================
// initialize 
// ============================================================================
StatusCode LoKi::DistanceCalculatorBase::initialize () 
{
  StatusCode sc = GaudiTool::initialize () ;
  if ( sc.isFailure() ) { return sc ; }         // RETURN
  //
  if ( msgLevel ( MSG::DEBUG ) &&  0 == m_prints ) 
  {
    m_prints = 10 ;
    warning () << "Redefine 'MaxPrints' property to " << m_prints << endmsg ;
  }
  //
  return StatusCode::SUCCESS ; 
}
// ============================================================================
// The END 
// ============================================================================
