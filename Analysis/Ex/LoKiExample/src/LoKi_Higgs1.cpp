// $Id: LoKi_Higgs1.cpp,v 1.1 2005-02-16 11:16:58 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ ; version $Release:$
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
// Include files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/LoKi.h"
// ============================================================================


LOKI_ALGORITHM( LoKi_Higgs1 )
{
  using namespace LoKi          ;
  using namespace LoKi::Fits    ;
  using namespace LoKi::Cuts    ;
  using namespace LoKi::MCTypes ;

  // get the event header 
  const EventHeader* evtHdr = 
    get<EventHeader>( EventHeaderLocation::Default ) ;
  
  MCMatch mc = mctruth() ;
  
  // # select all stable MC particles, produced near collision point
  MCCut   mccut = MCVXFUN( MCVZ ) < (  30 *  cm )  ;
  MCRange mcall = 
    mcselect( "mcall" , mccut && ( ( "e+"    == MCABSID ) |
                                   ( "mu+"   == MCABSID ) |
                                   ( "K+"    == MCABSID ) |
                                   ( "p+"    == MCABSID ) |
                                   ( "pi+"   == MCABSID ) |
                                   ( "gamma" == MCABSID ) ) ) ;
  
  // # book N-tuple for MC 
  Tuple tup1 = nTuple( "MC-tuple" ) ;
  
  // # fill energy flow ntuple for MC particles   
  for ( MCRange::iterator imcp = mcall.begin() ; mcall.end() != imcp ; ++imcp ) 
  {
    const MCParticle* mcp = *imcp ;
    if ( 0 == mcp ) { continue ; }                              // CONTINUE 
    
    
    const double eta = mcp->momentum().pseudoRapidity () ;
    const double phi = mcp->momentum().phi            () ;
    
    // from W or Z ? 
    const bool prim = 0 == mcp->mother() ;
    
    tup1 -> column ( "pid"  , MCID ( mcp )       ) ;
    tup1 -> column ( "pt"   , MCPT ( mcp ) / GeV ) ;
    tup1 -> column ( "e"    , MCE  ( mcp ) / GeV ) ;
    tup1 -> column ( "phi"  , phi                ) ;
    tup1 -> column ( "eta"  , eta                ) ;
    tup1 -> column ( ""     , evtHdr             ) ;
    tup1 -> column ( "prim" , prim               ) ;

  };
  
  // # get only MC leptons 
  MCRange mcleps = mcselect ( "mclep" , 
                              mcall   ,  
                              ( "e+" == MCABSID ) || ( "mu+"   == MCABSID ) ) ;
  
  Cut mcCutLep = PNONE ;
  MCCut mcMu   = "mu+" == MCABSID  ;
  for ( MCRange::iterator imcl = mcleps.begin() ; mcleps.end() != imcl ; ++imcl ) 
  {
    const MCParticle* mcl = *imcl ;
    if ( 0 == mcl      ) { continue ; }
    const MCParticle* moth = mcl->mother() ;
    if ( 0 != moth     ) { continue ; }
    if ( !mcMu ( mcl ) ) { continue ; }
    // update mc cut
    mcCutLep = mcCutLep || MCTRUTH ( mc , mcl ) ;
  }
  
  // # select *ALL* recontructed particles 
  Range all = select ( "all" , PALL ) ;
  
  // # retrieve tuple for reconstructed energy flow 
  Tuple tup2 = nTuple( "RC-tuple" ) ;
  for ( Range::iterator ip = all.begin() ; all.end() != ip ; ++ip ) 
  {
    const Particle* p = *ip ;
    if ( 0 == p ) { continue ; }
    
    const double eta = p->momentum().pseudoRapidity () ;
    const double phi = p->momentum().phi            () ;
    
    tup2 -> column ( "pid"  , ID ( p )       ) ;
    tup2 -> column ( "pt"   , PT ( p ) / GeV ) ;
    tup2 -> column ( "e"    , E  ( p ) / GeV ) ;
    tup2 -> column ( "phi"  , phi            ) ;
    tup2 -> column ( "eta"  , eta            ) ;
    tup2 -> column ( ""     , evtHdr         ) ;
    tup2 -> column ( "mcm"  , mcCutLep( p )  ) ;
    tup2 -> column ( "pidm" , PIDmu   ( p )  ) ;
    
    tup2 -> write() ;
  };
  

  return StatusCode::SUCCESS ;
};

// ============================================================================
// The END 
// ============================================================================
