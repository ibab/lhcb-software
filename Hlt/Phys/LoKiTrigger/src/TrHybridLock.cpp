// $Id: TrHybridLock.cpp,v 1.1.1.1 2007-07-27 15:56:42 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Report.h"
#include "LoKi/TrHybridLock.h"
#include "LoKi/TrHybridEngineActor.h"
// ============================================================================
/** @file 
 *  Implementation file for class LoKi::Hybrid::TrLock
 *  @date 2007-06-10 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 */
// ============================================================================
// contructor 
// ============================================================================
LoKi::Hybrid::TrLock::TrLock 
( LoKi::ITrHybridTool* tool ) 
  : m_tool ( tool ) 
{
  LoKi::Hybrid::TrEngineActor& actor = LoKi::Hybrid::TrEngineActor::instance() ;
  // connect the tool to the actor 
  StatusCode sc = actor.connectTool ( m_tool.getObject () ) ;  
  if ( sc.isFailure () ) 
  {
    LoKi::Report::Error
      ( "LoKi::Hybrid::TrLock: error from connectTool", sc ) .ignore() ;
  } 
}
// ============================================================================
// destructor 
// ============================================================================
LoKi::Hybrid::TrLock::~TrLock()
{
  LoKi::Hybrid::TrEngineActor& actor = LoKi::Hybrid::TrEngineActor::instance() ;
  // connect the tool to the actor 
  StatusCode sc = actor.releaseTool ( m_tool.getObject () ) ;  
  if ( sc.isFailure () ) 
  {
    LoKi::Report::Error
      ( "LoKi::Hybrid::TrLock: error from releaseTool", sc ) .ignore() ;
  } 
}
// ============================================================================
// The END 
// ============================================================================
