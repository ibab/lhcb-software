// $Id: HltLock.cpp,v 1.1.1.1 2008-09-21 14:41:20 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Report.h"
#include "LoKi/HltLock.h"
#include "LoKi/HltEngineActor.h"
// ============================================================================
/** @file 
 *  Implementation file for class LoKi::Hybrid::HltLock
 *  @date 2008-09-18 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 */
// ============================================================================
// contructor 
// ============================================================================
LoKi::Hybrid::HltLock::HltLock 
( LoKi::Hybrid::IHltAntiFactory* tool ) 
  : m_tool ( tool ) 
{
  LoKi::Hybrid::HltEngineActor& actor = 
    LoKi::Hybrid::HltEngineActor::instance() ;
  // connect the tool to the actor 
  StatusCode sc = actor.connectTool ( m_tool.getObject () ) ;  
  if ( sc.isFailure () ) 
  {
    LoKi::Report::Error
      ( "LoKi::Hybrid::HltLock: error from connectTool", sc ) .ignore() ;
  } 
}
// ============================================================================
// destructor 
// ============================================================================
LoKi::Hybrid::HltLock::~HltLock()
{
  LoKi::Hybrid::HltEngineActor& actor = 
    LoKi::Hybrid::HltEngineActor::instance() ;
  // connect the tool to the actor 
  StatusCode sc = actor.releaseTool ( m_tool.getObject () ) ;  
  if ( sc.isFailure () ) 
  {
    LoKi::Report::Error
      ( "LoKi::Hybrid::HltLock: error from releaseTool", sc ) .ignore() ;
  } 
}
// ============================================================================
// The END 
// ============================================================================
