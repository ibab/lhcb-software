// $Id$
// ============================================================================
// $URL$ 
// ============================================================================
// Include files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Report.h"
#include "LoKi/TrackFactoryLock.h"
#include "LoKi/TrackEngineActor.h"
// ============================================================================
/** @file 
 *
 *  Implementation file for class LoKi::Hybrid::TrackFactoryLock
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
LoKi::Hybrid::TrackFactoryLock::TrackFactoryLock 
( LoKi::ITrackFunctorAntiFactory* tool ) 
  : m_tool ( tool ) 
{
  LoKi::Hybrid::TrackEngineActor& actor = 
    LoKi::Hybrid::TrackEngineActor::instance() ;
  // connect the tool to the actor 
  StatusCode sc = actor.connectTool ( m_tool.getObject () ) ;  
  if ( sc.isFailure () ) 
  {
    LoKi::Report::Error
      ( "LoKi::Hybrid::TrackFactoryLock: error from connectTool", sc ) .ignore() ;
  } 
}
// ============================================================================
// destructor 
// ============================================================================
LoKi::Hybrid::TrackFactoryLock::~TrackFactoryLock()
{
  LoKi::Hybrid::TrackEngineActor& actor = 
    LoKi::Hybrid::TrackEngineActor::instance() ;
  // connect the tool to the actor 
  StatusCode sc = actor.releaseTool ( m_tool.getObject () ) ;  
  if ( sc.isFailure () ) 
  {
    LoKi::Report::Error
      ( "LoKi::Hybrid::TrackFactoryLock: error from releaseTool", sc ) .ignore() ;
  } 
}
// ============================================================================
// The END 
// ============================================================================
