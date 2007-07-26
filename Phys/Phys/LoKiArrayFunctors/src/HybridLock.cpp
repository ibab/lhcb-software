// $Id: HybridLock.cpp,v 1.1 2007-07-26 13:25:09 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Report.h"
#include "LoKi/HybridLock.h"
#include "LoKi/HybridEngineActor.h"
// ============================================================================
/** @file 
 *  Implementation file for class LoKi::HybridLock
 *  @date 2007-06-10 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 */
// ============================================================================
// contructor 
// ============================================================================
LoKi::Hybrid::Lock::Lock 
( LoKi::IHybridTool* tool ) 
  : m_tool ( tool ) 
{
  LoKi::Hybrid::EngineActor& actor = LoKi::Hybrid::EngineActor::instance() ;
  // connect the tool to the actor 
  StatusCode sc = actor.connectTool ( m_tool.getObject () ) ;  
  if ( sc.isFailure () ) 
  {
    LoKi::Report::Error
      ( "LoKi::Hybrid::Lock: error from connectTool", sc ) .ignore() ;
  } 
}
// ============================================================================
// destructor 
// ============================================================================
LoKi::Hybrid::Lock::~Lock()
{
  LoKi::Hybrid::EngineActor& actor = LoKi::Hybrid::EngineActor::instance() ;
  // connect the tool to the actor 
  StatusCode sc = actor.releaseTool ( m_tool.getObject () ) ;  
  if ( sc.isFailure () ) 
  {
    LoKi::Report::Error
      ( "LoKi::Hybrid::Lock: error from releaseTool", sc ) .ignore() ;
  } 
}
// ============================================================================
// The END 
// ============================================================================
