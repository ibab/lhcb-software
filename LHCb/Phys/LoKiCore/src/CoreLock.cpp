// $Id: CoreLock.cpp,v 1.1 2008-10-19 16:11:40 ibelyaev Exp $
// Include files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Report.h"
#include "LoKi/CoreLock.h"
#include "LoKi/CoreEngineActor.h"
// ============================================================================
/** @file 
 *  Implementation file for class LoKi::Hybrid::CoreLock
 *  @date 2008-09-18 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 */
// ============================================================================
// contructor 
// ============================================================================
LoKi::Hybrid::CoreLock::CoreLock 
( LoKi::Hybrid::ICoreAntiFactory* tool ) 
  : m_tool ( tool ) 
{
  LoKi::Hybrid::CoreEngineActor& actor = 
    LoKi::Hybrid::CoreEngineActor::instance() ;
  // connect the tool to the actor 
  StatusCode sc = actor.connectTool ( m_tool.getObject () ) ;  
  if ( sc.isFailure () ) 
  {
    LoKi::Report::Error
      ( "LoKi::Hybrid::CoreLock: error from connectTool", sc ) .ignore() ;
  } 
}
// ============================================================================
// destructor 
// ============================================================================
LoKi::Hybrid::CoreLock::~CoreLock()
{
  LoKi::Hybrid::CoreEngineActor& actor = 
    LoKi::Hybrid::CoreEngineActor::instance() ;
  // connect the tool to the actor 
  StatusCode sc = actor.releaseTool ( m_tool.getObject () ) ;  
  if ( sc.isFailure () ) 
  {
    LoKi::Report::Error
      ( "LoKi::Hybrid::CoreLock: error from releaseTool", sc ) .ignore() ;
  } 
}
// ============================================================================
// The END 
// ============================================================================
