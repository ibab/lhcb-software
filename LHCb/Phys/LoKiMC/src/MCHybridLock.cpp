// $Id: MCHybridLock.cpp,v 1.1 2007-07-25 15:16:07 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Report.h"
#include "LoKi/MCHybridLock.h"
#include "LoKi/MCHybridEngineActor.h"
// ============================================================================
/** @file 
 *  Implementation file for class LoKi::MCHybridLock
 *  @date 2007-06-10 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 */
// ============================================================================
// contructor 
// ============================================================================
LoKi::Hybrid::MCLock::MCLock 
( IMCHybridTool* tool ) 
  : m_tool ( tool ) 
{
  LoKi::Hybrid::MCEngineActor& actor = LoKi::Hybrid::MCEngineActor::instance() ;
  // connect the tool to the actor 
  StatusCode sc = actor.connectTool ( m_tool.getObject () ) ;  
  if ( sc.isFailure () ) 
  {
    LoKi::Report::Error
      ( "LoKi::Hybrid::MCLock: error from connectTool", sc ) .ignore() ;
  } 
}
// ============================================================================
// destructor 
// ============================================================================
LoKi::Hybrid::MCLock::~MCLock()
{
  LoKi::Hybrid::MCEngineActor& actor = LoKi::Hybrid::MCEngineActor::instance() ;
  // connect the tool to the actor 
  StatusCode sc = actor.releaseTool ( m_tool.getObject () ) ;  
  if ( sc.isFailure () ) 
  {
    LoKi::Report::Error
      ( "LoKi::Hybrid::MCLock: error from releaseTool", sc ) .ignore() ;
  } 
}
// ============================================================================
// The END 
// ============================================================================
