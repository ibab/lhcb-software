// $Id: LoKi_EventTagTuple.cpp,v 1.1.1.1 2003-07-24 16:43:50 ibelyaev Exp $
// ============================================================================
// CVS Tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2003/05/12 13:21:33  ibelyaev
//  add the options files for all examples
//
// Revision 1.2  2003/04/11 17:31:30  ibelyaev
//  fix for Win2K platform
//
// Revision 1.1  2003/03/21 18:15:48  ibelyaev
//  3 new examples
// 
// ============================================================================
// Include files
// LoKi 
#include "LoKi/LoKi.h"
// Event 
#include "Event/EventHeader.h"
// GaudiKErnel 
#include "GaudiKernel/IRegistry.h"

/** @file 
 * 
 *  Implementation file for class : LoKi_EventTagTuple.h 
 *
 *  @date  2003-03-21 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 */

// ============================================================================
LOKI_ALGORITHM( LoKi_EventTagTuple )
{
  // avoid long names 
  using namespace LoKi       ;
  using namespace LoKi::Cuts ;
  
  setFilterPassed( false ) ;
  
  // select all charged kaons with Confidence level in excess of 5 percent  
  Range kaons = select( "kaons" , 321 == abs( ID ) && 0.05 < CL ) ;

  // subdivide sample of kaons to subsamples of positive and negative kaons  
  Range kplus  = select( "K+"  , kaons , Q >  0.5 ) ;
  Range kminus = select( "K-"  , kaons , Q < -0.5 ) ;
  
  // create the cut for phi :  abs(delta mass) < 15 MeV 
  Fun dmass = DMASS( "phi(1020)" , ppSvc() ) ;
  Cut dmcut  = abs( dmass ) < 15 * MeV  ;
  
  for( Loop phi = loop( "K+ K-" , "phi(1020)" ) ; phi ; ++phi ) 
    {
      // save "good" phi 
      if( dmcut( phi ) ) { phi->save("phi"); }
    }
  
  // get all saved phi 
  Range phis = selected ( "phi" )  ;
  if( phis.empty() ) { return StatusCode::SUCCESS ; } // RETURN 
  
  // get the event tag collection/ntuple 
  Tuple tuple = evtCollection( "EvtCollection") ;
  
  // fill the tuple 
  tuple -> farray( "M"    , M     , 
                   "DM"   , dmass , 
                   "P"    , P     , 
                   "PT"   , PT    , 
                   phis.begin ()  , 
                   phis.end   ()  , 
                   "nPhi"         , 
                   100            ) ;
  
  // add the event information to the N-Tuple 
  const EventHeader* evt = 
    get( eventSvc() , EventHeaderLocation::Default , evt );
  
  // add event header information to N-Tuple 
  tuple -> column ( "" , evt );
  
  // get the event IOpaqueAddress  
  DataObject* event = get( eventSvc() , "/Event" , event );
  
  // add event address to N-Tuple 
  tuple -> column ( "Address" , event->registry()->address() ) ;
  
  // IMPORTANT !  
  setFilterPassed( true ) ;    /// <<< IMPORTANT !
  
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
