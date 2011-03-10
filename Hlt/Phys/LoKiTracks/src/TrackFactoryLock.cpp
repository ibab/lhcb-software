// $Id$
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
 *  Implementation file for class LoKi::Hybrid::TrackFactoryLock
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  By usage of this code one clearly states the disagreement 
 *  with the campain of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
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
