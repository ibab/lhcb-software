// $Id: HistoCondition.cpp,v 1.2 2009-10-24 09:54:33 marcocle Exp $
// ============================================================================
// Include files 
// ============================================================================
// local
// ============================================================================
#include "DetDesc/HistoCondition.h"
// ============================================================================
/** @file 
 *
 *  Implementation file for class DetDesc::Histo1DCond
 *                     and  class DetDesc::Histo2DCond
 *
 *  @see DetDesc::Histo1DCond 
 *  @see DetDesc::Histo2DCond 
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2009-10-22
 */
// ============================================================================
// 1D  
// ============================================================================
// the constructor 
// ============================================================================
DetDesc::Histo1DCond::Histo1DCond ()
  : Condition() 
  , m_histo ("unspecified","unspecified",1,0,1)
{}
// ============================================================================
// destructor 
// ============================================================================
DetDesc::Histo1DCond::~Histo1DCond (){}
// ============================================================================
// update from other data object: 
// ============================================================================
void DetDesc::Histo1DCond::update ( DetDesc::Histo1DCond& right ) 
{
  m_histo = right.m_histo ;
  Condition::update ( right ) ;
}
// ============================================================================
// update from other data object: 
// ============================================================================
void DetDesc::Histo1DCond::update ( ValidDataObject& right ) 
{
  ValidDataObject* tmp = &right ;
  DetDesc::Histo1DCond* aux = dynamic_cast<DetDesc::Histo1DCond*> ( tmp ) ;
  if ( 0 != aux ) { return update ( *aux ) ; }                        // RETURN 
  Condition::update ( right ) ;
}
// ============================================================================
// perform the specific initialization (==creation of the histogram)
// ============================================================================
StatusCode DetDesc::Histo1DCond::initialize () 
{
  StatusCode sc =  Condition::initialize() ; 
  if ( sc.isFailure() ) { return sc ; }  // RETURN 
  // set the histogram here , later ...
  return StatusCode::SUCCESS ;
}
// ============================================================================
// 2D 
// ============================================================================
// the constructor 
// ============================================================================
DetDesc::Histo2DCond::Histo2DCond ()
  : Condition() 
  , m_histo ("unspecified","unspecified",1,0,1,1,0,1)
{}
// ============================================================================
// destructor 
// ============================================================================
DetDesc::Histo2DCond::~Histo2DCond (){}
// ============================================================================
// update from other data object: 
// ============================================================================
void DetDesc::Histo2DCond::update ( DetDesc::Histo2DCond& right ) 
{
  m_histo = right.m_histo ;
  Condition::update ( right ) ;
}
// ============================================================================
// update from other data object: 
// ============================================================================
void DetDesc::Histo2DCond::update ( ValidDataObject& right ) 
{
  ValidDataObject* tmp = &right ;
  DetDesc::Histo2DCond* aux = dynamic_cast<DetDesc::Histo2DCond*> ( tmp ) ;
  if ( 0 != aux ) { return update ( *aux ) ; }                        // RETURN 
  Condition::update ( right ) ;
}
// ============================================================================
// perform the specific initialization (==creation of the histogram)
// ============================================================================
StatusCode DetDesc::Histo2DCond::initialize () 
{ 
  StatusCode sc =  Condition::initialize() ; 
  if ( sc.isFailure() ) { return sc ; }  // RETURN 
  // set the histogram here , later ...
  return StatusCode::SUCCESS ;
}
// ============================================================================




// ============================================================================
// The END 
// ============================================================================
