// $Id: DistanceCalculatorBase.cpp,v 1.1 2008-03-10 18:24:43 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// local
// ============================================================================
#include "DistanceCalculator.h"
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
{}
// ============================================================================
// virtual and protected desctrustor 
// ============================================================================
LoKi::DistanceCalculatorBase::~DistanceCalculatorBase(){}
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
