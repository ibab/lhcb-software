// $Id$
// ============================================================================
// $URL$ 
// ============================================================================
// Include files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Report.h"
#include "LoKi/TrgFactoryLock.h"
#include "LoKi/TrgEngineActor.h"
// ============================================================================
/** @file 
 *
 *  Implementation file for class LoKi::Hybrid::TrgFactoryLock
 *
 *  @date 2007-06-10 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *
 *  $Revision$
 *  Last Modification $Date$ 
 *                 by $Author$ 
 *
 */
// ============================================================================
// contructor 
// ============================================================================
LoKi::Hybrid::TrgFactoryLock::TrgFactoryLock 
( LoKi::ITrgFunctorAntiFactory* tool ) 
  : m_tool ( tool ) 
{
  auto& actor = LoKi::Hybrid::TrgEngineActor::instance() ;
  // connect the tool to the actor 
  StatusCode sc = actor.connectTool ( m_tool.getObject () ) ;  
  if ( sc.isFailure () ) 
  {
    LoKi::Report::Error
      ( "LoKi::Hybrid::TrgFactoryLock: error from connectTool", sc ) .ignore() ;
  } 
}
// ============================================================================
// destructor 
// ============================================================================
LoKi::Hybrid::TrgFactoryLock::~TrgFactoryLock()
{
  auto& actor = LoKi::Hybrid::TrgEngineActor::instance() ;
  // connect the tool to the actor 
  StatusCode sc = actor.releaseTool ( m_tool.getObject () ) ;  
  if ( sc.isFailure () ) 
  {
    LoKi::Report::Error
      ( "LoKi::Hybrid::TrgFactoryLock: error from releaseTool", sc ) .ignore() ;
  } 
}
// ============================================================================
// The END 
// ============================================================================
