// $Id: DistanceCalculatorBase.cpp,v 1.2 2008-07-15 10:12:21 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// local
// ============================================================================
#include "DistanceCalculatorBase.h"
// ============================================================================
/** @file 
 *  the implementation of the class LoKi::DistanceCalculator 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-03-05
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
