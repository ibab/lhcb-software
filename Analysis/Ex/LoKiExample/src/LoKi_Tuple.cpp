// $Id: LoKi_Tuple.cpp,v 1.3 2004-03-11 21:41:04 ibelyaev Exp $
// ============================================================================
// CVS Tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2004/03/03 14:17:29  ibelyaev
//  update for LoKi v2r0
//
// Revision 1.1.1.1  2003/07/24 16:43:50  ibelyaev
//  new package with LoKi examples 
//
// Revision 1.3  2003/05/12 13:21:33  ibelyaev
//  add the options files for all examples
//
// ============================================================================
// Include files
// LoKi 
#include "LoKi/LoKi.h"
// Event 
#include "Event/EventHeader.h"

/** @file 
 * 
 *  Implementation file for class : LoKi_Tuple.h 
 *
 *  @date  2003-03-21 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 */

// ============================================================================
LOKI_ALGORITHM( LoKi_Tuple )
{
  // avoid long names 
  using namespace LoKi       ;
  using namespace LoKi::Cuts ;
  
  // select all charged kaons 
  Range kaons = select( "kaons" , 321 == abs( ID ) ) ;

  // subdivide sample of kaons to subsamples of positive and negative kaons  
  Range kplus  = select( "K+"  , kaons , Q >  0.5 ) ;
  Range kminus = select( "K-"  , kaons , Q < -0.5 ) ;
  
  // create the cut for phi :  abs(delta mass) < 15 MeV 
  Fun dmass = DMASS( "phi(1020)" , ppSvc() ) ;
  Cut dmcut  = abs( dmass ) < 15 * MeV  ;

  // get the N-Tuple (book it if not yet done)
  Tuple tuple1  = nTuple ( "The first N-Tuple (scalar columns)" ) ;
  for( Loop phi = loop( "K+ K-" , "phi(1020)" ) ; phi ; ++phi ) 
  {
      // put 4-momentum of phi 
      tuple1 -> column ( "phiP"    , phi    -> momentum () ) ;
      // put 4-momentum of kplus
      tuple1 -> column ( "kplusP"  , phi(1) -> momentum () ) ;
      // put 4-momentum of kminus 
      tuple1 -> column ( "kminusP" , phi(2) -> momentum () ) ;
      
      // fill separate columns 
      tuple1 -> column ( "MASS"    , M     ( phi ) ) ;
      tuple1 -> column ( "DM"      , dmass ( phi ) ) ;
      tuple1 -> column ( "P"       , P     ( phi ) ) ;
      tuple1 -> column ( "PT"      , PT    ( phi ) ) ;
      
      // fill few columns at once 
      Record rec ( tuple1                            , 
                   "LV01,LV02,CL1,CL2"               , 
                   LV01 ( phi    ) , LV01 ( phi    ) , 
                   CL   ( phi(1) ) , CL   ( phi(2) ) ) ;
      
      // save "good" phi 
      if( dmcut( phi ) ) { phi->save("phi"); }
    }
  
  // get the second ntuple 
  Tuple tuple2 = nTuple( "The second N-Tuple (array-like columns)") ;
  
  // get all saved phi 
  Range phis = selected("phi") ;
  
  // fill the tuple 
  tuple2 -> farray ( "M"    , M     , 
                     "DM"   , dmass , 
                     "P"    , P     , 
                     "PT"   , PT    , 
                     phis.begin ()  , 
                     phis.end   ()  , 
                     "nPhi"         , 
                     100            ) ;
  
  // add the event information to the N-Tuple 
  const EventHeader* evt = get<EventHeader> ( EventHeaderLocation::Default );
  
  // add event header information to N-Tuple 
  tuple2 -> column ( "" , evt );
  
  // write N-Tuple 
  tuple2 -> write  () ;
  
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
